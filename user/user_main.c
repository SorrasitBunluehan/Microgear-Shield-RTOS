#include "uart.h"
#include "Microgear.h"
#include "ringbuf.h"
#include "user_main.h"
#include "string.h"
#include "/home/moun/ESP8266_RTOS_SDK/include/lwip/ipv4/lwip/inet.h"

// This code is alter old uart.c librar8y ->> xQueueUART = xQueueCreate(500, sizeof(uart_event_t)); from 32 to 500 size 


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



/**********************************************************************************************************************
*											 WIFI PART																  *
***********************************************************************************************************************/

//~ /* <| WiFi task |> */
LOCAL void ICACHE_FLASH_ATTR wifi_task(void *pvParameters){
	while(1){
		uint8_t status;
		if( xSemaphoreTake( SetWifi, 0 ) == pdTRUE )
		{
			struct station_config *config = (struct station_config *)zalloc(sizeof(struct station_config));
			sprintf(config->ssid, ssid);
			sprintf(config->password, password);
			wifi_station_set_config(config);
			free(config);
			
			
			os_printf("WiFi: Connecting to WiFi\n");
			wifi_station_connect();
			
			status = wifi_station_get_connect_status();
			int8_t retries = 30;
			while ((wifi_station_get_connect_status() != STATION_GOT_IP) && (retries > 0)) {
				status = wifi_station_get_connect_status();
				if (status == STATION_WRONG_PASSWORD) {
					os_printf("WRONG PASSWORD\n");
					break;
				}
				else if (status == STATION_NO_AP_FOUND) {
					os_printf("AP NOT FOUND\n");
					break;
				}
				else if (status == STATION_CONNECT_FAIL) {
					os_printf("CONNECTION FAIL\n");
					break;
				}
				vTaskDelay(1000 / portTICK_RATE_MS);
				--retries;
			}
			if ((status = wifi_station_get_connect_status()) == STATION_GOT_IP) {
				wifi_get_ip_info(STATION_IF, &ipconfig);
				os_printf("CONNECTED %d.%d.%d.%d\n", IP2STR(&ipconfig.ip));
				vTaskDelay(1000 / portTICK_RATE_MS);
			}
		}
		vTaskDelay(1000 / portTICK_RATE_MS);
		
	}
vTaskDelete( NULL );
}
 
/* <| Check status Wifi |> */
void checkstatus() {
	if( xSemaphoreTake( WifiReady, 0 ) == pdTRUE )
	{
		uint8_t status; 
		status = wifi_station_get_connect_status();
		if(status == STATION_GOT_IP){
			wifi_get_ip_info(STATION_IF, &ipconfig);
			os_printf("CONNECTED %d.%d.%d.%d\n", IP2STR(&ipconfig.ip));
		}else{
			os_printf("DISCONNECTED\n");
		}
	}
}
 
/* <| Need for handle function inside microgear.h |> */ 
void give_wifi_semaphore(void *pvParameters){
	while(1){
		while ( wifi_station_get_connect_status()== STATION_GOT_IP) {
				xSemaphoreGive(WifiReady);
				//os_printf("WiFi: Alive\n");
				vTaskDelay(1000 / portTICK_RATE_MS);
		}
		vTaskDelay(100 / portTICK_RATE_MS);
	}
	vTaskDelete( NULL );
}


/**********************************************************************************************************************
*											 CALLBACK FUNCTION PART  												  *
***********************************************************************************************************************/

/* <|DATA RECEIVED CALLBACK (FROM CONN1)|> */ 
void recv_cb1(void *arg, char *pData, unsigned short len){
	os_printf("Received data from conn1 size: %d\n",len);
	int x;
	for(x =0 ;x < len;x++){
		ringBufS_put(&data_from_conn1,pData[x]);
	}
	
}

/* <|DATA RECEIVED CALLBACK (FROM CONN2)|> */ 
void recv_cb2(void *arg, char *pData, unsigned short len){
	os_printf("Received data from conn2 size: %d\n",len);
	int x;
	for(x =0 ;x < len;x++){
		ringBufS_put(&data_from_conn2,pData[x]);
	}
}

