#include "esp_common.h"
#include "para.h"
#include "webserver.h"
#include "uart.h"

char http_webStr[4096] = {0};//4

char head[100] = {0};
char window_1Str2_1[303] = {0};
char window_1Str1_1[363] = {0};
char str_sta[300] = {0};
char str_ap[500] = {0};
char str_IPport[200] = {0};//实际160
//http_dat phttp_dat;

char disbuff[100] = {0};


const char * str_webhead = 
		"HTTP/1.1 200 OK\r\nContent-Type: text"
		"/html\r\nContent-Length: %d\r\n\r\n"
		;
const char *str_htmlhead = 
		"<meta http-equiv=\"Content-Type\"content=\"text/html; charset=gb2312\" />"
			"<title>测试网页</title>"
		"<style type=\"text/css\">"
		"<!--"
		"body,td,th{"
			"font-size: x-large;"
			"color: #FF0000;"
			"font-weight: bold;"
		"}"
		"body{"
			"background-color: #CCCCCC;"
		"}"
		"-->"
		"</style>"
		"<head>"
			"</head>"
		"<html>"
		"<body>"
		;	


const char *str_uartrate =
		"<form id=\"form1\" name=\"form1\" method=\"post\" action=\"\">"
		  "<p align=\"center\">"
		   " <input type=\"submit\" name=\"Submit3\" value=\"Refresh\" />"
		  "</p>"
		  "<p>波特率"
		    "<select name=\"select\">"
		      "<option value=\"rate_1200\"%s>1200</option>"
		      "<option value=\"rate_2400\"%s>2400</option>"
		      "<option value=\"rate_4800\"%s>4800</option>"
		      "<option value=\"rate_9600\"%s>9600</option>"
		    "</select>"
		;
const char *str_uarbit=		//窗口1第二帧数据 校验/停止位
		"校验位"
		"<select name=\"select3\">"
		  "<option value=\"None_0\"%s>None</option>"
		  "<option value=\"None_1\"%s>Odd</option>"
		  "<option value=\"None_2\"%s>Even</option>"
		"</select>"
		"停止位"
		"<select name=\"select4\">"
		  "<option value=\"stop_1\"%s>1</option>"
		  "<option value=\"stop_2\"%s>2</option>"
		"</select>"
		  "</p>"
		;

const char *str_htmlsta=
		" <p>STA:"
		  "SSID"
		  "<input name=\"STA_SSID\" type=\"text\" value=\"%s\"maxlength=\"15\" />"
		  "PASS"
		  "<input name=\"STA_PASS\" type=\"text\" value=\"%s\" maxlength=\"15\" /> "
		    "<input name=\"checkbox\" type=\"checkbox\" value=\"STA_CONN\" %s/>"
		  "是否启用连接</p>"
		  ;



const char *str_htmlap=
		"<p>AP:"
		  "AP_SSID"
		  "<input name=\"AP_SSID\" type=\"text\" value=\"%s\" maxlength=\"15\" /> "
		  "AP_PASS"
		  "<input name=\"AP_PASSWORD\" type=\"text\" value=\"%s\" maxlength=\"15\" />"
		  "加密方式"
		    "<select name=\"select5\">"
		      "<option value=\"AP_OPEN\"%s>OPEN</option>"
		      "<option value=\"AP_WPA\"%s>WPA</option>"
		      "<option value=\"AP_WPA2\"%s>WPA2</option>"
		      "<option value=\"AP_WPA_WPA2\"%s>WPA_WPA2</option>"
		    "</select>"
		  "隐藏 SSID"
		    "<input name=\"AP_hidden\" type=\"checkbox\" value=\"AP_hidde\"%s />"
		  "</p>"
		;
const char *str_IPPort = 
		  "<p>IP:"
		  "<input name=\"TX_IP\" type=\"text\" value=\"%s\" maxlength=\"15\" />"
		  "PORT:"
		  "<input name=\"TX_PORT\" type=\"text\" value=\"%s\" maxlength=\"4\" />"
		  "</p>"
		  ;

const char *str_htmlbut=
		"<p>test</p>"
		"<p></p>"
		  "<p>"
		    "<input name=\"Submit\" type=\"submit\" value=\"configure_\" />"
		  "</p>"
		  "<p>"
		    "<input name=\"Submit2\" type=\"submit\" value=\"Restore_\" />"
		  "</p>"
		;

