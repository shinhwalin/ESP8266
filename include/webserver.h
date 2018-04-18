#ifndef YS_ESP_WEBSERVER_H
#define YS_ESP_WEBSERVER_H
#include "esp_common.h"
#include "para.h"
//����һ�����ݵĳ���
#define QUEUE_DATA_LENGTH  256
//�������ݸ���
#define QUEUE_DATA_SUM 4
//���ڽ��ܳ�ʱʱ��
#define UART_RECV_TOUT 100

//����͸��������
extern int clicoon;

//add by ys  to uart 2018-04-11 17:24:33  ����͸������
//#include "freertos/semphr.h"
//extern xSemaphoreHandle xSemaphore;
//end uart
//add ys to uart ʹ����Ϣ����ʵ�ִ��ں�TCP���������ͨ��
#include "freertos/queue.h"
//�����жϺ�����ͨѶ�õ���Ϣ����
extern xQueueHandle uart_queue;
//end add to uart
extern char http_webStr[4096];//4//4k
extern void string_Clear(char *arg ,uint16 len);
extern char Seek_Str(char *s ,char *d);
extern void GetString(char *arg ,char *dat,char *data);
extern void  http_Handle(char *arg);
//����ڴ�����ʾhtml�ַ���
extern void html_raut1();
extern void html_raut2();

//�����wifi ��ʾhtml�ַ���
extern void html_sta();//sta
extern void html_ap();//ap

//����httpЭ��İ��峤��
extern int html_len();


extern int32 sock_fd;//client��socket�׽���
extern int listenfd;//server��socket�׽���



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


//������ʾbuff
extern char disbuff[100];


extern char window_1Str1_1[363]; //������
extern char window_1Str2_1[303];//У��λ
extern char head[100];
extern char str_sta[300];
extern char str_ap[500];
extern char str_IPport[200];

#endif
