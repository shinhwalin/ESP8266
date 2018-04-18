#ifndef PARA_H
#define PARA_H
struct esp_platform_saved_param {
		//串口
		uint32 urat_rate; 	//波特率                                                             8 
		char urat_stop_bits;//串口停止位						1
		char urat_parity;	   //串口校验位						1

		//STA信息
		char sta_ssid[20];	//作为sta模式下的连接到AP的SSID    20
		char sta_pwd[20];//作为sta模式下的连接到AP的密码20 
		char sta_connect;//在STA模式下的AP连接标识               1
		
		//AP信息
		char ap_ssid[20];//作为AP模式的SSID                                      20
		char ap_pwd[20];//作为AP模式的密码                                 20
		char ap_hide;//隐藏标志位                                                      1 
		char ap_en;		   //AP模式下的加密模式			1
		

		
		char save_flag;//保存标识，0:已保存				1		
		char buwei[3];//补位									3

		char local_IP[15];//本地IP								15
		char local_port[4];//本地端口							4
		
};//116

#define ESP_PARAM_START_SEC		(0xFE - 10)//第一个扇区的位置

bool read_para(struct esp_platform_saved_param * data);
bool save_para(struct esp_platform_saved_param * data);
#endif

