#ifndef __VAR
#define __VAR


 #define MAX_SIZE_TCP_PRINT 1024
extern xQueueHandle xQueueUART;

 /* <|General Variables declaration|>
 *		@message_sr -> Buffer for serial read
 * 	@param -> Indicator use to indicate state of switch case  				
 * 	@message_index -> Index of message_sr, send_buf1 and send_buf2
 * 	@client1_buf -> Ringbuffer strore data from TCP connection[client 1]
 * 	@client2_buf -> Ringbuffer strore data from TCP connection[client 2]
 * 	@mg_buf -> Ringbuffer strore data from NETPIE  
 * 	@WifiReady -> Semaphore handler use to indicate ready state of wifi
 * 	@SetWifi -> Semaphore handler use to set up ssid and password
 * 	@xQueueHandleUart -> Queue use to handle Uart packet
 */ 
 char message_sr[100] = "";
 RingBuf *client1_buf,*client2_buf,*mg_buf;
 int param=0;
 int message_index;
 xSemaphoreHandle WifiReady,SetWifi;
 uart_event_t xQueueHandleUart;
 Microgear mg;

/* <|MQTT Variables declaration|>
 *		@str -> MgStruct structure type keep all information of arrival message (topic,msg,msglen)
 *  	@token,tokensecret,appid,key,secret,alias -> Required info. to connect to NETPIE
 */ 
struct MgStruct str;	
char token[50],tokensecret[50],appid[50],key[50],secret[50],alias[50];
struct ip_info ipconfig;				//Use to get IP address of ESP8266


/* <|Network Variables declaration|>
 *		@send_buf -> Passing buffer store TCP sending messages
 *  	@conn -> TCP socket (2 sockets are available at the same time)
 * 	@tcp -> esp_tcp struct for process TCP functions
 * 	@HostResolve_Ip -> Ip_addr_t type store address of host after traslation
 */ 
 char ssid[25],password[25];
 char send_buf1[MAX_SIZE_TCP_PRINT],send_buf2[MAX_SIZE_TCP_PRINT];
 struct espconn conn1,conn2;
 esp_tcp tcp1,tcp2;
 ip_addr_t HostResolve_Ip1,HostResolve_Ip2;
 

 


#endif
