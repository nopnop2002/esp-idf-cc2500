/* The example of CC250
 *
 * This sample code is in the public domain.
 */

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include <cc2500.h>

static const char *TAG = "MAIN";

#if CONFIG_SENDER
void tx_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t txBuf[64];

	while (1) {
		int txLen = sprintf((char *)txBuf, "Hello World %"PRIu32, xTaskGetTickCount());
		sendPacket(txBuf, txLen);
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}

	// never reach here
	vTaskDelete( NULL );
}
#endif // CONFIG_SENDER

#if CONFIG_RECEIVER
void rx_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t rxBuf[64] = {0};
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
		} // end if
		vTaskDelay(1);
	} // end while

	// never reach here
	vTaskDelete( NULL );
}
#endif // CONFIG_RECEIVER

void app_main()
{
	esp_err_t ret = begin(CONFIG_CC2500_CHANNEL);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "CC2500 not installed");
		while(1) { vTaskDelay(1); }
	}

#if CONFIG_SENDER
	xTaskCreate(&tx_task, "TX", 1024*4, NULL, 5, NULL);
#endif
#if CONFIG_RECEIVER
	xTaskCreate(&rx_task, "RX", 1024*4, NULL, 1, NULL);
#endif

}
