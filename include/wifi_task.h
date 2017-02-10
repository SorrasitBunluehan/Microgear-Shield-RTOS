#ifndef WIFI_TASK
#define WIFI_TASK

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
	//~ if( xSemaphoreTake( WifiReady, 0 ) == pdTRUE )
	//~ {
		uint8_t status; 
		status = wifi_station_get_connect_status();
		if(status == STATION_GOT_IP){
			wifi_get_ip_info(STATION_IF, &ipconfig);
			os_printf("CONNECTED %d.%d.%d.%d\n", IP2STR(&ipconfig.ip));
		}else{
			os_printf("DISCONNECTED\n");
		}
	//~ }
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

#endif
