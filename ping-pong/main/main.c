/* The example of CC250
 *
 * This sample code is in the public domain.
 */

#include <string.h>
#include <ctype.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include <cc2500.h>

#define TAG "MAIN"

#if CONFIG_PRIMARY
void primary_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(0), "Start");
	uint8_t txBuf[64];
	uint8_t rxBuf[64];
	uint8_t rssi;
	uint8_t lqi;

	while (1) {
		int txLen = sprintf((char *)txBuf, "Hello World %d", xTaskGetTickCount());
		ESP_LOGI(pcTaskGetName(0), "txLen=%d", txLen);
		sendPacket(txBuf, txLen);

		// Wait for a response from the other party
		bool waiting = true;
		TickType_t startTick = xTaskGetTickCount();
		while(waiting) {
			int rxLen = listenForPacket(rxBuf, 64, &rssi, &lqi);
			if (rxLen) {
				TickType_t respTick = xTaskGetTickCount() - startTick;
				ESP_LOGI(pcTaskGetName(0), "Received packet...");
				ESP_LOGI(pcTaskGetName(0), "respTick=%d", respTick);
				ESP_LOGI(pcTaskGetName(0), "rxLen=%d", rxLen);
				rxBuf[rxLen] = 0;
				ESP_LOGI(pcTaskGetName(0), "%s --> %s", txBuf, rxBuf);
				//ESP_LOG_BUFFER_HEXDUMP(pcTaskGetName(0), rxBuf, rxLen, ESP_LOG_INFO);
				waiting = false;
			} // end if
			TickType_t diffTick = xTaskGetTickCount() - startTick;
			if (diffTick > 100) {
				ESP_LOGE(pcTaskGetName(0), "No responce from others");
				waiting = false;
			}
			vTaskDelay(1);
		} // end while
		vTaskDelay(1000/portTICK_PERIOD_MS);
	} // end while

	// never reach here
	vTaskDelete( NULL );
}
#endif // CONFIG_PRIMARY

#if CONFIG_SECONDARY
void secondary_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(0), "Start");
	uint8_t txBuf[64];
	uint8_t rxBuf[64];
	uint8_t rssi;
	uint8_t lqi;
	while(1) {
		int rxLen = listenForPacket(rxBuf, 64, &rssi, &lqi);
		if (rxLen) {
			ESP_LOGI(pcTaskGetName(0), "rxLen=%d", rxLen);
			ESP_LOG_BUFFER_HEXDUMP(pcTaskGetName(0), rxBuf, rxLen, ESP_LOG_INFO);
			int dbm;
			if (rssi < 0x7F) {
				dbm = (rssi/2) - 72;
			} else {
				dbm = abs(rssi-256);
				dbm = (dbm/2) - 72;
			}
	
			ESP_LOGI(pcTaskGetName(0), "RSSI(Raw): 0x%02x", rssi);
			ESP_LOGI(pcTaskGetName(0), "RSSI(dBm): %d", dbm);
			ESP_LOGI(pcTaskGetName(0), "LQI: 0x%02x", lqi);

			for (int i=0;i<rxLen;i++) {
				if (islower(rxBuf[i])) {
					txBuf[i] = toupper(rxBuf[i]);
				} else {
					txBuf[i] = tolower(rxBuf[i]);
				}
			}
			sendPacket(txBuf, rxLen);
		} // end if
		vTaskDelay(1);
	} // end while

	// never reach here
	vTaskDelete( NULL );
}
#endif // CONFIG_SECONDARY

void app_main()
{
	bool ret = begin(CONFIG_CC2500_CHANNEL);
	if (ret != true) {
		ESP_LOGE(TAG, "CC2500 not installed");
		while(1) { vTaskDelay(1); }
	}

#if CONFIG_PRIMARY
	xTaskCreate(&primary_task, "primary_task", 1024*4, NULL, 5, NULL);
#endif
#if CONFIG_SECONDARY
	xTaskCreate(&secondary_task, "secondary_task", 1024*4, NULL, 1, NULL);
#endif

}
