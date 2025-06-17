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
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "esp_log.h"

#include <cc2500.h>

static const char *TAG = "MAIN";

MessageBufferHandle_t xMessageBufferTrans;
MessageBufferHandle_t xMessageBufferRecv;
QueueHandle_t xQueueTinyusb;

// The total number of bytes (not single messages) the message buffer will be able to hold at any one time.
size_t xBufferSizeBytes = 1024;
// The size, in bytes, required to hold each item in the message,
size_t xItemSize = 64; // Maximum Payload size of CC2500 is 64

void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event)
{
	/* initialization */
	size_t rx_size = 0;
	ESP_LOGD(TAG, "CONFIG_TINYUSB_CDC_RX_BUFSIZE=%d", CONFIG_TINYUSB_CDC_RX_BUFSIZE);

	/* read */
	uint8_t buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE];
	esp_err_t ret = tinyusb_cdcacm_read(itf, buf, CONFIG_TINYUSB_CDC_RX_BUFSIZE, &rx_size);
	if (ret == ESP_OK) {
		ESP_LOGD(TAG, "Data from channel=%d rx_size=%d", itf, rx_size);
		ESP_LOG_BUFFER_HEXDUMP(TAG, buf, rx_size, ESP_LOG_INFO);
		for(int i=0;i<rx_size;i++) {
			xQueueSendFromISR(xQueueTinyusb, &buf[i], NULL);
		}
	} else {
		ESP_LOGE(TAG, "tinyusb_cdcacm_read error");
	}
}

void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event)
{
	int dtr = event->line_state_changed_data.dtr;
	int rts = event->line_state_changed_data.rts;
	ESP_LOGI(TAG, "Line state changed on channel %d: DTR:%d, RTS:%d", itf, dtr, rts);
}

#if CONFIG_SENDER
void tx_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t buf[xItemSize];
	while (1) {
		size_t received = xMessageBufferReceive(xMessageBufferRecv, buf, sizeof(buf), portMAX_DELAY);
		ESP_LOGI(pcTaskGetName(NULL), "xMessageBufferReceive received=%d", received);
		ESP_LOG_BUFFER_HEXDUMP(pcTaskGetName(NULL), buf, received, ESP_LOG_INFO);
		sendPacket(buf, received);
	} // end while
	vTaskDelete( NULL );
}

void usb_rx(void *pvParameters)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	char buffer[xItemSize];
	int index = 0;
	while(1) {
		char ch;
		if(xQueueReceive(xQueueTinyusb, &ch, portMAX_DELAY)) {
			ESP_LOGD(pcTaskGetName(NULL), "ch=0x%x",ch);
			if (ch == 0x0d || ch == 0x0a) {
				if (index > 0) {
					ESP_LOGI(pcTaskGetName(NULL), "[%.*s]", index, buffer);
					size_t spacesAvailable = xMessageBufferSpacesAvailable( xMessageBufferRecv );
					ESP_LOGI(pcTaskGetName(NULL), "spacesAvailable=%d", spacesAvailable);
					size_t sended = xMessageBufferSend(xMessageBufferRecv, buffer, index, 100);
					if (sended != index) {
						ESP_LOGE(pcTaskGetName(NULL), "xMessageBufferSend fail index=%d sended=%d", index, sended);
						break;
					}
					index = 0;
				}
			} else {
				if (index == xItemSize) continue;
				buffer[index++] = ch;
			}
		}
	} // end while
	vTaskDelete(NULL);
}
#endif // CONFIG_SENDER

#if CONFIG_RECEIVER
void rx_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t buf[xItemSize];
	uint8_t rssi;
	uint8_t lqi;

	while(1) {
		int rxLen = listenForPacket(buf, sizeof(buf), &rssi, &lqi);
		if (rxLen) {
			ESP_LOGI(pcTaskGetName(NULL), "%d byte packet received:[%.*s]", rxLen, rxLen, buf);
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
			size_t sended = xMessageBufferSend(xMessageBufferTrans, buf, rxLen, 100);
			if (sended != rxLen) {
				ESP_LOGE(pcTaskGetName(NULL), "xMessageBufferSend fail rxLen=%d sended=%d", rxLen, sended);
				break;
			}
		}
		vTaskDelay(1);
	} // end while
	vTaskDelete( NULL );
}

void usb_tx(void *pvParameters)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t buf[xItemSize];
	uint8_t crlf[2] = { 0x0d, 0x0a };
	while(1) {
		size_t received = xMessageBufferReceive(xMessageBufferTrans, buf, sizeof(buf), portMAX_DELAY);
		ESP_LOGI(pcTaskGetName(NULL), "%d byte packet received:[%.*s]", received, received, buf);
		tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, buf, received);
		tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, crlf, 2);
		tinyusb_cdcacm_write_flush(TINYUSB_CDC_ACM_0, 0);
	} // end while
	vTaskDelete(NULL);
}
#endif // CONFIG_RECEIVER

void app_main()
{
	ESP_LOGI(TAG, "USB initialization");
	const tinyusb_config_t tusb_cfg = {
		.device_descriptor = NULL,
		.string_descriptor = NULL,
		.external_phy = false,
		.configuration_descriptor = NULL,
	};
	ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

	tinyusb_config_cdcacm_t acm_cfg = {
		.usb_dev = TINYUSB_USBDEV_0,
		.cdc_port = TINYUSB_CDC_ACM_0,
		.rx_unread_buf_sz = 64,
		.callback_rx = &tinyusb_cdc_rx_callback, // the first way to register a callback
		.callback_rx_wanted_char = NULL,
		.callback_line_state_changed = &tinyusb_cdc_line_state_changed_callback,
		.callback_line_coding_changed = NULL
	};
	ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));

	// Create MessageBuffer
	xMessageBufferTrans = xMessageBufferCreate(xBufferSizeBytes);
	configASSERT( xMessageBufferTrans );
	xMessageBufferRecv = xMessageBufferCreate(xBufferSizeBytes);
	configASSERT( xMessageBufferRecv );

	// Create Queue
	xQueueTinyusb = xQueueCreate(100, sizeof(char));
	configASSERT( xQueueTinyusb );

	// Initialize CC2500
	esp_err_t ret = begin(CONFIG_CC2500_CHANNEL);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "CC2500 not installed");
		while(1) { vTaskDelay(1); }
	}

#if CONFIG_SENDER
	xTaskCreate(&tx_task, "TX", 1024*4, NULL, 5, NULL);
	xTaskCreate(&usb_rx, "USB_RX", 1024*4, NULL, 5, NULL);
#endif
#if CONFIG_RECEIVER
	xTaskCreate(&rx_task, "RX", 1024*4, NULL, 5, NULL);
	xTaskCreate(&usb_tx, "USB_TX", 1024*4, NULL, 5, NULL);
#endif

}