const char *str_htmlend = 
		"</body>"
		"</html>";








/*
 * 函数：void GetString(char *arg ,char *dat,char *data)
 * 功能:获取已知字符串值
 * arg：字符串 dat:已知字符串 data:获取的位置
 */
void GetString(char *arg ,char *dat,char *data)//获取字符串
{
	char len;
	char *buff=NULL;
	len=strlen(dat);
	buff= (char *)strstr(arg,dat);
	buff=buff+len+1;
	while(*buff!='&')
	{
		*data=*buff;
		data++;
		buff++;
	}
}

/*
 * 函数:char Seek_Str(char *s ,char *d) //寻找字符串
 * 功能:判断s是否包含d有字符串包含返回:1否则:0
 */
char Seek_Str(char *s ,char *d) //寻找字符串
{
	char *pbuffer = NULL;
	 pbuffer=(char *)strstr(s,d);
	 if(pbuffer!=NULL)
	 {
		 return 1;
	 }
	 else
	 {
		 return 0;
	 }
}


void string_Clear(char *arg ,uint16_t len)
{
	for(;len>0;len--)
	{
		arg[len]=0x00;
	}
}


void window_Rate(char *arg)
{
	if(Seek_Str(arg,"configure_"))
	{
		if(Seek_Str(arg,"rate_1200")){sys_para.urat_rate=1200;}
		if(Seek_Str(arg,"rate_2400")){sys_para.urat_rate=2400;}
		if(Seek_Str(arg,"rate_4800")){sys_para.urat_rate=4800;}
		if(Seek_Str(arg,"rate_9600")){sys_para.urat_rate=9600;}
		if(Seek_Str(arg,"None_0")){sys_para.urat_parity=2;}
		if(Seek_Str(arg,"None_1")){sys_para.urat_parity=1;}
		if(Seek_Str(arg,"None_2")){sys_para.urat_parity=0;}

		if(Seek_Str(arg,"stop_1")){sys_para.urat_stop_bits=1;}
		if(Seek_Str(arg,"stop_2")){sys_para.urat_stop_bits=3;}
		char snum[15];
		char pnum[15];
		//获取STA的ssid 和PASSWORD
		memset(snum,0,sizeof(snum));
		memset(pnum,0,sizeof(pnum));//清楚缓存器
		GetString(arg,"STA_SSID",snum);				//提取ssid字符串
		GetString(arg,"STA_PASS",pnum);			//提取ssid字符串
		memset(sys_para.sta_ssid,0,sizeof(sys_para.sta_ssid));
		memset(sys_para.sta_pwd,0,sizeof(sys_para.sta_pwd));
		strcpy(sys_para.sta_ssid,snum);
		strcpy(sys_para.sta_pwd,pnum);
		if(Seek_Str(arg,"STA_CONN")){sys_para.sta_connect = 1;}else{sys_para.sta_connect=0;}
		//获取AP的ssid和PASSWORD
		memset(snum,0,sizeof(snum));
		memset(pnum,0,sizeof(pnum));//清楚缓存器
		GetString(arg,"AP_SSID",snum);				//提取ssid字符串
		GetString(arg,"AP_PASSWORD",pnum);			//提取ssid字符串
		memset(sys_para.ap_ssid,0,sizeof(sys_para.ap_ssid));
		memset(sys_para.ap_pwd,0,sizeof(sys_para.ap_pwd));
		strcpy(sys_para.ap_ssid,snum);
		if(strlen(pnum) < 8)
		{
			strcpy(sys_para.ap_pwd,"12345678");
		}
		else
		{
			strcpy(sys_para.ap_pwd,pnum);
		}		
		if(Seek_Str(arg,"AP_OPEN")){sys_para.ap_en=0;}
		if(Seek_Str(arg,"AP_WPA")){sys_para.ap_en=2;}
		if(Seek_Str(arg,"AP_WPA2")){sys_para.ap_en=3;}
		if(Seek_Str(arg,"AP_WPA_WPA2")){sys_para.ap_en=4;}
		if(Seek_Str(arg,"AP_hidde")){sys_para.ap_hide=1;}else{sys_para.ap_hide=0;}

		memset(snum,0,sizeof(snum));
		GetString(arg,"TX_IP",snum);
		memset(sys_para.local_IP,0,sizeof(sys_para.local_IP));
		strcpy(sys_para.local_IP,snum);
		//debug
		memset(disbuff, 0, sizeof(disbuff));
		sprintf(disbuff,"-\nIP %s-\n",sys_para.local_IP);
		print_data(disbuff);
		
		memset(sys_para.local_port,0,sizeof(sys_para.local_port));
		GetString(arg,"TX_PORT",sys_para.local_port);
		//debug
		memset(disbuff, 0, sizeof(disbuff));
		sprintf(disbuff,"-\nport %s-\n",sys_para.local_port);
		print_data(disbuff);
		
		sys_para.save_flag = 0;
		save_para(&sys_para);
		system_restart();
	}
}



