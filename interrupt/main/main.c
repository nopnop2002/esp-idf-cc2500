/* The example of CC2500
 *
 * This sample code is in the public domain.
 */

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include <cc2500.h>

#define GPIO_INPUT_IO CONFIG_INPUT_GPIO
#define GPIO_INPUT_PIN_SEL	(1ULL<<GPIO_INPUT_IO)
#define ESP_INTR_FLAG_DEFAULT 0

static const char *TAG = "MAIN";

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
	uint32_t gpio_num = (uint32_t) arg;
	xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void gpio(void *pvParameters)
{
	uint32_t io_num;
	while (1) {
		if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
			ESP_LOGW(pcTaskGetName(NULL), "GPIO[%"PRIu32"] intr, val: %d", io_num, gpio_get_level(io_num));
		} else {
			ESP_LOGE(pcTaskGetName(NULL), "xQueueReceive fail");
			break;
		}
	}
	vTaskDelete( NULL );
}

#if CONFIG_SENDER
void tx_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t txBuf[64];

	while (1) {
		int txLen = sprintf((char *)txBuf, "Hello World %"PRIu32, xTaskGetTickCount());
		ESP_LOGI(TAG, "[%s]", txBuf);
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
	//zero-initialize the config structure.
	gpio_config_t io_conf = {};
	//interrupt of rising edge
	io_conf.intr_type = GPIO_INTR_POSEDGE;
	//set as input mode
	io_conf.mode = GPIO_MODE_INPUT;
	//bit mask of the pins that you want to set
	io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
	//disable pull-down mode
	io_conf.pull_down_en = 0;
	//disable pull-up mode
	io_conf.pull_up_en = 0;
	//configure GPIO with the given settings
	gpio_config(&io_conf);

	//create a queue to handle gpio event from isr
	gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

	// Install gpio isr service
	gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
	// Hook isr handler for specific gpio pin
	gpio_isr_handler_add(GPIO_INPUT_IO, gpio_isr_handler, (void*) GPIO_INPUT_IO);

	// Initialize CC2500
	esp_err_t ret = begin(CONFIG_CC2500_CHANNEL);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "CC2500 not installed");
		while(1) { vTaskDelay(1); }
	}

	// Asserts when RX FIFO is filled at or above the RX FIFO threshold or the end of packet is reached.
	// De-asserts when the RX FIFO is empty.
	//setAssert(0x01);

	// Asserts when sync word has been sent / received, and de-asserts at the end of the packet.
	setAssert(0x06);

	// Asserts when a packet has been received with CRC OK.
	// De-asserts when the first byte is read from the RX FIFO.
	//setAssert(0x07);

	// Start gpio interrupt task
	xTaskCreate(&gpio, "GPIO", 1024*2, NULL, 5, NULL);

#if CONFIG_SENDER
	xTaskCreate(&tx_task, "TX", 1024*4, NULL, 5, NULL);
#endif
#if CONFIG_RECEIVER
	xTaskCreate(&rx_task, "RX", 1024*4, NULL, 5, NULL);
#endif

}
