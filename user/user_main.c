
#include "user_main.h"
	

/**********************************************************************************************************************
*											 CALLBACK FUNCTION PART  												  *
***********************************************************************************************************************/


/* <|DATA RECEIVED CALLBACK (FOR CLIENT 1)|> */ 
void recv_cb1(void *arg, char *pData, unsigned short len){
	int i;
	for(i=0;i<len;i++)client1_buf->add(client1_buf,(pData+i));
}


/* <|DATA RECEIVED CALLBACK (FRO CLIENT 2)|> */ 
void recv_cb2(void *arg, char *pData, unsigned short len){
	int i;
	for(i=0;i<len;i++)client2_buf->add(client2_buf,(pData+i));
}

/*	<|CLIENT1 CONNECTED CALLBACK|> */
void connectCB1(void *arg) {
	os_printf("OK\n");
}

/*	<|CLIENT2 CONNECTED CALLBACK|> */
void connectCB2(void *arg) {
	os_printf("OK\n");
}

/*	<|NETPIE CONNECTED CALLBACK|> */
void onConnected(char *attribute, uint8_t* msg, uint16_t msglen) {
    os_printf("OK\n"); 
}

/*	<|NETPIE ON MESSAGE HANDLER CALLBACK|> */
void onMsghandler(char *topic, uint8_t* msg, uint16_t msglen) {
	memset(&str,0,sizeof(struct MgStruct));
	memcpy(str.msg,msg,msglen);
	strcpy(str.topic,topic);
	str.msglen = msglen;
	mg_buf->add(mg_buf,&str);
    //os_printf("incoming message --> %s : %s\n",str.topic,str.msg);
}

/* <|CLIENT1 ERROR CALLBACK|> */
void errorCB1(void *arg, sint8 err) {
	os_printf("Error: %d\n", err);
}

/* <|CLIENT2 ERROR CALLBACK|> */
void errorCB2(void *arg, sint8 err) {
	os_printf("Error: %d\n", err);
}

/*	<|CLIENT1 DISCONNECT CALLBACK|> */
void disconnectCB1(void *arg) {
	os_printf("CLIENT1 DISCONN \n");
}

/*	<|CLIENT1 DISCONNECT CALLBACK|> */
void disconnectCB2(void *arg) {
	os_printf("CLIENT2 DISCONN\n");
}

/* <|DNS callback for conn1 if there is no maping ip in cache|> */
void ResolveDNS_for_conn1( const char *name, ip_addr_t *ipaddr, void *arg ){
	os_printf("Can't find ip in cache system. Asking from DNS server\n");
	memcpy(conn1.proto.tcp->remote_ip, ipaddr, 4);
	os_printf("Connecting . . .\n");
	espconn_connect(&conn1);
}	

/* <|DNS callback for conn2 if there is no maping ip in cache|> */
void ResolveDNS_for_conn2( const char *name, ip_addr_t *ipaddr, void *arg ){
	os_printf("Can't find ip in cache system. Asking from DNS server\n");
	memcpy(conn2.proto.tcp->remote_ip, ipaddr, 4);
	os_printf("Connecting . . .\n");
	espconn_connect(&conn2);
}	



/**********************************************************************************************************************
*											 SERIAL COMMUNICATION PART 												 																					 *
***********************************************************************************************************************/
/*<|Read Serial|>
 * 
 * 	Description: Compare data from serial for several purpose.
 *		@Available clients: 2 (Use client1 and client2)   
 *		@Member functions: 
 * 					 <1> Setup Wifi
 * 					 <2> Check Status Wifi
 * 					 <3> Connect to server 
 * 					 <4> Stop connection 
 * 					 <5> Check status of connection
 * 					 <6> Print data to TCP socket 
 * 					 <7> Read data from TCP socket and print to serial without header [Standalone mode]
 * 					 <8> Read data from TCP socket and print to serial with header [Arduino mode]
 * 					 <9> Set Token and Token secret for Netpie connection
 * 					 <10> Init microgear (Set KEY,SECRET,ALIAS)
 * 					 <11> Connect to Netpie
 * 					 <12> Disconnect from Netpie
 * 					 <13> Publish data to the topic
 * 					 <14> Subscribe the Topic
 *  	  			     <15> Change Alias name
 *   	  			     <16> Chat to the alias 
 * 					 
 * 					 
 */ 
