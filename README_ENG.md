STM32_ESP01_AT_Driver
This is a STM32 ESP-01 driver library. Since I am very interested in OOP during this time, this driver will write this ESP-01 driver library in my crappy OOP way. Please don't vomit my C language OOP writing, I know it's bad, and it doesn't conform to the specifications, but this is my first attempt, one according to my OOP idea.

First, let's introduce the structure of the warehouse:
Examlpe: There is a Template folder. The Template contains a complete migration example (based on STM32F4).
Src: Contains AT_ESP.c and AT_ESP.h, this is the driver library file.

Hardware connection:
3V3 and GND of ESP01 are connected to 3V3 and GND of single chip microcomputer. The TX and RX of ESP01 and the TX and RX of the single chip are reversely connected, and the EN pin of ESP01 is connected to the 3V3 of the single chip.

I use the STD library of STM32 to write the program, which may make most students who use HAL feel unfriendly. But when you spend a few minutes to understand the principle of this driver library, you will find that as long as you transplant a few key functions, you can use the HAL library to run this ESP-01 driver.

The general idea of ​​the driver: Use a UART of STM32 to communicate with ESP01. The parameters of the serial port are 115200 baud rate, 8 data bits, 1 stop bit, no check, no hardware control. Use the serial port interrupt to receive the information returned by ESP01 to the microcontroller to determine whether the command we want to execute is running correctly. The rest are basic string operations.

Here are examples of users of STD library functions: For users of STD library functions: only need to implement:
1. The serial port initialization function connected to ESP01, remember to open the USART_IT_RXNE interrupt, the serial port initialization parameters are mentioned above.
2. Serial port sending function based on character string, used to send instructions to ESP01.
3. Serial port sending function based on sending length, used to send special commands to ESP01. If you are just sending string commands, you can not implement this function.
4. Delay function in milliseconds. Generally implemented with SYSTICK.

打开AT_ESP.c，在以下宏定义填写上述所需要的函数：
#define ESP_UART_CONFIG        //和ESP01对接的串口初始化函数
#define SendMessageToESP       //基于字符串的串口发送函数
#define SendMessageToESP2      //基于发送长度的串口发送函数
#define Delay_ms               //毫秒级的延时函数

可能有人会对要求2，3的所说的东西感到疑惑。
在这里，我举一个例子：
首先是一个串口字节发送函数
void USART2_SendByte(uint8_t data)
{
	USART_SendData(USART2,data);

	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);
}
基于字符串的串口发送函数和基于发送长度的串口发送函数都需要这个串口字节发送函数.
因为我的ESP01模块对接我的UART2，所以填写UART2。当你们使用的时候，只需填回对应的串口即可。
基于字符串的串口发送函数：
void USART2_SendString(char *buffer)
{
     while(*(buffer) != '\0')
 {
    USART2_SendByte(*(buffer));
    buffer++;
     }

 while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
}
基于发送长度的串口发送函数：
void USART2_SendData(char *buffer,int len)
{
    while(len--)
    {
        USART2_SendByte(*(buffer));
        buffer++;
    }
    while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
}
Perhaps you will be wondering why these two almost identical functions are needed. This is because when you use ESP01 to send information about memory objects such as structures to the server, these memory objects will often have 0X00 data. The form will not be sent. You may also ask, why not directly use the serial port sending function based on the sending length? This is because the string-based serial sending function exists in most application scenarios. If all USARTs are used? Also personally think. You can change it according to your needs.

Next, the interrupt receiving function for receiving ESP01 feedback information is introduced. It is only a function of using conventional interrupts.

void USART2_IRQHandler(void)
{
if(USART_GetFlagStatus(USART2, USART_FLAG_ORE) == SET)
{
	USART_ReceiveData( USART2 );
	USART_ClearITPendingBit(USART2,USART_IT_ORE);
}
if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET)
{
	if(ESPRxIndex < 128)
	{
		ESPBuffer[ESPRxIndex++] = USART_ReceiveData(USART2);
	}
	else
	{
		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	}
}
 }  
There is a buffer for receiving feedback information from ESP01. After each command is sent to ESP01, we will judge whether our command is executed correctly according to the information fed back by AT. For details, check out the CheckResponse() function. And every time we send a command to ESP01, we will clear this buffer.

Usage (after implementing the functions required by the library):

第一步：新建一个结构体esp8266，调用esp8266Init()函数初始化这个结构体,并检查单片机能否正确和ESP01通信
'''
你别的需要初始化函数....
'''
esp8266 test;
esp8266Init(&test);
if(!test.CheckESP())
{
	printf("无法和ESP01正确通信........\r\n");
}

第二步：Do what you want..._-_
例如，设置ESP01的WIFI模式，连接WIFI，连接TCPServer,向TCPServer发送字符串数据
test.SetEspMode(1);      //设置ESP01的WIFI模式为Station模式
test.ConnectWiFi(SSID,PassWD);   //连接指定WIFI
test.ConnectServer(IPaddress,Port,0);     //连接TCPServer
test.SendStringDataToServer("I AM JACK");   //向TCPServer发送"I AM JACK"字符串

更多用法，请查看源码注释
The driver library requires users to comply with the MIT agreement.