void  http_Handle(char *arg) //字符串的解析
{
	if((Seek_Str(arg,"GET")==0)&&(!Seek_Str(arg,"HTTP/1.1")==0))
	{
		if(Seek_Str(arg,"POST / HTTP/1.1"))
		{
			window_Rate(arg);
		}
	}

}
void html_raut1()
{
	memset(window_1Str1_1,0,sizeof(window_1Str1_1));
	char *rate1200 = ".";
	char *rate2400 = ".";
	char *rate4800 = ".";
	char *rate9600 = ".";
	switch(sys_para.urat_rate)
	{
	case 1200:
		rate1200=SELECT_SHOW; 
		break;
	case 2400: 
		rate2400=SELECT_SHOW;
		break;
	case 4800:
		rate4800=SELECT_SHOW; 
		break;
	case 9600:
		rate9600=SELECT_SHOW; 
		break;
	default :
		rate9600=SELECT_SHOW; 
		break;
	}
	sprintf(window_1Str1_1,str_uartrate,rate1200,rate2400,rate4800,rate9600);
}

void html_raut2()
{
	char *none = " "; char *odd = " "; char * even = " ";
	char* stop1=" ";char *stop2=" ";
	switch(sys_para.urat_parity)
	{
	case 0:
		even=SELECT_SHOW;
		break;
	case 1:
		odd=SELECT_SHOW;
		break;
	case 2:
		none=SELECT_SHOW;
		break;
	default:
		none=SELECT_SHOW; 
		break;
	}

	switch(sys_para.urat_stop_bits)
	{
	case 1:
		stop1=SELECT_SHOW;
		break;
	case 3:
		stop2=SELECT_SHOW;
		break;
	default:
		stop1=SELECT_SHOW; 
		break;
	}
	
	memset(window_1Str2_1,0,sizeof(window_1Str2_1));
	sprintf(window_1Str2_1,str_uarbit,none,odd,even,stop1,stop2);
}
void html_sta()
{
	char *conn = " ";
	memset(str_sta,0,sizeof(str_sta));
	if(sys_para.sta_connect == 1)
	{
		conn = CHECKED_SHOW;
	}
	sprintf(str_sta,str_htmlsta,sys_para.sta_ssid,sys_para.sta_pwd,conn);
}
void html_ap()
{
	char *en_open=" "; char *en_wpa=" "; char* en_wpa2=" "; char* en_wwp=" ";
	char *hide = " ";
	memset(str_ap,0,sizeof(str_ap));
	switch(sys_para.ap_en)
	{
	case 0:
		en_open=SELECT_SHOW;
		break;
	case 2:
		en_wpa=SELECT_SHOW;
		break;
	case 3:
		en_wpa2=SELECT_SHOW;
		break;
	case 4:
		en_wwp=SELECT_SHOW;
		break;
	default:
		en_open=SELECT_SHOW;
		break;
	}
	if(sys_para.ap_hide == 1)
	{
		hide = CHECKED_SHOW;
	}
	sprintf(str_ap,str_htmlap,sys_para.ap_ssid,sys_para.ap_pwd,en_open,en_wpa,en_wpa2,en_wwp,hide);
}
void html_IPport()
{
	char buff[5];
	memset(buff,0,5);
	memcpy(buff,sys_para.local_port,4);
	memset(str_IPport,0,sizeof(str_IPport));
	sprintf(str_IPport,str_IPPort,sys_para.local_IP,buff);
}
int html_len()
{
	return strlen(str_htmlhead) 
		+ strlen(window_1Str1_1)
		+ strlen(window_1Str2_1)
		+ strlen(str_sta)
		+ strlen(str_ap)
		+ strlen(str_IPport)
		+ strlen(str_htmlbut)
		+strlen(str_htmlend);	
}





