/*	<|CLIENT1 CONNECTED CALLBACK|> */
void connectCB1(void *arg) {
	os_printf("%s CONNECTED!!!\n",CONNECT_TO_SERVER1_BY_CLIENT1);
}

/*	<|CLIENT2 CONNECTED CALLBACK|> */
void connectCB2(void *arg) {
	os_printf("%s CONNECTED!!!\n",CONNECT_TO_SERVER2_BY_CLIENT2);
}

/*	<|NETPIE CONNECTED CALLBACK|> */
void onConnected(char *attribute, uint8_t* msg, uint16_t msglen) {
    os_printf("\nConnected with NECTPIE...\n"); 
    //microgear_subscribe(&mg, "/#");   
}

/*	<|NETPIE ON MESSAGE HANDLER CALLBACK|> */
void onMsghandler(char *topic, uint8_t* msg, uint16_t msglen) {
    uint16_t i;
    os_printf("incoming message --> %s : ",topic);
    for (i=0;i<msglen;i++) {
        dmsg_putchar((char)(msg[i]));
    }
    os_printf("\n");    
}

/* <|CLIENT1 ERROR CALLBACK|> */
void errorCB1(void *arg, sint8 err) {
	os_printf("We have an error: %d\n", err);
}

/* <|CLIENT2 ERROR CALLBACK|> */
void errorCB2(void *arg, sint8 err) {
	os_printf("We have an error: %d\n", err);
}

/*	<|CLIENT1 DISCONNECT CALLBACK|> */
void disconnectCB1(void *arg) {
	os_printf("Disconnected from the server(1)\n");
}

/*	<|CLIENT1 DISCONNECT CALLBACK|> */
void disconnectCB2(void *arg) {
	os_printf("Disconnected from the server(2)\n");
}

/* <|DNS callback for conn1 if there is no maping ip in cache|> */
void ResolveDNS_for_conn1( const char *name, ip_addr_t *ipaddr, void *arg ){
	os_printf("Can't find ip in cache system. Start to ask from DNS server\n");
	memcpy(conn1.proto.tcp->remote_ip, ipaddr, 4);
	os_printf("Connecting . . .\n");
	espconn_connect(&conn1);
}	

/* <|DNS callback for conn2 if there is no maping ip in cache|> */
void ResolveDNS_for_conn2( const char *name, ip_addr_t *ipaddr, void *arg ){
	os_printf("Can't find ip in cache system. Start to ask from DNS server\n");
	memcpy(conn2.proto.tcp->remote_ip, ipaddr, 4);
	os_printf("Connecting . . .\n");
	espconn_connect(&conn2);
}	



/**********************************************************************************************************************
*											 SERIAL COMMUNICATION PART 												  *
***********************************************************************************************************************/
/*<|Read Serial|>
 * 
 * 	Description: Compare data from serial for several purpose.
 *  Member function: <1> Setup Wifi
 * 					 <2> Check Status Wifi
 * 					 <3> Connect to server (For conn1,conn2)
 * 					 <4> Stop connection (For conn1,conn2)
 * 					 <5> Check status of connection
 * 					 <6> Print data to TCP socket
 * 					 <7> Read data from TCP socket
 * 					 <8> Set Token and Token secret for Netpie connection
 * 					 <9> Init microgear (Set KEY,SECRET,ALIAS)
 * 					 <10> Connect to Netpie
 * 					 <11> Disconnect from Netpie
 * 					 <12> Publish data to the topic
 * 					 <13> Subscribe the Topic
 *  	  			 <14> Change Alias name
 *   	  			 <15> Chat to the alias 
 * 					 
 * 					 
 */ 
