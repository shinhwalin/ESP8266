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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/lwip/sockets.h"
#include "para.h"
#include "webserver.h"

#define TCP_LOCAL_PORT 5188
#define TCP_DATA_LEN 1024
static unsigned char tcp_msg[TCP_DATA_LEN];

void  vTask_webserver( void * pvParameters )
{
	int listenfd;
	int ret = 0;
	int coon = 0;
	int nNetTimeout = 800;
	do
	{
		listenfd = socket(AF_INET,  SOCK_STREAM, 0);
		if (listenfd < 0) 
		{
			vTaskDelay(1000/portTICK_RATE_MS);
		}
	}while(listenfd < 0);


	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(80);
	servaddr.sin_addr.s_addr = INADDR_ANY;

	do
	{
		ret = bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
		if (ret != 0) 
		{
			vTaskDelay(1000/portTICK_RATE_MS);
		}
	}while(ret != 0);
	

	do
	{
		ret = listen(listenfd, 5);
		if (ret != 0) 
		{
			vTaskDelay(1000/portTICK_RATE_MS);
		}
	}while(ret != 0);

	ret  = setsockopt(listenfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout,sizeof(int));
	//sprintf("");
	struct sockaddr_in peeraddr;
	socklen_t peerlen = sizeof(peeraddr);
	print_data("\nready WEB\n");
	while(1)
	{
		
		coon = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen);
		if(coon < 0)
		{
			vTaskDelay(1000/portTICK_RATE_MS);
			continue;
		}
		while(1) 
		{
			memset(tcp_msg, 0, sizeof(tcp_msg));
			int ret = read(coon, tcp_msg, sizeof(tcp_msg));
			if(ret <= 0)
			{
				close(coon);
				break;
			}
			else if(ret == 0)
			{
				close(coon);
				break;
			}
			else
			{
				http_Handle(tcp_msg);
				if(Seek_Str(tcp_msg, "GET"))
				{
					html_raut1();
					html_raut2();
					html_sta();
					html_ap();
					html_IPport();
					int len = html_len();
					memset(head,0,sizeof(head));
					//sprintf(head,str_webhead,len);
					sprintf(head,str_webhead,1850);
					
					memset(http_webStr,0,sizeof(http_webStr));
					sprintf(http_webStr,"%s%s%s%s%s%s%s%s",head,str_htmlhead,
							window_1Str1_1, window_1Str2_1,//窗口1
							str_sta,
							str_ap,
							str_IPport,//add by ys2018-04-11 10:50:14 本机IP和端口
							str_htmlbut,
							str_htmlend);
					write(coon, http_webStr, strlen(http_webStr));
				}
				else if(Seek_Str(tcp_msg, "POST"))
				{
					html_raut1();
					html_raut2();
					html_sta();
					html_ap();
					html_IPport();
					int len = html_len();
					memset(head,0,sizeof(head));
					sprintf(head,str_webhead,1850);
						
					memset(http_webStr,0,sizeof(http_webStr));
					sprintf(http_webStr,"%s%s%s%s%s%s%s%s",head,str_htmlhead,
							window_1Str1_1, window_1Str2_1,//窗口1
							str_sta,
							str_ap,
							str_IPport,//add by ys2018-04-11 10:50:14 本机IP和端口
							str_htmlbut,
							str_htmlend);
					write(coon, http_webStr, strlen(http_webStr));
				}
			}	
		}
	}
}