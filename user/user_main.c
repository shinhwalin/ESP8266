/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "esp_common.h"
#include "uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "para.h"
#include "webserver.h"
struct esp_platform_saved_param sys_para;
//add by ys  to uart 2018年4月11日 17:43:23
//xSemaphoreHandle xSemaphore = NULL;  
//定义串口中断和TCP发送通讯用队列
 xQueueHandle uart_queue;

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;
        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}
/******************************************************************************
 * FunctionName : TCP_init
 * Description  : 开启串口到TCP数据透传任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
extern void  vTask_wifi_tc( void * pvParameters );
extern void  vTask_RS_tc( void * pvParameters );
extern void  vTask_webserver( void * pvParameters );
void TCP_init()
{
	xTaskHandle xHandle = NULL;
	
	xTaskCreate(
	      vTask_RS_tc,       /* Function that implements the task. */
	      "vTask_RS_tc",          /* Text name for the task. */
	      512,      /* Stack size in words, not bytes. */
	      NULL,    /* Parameter passed into the task. */
	      3,/* Priority at which the task is created. */
	      &xHandle );      /* Used to pass out the created task's handle. */
}
/******************************************************************************
 * FunctionName : wifi_event_handler_cb
 * Description  : wifi状态回调函数
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void wifi_handle_event_cb(System_Event_t* event)
{
    if (event == NULL) {
        return;
    }

    switch (event->event_id) {
        case EVENT_STAMODE_CONNECTED:
		//if(xSemaphore != NULL)
		if(uart_queue != NULL)
		{
		     TCP_init();
	            break;
		}
        default:
            break;
    }
}

/******************************************************************************
 * FunctionName : wifi_init
 * Description  : 初始化AP和STA信息
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void wifi_init()
{
	struct softap_config *config = (struct softap_config *)zalloc(sizeof(struct	softap_config)); // initialization
	wifi_set_event_handler_cb(wifi_handle_event_cb);
	wifi_softap_get_config(config); // Get soft-AP config first.
	sprintf(config->ssid, sys_para.ap_ssid);
	sprintf(config->password, sys_para.ap_pwd);
	config->authmode = sys_para.ap_en;
	config->ssid_len = 0; // or its actual SSID length
	config->max_connection = 4;
	config->ssid_hidden = sys_para.ap_hide;
	wifi_softap_set_config(config); // Set ESP8266 soft-AP config
	free(config);
	//add by ys 2018-04-11 09:05:22 通过系统参数判断是否连接
	if(sys_para.sta_connect == 1)
	{
		struct station_config * staconfig = (struct station_config *)zalloc(sizeof(struct station_config));
		sprintf(staconfig->ssid, "%s",sys_para.sta_ssid);
		sprintf(staconfig->password, "%s",sys_para.sta_pwd);
		wifi_station_set_config(staconfig);
		free(staconfig);
		
		wifi_station_dhcpc_stop();
		uint8 a;
		uint8 b;
		uint8 c;
		uint8 d;
		char buff[15] = {0};
		char *result = NULL;
		memset(buff,0,sizeof(buff));
		strcpy(buff,sys_para.local_IP);
		result = strtok(buff , "." );
		a = atoi(result);
	    	result = strtok( NULL, "." );
		b = atoi(result);
	   	result = strtok( NULL, "." );
		c = atoi(result);
	    	result = strtok( NULL, "." );
		d = atoi(result);
		//debug
		memset(disbuff,0,sizeof(disbuff));
		sprintf(disbuff,"\nIP: %d  %d  %d %d\n",a,b,c,d);
		print_data(disbuff);
		struct ip_info info;
		//IP4_ADDR(&info.ip,172,26,36,5);
		IP4_ADDR(&info.ip,a,b,c,d);
		IP4_ADDR(&info.gw,172,26,36,1);
		IP4_ADDR(&info.netmask,255,255,255,0);		
		wifi_set_ip_info(STATION_IF,&info); //设置sta模式的IP
		
		wifi_station_connect();			
	}

}
/******************************************************************************
 * FunctionName : sys_init
 * Description  : 系统参数初始化参数
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void PARA_init()
{
	read_para(&sys_para);
	if(sys_para.save_flag != 0)
	{	
		sys_para.urat_rate = 9600;
		sys_para.urat_stop_bits = 1;
		sys_para.urat_parity = 2;	 
	
		strcpy(sys_para.sta_ssid,"TP-LINK-8266");
		strcpy(sys_para.sta_pwd,"147258369");
		sys_para.sta_connect = 1;
	
		strcpy(sys_para.ap_ssid,"test");
		strcpy(sys_para.ap_pwd,"11111111");
		
		strcpy(sys_para.local_IP,"172.26.36.2");
		strcpy(sys_para.local_port,"4001");
		sys_para.ap_en = 4;
		sys_para.ap_hide = 0;		
	}
}


/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
	
	xTaskHandle xHandle = NULL;
	PARA_init();
	
	uart_init_new();
	if(wifi_set_opmode_current(STATIONAP_MODE) == 0)
	{
		printf("wifi set mode 0x03 failed!\r\n");
		while(1);
	}
	//凡事有xSemaphore 变量的都是新加调试串口和任务
	//之间的变量互锁用
	//vSemaphoreCreateBinary(xSemaphore); 
	uart_queue = xQueueCreate(QUEUE_DATA_SUM,QUEUE_DATA_LENGTH);
	
	if(uart_queue != NULL)
	{
		print_data("\n start \n");	

		wifi_init();

		xTaskCreate(
	                    vTask_wifi_tc,       /* Function that implements the task. */
	                    "vTask_wifi_tc",          /* Text name for the task. */
	                    512,      /* Stack size in words, not bytes. */
	                    NULL,    /* Parameter passed into the task. */
	                    2,/* Priority at which the task is created. */
	                    &xHandle );      /* Used to pass out the created task's handle. */
		xTaskCreate(
	                    vTask_webserver,       /* Function that implements the task. */
	                    "vTask_webserver",          /* Text name for the task. */
	                    512,      /* Stack size in words, not bytes. */
	                    NULL,    /* Parameter passed into the task. */
	                    1,/* Priority at which the task is created. */
	                    &xHandle );      /* Used to pass out the created task's handle. */
	}
	else
	{
		print_data("\n system error \n");
	}
}