void read_sr(void *pvParameters) {
	char message_sr[50] = "";
	int n;
	int param=0;
	while(1){
		n=0;
		while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){
			message_sr[n++] = xQueueHandleUart.param;
			message_sr[n] = '\0';			
			
			/*	<|SETUP WIFI|> */
			if(strcmp(message_sr,SETUPWIFI)== 0){ 
				ssid[0] = '\0';
				password[0] = '\0';
				message_sr[0] = '\0';
				n=0; 
				param =0;				
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 34){
						param++;
						n=0;
						switch (param){
							case 1: break;
							case 2: strcpy(ssid,message_sr);break;
							case 3: break;
							case 4: strcpy(password,message_sr);break;
						}
					}else{
						message_sr[n++] = xQueueHandleUart.param;
						message_sr[n]=0;
					}
					vTaskDelay( 10 / portTICK_RATE_MS);		
				}
				xSemaphoreGive(SetWifi);			
			}
			
			/*	<|CHECKING STATUS WIFI|> */
			if(strcmp(message_sr,CHECKSTATUS_WIFI)==0){
				checkstatus();
			}
			
			/*	<|CLIENT1 CONNECT TO SERVER|>	*/
			if(strcmp(message_sr,CONNECT_TO_SERVER1_BY_CLIENT1) == 0){		
				n=0;  				
				char host[15]="";
				int port;
				message_sr[0] = '\0';
				param=0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 34 || xQueueHandleUart.param == 13 || xQueueHandleUart.param == 44 ){
						param++;
						n=0;
						switch (param){
							case 1:break;
							case 2:strcpy(host,message_sr);break;
							case 3:break;
							case 4: 
								port = atoi(message_sr);		
								conn1.proto.tcp->remote_port = port;
								if(espconn_gethostbyname(&conn1, host, &HostResolve_Ip1, ResolveDNS_for_conn1) == ESPCONN_OK){
									memcpy(conn1.proto.tcp->remote_ip, &HostResolve_Ip1, 4);
									espconn_connect(&conn1);
								}
							break;
						}
					}else{
						message_sr[n++] = xQueueHandleUart.param;
						message_sr[n]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
			}
			
			/*	<|CLIENT2 CONNECT TO SERVER|>	*/
			if(strcmp(message_sr,CONNECT_TO_SERVER2_BY_CLIENT2) == 0){
				n=0;  					
				char host[15]="";
				int port;
				message_sr[0] = '\0';
				param = 0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 34 || xQueueHandleUart.param == 13 || xQueueHandleUart.param == 44){
						param++;
						n=0;
						switch (param){
							case 1: break;
							case 2: strcpy(host,message_sr);break;
							case 3: break;
							case 4: 
							port = atoi(message_sr);		
							conn2.proto.tcp->remote_port = port;
							if(espconn_gethostbyname(&conn2, host, &HostResolve_Ip2, ResolveDNS_for_conn2) == ESPCONN_OK){
								memcpy(conn2.proto.tcp->remote_ip, &HostResolve_Ip2, 4);
								espconn_connect(&conn2);
							}
							break;
						}
					}else{
						message_sr[n++] = xQueueHandleUart.param;
						message_sr[n]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
			}
			
			/*	<|CLIENT1 DISCONNECT|>	*/
			if(strcmp(message_sr,DISCONNECT_FROM_SERVER1) == 0){
				espconn_disconnect(&conn1);
			}
			
			/*	<|CLIENT2 DISCONNECT|>	*/
			if(strcmp(message_sr,DISCONNECT_FROM_SERVER2) == 0){
				espconn_disconnect(&conn2);
			}
			
			/*	<|CHECK STATUS OF CLIENT 1|>	*/
			if(strcmp(message_sr,CHECKSTATUS_CLIENT1) == 0){
				switch(conn1.state){
					case ESPCONN_CONNECT: os_printf("CONNECTED"); break;
					case ESPCONN_NONE: os_printf("NONE"); break;
					case ESPCONN_LISTEN: os_printf("LISTENING"); break;
					case ESPCONN_WAIT: os_printf("WAITING"); break;
					case ESPCONN_WRITE: os_printf("WRITING"); break;
					case ESPCONN_READ: os_printf("READING"); break;
					case ESPCONN_CLOSE: os_printf("NOT CONNECTED"); break;
				}
			}
			
			/*	<|CHECK STATUS OF CLIENT 2|>	*/
			if(strcmp(message_sr,CHECKSTATUS_CLIENT2) == 0){
				switch(conn2.state){
					case ESPCONN_CONNECT: os_printf("CONNECTED"); break;
					case ESPCONN_NONE: os_printf("NONE"); break;
					case ESPCONN_LISTEN: os_printf("LISTENING"); break;
					case ESPCONN_WAIT: os_printf("WAITING"); break;
					case ESPCONN_WRITE: os_printf("WRITING"); break;
					case ESPCONN_READ: os_printf("READING"); break;
					case ESPCONN_CLOSE: os_printf("CLOSED"); break;
				}
			}
			
			/*	<|CLIENT1 PRINT DATA TO SERVER|>
			 * 	MAXIMUM DATA TO SEND: 500 BYTE
			 * 
			 * 	*/
			if(strcmp(message_sr,PRINT_TO_SERVER1)==0){
				char *data_to_send = (char*) malloc (sizeof(char)* MAX_SIZE_TCP_PRINT);
				n=0;
				param =0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 34){
						param++;
						switch (param){
							case 1:break;
							case 2:
								os_printf("Data: %s\n",data_to_send);
								if(!(espconn_send(&conn1,data_to_send,n))){
									os_printf("Send complete \n");				
								}else{
									os_printf("Can't send\n");
								}
								free(data_to_send);
								n=0;
								break;
						}
					}else{
						data_to_send[n++] = xQueueHandleUart.param;
						data_to_send[n]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
			}
			
			/*	<|CLIENT2 PRINT DATA TO SERVER|>	
			 * 	MAXIMUM DATA TO SEND: 500 BYTE
			 * 	
			 * */
			if(strcmp(message_sr,PRINT_TO_SERVER2)==0){
				char *data_to_send = (char*) malloc (sizeof(char)* MAX_SIZE_TCP_PRINT);
				n=0;
				param =0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 34){
						param++;	
						switch (param){
							case 1:break;
							case 2:
								os_printf("Data: %s\n",data_to_send);
								if(!(espconn_send(&conn2,data_to_send,n))){
									os_printf("Send complete \n");				
								}else{
									os_printf("Can't send\n");
								}
								free(data_to_send);
								n=0;
								break;
						}
					}else{
						data_to_send[n++] = xQueueHandleUart.param;
						data_to_send[n]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
			}
			
			/* <|READ DATA FROM CLIENT1|> */
			if(strcmp(message_sr,READ_DATA_FROM_CLIENT1_BUFFER)==0){
				char num_to_send[5];
				int for_loop_count,x;
				n=0;
				param = 0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 13){
						param++;
						switch (param){
							case 1:
								for_loop_count = atoi(num_to_send);
								for(x = 0;x<for_loop_count;x++){   		
									if(!ringBufS_empty(&data_from_conn1)){
										os_printf("%c",ringBufS_get(&data_from_conn1));
									}
								}
							n=0;	
							break;
						}
					}else{
						num_to_send[n++] = xQueueHandleUart.param;
						num_to_send[n]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
			}
			
			/* <|READ DATA FROM CLIENT2|> */
			if(strcmp(message_sr,READ_DATA_FROM_CLIENT2_BUFFER)==0){
				char num_to_send[5];
				int for_loop_count,x;
				n=0;
				param =0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 13){
						param++;
						switch (param){
							case 1:
								for_loop_count = atoi(num_to_send);
								for(x = 0;x<for_loop_count;x++){ 
									if(!ringBufS_empty(&data_from_conn2)){  		
										os_printf("%c",ringBufS_get(&data_from_conn2));
									}
								}
								n=0;					
								break;
						}
					}else{
						num_to_send[n++] = xQueueHandleUart.param;
						num_to_send[n]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
			}
				
			/* <|CLIENT SECURE CONNECTION CONNECT|> */
			if(strcmp(message_sr,SECURE_CONNECT)==0){
				
			}
			/* <|CLIENT SECURE CONNECTION CONNECTED|> */
			if(strcmp(message_sr,SECURE_CONNECTED)==0){
			}
			/* <|CLIENT SECURE VARIFY FOOTPRINT|> */
			if(strcmp(message_sr,SECURE_VERIFY)==0){
			}
			/* <|CLIENT SECURE READ|> */
			if(strcmp(message_sr,SECURE_READ)==0){
			}
			/* <|CLIENT SECURE PRINT|> */
			if(strcmp(message_sr,SECURE_PRINT)==0){
			}	
			
			/*	<|MICROGEAR CONNECT|> */		
			if(strcmp(message_sr,CONNECT_TO_NETPIE)==0){	
				microgear_connect(&mg,appid);
				microgear_on(&mg, CONNECTED, onConnected);
				microgear_on(&mg, MESSAGE, onMsghandler);
			}
			
			/*	<|MICROGEAR DISCONNECT|> */
			if(strcmp(message_sr,DISCONNECT_FROM_NETPIE)==0){
				microgear_disconnect(&mg);	
			}
				
			 /*	<|MICROGEAR SETUP TOKEN|> */
			if(strcmp(message_sr,SETUP_TOKEN)==0){
				n=0;
				tokensecret[0] = '\0';
				token[0] = '\0';
				param=0;
				message_sr[0] = '\0';
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 34){
						param++;
						n=0;
						switch (param){
							case 1: break;
							case 2:	strcpy(token,message_sr);break;
							case 3: break;
							case 4: strcpy(tokensecret,message_sr);break;
						}
					}else{
						message_sr[n++] = xQueueHandleUart.param;
						message_sr[n]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
				os_printf("Token: %s\nToken Secret: %s\n",token,tokensecret);
				microgear_setToken(&mg, token, tokensecret, NULL);
			}

			//~ /*	<|MICROGEAR INIT (SETUP APPID,KEY,SECRET,ALIAS)|> */
			if(strcmp(message_sr,INIT_MICROGEAR)==0){
				n=0;
				param=0;
				appid[0] = '\0';
				key[0] = '\0';
				secret[0] = '\0';
				alias[0] = '\0';
				message_sr[0]= '\0';
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){
					if(xQueueHandleUart.param == 34 ){
						param++;
						n=0;
						switch (param){
							case 1: break;
							case 2: strcpy(appid,message_sr);break;
							case 3: break;
							case 4: strcpy(key,message_sr);break;
							case 5: break;
							case 6: strcpy(secret,message_sr);break;
							case 7: break;
							case 8: strcpy(alias,message_sr);break;
						}
					}else{
						message_sr[n++] = xQueueHandleUart.param;
						message_sr[n]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
				os_printf("Appid: %s\nKey: %s\nSecret: %s\nAlias: %s\n",appid,key,secret,alias);	
				microgear_init(&mg, key, secret, alias);
			}
			
			/*	<|MICROGEAR SET ALIAS|> */
			if(strcmp(message_sr,SET_ALIAS_NAME)==0){
				n=0;
				message_sr[0] = '\0';
				param = 0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 34){ 
						param++;  												
						n=0;
						switch (param){
							case 1: break;
							case 2:	
								os_printf("Set alias name to: %s\n",message_sr);
								microgear_setAlias(&mg, message_sr);	
								break;
						}										
					}else{
						message_sr[n++] = xQueueHandleUart.param;
						message_sr[n]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
			}
			
			/*	<|MICROGEAR PUBLISH|> */
			//should not use white space in data_pub
			if(strcmp(message_sr,PUBLISH)==0){
				n=0;
				int param =0;
				char topic[MAX_SIZE_TOPIC],data_pub[MAX_SIZE_PUBLISH];
				message_sr[0] = '\0'; 
				topic[0] = '\0';
				data_pub[0] = '\0';
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){
					if(xQueueHandleUart.param == 44 || xQueueHandleUart.param == 34 ){
						param++;
						n=0;
						switch (param){
							case 1: break;
							case 2: strcpy(topic,message_sr);break;
							case 3: break;
							case 4:	strcpy(data_pub,message_sr);break;
						}
					}else{
						message_sr[n++] = xQueueHandleUart.param;
						message_sr[n]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
				microgear_publish(&mg, topic, data_pub, NULL);
			}
			
			/*	<|MICROGEAR SUBSCRIBE|> */
			if(strcmp(message_sr,SUBSCRIBE)==0){
				n=0;
				message_sr[0] = '\0';
				param=0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 34){
						param++;
						n=0;
						switch (param){
							case 1: break;
							case 2: 
								os_printf("Subscribe topic: %s\n",message_sr);
								microgear_subscribe(&mg, message_sr);
								break;
						}												
					}else{
						message_sr[n++] = xQueueHandleUart.param;
						message_sr[n]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
			}
			
			/*	<|MICROGEAR UNSUBSCRIBE|> */
			if(strcmp(message_sr,UNSUBSCRIBE)==0){
				n=0;
				message_sr[0] = '\0';
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 34){
						n=0;
						param++;
						switch(param) {
							case 1: break;
							case 2: 
								os_printf("Unsubscribe topic: %s\n",message_sr);
								microgear_unsubscribe(&mg, message_sr);		
								break;
						}										
					}else{
						message_sr[n++] = xQueueHandleUart.param;
						message_sr[n]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
			}
			
			/*	<|MICROGEAR CHAT|> */
			if(strcmp(message_sr,CHAT)==0){
				n=0;
				char alias[50],payload[MAX_SIZE_PUBLISH];
				message_sr[0] = '\0';
				payload[0] = '\0';
				alias[0] = '\0';
				int param =0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){
					if(xQueueHandleUart.param == 34){
						param++;
						n=0;
						switch (param){
							case 1: break;
							case 2: strcpy(alias,message_sr);break;
							case 3: break;
							case 4: strcpy(payload,message_sr);break;
						}
					}else{
						message_sr[n++] = xQueueHandleUart.param;
						message_sr[n]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
				microgear_chat(&mg, alias, payload);
			}
			
			
			vTaskDelay(10 / portTICK_RATE_MS);
		}
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
    vTaskDelete( NULL );
}




/**********************************************************************************************************************
*											 USER INIT PART           												  *
***********************************************************************************************************************/
void ICACHE_FLASH_ATTR user_init(void) {
   
    /* Uart init */
    uart_param_t para;
	para.uart_baud_rate = UART_BAUD_RATE_115200;
	para.uart_xfer_bit = UART_XFER_8_BIT;
	para.uart_parity_mode = UART_PARITY_NONE;
	para.uart_stop_bit = UART_1_STOP_BIT;
	para.uart_flow_ctrl = UART_NONE_FLOW_CTRL;
	uart0_init(&para);
	
	/* Semaphore init */
	vSemaphoreCreateBinary(WifiReady);
	xSemaphoreTake(WifiReady, 0);              // take the default semaphore
	vSemaphoreCreateBinary(SetWifi);
	xSemaphoreTake(SetWifi, 0);
	microgear_setWifiSemaphore(&WifiReady);
	
	/* tcp1 init */
	espconn_init();								//HAVE TO BE CALL UNLESS THE CONN WILL FAIL
	conn1.type = ESPCONN_TCP;
	conn1.state = ESPCONN_NONE;
	conn1.proto.tcp = &tcp1;
	conn1.proto.tcp->local_port=espconn_port();
	conn2.type = ESPCONN_TCP;
	conn2.state = ESPCONN_NONE;
	conn2.proto.tcp = &tcp2;
	conn2.proto.tcp->local_port=espconn_port();
	
	//Register callback function for espconn1
	espconn_regist_connectcb(&conn1,connectCB1);
	espconn_regist_reconcb(&conn1, errorCB1);
	espconn_regist_disconcb(&conn1,disconnectCB1);
	espconn_regist_recvcb(&conn1,recv_cb1);
	
	//Register callback function for espconn2
	espconn_regist_connectcb(&conn2,connectCB2);
	espconn_regist_reconcb(&conn2, errorCB2);
	espconn_regist_disconcb(&conn2,disconnectCB2);
	espconn_regist_recvcb(&conn2,recv_cb2);
	
	//Ringbuf init
	ringBufS_init(&data_from_conn1);
	ringBufS_init(&data_from_conn2);
	
	//~ microgear_init(&mg, KEY, SECRET, ALIAS);
	//~ microgear_setToken(&mg, TOKEN, TOKENSECRET, NULL);
	//~ microgear_connect(&mg,APPID);
	//~ microgear_on(&mg, CONNECTED, onConnected);
	//~ microgear_on(&mg, MESSAGE, onMsghandler);
	

 
	/* Task init */
	xTaskCreate(wifi_task, "wifi_task", 254,NULL,tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(give_wifi_semaphore, "give_wifiready_semaphore", 254,NULL,tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(read_sr, "read_sr", 254,NULL,tskIDLE_PRIORITY + 1, NULL);
	
   
}
