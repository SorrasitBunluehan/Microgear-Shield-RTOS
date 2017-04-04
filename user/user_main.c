
#include "user_main.h"

	

/**********************************************************************************************************************
*											 CALLBACK FUNCTION PART  												  *
***********************************************************************************************************************/


/* <|DATA RECEIVED CALLBACK (FOR CLIENT 1)|> */ 
void recv_cb1(void *arg, char *pData, unsigned short len){
	int i;
	if(push_mode == 1) {
		os_printf("Received data from server 1");
	}
	for(i=0;i<len;i++)client1_buf->add(client1_buf,(pData+i));
}


/* <|DATA RECEIVED CALLBACK (FRO CLIENT 2)|> */ 
void recv_cb2(void *arg, char *pData, unsigned short len){
	int i;
	if(push_mode == 1){
		 os_printf("Received data from server 2");
	 }
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
   
   if(push_mode == 1){
			int buffer_ele = mg_buf->numElements(mg_buf);
		if(buffer_ele != 0){
			mg_buf->pull(mg_buf,&str_pull);
			os_printf("%s:%s\r\n",str_pull.topic,str_pull.msg);
		}
	}
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
	if(push_mode == 1){
		 os_printf("CLIENT1 DISCONN \n");
	 }
	
}

/*	<|CLIENT1 DISCONNECT CALLBACK|> */
void disconnectCB2(void *arg) {
	if(push_mode == 1){  
		os_printf("CLIENT2 DISCONN\n");
	}
}

/* <|DNS callback for conn1 if there is no maping ip in cache|> */
void ResolveDNS_for_conn1( const char *name, ip_addr_t *ipaddr, void *arg ){
	if(push_mode == 1) {
		os_printf("Can't find ip in cache system. Asking from DNS server\n"); 		 
	}

	if(ipaddr == NULL){
		switch(push_mode){
			case 0: os_printf("Error: %d \n",NO_LOOKUP_IP_FOUND); break;
			case 1: os_printf("Unable to address from %s",name); break;
		}
	}else{
		memcpy(conn1.proto.tcp->remote_ip, ipaddr, 4);
		if(push_mode ==1) {
			os_printf("Connecting . . .\n");
		}
		switch(conn1.state){
			case ESPCONN_CONNECT: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
			case ESPCONN_NONE: espconn_connect(&conn1); break;
			case ESPCONN_LISTEN: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
			case ESPCONN_WAIT: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
			case ESPCONN_WRITE: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
			case ESPCONN_READ: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
			case ESPCONN_CLOSE: espconn_connect(&conn1);  break;
		}
	}
}	

