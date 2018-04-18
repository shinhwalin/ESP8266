#include "esp_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/lwip/sockets.h"
#include "para.h"
#include "uart.h"
#include "user_main.h"
#include "webserver.h"
#define TEST_DEBUG 1

#define TCP_LENGTH 512
static char TCP_msg[TCP_LENGTH] = {0};
int32 sock_fd;//client的socket套接字
static xTaskHandle SendxHandle = NULL;
//防止AP突然断开后继续向sockert套接字写数据
int sendflag = 0;
static struct sockaddr_in servaddr;

void  vTask_TCP_send( void * pvParameters )
{
	char data[256];
	for(;;)
	{
		memset(data,0,256);
		if (xQueueReceive(uart_queue, (void * )data, (portTickType)portMAX_DELAY))//从队列中读取消息	
		{
			if(memcmp(data,"11",2) == 0)
			{
				if(sendflag == 0)
				{
					write(sock_fd,data,strlen(data));
				}					
			}
			else if(memcmp(data,"12",2) == 0)
			{
				if(clicoon > 0)
				{
					write(clicoon,data,strlen(data));
				}					
			}
		}		  		
	}
	vTaskDelete(NULL);
}


void Send_Task_init()
{	
	xTaskCreate(
	      vTask_TCP_send,       /* Function that implements the task. */
	      "vTask_TCP_send",          /* Text name for the task. */
	      512,      /* Stack size in words, not bytes. */
	      NULL,    /* Parameter passed into the task. */
	      4,/* Priority at which the task is created. */
	      &SendxHandle );      /* Used to pass out the created task's handle. */
}
void TC_socket_init()
{
	sock_fd = socket(AF_INET,  SOCK_STREAM, 0);
	if(sock_fd  < 0)
	{
		printf("ESP8266 TCP task socket > failed to create sock!\n");
		vTaskDelay(1000/portTICK_RATE_MS);
		while(1);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr =inet_addr("172.26.36.1");
	servaddr.sin_len = sizeof(servaddr);
	print_data("\ncreate socket\n");
}

//one tick 10ms
void  vTask_RS_tc( void * pvParameters )
{
	int ret;
	uint8 fifo_len;

	uint8 fifo_i = 0;
	uint8 data_i = 0;
	//ap confi

	TC_socket_init();
	
	while(1)
	{
		print_data("\n connect AP... \n");
		do{
			ret = connect(sock_fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
			vTaskDelay(1000/portTICK_RATE_MS);
		}while(ret < 0);
		sendflag = 0;
		if(SendxHandle == NULL)
		{
			Send_Task_init();
		}

		print_data("\nready tc\n");
		//TCP通过串口透传
		while(1)
		{
			int ret = read(sock_fd,TCP_msg,TCP_LENGTH);
			//print_data("\nread data\n");
			if(ret < 0)
			{	
				print_data("\nAP error connect AP... \n");
				sendflag = 1;
				close(sock_fd);
				TC_socket_init();
				break;
			}
			else if(ret == 0)
			{
				print_data("\nAP error connect AP... \n");
				sendflag = 1;
				close(sock_fd);
				TC_socket_init();
				break;
			}
			else
			{
				print_data(TCP_msg);
				memset(TCP_msg,0,sizeof(TCP_msg));
			}
		}
	}
	vTaskDelete(NULL);
}






