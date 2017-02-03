#ifndef __USER_MAIN_H
#define __USER_MAIN_H

#include "uart.h"
#include "Microgear.h"
#include "ringbuf.h"
#include "string.h"
#include "lwip/inet.h"
#include "var.h"
#include "wifi_task.h"


#define SETUPWIFI "AT+CW "											//AT+CW "<SSID>","<PASS>"
#define CHECKSTATUS_WIFI "AT+CW?"									//AT+CW?
#define CONNECT_TO_SERVER1_BY_CLIENT1 "AT+CCS1 "					//AT+CCS1 "<IP>",<PORT>
#define CONNECT_TO_SERVER2_BY_CLIENT2 "AT+CCS2 "					//AT+CCS2 "<IP>",<PORT>
#define DISCONNECT_FROM_SERVER1 "AT+CD1"							//AT+CD1
#define DISCONNECT_FROM_SERVER2 "AT+CD2"							//AT+CD2
#define CHECKSTATUS_CLIENT1 "AT+CCS1?"								//AT+CCS1?
#define CHECKSTATUS_CLIENT2 "AT+CCS2?"								//AT+CCS2?
#define PRINT_TO_SERVER1 "AT+CP1 "									//AT+CP1 "<PAYLOAD>"
#define PRINT_TO_SERVER2 "AT+CP2 "									//AT+CP2 "<PAYLOAD>"
#define READ_DATA_FROM_CLIENT1_BUFFER "AT+CR1 "						//AT+CR1 <SIZE_TO_READ>
#define READ_DATA_FROM_CLIENT1_BUFFER_TO_ARDUINO_LIB "AT+CRTA1 "	//AT+CRTA1 <SIZE_TO_READ>
#define READ_DATA_FROM_CLIENT2_BUFFER "AT+CR2 "						//AT+CR1 <SIZE_TO_READ>
#define READ_DATA_FROM_CLIENT2_BUFFER_TO_ARDUINO_LIB "AT+CRTA2 "	//AT+CRTA2 <SIZE_TO_READ>
#define SECURE_CONNECT "AT+SCC "									//AT+SCC <SERVER>,<PORT>,<OPTION>,<FOOTPRINT>
#define SECURE_CONNECTED "AT+SCC?"									//AT+SCC?
#define SECURE_VERIFY "AT+SCVF"										//AT+SCVF 
#define SECURE_READ "AT+SCR"										//SECURE CLIENT READ
#define SECURE_PRINT "AT+SCP"										//SECURE CLIENT PRINT





#define CONNECT_TO_NETPIE "AT+MGCN"									//AT+MGCN
#define DISCONNECT_FROM_NETPIE "AT+MGDC"							//AT+MGDC  
#define SETUP_TOKEN "AT+MGST "										//AT+MGST "RKJy30tYkUKoXa7G","oWfxRNqRaXdwffVdFvuU27qJqr1KNpIy"
#define INIT_MICROGEAR "AT+MGIN "									//AT+MGIN "HelloNetpie1","YhtHPvlmMxL5yJB","YphWgyUI31q8sEMu6qtNrIPn1","Light_control"
#define SET_ALIAS_NAME "AT+MGSA "									//AT+MGSA "ALIAS_NEW"
#define PUBLISH "AT+MGP "											//AT+MGP "<TOPIC>","<PAYLOAD>"
#define SUBSCRIBE "AT+MGS "											//AT+MGS "<TOPIC>"  
#define UNSUBSCRIBE "AT+MGUS "										//AT+MGUS "<TOPIC>"
#define CHAT "AT+MGC "												//AT+MGC "<ALIAS>","<PAYLOAD>"

#define MAX_SIZE_TCP_PRINT 500
#define MAX_SIZE_TCP_RECEV 500
#define MAX_SIZE_PUBLISH 50
#define MAX_SIZE_TOPIC 50






#endif