void read_sr(void *pvParameters) {
	while(1){
		message_index=0;
		while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){
			message_sr[message_index++] = xQueueHandleUart.param;
			message_sr[message_index] = '\0';			
			
			/*	<|SETUP WIFI|> */
			if(strcmp(message_sr,SETUPWIFI)== 0){ 
				ssid[0] = '\0';
				password[0] = '\0';
				message_sr[0] = '\0';
				message_index=0; 
				param =0;				
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 34 || xQueueHandleUart.param == 44){
						param++;
						message_index=0;
						switch (param){
							case 1: break;
							case 2: strcpy(ssid,message_sr);break;
							case 3: break;
							case 4: break;
							case 5: strcpy(password,message_sr);break;
						}
					}else{
						message_sr[message_index++] = xQueueHandleUart.param;
						message_sr[message_index]=0;
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
					message_index=0;  				
					char host[15]="";
					int port;
					message_sr[0] = '\0';
					param=0;
					while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
						if(xQueueHandleUart.param == 34 || xQueueHandleUart.param == 13 || xQueueHandleUart.param == 44 ){
							param++;
							message_index=0;
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
							message_sr[message_index++] = xQueueHandleUart.param;
							message_sr[message_index]=0;
						}
						vTaskDelay(10/ portTICK_RATE_MS);		
					}
			}
			
			/*	<|CLIENT2 CONNECT TO SERVER|>	*/
			if(strcmp(message_sr,CONNECT_TO_SERVER2_BY_CLIENT2) == 0){
				message_index=0;  					
				char host[15]="";
				int port;
				message_sr[0] = '\0';
				param = 0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 34 || xQueueHandleUart.param == 13 || xQueueHandleUart.param == 44){
						param++;
						message_index=0;
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
						message_sr[message_index++] = xQueueHandleUart.param;
						message_sr[message_index]=0;
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
					case ESPCONN_CONNECT: os_printf("%s OK\n", CHECKSTATUS_CLIENT1); break;
					case ESPCONN_NONE: os_printf("NONE\n"); break;
					case ESPCONN_LISTEN: os_printf("LISTENING\n"); break;
					case ESPCONN_WAIT: os_printf("WAITING\n"); break;
					case ESPCONN_WRITE: os_printf("WRITING\n"); break;
					case ESPCONN_READ: os_printf("READING\n"); break;
					case ESPCONN_CLOSE: os_printf("NOT CONNECTED\n"); break;
				}
			}
			
			/*	<|CHECK STATUS OF CLIENT 2|>	*/
			if(strcmp(message_sr,CHECKSTATUS_CLIENT2) == 0){
				switch(conn2.state){
					case ESPCONN_CONNECT: os_printf("%s OK\n",CHECKSTATUS_CLIENT2); break;
					case ESPCONN_NONE: os_printf("NONE\n"); break;
					case ESPCONN_LISTEN: os_printf("LISTENING\n"); break;
					case ESPCONN_WAIT: os_printf("WAITING\n"); break;
					case ESPCONN_WRITE: os_printf("WRITING\n"); break;
					case ESPCONN_READ: os_printf("READING\n"); break;
					case ESPCONN_CLOSE: os_printf("CLOSED\n"); break;
				}
			}
			
			/*	<|CLIENT1 PRINT DATA TO SERVER|>
			 * 	@maxsize data: 1024 bytes
			 * 
			 * 	*/
			if(strcmp(message_sr,PRINT_TO_SERVER1)==0){
				//~ char data_to_send[MAX_SIZE_TCP_PRINT];
				message_index=0;
				param =0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 34){
						param++;
						switch (param){
							case 1:break;
							case 2:
								if(!(espconn_send(&conn1,send_buf1,message_index))){
									os_printf("OK\n");				
								}else{
									os_printf("ERROR\n");
								}
								message_index=0;
								break;
						}
					}else{
						send_buf1[message_index++] = xQueueHandleUart.param;
						send_buf1[message_index]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
			}
			
			/*	<|CLIENT2 PRINT DATA TO SERVER|>	
			 * @maxsize data: 1024 bytes
			 * 	
			 * */
			if(strcmp(message_sr,PRINT_TO_SERVER2)==0){
				//~ char *data_to_send = (char*) malloc (sizeof(char)* MAX_SIZE_TCP_PRINT);
				message_index=0;
				param =0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 34){
						param++;	
						switch (param){
							case 1:break;
							case 2:
								if(!(espconn_send(&conn2,send_buf2,message_index))){
									os_printf("OK\n");				
								}else{
									os_printf("ERROR\n");
								}
								//~ free(send_buf2);
								message_index=0;
								break;
						}
					}else{
						send_buf2[message_index++] = xQueueHandleUart.param;
						send_buf2[message_index]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
			}
			
			/*	<|READ DATA FROM CLIENT1 BUFFER|> 
			 *	@Print data from client1 buffer to serial without header  		
			 * 
			 */
			if(strcmp(message_sr,READ_DATA_FROM_CLIENT1_BUFFER)==0){
				char c;
				int sending_bytes,x;
				message_index=0;
				param = 0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 13){
						param++;
						switch (param){
							case 1:
								sending_bytes = atoi(message_sr);
								int buffer_ele = client1_buf->numElements(client1_buf);
								if(buffer_ele != 0){
										if(buffer_ele > sending_bytes){
													for(x = 0;x<sending_bytes;x++){   					
															client1_buf->pull(client1_buf,&c);
															os_printf("%c",c);
													}
										}else {
													for(x = 0;x<buffer_ele;x++){   					
															client1_buf->pull(client1_buf,&c);
															os_printf("%c",c);
													}
										}
								}	
							message_index=0;	
							break;
						}
					}else{
						message_sr[message_index++] = xQueueHandleUart.param;
						message_sr[message_index]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
			}
			
			/*	<|READ DATA FROM CLIENT1 BUFFER TO ARDUINO|> 
			 *	@Print data from client1 buffer to serial with header  		
			 * 
			 */
			if(strcmp(message_sr,READ_DATA_FROM_CLIENT1_BUFFER_TO_ARDUINO_LIB)==0){
				char header,c;
				int sending_bytes,x;
				message_index=0;
				param = 0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 13){
						param++;
						switch (param){
							case 1:
							sending_bytes = atoi(message_sr);
								int buffer_ele = client1_buf->numElements(client1_buf);
								if(buffer_ele != 0){
										if(buffer_ele > sending_bytes){
													 os_printf("%c",32| sending_bytes);
													for(x = 0;x<sending_bytes;x++){   					
															client1_buf->pull(client1_buf,&c);
															os_printf("%c",c);
													}
										}else {
													os_printf("%c",32| buffer_ele);				
													for(x = 0;x<buffer_ele;x++){   																
															client1_buf->pull(client1_buf,&c);
															os_printf("%c",c);
													}
										}
								}	
							message_index=0;						
							break;
						}
					}else{
						message_sr[message_index++] = xQueueHandleUart.param;
						message_sr[message_index]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
			}
			
			/*	<|READ DATA FROM CLIENT2 BUFFER|> 
			 *	@Print data from client2 buffer to serial without header  		
			 * 
			 */
			if(strcmp(message_sr,READ_DATA_FROM_CLIENT2_BUFFER)==0){
				char c;
				int sending_bytes,x;
				message_index=0;
				param =0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 13){
						param++;
						switch (param){
							case 1:
								sending_bytes = atoi(message_sr);
									int buffer_ele = client2_buf->numElements(client2_buf);
									if(buffer_ele != 0){
											if(buffer_ele > sending_bytes){
														for(x = 0;x<sending_bytes;x++){   					
																client2_buf->pull(client2_buf,&c);
																os_printf("%c",c);
														}
											}else {
														for(x = 0;x<buffer_ele;x++){   					
																client2_buf->pull(client2_buf,&c);
																os_printf("%c",c);
														}
											}
									}
								message_index=0;					
								break;
						}
					}else{
						message_sr[message_index++] = xQueueHandleUart.param;
						message_sr[message_index]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
			}
			
			/*	<|READ DATA FROM CLIENT2 BUFFER TO ARDUINO|>
			 * @Print data from client2 buffer to serial with header  		
			 * 
			 */
			if(strcmp(message_sr,READ_DATA_FROM_CLIENT2_BUFFER_TO_ARDUINO_LIB)==0){
				char c;
				int sending_bytes,x;
				message_index=0;
				param =0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 13){
						param++;
						switch (param){
							case 1:
							sending_bytes = atoi(message_sr);
								int buffer_ele = client2_buf->numElements(client2_buf);
								if(buffer_ele != 0){
										if(buffer_ele > sending_bytes){
													 os_printf("%c",64| sending_bytes);
													for(x = 0;x<sending_bytes;x++){   					
															client2_buf->pull(client2_buf,&c);
															os_printf("%c",c);
													}
										}else {
													os_printf("%c",64| buffer_ele);				
													for(x = 0;x<buffer_ele;x++){   																
															client2_buf->pull(client2_buf,&c);
															os_printf("%c",c);
													}
										}
								}							
								break;
						}
					}else{
						message_sr[message_index++] = xQueueHandleUart.param;
						message_sr[message_index]=0;
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
				message_index=0;
				tokensecret[0] = '\0';
				token[0] = '\0';
				param=0;
				message_sr[0] = '\0';
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 34 || xQueueHandleUart.param == 44){
						param++;
						message_index=0;
						switch (param){
							case 1: break;
							case 2:	strcpy(token,message_sr);break;
							case 3: break;
							case 4: break;
							case 5: 
								strcpy(tokensecret,message_sr);
								os_printf("Token: %s\nToken Secret: %s\n",token,tokensecret);
								microgear_setToken(&mg, token, tokensecret, NULL);
								break;
						}
					}else{
						message_sr[message_index++] = xQueueHandleUart.param;
						message_sr[message_index]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
				
			}

			//~ /*	<|MICROGEAR INIT (SETUP APPID,KEY,SECRET,ALIAS)|> */
			if(strcmp(message_sr,INIT_MICROGEAR)==0){
				message_index=0;
				param=0;
				appid[0] = '\0';
				key[0] = '\0';
				secret[0] = '\0';
				alias[0] = '\0';
				message_sr[0]= '\0';
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){
					if(xQueueHandleUart.param == 34 || xQueueHandleUart.param == 44 ){
						param++;
						message_index=0;
						switch (param){
							case 1: break;
							case 2: strcpy(appid,message_sr);break;
							case 3: break;
							case 4: break;
							case 5: strcpy(key,message_sr);break;
							case 6: break;
							case 7: break;
							case 8: strcpy(secret,message_sr);break;
							case 9: break;
							case 10: break;
							case 11:
								strcpy(alias,message_sr);
								os_printf("Appid: %s\nKey: %s\nSecret: %s\nAlias: %s\n",appid,key,secret,alias);	
								microgear_init(&mg, key, secret, alias); 
								break;
						}
					}else{
						message_sr[message_index++] = xQueueHandleUart.param;
						message_sr[message_index]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
			}
			
			/*	<|MICROGEAR SET ALIAS|> */
			if(strcmp(message_sr,SET_ALIAS_NAME)==0){
				message_index=0;
				message_sr[0] = '\0';
				param = 0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 34){ 
						param++;  												
						message_index=0;
						switch (param){
							case 1: break;
							case 2:	
								os_printf("Set alias name to: %s\n",message_sr);
								microgear_setAlias(&mg, message_sr);	
								break;
						}										
					}else{
						message_sr[message_index++] = xQueueHandleUart.param;
						message_sr[message_index]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
			}
			
			/*	<|MICROGEAR PUBLISH|> */
			//should not use white space in data_pub
			if(strcmp(message_sr,PUBLISH)==0){
				message_index=0;
				param =0;
				char topic[MAX_SIZE_TOPIC],data_pub[MAX_SIZE_PUBLISH];
				message_sr[0] = '\0'; 
				topic[0] = '\0';
				data_pub[0] = '\0';
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){
					if(xQueueHandleUart.param == 44 || xQueueHandleUart.param == 34 ){
						param++;
						message_index=0;
						switch (param){
							case 1: break;
							case 2: strcpy(topic,message_sr);break;
							case 3: break;
							case 4: break;
							case 5:	
								strcpy(data_pub,message_sr);
								os_printf("Topis is: %s \ndata is: %s\n",topic,message_sr);
								microgear_publish(&mg, topic, data_pub, NULL);
								break;
						}
					}else{
						message_sr[message_index++] = xQueueHandleUart.param;
						message_sr[message_index]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
				
			}
			
			/*	<|MICROGEAR SUBSCRIBE|> */
			if(strcmp(message_sr,SUBSCRIBE)==0){
				message_index=0;
				message_sr[0] = '\0';
				param=0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 34){
						param++;
						message_index=0;
						switch (param){
							case 1: break;
							case 2: 
								os_printf("Subscribe topic: %s\n",message_sr);
								microgear_subscribe(&mg, message_sr);
								break;
						}												
					}else{
						message_sr[message_index++] = xQueueHandleUart.param;
						message_sr[message_index]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
			}
			
			/*	<|MICROGEAR UNSUBSCRIBE|> */
			if(strcmp(message_sr,UNSUBSCRIBE)==0){
				message_index=0;
				message_sr[0] = '\0';
				param =0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){	
					if(xQueueHandleUart.param == 34){
						message_index=0;
						param++;
						switch(param) {
							case 1: break;
							case 2: 
								os_printf("Unsubscribe topic: %s\n",message_sr);
								microgear_unsubscribe(&mg, message_sr);		
								break;
						}										
					}else{
						message_sr[message_index++] = xQueueHandleUart.param;
						message_sr[message_index]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
			}
			
			/*	<|MICROGEAR CHAT|> */
			if(strcmp(message_sr,CHAT)==0){
				message_index=0;
				char alias[50],payload[MAX_SIZE_PUBLISH];
				message_sr[0] = '\0';
				payload[0] = '\0';
				alias[0] = '\0';
				param =0;
				while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){
					if(xQueueHandleUart.param == 34 || xQueueHandleUart.param == 44){
						param++;
						message_index=0;
						switch (param){
							case 1: break;
							case 2: strcpy(alias,message_sr);break;
							case 3: break;
							case 4: break;
							case 5: strcpy(payload,message_sr);break;
						}
					}else{
						message_sr[message_index++] = xQueueHandleUart.param;
						message_sr[message_index]=0;
					}
					vTaskDelay(10/ portTICK_RATE_MS);		
				}
				microgear_chat(&mg, alias, payload);
			}	
			
			/* 	<| PULL MESSAGE TO SERIAL FROM MICROGEAR BUFFER |>	*/
			if(strcmp(message_sr,PULL_MESSAGE)==0){				
				int buffer_ele = mg_buf->numElements(mg_buf);
				if(buffer_ele != 0){
					mg_buf->pull(mg_buf,&str_pull);
					os_printf("Topic is: %s\n",str_pull.topic);
					os_printf("Msg is: %s\n",str_pull.msg);
					os_printf("Size is: %d\n",str_pull.msglen);					
				}							
			}																			
			vTaskDelay(10 / portTICK_RATE_MS);
		}
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
    vTaskDelete( NULL );
}




