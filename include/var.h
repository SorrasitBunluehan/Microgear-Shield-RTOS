#ifndef __VAR
#define __VAR


 #define MAX_SIZE_TCP_PRINT 1024
 #define MAX_SIZE_FEED_DATA 500
 #define MAX_SIZE_APIKEY 50
 #define MAX_SIZE_TOKEN 50
 #define MAX_SIZE_TOKENSECRET 50
 #define MAX_SIZE_APPID 50
 #define MAX_SIZE_KEY 50
 #define MAX_SIZE_SECRET 50
 #define MAX_SIZE_ALIAS 50
 #define MAX_SIZE_FEEDNAME 50
 #define MAX_SIZE_PUBLISH 500
 #define MAX_SIZE_TOPIC 500
 #define MAX_SIZE_CHAT 500
 
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
 * 	@echo_modoe -> Select mode of echo
 */ 
 
 char message_sr[200] = "";
 RingBuf *client1_buf,*client2_buf,*mg_buf;
 int param=0;
 int message_index;
 xSemaphoreHandle WifiReady,SetWifi;
 uart_event_t xQueueHandleUart;
 Microgear mg;
 int echo_mode = 1;
 int push_mode = 1;

/* <|MQTT Variables declaration|>
 *		@str -> MgStruct structure type keep all information of arrival message (topic,msg,msglen)
 * 	@str_pull -> MgStruct structure type store data that pull from microgear buffer
 *  	@token,tokensecret,appid,key,secret,alias -> Required info. to connect to NETPIE
 * 	@feedmode -> Indicator to choose apikey or not
 */ 
struct MgStruct str,str_pull;	
char token[MAX_SIZE_TOKEN],tokensecret[MAX_SIZE_TOKENSECRET],appid[MAX_SIZE_APPID],key[MAX_SIZE_KEY],secret[MAX_SIZE_SECRET],alias[MAX_SIZE_ALIAS];
char feedname[MAX_SIZE_FEEDNAME],feeddata[MAX_SIZE_FEED_DATA],apikey[MAX_SIZE_APIKEY];
char data_pub[MAX_SIZE_PUBLISH],topic_pub[MAX_SIZE_TOPIC];
char data_chat[MAX_SIZE_CHAT],device_name[MAX_SIZE_ALIAS];
char feed_buff[MAX_SIZE_TOPIC];
struct ip_info ipconfig;				//Use to get IP address of ESP8266
int feedmode;


/* <|Network Variables declaration|>
 *		@send_buf -> Passing buffer store TCP sending messages
 *  	@conn -> TCP socket (2 sockets are available at the same time)
 * 	@tcp -> esp_tcp struct for process TCP functions
 * 	@HostResolve_Ip -> Ip_addr_t type store address of host after traslation
 */ 
 char ssid[25],password[25];
 int sending_bytes,port;
 char send_buf1[MAX_SIZE_TCP_PRINT],send_buf2[MAX_SIZE_TCP_PRINT];
 struct espconn conn1,conn2;
 esp_tcp tcp1,tcp2;
 ip_addr_t HostResolve_Ip1,HostResolve_Ip2;
 

 


#endif
