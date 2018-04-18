#ifndef PARA_H
#define PARA_H
struct esp_platform_saved_param {
		//����
		uint32 urat_rate; 	//������                                                             8 
		char urat_stop_bits;//����ֹͣλ						1
		char urat_parity;	   //����У��λ						1

		//STA��Ϣ
		char sta_ssid[20];	//��Ϊstaģʽ�µ����ӵ�AP��SSID    20
		char sta_pwd[20];//��Ϊstaģʽ�µ����ӵ�AP������20 
		char sta_connect;//��STAģʽ�µ�AP���ӱ�ʶ               1
		
		//AP��Ϣ
		char ap_ssid[20];//��ΪAPģʽ��SSID                                      20
		char ap_pwd[20];//��ΪAPģʽ������                                 20
		char ap_hide;//���ر�־λ                                                      1 
		char ap_en;		   //APģʽ�µļ���ģʽ			1
		

		
		char save_flag;//�����ʶ��0:�ѱ���				1		
		char buwei[3];//��λ									3

		char local_IP[15];//����IP								15
		char local_port[4];//���ض˿�							4
		
};//116

#define ESP_PARAM_START_SEC		(0xFE - 10)//��һ��������λ��

bool read_para(struct esp_platform_saved_param * data);
bool save_para(struct esp_platform_saved_param * data);
#endif