/**********************************************************************************************************************
*											 USER INIT PART           																																					  *
***********************************************************************************************************************/
void ICACHE_FLASH_ATTR user_init(void) {
   
    //////////* Uart init *//////////
    uart_param_t para;
	para.uart_baud_rate = UART_BAUD_RATE_9600;
	para.uart_xfer_bit = UART_XFER_8_BIT;
	para.uart_parity_mode = UART_PARITY_NONE;
	para.uart_stop_bit = UART_1_STOP_BIT;
	para.uart_flow_ctrl = UART_NONE_FLOW_CTRL;
	uart0_init(&para);
	
	
	//////////* Semaphore init *//////////
	vSemaphoreCreateBinary(WifiReady);
	xSemaphoreTake(WifiReady, 0);              // take the default semaphore
	vSemaphoreCreateBinary(SetWifi);
	xSemaphoreTake(SetWifi, 0);
	microgear_setWifiSemaphore(&WifiReady);
	
	
	//////////* tcp1 init *//////////
	espconn_init();								//HAVE TO BE CALL UNLESS THE CONN WILL FAIL
	conn1.type = ESPCONN_TCP;
	conn1.state = ESPCONN_NONE;
	conn1.proto.tcp = &tcp1;
	conn1.proto.tcp->local_port=espconn_port();
	conn2.type = ESPCONN_TCP;
	conn2.state = ESPCONN_NONE;
	conn2.proto.tcp = &tcp2;
	conn2.proto.tcp->local_port=espconn_port();
	
	
	/////////* Register callback function for espconn1 */////////
	espconn_regist_connectcb(&conn1,connectCB1);
	espconn_regist_reconcb(&conn1, errorCB1);
	espconn_regist_disconcb(&conn1,disconnectCB1);
	espconn_regist_recvcb(&conn1,recv_cb1);
	
	
	/////////* Register callback function for espconn2 */////////
	espconn_regist_connectcb(&conn2,connectCB2);
	espconn_regist_reconcb(&conn2, errorCB2);
	espconn_regist_disconcb(&conn2,disconnectCB2);
	espconn_regist_recvcb(&conn2,recv_cb2);
	
	
	
	/* <| FOR DEBUGGING PURPOSE ONLY  |> */
	//~ microgear_init(&mg, "YhtHPvlmMxL5yJB", "YphWgyUI31q8sEMu6qtNrIPn1", "Light_control"); 
	//~ microgear_setToken(&mg, "RKJy30tYkUKoXa7G", "oWfxRNqRaXdwffVdFvuU27qJqr1KNpIy", NULL);
	//~ microgear_connect(&mg,"HelloNetpie1");
	//~ microgear_on(&mg, CONNECTED, onConnected);
	//~ microgear_on(&mg, MESSAGE, onMsghandler);

	
	
	//////////* Task init *//////////////
	xTaskCreate(wifi_task, "wifi_task", 254,NULL,tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(give_wifi_semaphore, "give_wifiready_semaphore", 254,NULL,tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(read_sr, "read_sr", 254,NULL,tskIDLE_PRIORITY + 1, NULL);
	
	
	//////////* Ring buf init *//////////
	client1_buf = (RingBuf *)RingBuf_new(1, 1024);
	client2_buf = (RingBuf *)RingBuf_new(1, 1024);
	mg_buf = RingBuf_new(sizeof(struct MgStruct ), 20);
	
   
}