/* <|DNS callback for conn2 if there is no maping ip in cache|> */
void ResolveDNS_for_conn2( const char *name, ip_addr_t *ipaddr, void *arg ){
	if(push_mode== 1){
		 os_printf("Can't find ip in cache system. Asking from DNS server\n"); 
	 }
			 
	if(ipaddr == NULL){
		switch(push_mode){
			case 0: os_printf("Error: %d \n",NO_LOOKUP_IP_FOUND); break;
			case 1: os_printf("Unable to address from %s",name); break;
		}
	}else{
		memcpy(conn2.proto.tcp->remote_ip, ipaddr, 4);
		if(push_mode ==1){
			os_printf("Connecting . . .\n");
		}
		switch(conn2.state){
			case ESPCONN_CONNECT: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
			case ESPCONN_NONE: espconn_connect(&conn2); break;
			case ESPCONN_LISTEN: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
			case ESPCONN_WAIT: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
			case ESPCONN_WRITE: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
			case ESPCONN_READ: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
			case ESPCONN_CLOSE: espconn_connect(&conn2);  break;
		}	
	}
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
 * 					 <17> Pull message from microgear buffer to serial
 * 					 <18> Write feed
 * 					 <19> Echo mode selection
 * 					 <20> Check status netpie connection
 * 					 <21> Push mode selection
 * 					 
 * 					 
 */ 
 
void read_sr(void *pvParameters) {
	while(1){
		message_index=0;
		while(xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS){
			
			if((xQueueHandleUart.param == 10) || (xQueueHandleUart.param == 13)){
			
			}else{
				int break_loop =0;
				message_sr[message_index++] = xQueueHandleUart.param;
				message_sr[message_index] = '\0';	
						
				/*	<| Echo mode selection |>	*/
				if(strcmp(message_sr,ECHOMODE)==0){
					message_index = 0;
					param = 0;
					os_printf("inside echomode\n");
						while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS)   && break_loop == 0 ){	
						if(xQueueHandleUart.param == 13 ){
							param++;
							message_index=0;
							echo_mode = atoi(message_sr);
							switch (echo_mode){
								case 0: os_printf("Echo mode: 0\n"); break_loop =1; break;
								case 1: os_printf("Echo mode: 1\n"); break_loop = 1; break;		 
							}
						}else{
							message_sr[message_index++] = xQueueHandleUart.param;
							
							message_sr[message_index]=0;
						}
						vTaskDelay( 1 / portTICK_RATE_MS);		
					}
					message_index=0;
				}
				
				/*	<| Push mode selection |>	*/
				if(strcmp(message_sr,PUSHMODE)==0){
					message_index = 0;
					param = 0;
					os_printf("Inside pushmode\n");
						while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS) && break_loop == 0){	
						if(xQueueHandleUart.param == 13 ){
							param++;
							message_index=0;
							push_mode = atoi(message_sr);
							switch (push_mode){
								case 0: os_printf("Push mode: 0\n"); break_loop=1; break;
								case 1: os_printf("Push mode: 1\n"); break_loop = 1; break;		 
							}
						}else{
							message_sr[message_index++] = xQueueHandleUart.param;
						
							message_sr[message_index]=0;
						}
						vTaskDelay( 1 / portTICK_RATE_MS);		
					}
					message_index=0;
				}
				
				/*	<|SETUP WIFI|> */
				if(strcmp(message_sr,SETUPWIFI)== 0){ 
					ssid[0] = '\0';
					password[0] = '\0';
					message_sr[0] = '\0';
					message_index=0; 
					param =0;				
					while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS) && break_loop == 0){	
						if(xQueueHandleUart.param == 34 || xQueueHandleUart.param == 44){
							param++;
							message_index=0;
							switch (param){
								case 1: break;
								case 2: strcpy(ssid,message_sr);break;
								case 3: break;
								case 4: break;
								case 5: 
								strcpy(password,message_sr);
								xSemaphoreGive(SetWifi);
								break_loop = 1;
								break;
							}
						}else{
							message_sr[message_index++] = xQueueHandleUart.param;
							message_sr[message_index]=0;
						}
						vTaskDelay( 10 / portTICK_RATE_MS);		
					}
					message_index=0;			
				}
				
				/*	<|CHECKING STATUS WIFI|> */
				if(strcmp(message_sr,CHECKSTATUS_WIFI)==0){
					checkstatus();
					message_index=0;
				}
				
				/*	<|CLIENT1 CONNECT TO SERVER|>	*/
				if(strcmp(message_sr,CONNECT_TO_SERVER1_BY_CLIENT1) == 0){	
						message_index=0;  				
						char host[15]="";
						message_sr[0] = '\0';
						param=0;
						while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS) && break_loop == 0){	
							if(xQueueHandleUart.param == 34 || xQueueHandleUart.param == 13 || xQueueHandleUart.param == 44 ){
								param++;
								message_index=0;
								switch (param){
									case 1:break;
									case 2:strcpy(host,message_sr);break;
									case 3:break;
									case 4: 										
										port = atoi(message_sr);		
										if(echo_mode == 1){
											 os_printf("AT+CCS1 \"%s\",%d ",host,port);
										 }
										conn1.proto.tcp->remote_port = port;
										if(espconn_gethostbyname(&conn1, host, &HostResolve_Ip1, ResolveDNS_for_conn1) == ESPCONN_OK){
											memcpy(conn1.proto.tcp->remote_ip, &HostResolve_Ip1, 4);
											switch(conn1.state){
												case ESPCONN_CONNECT: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
												case ESPCONN_NONE: espconn_connect(&conn1); break;
												case ESPCONN_LISTEN: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
												case ESPCONN_WAIT: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
												case ESPCONN_WRITE: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
												case ESPCONN_READ: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
												case ESPCONN_CLOSE: espconn_connect(&conn1);  break_loop = 1; break;
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
						message_index=0;
				}
				
				/*	<|CLIENT2 CONNECT TO SERVER|>	*/
				if(strcmp(message_sr,CONNECT_TO_SERVER2_BY_CLIENT2) == 0){
					message_index=0;  					
					char host[15]="";
					message_sr[0] = '\0';
					param = 0;
					while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS) && break_loop == 0){	
						if(xQueueHandleUart.param == 34 || xQueueHandleUart.param == 13 || xQueueHandleUart.param == 44){
							param++;
							message_index=0;
							switch (param){
								case 1: break;
								case 2: strcpy(host,message_sr);break;
								case 3: break;
								case 4: 
								port = atoi(message_sr);		
								if(echo_mode == 1){
									 os_printf("AT+CCS2 \"%s\",%d ",host,port);
								 }
								conn2.proto.tcp->remote_port = port;
								if(espconn_gethostbyname(&conn2, host, &HostResolve_Ip2, ResolveDNS_for_conn2) == ESPCONN_OK){
									memcpy(conn2.proto.tcp->remote_ip, &HostResolve_Ip2, 4);
									switch(conn2.state){
										case ESPCONN_CONNECT: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
										case ESPCONN_NONE: espconn_connect(&conn2); break;
										case ESPCONN_LISTEN: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
										case ESPCONN_WAIT: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
										case ESPCONN_WRITE: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
										case ESPCONN_READ: os_printf("Error: %d \n", CONN_NOT_READY_TO_CONNECT); break;
										case ESPCONN_CLOSE: espconn_connect(&conn2);  break_loop =1; break;
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
					message_index=0;
				}
				
				/*	<|CLIENT1 DISCONNECT|>	*/
				if(strcmp(message_sr,DISCONNECT_FROM_SERVER1) == 0){
					//if(echo_mode == 1)
					espconn_disconnect(&conn1);
					message_index=0;
				}
				
				/*	<|CLIENT2 DISCONNECT|>	*/
				if(strcmp(message_sr,DISCONNECT_FROM_SERVER2) == 0){
					espconn_disconnect(&conn2);
					message_index=0;
				}
				
				/*	<|CHECK STATUS OF CLIENT 1|>	*/
				if(strcmp(message_sr,CHECKSTATUS_CLIENT1) == 0){
					if(echo_mode==1) os_printf("%s ",CHECKSTATUS_CLIENT1);
					switch(conn1.state){
						case ESPCONN_CONNECT: os_printf("OK\n"); break;
						case ESPCONN_NONE: os_printf("NONE\n"); break;
						case ESPCONN_LISTEN: os_printf("LISTENING\n"); break;
						case ESPCONN_WAIT: os_printf("WAITING\n"); break;
						case ESPCONN_WRITE: os_printf("WRITING\n"); break;
						case ESPCONN_READ: os_printf("READING\n"); break;
						case ESPCONN_CLOSE: os_printf("NOT CONNECTED\n"); break;
					}
					message_index=0;
				}
				
				/*	<|CHECK STATUS OF CLIENT 2|>	*/
				if(strcmp(message_sr,CHECKSTATUS_CLIENT2) == 0){
					if(echo_mode==1) os_printf("%s ",CHECKSTATUS_CLIENT2	);
					switch(conn2.state){
						case ESPCONN_CONNECT: os_printf("OK\n"); break;
						case ESPCONN_NONE: os_printf("NONE\n"); break;
						case ESPCONN_LISTEN: os_printf("LISTENING\n"); break;
						case ESPCONN_WAIT: os_printf("WAITING\n"); break;
						case ESPCONN_WRITE: os_printf("WRITING\n"); break;
						case ESPCONN_READ: os_printf("READING\n"); break;
						case ESPCONN_CLOSE: os_printf("NOT CONNECTED\n"); break;
					}
					message_index=0;
				}
				
				/*	<|CLIENT1 PRINT DATA TO SERVER|>
				 * 	@maxsize data: 1024 bytes
				 * 
				 * 	*/
				if(strcmp(message_sr,PRINT_TO_SERVER1)==0){
					message_index=0;
					param =0;
					while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS )&& break_loop == 0){	
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
									break_loop =1;
									break;
							}
						}else{
							send_buf1[message_index++] = xQueueHandleUart.param;
							//os_printf("Data is: %c\n",xQueueHandleUart.param);
							send_buf1[message_index]=0;
						}
						vTaskDelay(10/ portTICK_RATE_MS);				
					}
					message_index=0;
				}
				
				/*	<|CLIENT2 PRINT DATA TO SERVER|>	
				 * @maxsize data: 1024 bytes
				 * 	
				 * */
				if(strcmp(message_sr,PRINT_TO_SERVER2)==0){
					message_index=0;
					param =0;
					while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS) && break_loop == 0){	
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
									break_loop = 1;
									message_index=0;
									break;
							}
						}else{
							send_buf2[message_index++] = xQueueHandleUart.param;
							send_buf2[message_index]=0;
						}
						vTaskDelay(10/ portTICK_RATE_MS);				
					}
					message_index=0;
				}
				
				/*	<|READ DATA FROM CLIENT1 BUFFER|> 
				 *	@Print data from client1 buffer to serial without header  		
				 * 
				 */
				if(strcmp(message_sr,READ_DATA_FROM_CLIENT1_BUFFER)==0){
					char c;
					int x;
					message_index=0;
					param = 0;
					while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS) && break_loop == 0){	
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
								break_loop =1;
								break;
							}
						}else{
							message_sr[message_index++] = xQueueHandleUart.param;
							message_sr[message_index]=0;
						}
						vTaskDelay(10/ portTICK_RATE_MS);				
					}
					message_index=0;
				}
				
				/*	<|READ DATA FROM CLIENT1 BUFFER TO ARDUINO|> 
				 *	@Print data from client1 buffer to serial with header  		
				 * 
				 */
				if(strcmp(message_sr,READ_DATA_FROM_CLIENT1_BUFFER_TO_ARDUINO_LIB)==0){
					char c;
					int x;
					message_index=0;
					param = 0;
					while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS) && break_loop == 0){	
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
									break_loop =1;
								message_index=0;						
								break;
							}
						}else{
							message_sr[message_index++] = xQueueHandleUart.param;
							message_sr[message_index]=0;
						}
						vTaskDelay(10/ portTICK_RATE_MS);				
					}
					message_index=0;
				}
				
				/*	<|READ DATA FROM CLIENT2 BUFFER|> 
				 *	@Print data from client2 buffer to serial without header  		
				 * 
				 */
				if(strcmp(message_sr,READ_DATA_FROM_CLIENT2_BUFFER)==0){
					char c;
					int x;
					message_index=0;
					param =0;
					while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS) && break_loop == 0){	
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
									break_loop=1;			
									break;
							}
						}else{
							message_sr[message_index++] = xQueueHandleUart.param;
							message_sr[message_index]=0;
						}
						vTaskDelay(10/ portTICK_RATE_MS);				
					}
					message_index=0;
				}
				
				/*	<|READ DATA FROM CLIENT2 BUFFER TO ARDUINO|>
				 * @Print data from client2 buffer to serial with header  		
				 * 
				 */
				if(strcmp(message_sr,READ_DATA_FROM_CLIENT2_BUFFER_TO_ARDUINO_LIB)==0){
					char c;
					int x;
					message_index=0;
					param =0;
					while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS) && break_loop == 0){	
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
									break_loop =1;					
									break;
							}
						}else{
							message_sr[message_index++] = xQueueHandleUart.param;
							message_sr[message_index]=0;
						}
						vTaskDelay(10/ portTICK_RATE_MS);				
					}
					message_index=0;
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
					
				//Unuseable due to fault detection between AT+MGCN and AT+MGCN? need to solved	
				//~ /* 	<| CHECK MICROGEAR STATUS |>	*/
				//~ if(strcmp(message_sr,CHECKSTATUS_NETPIE)==0){
					//~ os_printf("inside");
					//~ if (microgear_isConnected(&mg)) os_printf("OK\n");
					//~ else os_printf("Error ");
					//~ message_index=0;
				//~ }
				
				/*	<|MICROGEAR CONNECT|> */		
				if(strcmp(message_sr,CONNECT_TO_NETPIE)==0){	
					microgear_connect(&mg,appid);
					microgear_on(&mg, CONNECTED, onConnected);
					microgear_on(&mg, MESSAGE, onMsghandler);
					message_index=0;
				}
				
				/*	<|MICROGEAR DISCONNECT|> */
				if(strcmp(message_sr,DISCONNECT_FROM_NETPIE)==0){
					microgear_disconnect(&mg);	
					message_index=0;
				}
					
				 /*	<|MICROGEAR SETUP TOKEN|> */
				if(strcmp(message_sr,SETUP_TOKEN)==0){
					message_index=0;
					tokensecret[0] = '\0';
					token[0] = '\0';
					param=0;
					message_sr[0] = '\0';
					while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS )&& break_loop == 0){	
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
									if(push_mode == 1){
										 os_printf("Token: %s\nToken Secret: %s\n",token,tokensecret);
									 }
									microgear_setToken(&mg, token, tokensecret, NULL);
									break_loop = 1;
									break;
							}
						}else{
							message_sr[message_index++] = xQueueHandleUart.param;							
							message_sr[message_index]=0;
						}
						vTaskDelay(10/ portTICK_RATE_MS);				
					}
					message_index=0;
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
					while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS) && break_loop == 0 ){
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
									if(push_mode == 1){
										 os_printf("Appid: %s\nKey: %s\nSecret: %s\nAlias: %s\n",appid,key,secret,alias);	
									 }
									microgear_init(&mg, key, secret, alias); 
									break_loop=1;
									break;
							}
						}else{
							message_sr[message_index++] = xQueueHandleUart.param;
							message_sr[message_index]=0;
						}
						vTaskDelay(10/ portTICK_RATE_MS);				
					}
					message_index=0;
				}
				
				/*	<|MICROGEAR SET ALIAS|> */
				if(strcmp(message_sr,SET_ALIAS_NAME)==0){
					message_index=0;
					message_sr[0] = '\0';
					param = 0;
					while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS) && break_loop == 0){	
						if(xQueueHandleUart.param == 34){ 
							param++;  												
							message_index=0;
							switch (param){
								case 1: break;
								case 2:	
									if(push_mode ==1){
										os_printf("Set alias name to: %s\n",message_sr);
									}
									microgear_setAlias(&mg, message_sr);	
									break_loop =1;
									break;
					}										
						}else{
							message_sr[message_index++] = xQueueHandleUart.param;
							message_sr[message_index]=0;
						}
						vTaskDelay(10/ portTICK_RATE_MS);				
					}
					message_index=0;
				}
				
				/*	<|MICROGEAR PUBLISH|> */
				//should not use white space in data_pub
				if(strcmp(message_sr,PUBLISH)==0){
					message_index=0;
					param =0;
					message_sr[0] = '\0'; 
					topic_pub[0] = '\0';
					data_pub[0] = '\0';
					while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS) && break_loop == 0){
						if(xQueueHandleUart.param == 44 || xQueueHandleUart.param == 34 ){
							param++;
							message_index=0;
							switch (param){
								case 1: break;
								case 2: strcpy(topic_pub,message_sr);break;
								case 3: break;
								case 4: break;
								case 5:	
									strcpy(data_pub,message_sr);
									if(push_mode ==1) {
										os_printf("Topis is: %s \ndata is: %s\n",topic_pub,message_sr);
									}
									microgear_publish(&mg, topic_pub, data_pub, NULL);
									break_loop =1;
									break;
							}
						}else{
							message_sr[message_index++] = xQueueHandleUart.param;
							message_sr[message_index]=0;
						}
						vTaskDelay(10/ portTICK_RATE_MS);				
					}
					message_index=0;
				}
				
				/*	<|MICROGEAR SUBSCRIBE|> */
				if(strcmp(message_sr,SUBSCRIBE)==0){
					message_index=0;
					message_sr[0] = '\0';
					param=0;
					while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS) && break_loop == 0){	
						if(xQueueHandleUart.param == 34){
							param++;
							message_index=0;
							switch (param){
								case 1: break;
								case 2: 
									if(push_mode ==1) {
										os_printf("Subscribe topic: %s\n",message_sr);
									}
									microgear_subscribe(&mg, message_sr);
									break_loop=1;
									break;
							}												
						}else{
							message_sr[message_index++] = xQueueHandleUart.param;
							message_sr[message_index]=0;
						}
						vTaskDelay(10/ portTICK_RATE_MS);				
					}
					message_index=0;
				}
				
				/*	<|MICROGEAR UNSUBSCRIBE|> */
				if(strcmp(message_sr,UNSUBSCRIBE)==0){
					message_index=0;
					message_sr[0] = '\0';
					param =0;
					while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS) && break_loop == 0){	
						if(xQueueHandleUart.param == 34){
							message_index=0;
							param++;
							switch(param) {
								case 1: break;
								case 2: 
									if(push_mode ==1) os_printf("Unsubscribe topic: %s\n",message_sr);
									microgear_unsubscribe(&mg, message_sr);		
									break_loop=1;
									break;
							}										
						}else{
							message_sr[message_index++] = xQueueHandleUart.param;
							message_sr[message_index]=0;
						}
						vTaskDelay(10/ portTICK_RATE_MS);				
					}
				message_index=0;
				}
				
				
				/*	<|MICROGEAR CHAT|> */
				if(strcmp(message_sr,CHAT)==0){
					message_index=0;
					message_sr[0] = '\0';
					data_chat[0] = '\0';
					device_name[0] = '\0';
					param =0;
					while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS) && break_loop == 0){
						if(xQueueHandleUart.param == 34 || xQueueHandleUart.param == 44){
							param++;
							message_index=0;
							switch (param){
								case 1: break;
								case 2: strcpy(device_name,message_sr);break;
								case 3: break;
								case 4: break;
								case 5: strcpy(data_chat,message_sr); break_loop =1; break;
							}
						}else{
							message_sr[message_index++] = xQueueHandleUart.param;
							message_sr[message_index]=0;
						}
						vTaskDelay(10/ portTICK_RATE_MS);				
					}
					microgear_chat(&mg, device_name, data_chat);
					message_index=0;
				}	
				
				/* 	<| PULL MESSAGE TO SERIAL FROM MICROGEAR BUFFER |>	*/
				if(strcmp(message_sr,PULL_MESSAGE)==0){				
					int buffer_ele = mg_buf->numElements(mg_buf);
					if(buffer_ele != 0){
						mg_buf->pull(mg_buf,&str_pull);
						os_printf("%s:%s\r\n",str_pull.topic,str_pull.msg);
					}			
					message_index=0;				
				}
				
				/* 	<| WRITE DATA TO FEED |>	*/
				if(strcmp(message_sr,WRITE_FEED)==0){
					message_index=0;
					feeddata[0] = '\0';
					feedname[0] = '\0';
					feedmode = 0;
					apikey[0] = '\0';
					feed_buff[0] = '\0';
					strcpy(feed_buff,"/@writefeed/");
					param =0;
					while((xQueueReceive(xQueueUART,(void *)&xQueueHandleUart,0) == pdPASS) && break_loop == 0){
						if(xQueueHandleUart.param == 34 || xQueueHandleUart.param == 44){
							param++;
							message_index=0;
							switch (param){
								case 1: feedmode = atoi(message_sr);break;
								case 2: break;
								case 3: strcpy(feedname,message_sr);break;
								case 4: break;
								case 5: break;
								case 6: strcpy(feeddata,message_sr);break;
								case 7:break;
								case 8:break;
								case 9: 
								if(feedmode) strcpy(apikey,message_sr);
								break_loop=1;
								break;
							}
						}else{
							message_sr[message_index++] = xQueueHandleUart.param;
							message_sr[message_index]=0;
						}	
						vTaskDelay(10/ portTICK_RATE_MS);				
					}
					strcat(feed_buff,feedname);
					if(feedmode){
						 strcat(feed_buff,"/"); strcat(feed_buff,apikey);
					 }
					if(push_mode == 1) {
						os_printf("Topis: %s\n",feed_buff);
						os_printf("Data: %s\n",feeddata);
					}
					microgear_publish(&mg, feed_buff, feeddata, NULL);
					message_index=0;
				}																				
			}
			vTaskDelay(10/ portTICK_RATE_MS);
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
