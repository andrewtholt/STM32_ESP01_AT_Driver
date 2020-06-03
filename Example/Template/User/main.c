#include "stm32f4xx.h"
#include "./AT/AT_ESP.h"
#include "./debug/debug.h"
#include "./systick/bsp_systick.h"
#include "./usart2/bsp_usart2.h"
#include "stdlib.h"

extern char ESPBuffer[128];

typedef struct Message{
	int Number;
	char name[5];
	float pi;
}Messsage;

void MessageInit(Messsage *handle)
{
	handle->Number = 1024;
	handle->name[0] = 'j';
	handle->name[1] = 'a';
	handle->name[2] = 'c';
	handle->name[3] = 'k';
	handle->pi = 3.142312312;
}

int main(void)
{
	esp8266 test;
	Messsage a;
	MessageInit(&a);
	Debug_Config();
	MySysTick_Config();
	esp8266Init(&test);
	while(!test.CheckESP()) //���ESP01
	{
		Debug_Info("�޷���⵽ESP01.....\r\n");
	}
	if(test.ConnectWiFi("ChinaNet-jnUe","ztfgvk9t"))  //����ָ��WIFI
	{
		if(test.ConnectServer("192.168.1.6","13142",0))//����ָ��TCPServer
		{
			while(1)
			{
				test.SendStringDataToServer("I AM JACK"); //�����ַ�����Ϣ��������
				test.SendDataToServer((char *)&a,sizeof(Messsage));//�����ڴ������Ϣ��������
				test.CloseTCPOrUDPConnect();
			}
		}
	}
  while(1);
}

