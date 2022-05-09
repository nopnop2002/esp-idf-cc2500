/* The example of CC250
 *
 * This sample code is in the public domain.
 */

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include <cc2500.h>

#define TAG "MAIN"

#if CONFIG_TRANSMITTER
void tx_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(0), "Start");
	uint8_t txBuf[64];
	uint8_t sendData = 0x00;

	while (1) {
		int txLen = 32;
		for (int i=0;i<txLen;i++) txBuf[i] = sendData;
		sendPacket(txBuf, txLen);
		vTaskDelay(1000/portTICK_PERIOD_MS);
		sendData++;
	} // end while

	// never reach here
	vTaskDelete( NULL );
}
#endif // CONFIG_TRANSMITTER

#if CONFIG_RECEIVER
void rx_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(0), "Start");
	uint8_t rxBuf[64] = {0};
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
		} // end if
		vTaskDelay(1);
	} // end while

	// never reach here
	vTaskDelete( NULL );
}
#endif // CONFIG_RECEIVER

void app_main()
{
	bool ret = begin(CONFIG_CC2500_CHANNEL);
	if (ret != true) {
		ESP_LOGE(TAG, "CC2500 not installed");
		while(1) { vTaskDelay(1); }
	}

#if CONFIG_TRANSMITTER
	xTaskCreate(&tx_task, "tx_task", 1024*4, NULL, 5, NULL);
#endif
#if CONFIG_RECEIVER
	xTaskCreate(&rx_task, "rx_task", 1024*4, NULL, 1, NULL);
#endif

}
