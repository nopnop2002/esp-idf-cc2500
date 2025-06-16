/* The example of CC2500
 *
 * This sample code is in the public domain.
 */

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include <cc2500.h>

static const char *TAG = "MAIN";

#if CONFIG_PRIMARY
void primary_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t txBuf[64];
	uint8_t rxBuf[64];
	uint8_t rssi;
	uint8_t lqi;

	while (1) {
		int txLen = sprintf((char *)txBuf, "Hello World %"PRIu32, xTaskGetTickCount());
		ESP_LOGI(pcTaskGetName(NULL), "txLen=%d", txLen);
		sendPacket(txBuf, txLen);

		// Wait for a response from the other party
		bool waiting = true;
		TickType_t startTick = xTaskGetTickCount();
		while(waiting) {
			int rxLen = listenForPacket(rxBuf, 64, &rssi, &lqi);
			if (rxLen) {
				TickType_t respTick = xTaskGetTickCount() - startTick;
				ESP_LOGI(pcTaskGetName(NULL), "Received packet...");
				ESP_LOGI(pcTaskGetName(NULL), "respTick=%"PRIx32, respTick);
				ESP_LOGI(pcTaskGetName(NULL), "rxLen=%d", rxLen);
				rxBuf[rxLen] = 0;
				ESP_LOGI(pcTaskGetName(NULL), "%s --> %s", txBuf, rxBuf);
				//ESP_LOG_BUFFER_HEXDUMP(pcTaskGetName(NULL), rxBuf, rxLen, ESP_LOG_INFO);
				waiting = false;
			} // end if
			TickType_t diffTick = xTaskGetTickCount() - startTick;
			if (diffTick > 100) {
				ESP_LOGE(pcTaskGetName(NULL), "No responce from others");
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
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t txBuf[64];
	uint8_t rxBuf[64];
	uint8_t rssi;
	uint8_t lqi;
	while(1) {
		int rxLen = listenForPacket(rxBuf, 64, &rssi, &lqi);
		if (rxLen) {
			ESP_LOGI(pcTaskGetName(NULL), "rxLen=%d", rxLen);
			ESP_LOGI(pcTaskGetName(NULL), "rxBuf=[%.*s]", rxLen, rxBuf);
			int dbm;
			if (rssi < 0x7F) {
				dbm = (rssi/2) - 72;
			} else {
				dbm = abs(rssi-256);
				dbm = (dbm/2) - 72;
			}
	
			ESP_LOGI(pcTaskGetName(NULL), "RSSI(Raw): 0x%02x", rssi);
			ESP_LOGI(pcTaskGetName(NULL), "RSSI(dBm): %d", dbm);
			ESP_LOGI(pcTaskGetName(NULL), "LQI: 0x%02x", lqi);

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
	// Initialize CC2500
	esp_err_t ret = begin(CONFIG_CC2500_CHANNEL);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "CC2500 not installed");
		while(1) { vTaskDelay(1); }
	}

#if CONFIG_PRIMARY
	xTaskCreate(&primary_task, "PRIMARY", 1024*4, NULL, 5, NULL);
#endif
#if CONFIG_SECONDARY
	xTaskCreate(&secondary_task, "SECONDARY", 1024*4, NULL, 5, NULL);
#endif

}
