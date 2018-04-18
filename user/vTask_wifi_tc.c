#include "esp_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/lwip/sockets.h"
#include "para.h"

#include "uart.h"
#include "user_main.h"
#include "webserver.h"
int listenfd;//server的socket套接字
int clicoon;//在server中的client连接套接字

#define TCP_LOCAL_PORT 5188
#define TCP_DATA_LEN 500
static unsigned char tcp_msg[TCP_DATA_LEN];



 void  vTask_wifi_tc( void * pvParameters )
{

	int ret = 0;
	clicoon = -1;
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
	servaddr.sin_port = htons(4001);
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
		
	struct sockaddr_in peeraddr;
	socklen_t peerlen = sizeof(peeraddr);
	print_data("\nready TCP\n");
	while(1)
	{
		
		clicoon = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen);
		if(clicoon < 0)
		{
			vTaskDelay(1000/portTICK_RATE_MS);
			continue;
		}
		while(1) 
		{
			memset(tcp_msg, 0, sizeof(tcp_msg));
			int ret = read(clicoon, tcp_msg, sizeof(tcp_msg));
			
			if(ret <= 0)
			{
				close(clicoon);
				break;
			}
			else
			{
				print_data(tcp_msg);			
			}
		}
	}
}



