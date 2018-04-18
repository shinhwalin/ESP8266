#ifndef YS_ESP_WEBSERVER_H
#define YS_ESP_WEBSERVER_H
#include "esp_common.h"
#include "para.h"
//队列一个数据的长度
#define QUEUE_DATA_LENGTH  256
//队列数据个数
#define QUEUE_DATA_SUM 4
//串口接受超时时间
#define UART_RECV_TOUT 100

//串口透传的数据
extern int clicoon;

//add by ys  to uart 2018-04-11 17:24:33  串口透传变量
//#include "freertos/semphr.h"
//extern xSemaphoreHandle xSemaphore;
//end uart
//add ys to uart 使用消息队列实现串口和TCP发送任务的通信
#include "freertos/queue.h"
//串口中断和任务通讯用的消息队列
extern xQueueHandle uart_queue;
//end add to uart
extern char http_webStr[4096];//4//4k
extern void string_Clear(char *arg ,uint16 len);
extern char Seek_Str(char *s ,char *d);
extern void GetString(char *arg ,char *dat,char *data);
extern void  http_Handle(char *arg);
//组关于串口显示html字符串
extern void html_raut1();
extern void html_raut2();

//组关于wifi 显示html字符串
extern void html_sta();//sta
extern void html_ap();//ap

//计算http协议的包体长度
extern int html_len();


extern int32 sock_fd;//client的socket套接字
extern int listenfd;//server的socket套接字



#define SELECT_SHOW   "selected=\"selected\""
#define CHECKED_SHOW "checked=\"checked\""

extern struct esp_platform_saved_param sys_para;
extern const char * str_webhead;
extern const char *str_htmlhead;
extern const char *str_uartrate;
extern const char *str_uarbit;
extern const char *str_htmlbut;
extern const char *str_htmlend;
extern const char *str_htmlsta;
extern const char *str_htmlap;


//测试显示buff
extern char disbuff[100];


extern char window_1Str1_1[363]; //波特率
extern char window_1Str2_1[303];//校验位
extern char head[100];
extern char str_sta[300];
extern char str_ap[500];
extern char str_IPport[200];

#endif
