#include "AT_ESP.h"
#include <string.h>
#include <stdio.h>


char ESPBuffer[128];
__IO uint16_t ESPRxIndex = 0x00;
const char ResponseOK[] = "OK\r\n";
const char SendResponse[] = "OK\r\n> ";


#define ESP_UART_CONFIG
#define SendMessageToESP
#define SendMessageToESP2
#define Delay_ms


void USART2_IRQHandler (void) {
    if (USART_GetFlagStatus (USART2, USART_FLAG_ORE) == SET) {
        USART_ReceiveData (USART2);
        USART_ClearITPendingBit (USART2, USART_IT_ORE);
    }
    if (USART_GetITStatus (USART2, USART_IT_RXNE) == SET) {
        if (ESPRxIndex < 128) {
            ESPBuffer[ESPRxIndex++] = USART_ReceiveData (USART2);
        }
        else {
            USART_ITConfig (USART2, USART_IT_RXNE, DISABLE);
        }
    }
}

// TODO replace with proper delay
static void delay () {
    int i, j;
    i = 1000;
    while (i--) {
        j = 350;
        while (j--);
    }
}

static void ClearESPBuffer () {
    ESPRxIndex = 0x00;
    memset (ESPBuffer, '\0', ESP8266_RX_BUFFER_LEN);
}

static void SendEspCommand (char *order) {
    ClearESPBuffer ();
    SendMessageToESP (order);
}

static void SendEspCommand2 (char *data, int len) {
    ClearESPBuffer ();
    SendMessageToESP2 (data, len);
}


static char CheckResponse () {
    if (strstr (ESPBuffer, ResponseOK) == NULL) {
        return 0;
    }
    return 1;
}

uint8_t CheckEsp (void) {
    SendEspCommand ("AT\r\n");
    delay ();
    if (CheckResponse ()) {
        return 1;
    }
    return 0;
}

uint8_t SetEspMode (char mode) {
    char tmp[3];
    tmp[0] = mode;
    tmp[1] = '\r';
    tmp[2] = '\n';
    SendEspCommand ("AT+CWMODE_CUR=");
    SendEspCommand (tmp);
    delay ();
    if (CheckResponse ()) {
        return 1;
    }
    return 0;
}

uint8_t ConnectWiFi (char *SSID, char *Passwd) {
    SendEspCommand ("AT+CWJAP=\"");
    SendEspCommand (SSID);
    SendEspCommand ("\",\"");
    SendEspCommand (Passwd);
    SendEspCommand ("\"\r\n");
    Delay_ms (6500);

    if (CheckResponse ()) {
        return 1;
    }
    return 0;
}

uint8_t GetIpAddress (char *RecvBuffer) {
    char i = 0;
    char *p;
    SendEspCommand ("AT+CIFSR\r\n");
    delay ();
    if (CheckResponse ()) {
        p = strtok (ESPBuffer, "\"");
        p = strtok (NULL, "\"");
        while (*p) {
            RecvBuffer[i] = *p;
            p++;
            i++;
        }
        return 1;
    }
    return 0;
}

uint8_t GetMACaddress (char *Recvbuffer) {
    char i = 0;
    char *p;
    SendEspCommand ("AT+CIFSR\r\n");
    delay ();
    if (CheckResponse ()) {
        p = strtok (ESPBuffer, "\"");
        p = strtok (NULL, "\"");
        p = strtok (NULL, "\"");
        p = strtok (NULL, "\"");
        while (*p) {
            Recvbuffer[i] = *p;
            p++;
            i++;
        }
        return 1;
    }
    return 0;
}

uint8_t DisconnectWiFi () {
    SendEspCommand ("AT+CWQAP\r\n");
    delay ();
    if (CheckResponse ()) {
        return 1;
    }
    return 0;
}

uint8_t ConnectServer (char *IPAddress, char *port, char mode) {
    if (mode == 0) {
        SendEspCommand ("AT+CIPSTART=\"TCP\",\"");
        SendEspCommand (IPAddress);
        SendEspCommand ("\",");
        SendEspCommand (port);
        SendEspCommand ("\r\n");
        Delay_ms (1000);
        if (CheckResponse ()) {
            return 1;
        }
        return 0;
    }
    else if (mode == 1) {
        SendEspCommand ("AT+CIPSTART=\"UDP\",\"");
        SendEspCommand (IPAddress);
        SendEspCommand ("\",");
        SendEspCommand (port);
        SendEspCommand ("\r\n");
        Delay_ms (1000);
        if (CheckResponse ()) {
            return 1;
        }
        return 0;
    }
    else {
        return 0;
    }
}

uint8_t CloseTCPOrUDPConnect (void) {
    SendEspCommand ("AT+CIPCLOSE\r\n");
    Delay_ms (1000);
    if (CheckResponse ()) {
        return 1;
    }
    return 0;
}

uint8_t SendStringDataToServer (char *Data) {
    char params[8];
    int len;
    len = strlen (Data);
    sprintf (params, "%d", len);
    SendEspCommand ("AT+CIPSEND=");
    SendEspCommand (params);
    SendEspCommand ("\r\n");
    delay ();
    if (CheckResponse ()) {
        SendEspCommand (Data);
        delay ();
        delay ();
        return 1;
    }
    return 0;
}

uint8_t SendDataToServer (char *Data, int len) {
    char params[8];
    sprintf (params, "%d", len);
    SendEspCommand ("AT+CIPSEND=");
    SendEspCommand (params);
    SendEspCommand ("\r\n");
    delay ();
    if (CheckResponse ()) {
        SendEspCommand2 (Data, len);
        delay ();
        delay ();
        return 1;
    }
    return 0;
}


uint8_t ModemSleep_Getinto (void) {
    SendEspCommand ("AT+SLEEP=2\r\n");
    delay ();
    if (CheckResponse ()) {
        return 1;
    }
    return 0;
}


uint8_t ModemSleep_Quit (void) {
    SendEspCommand ("AT+SLEEP=0\r\n");
    delay ();
    if (CheckResponse ()) {
        return 1;
    }
    return 0;
}


uint8_t DeepSleep_Getinto (void) {
    SendEspCommand ("AT+GSLP=0\r\n");	  delay ();
    if (CheckResponse ()) {
        return 1;
    }
    return 0;
}


void esp8266Init (esp8266 * handle) {
    ESP_UART_CONFIG ();		  handle->CheckESP = CheckEsp;	  handle->SetEspMode = SetEspMode;
    handle->ConnectWiFi = ConnectWiFi;
    handle->DisconnectWiFi = DisconnectWiFi;
    handle->GetIpAddress = GetIpAddress;
    handle->GetMACaddress = GetMACaddress;
    handle->ConnectServer = ConnectServer;
    handle->CloseTCPOrUDPConnect = CloseTCPOrUDPConnect;
    handle->SendStringDataToServer = SendStringDataToServer;
    handle->SendDataToServer = SendDataToServer;
    handle->ModemSleep_Getinto = ModemSleep_Getinto;
    handle->ModemSleep_Quit = ModemSleep_Quit;
    handle->DeepSleep_Getinto = DeepSleep_Getinto;
}
