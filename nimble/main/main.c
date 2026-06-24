/* The example of CC2500
 *
 * This sample code is in the public domain.
 */

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/message_buffer.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include <cc2500.h>

static const char *TAG = "MAIN";

MessageBufferHandle_t xMessageBufferTrans;
MessageBufferHandle_t xMessageBufferRecv;

// The total number of bytes (not single messages) the message buffer will be able to hold at any one time.
size_t xBufferSizeBytes = 1024;
// The size, in bytes, required to hold each item in the message,
size_t xItemSize = 64; // Maximum Payload size of CC2500 is 64

#if CONFIG_SENDER
void tx_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t txBuf[xItemSize];

	while (1) {
		size_t received = xMessageBufferReceive(xMessageBufferRecv, txBuf, sizeof(txBuf), portMAX_DELAY);
		ESP_LOGI(pcTaskGetName(NULL), "xMessageBufferReceive received=%d", received);
		sendPacket(txBuf, received);
	}

	// never reach here
	vTaskDelete( NULL );
}
#endif // CONFIG_SENDER

#if CONFIG_RECEIVER
void rx_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t rxBuf[xItemSize];
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

			size_t spacesAvailable = xMessageBufferSpacesAvailable( xMessageBufferTrans );
			ESP_LOGI(pcTaskGetName(NULL), "spacesAvailable=%d", spacesAvailable);
			size_t sended = xMessageBufferSend(xMessageBufferTrans, rxBuf, rxLen, 100);
			if (sended != rxLen) {
				ESP_LOGE(pcTaskGetName(NULL), "xMessageBufferSend fail rxLen=%d sended=%d", rxLen, sended);
				break;
			}
		} // end if
		vTaskDelay(1); // Avoid Watchdog asserts
	} // end while

	vTaskDelete( NULL );
}

void dummy(void *pvParameters)
{
    ESP_LOGI(pcTaskGetName(NULL), "Start");
    uint8_t buf[xItemSize];
    while(1) {
        size_t received = xMessageBufferReceive(xMessageBufferRecv, buf, sizeof(buf), portMAX_DELAY);
        ESP_LOGI(pcTaskGetName(NULL), "xMessageBufferReceive received=%d", received);
    } // end while
    vTaskDelete(NULL);
}
#endif // CONFIG_RECEIVER

void nimble_spp_task(void * pvParameters);

void app_main()
{
	// Initialize NVS
	// It is used to store PHY calibration data
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	// Create MessageBuffer
	xMessageBufferTrans = xMessageBufferCreate(xBufferSizeBytes);
	configASSERT( xMessageBufferTrans );
	xMessageBufferRecv = xMessageBufferCreate(xBufferSizeBytes);
	configASSERT( xMessageBufferRecv );

	// Initialize CC2500
	ret = begin(CONFIG_CC2500_CHANNEL);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "CC2500 not installed");
		while(1) { vTaskDelay(1); }
	}


	xTaskCreate(nimble_spp_task, "NIMBLE_SPP", 1024*4, NULL, 5, NULL);
#if CONFIG_SENDER
	xTaskCreate(&tx_task, "TX", 1024*4, NULL, 5, NULL);
#endif
#if CONFIG_RECEIVER
	xTaskCreate(&rx_task, "RX", 1024*4, NULL, 5, NULL);
	xTaskCreate(&dummy, "DUMMY", 1024*4, NULL, 5, NULL);
#endif
}
