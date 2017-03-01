#ifndef __VAR
#define __VAR

/* <|Global variable declaration|>
 * 
 * 	- WifiReady : semaphore handler use to indicate the ready state of wifi
 * 	- SetWifi : semaphore handler use to set up ssid and password
 * 	- xQueueHandleUart : Queue use to handle Uart packet
 *  - data_from_conn(x) : Buffer for TCP stack 	
 */
 #define MAX_SIZE_TCP_PRINT 1024
extern xQueueHandle xQueueUART;

Microgear mg;
xSemaphoreHandle WifiReady;
xSemaphoreHandle SetWifi;
uart_event_t xQueueHandleUart;

/** Ring Buf **/
RingBuf *client1_buf;
RingBuf *client2_buf;
RingBuf *mg_buf;

/** Microgear struct **/
struct MgStruct str;	

char ssid[25],password[25],token[50],tokensecret[50],appid[50],key[50],secret[50],alias[50];
struct ip_info ipconfig;				//Use to get IP address of ESP8266

/* <|Serial variables declaration|>
 *		@message_sr -> Buffer for serial read
 * 	@param -> Indicator use to indicate state of switch case  				
 * 	@message_index -> Index of message_sr, send_buf1 and send_buf2
 * 
 */ 
 char message_sr[100] = "";
 int param=0;
 int message_index;



/* <|TCP variables declaration|>
 *		@conn -> socket client for TCP connection [Only 2 are availables at the same time]
 * 	@send_buf[x] -> Temporary buffer for sending message 				
 * 	- conn1 and conn2 for tcp communication	
 * 
 */ 
struct espconn conn1,conn2;
esp_tcp tcp1,tcp2;
ip_addr_t HostResolve_Ip1,HostResolve_Ip2;
char send_buf1[MAX_SIZE_TCP_PRINT],send_buf2[MAX_SIZE_TCP_PRINT];

#endif
