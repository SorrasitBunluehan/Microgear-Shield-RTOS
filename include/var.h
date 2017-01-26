#ifndef __VAR
#define __VAR

/* <|Global variable declaration|>
 * 
 * 	- WifiReady : semaphore handler use to indicate the ready state of wifi
 * 	- SetWifi : semaphore handler use to set up ssid and password
 * 	- xQueueHandleUart : Queue use to handle Uart packet
 *  - data_from_conn(x) : Buffer for TCP stack 	
 */
 
extern xQueueHandle xQueueUART;

Microgear mg;
xSemaphoreHandle WifiReady;
xSemaphoreHandle SetWifi;
uart_event_t xQueueHandleUart;
ringBufS data_from_conn1,data_from_conn2;
char ssid[25],password[25],token[50],tokensecret[50],appid[50],key[50],secret[50],alias[50];
struct ip_info ipconfig;				//Use to get IP address of ESP8266

/* <|TCP variables declaration|>
 *	
 * 	- conn0 reserve for Microgear				
 * 	- conn1 and conn2 for tcp communication	
 * 
 */ 
struct espconn conn0,conn1,conn2;
esp_tcp tcp0,tcp1,tcp2;
ip_addr_t HostResolve_Ip0,HostResolve_Ip1,HostResolve_Ip2;

#endif
