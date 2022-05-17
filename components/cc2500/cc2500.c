#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"

#include "cc2500.h"

#define TAG "CC2500"

// SPI Stuff
#if CONFIG_SPI2_HOST
#define HOST_ID SPI2_HOST
#elif CONFIG_SPI3_HOST
#define HOST_ID SPI3_HOST
#endif

static spi_device_handle_t _handle;

void spi_init()
{
	gpio_reset_pin(CONFIG_CSN_GPIO);
	gpio_set_direction(CONFIG_CSN_GPIO, GPIO_MODE_OUTPUT);
	gpio_set_level(CONFIG_CSN_GPIO, 1);

	spi_bus_config_t buscfg = {
		.sclk_io_num = CONFIG_SCK_GPIO, // set SPI CLK pin
		.mosi_io_num = CONFIG_MOSI_GPIO, // set SPI MOSI pin
		.miso_io_num = CONFIG_MISO_GPIO, // set SPI MISO pin
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
	};

	esp_err_t ret;
	ret = spi_bus_initialize( HOST_ID, &buscfg, SPI_DMA_CH_AUTO );
	ESP_LOGI(TAG, "spi_bus_initialize=%d",ret);
	assert(ret==ESP_OK);

	// Hardware CS control don't work.
	spi_device_interface_config_t devcfg = {
		.clock_speed_hz = 5000000, // SPI clock is 5 MHz!
		.queue_size = 7,
		.mode = 0, // SPI mode 0
		.spics_io_num = -1, // we will use manual CS control
		.flags = SPI_DEVICE_NO_DUMMY
	};

	ret = spi_bus_add_device( HOST_ID, &devcfg, &_handle);
	ESP_LOGI(TAG, "spi_bus_add_device=%d",ret);
	assert(ret==ESP_OK);
}

bool spi_write_byte(uint8_t* Dataout, size_t DataLength )
{
	spi_transaction_t SPITransaction;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Dataout;
		SPITransaction.rx_buffer = NULL;
		spi_device_transmit( _handle, &SPITransaction );
	}

	return true;
}

bool spi_read_byte(uint8_t* Datain, uint8_t* Dataout, size_t DataLength )
{
	spi_transaction_t SPITransaction;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Dataout;
		SPITransaction.rx_buffer = Datain;
		spi_device_transmit( _handle, &SPITransaction );
	}

	return true;
}

uint8_t spi_transfer(uint8_t address) {
	uint8_t datain[1];
	uint8_t dataout[1];
	dataout[0] = address;
	//spi_write_byte(dev, dataout, 1 );
	spi_read_byte(datain, dataout, 1 );
	return datain[0];
}


bool begin(uint8_t channel) {
	spi_init();

	resetDevice();
	ESP_LOGI(TAG, "channel=%d", channel);
	init(channel);

	uint8_t ChipPart = getChipPart();
	uint8_t ChipVersion = getChipVersion();
	ESP_LOGI(TAG, "ChipPart=%x", ChipPart);
	ESP_LOGI(TAG, "ChipVersion=%x", ChipVersion);
	if (ChipPart != 0x80 || ChipVersion != 0x03) {
		ESP_LOGE(TAG, "CC2500 Not Installed");
		return false;
	}
	ESP_LOGI(TAG, "CC2500 Installed");
	return true;
}

void init(uint8_t channel) {
	WriteRegister(REG_IOCFG2,0x06);
	WriteRegister(REG_IOCFG0,0x01);
	WriteRegister(REG_IOCFG1,0x06);

	//WriteRegister(REG_FIFOTHR, 0x02);
	WriteRegister(REG_FIFOTHR, VAL_FIFOTHR);
	WriteRegister(REG_SYNC1,VAL_SYNC1);
	WriteRegister(REG_SYNC0,VAL_SYNC0);
	WriteRegister(REG_PKTLEN,VAL_PKTLEN);
	//WriteRegister(REG_PKTCTRL1,0x8C);
	WriteRegister(REG_PKTCTRL1,VAL_PKTCTRL1);
	//WriteRegister(REG_PKTCTRL0, 0x0D);
	WriteRegister(REG_PKTCTRL0, 0x44); // Changed to Fixed packet length mode.

	WriteRegister(REG_ADDR,VAL_ADDR);
	//WriteRegister(REG_CHANNR,VAL_CHANNR);
	WriteRegister(REG_CHANNR,channel);
	WriteRegister(REG_FSCTRL1,VAL_FSCTRL1);
	WriteRegister(REG_FSCTRL0,VAL_FSCTRL0);
	WriteRegister(REG_FREQ2,VAL_FREQ2);
	WriteRegister(REG_FREQ1,VAL_FREQ1);
	WriteRegister(REG_FREQ0,VAL_FREQ0);
	WriteRegister(REG_MDMCFG4,VAL_MDMCFG4);
	WriteRegister(REG_MDMCFG3,VAL_MDMCFG3);
	WriteRegister(REG_MDMCFG2,VAL_MDMCFG2);
	WriteRegister(REG_MDMCFG1,VAL_MDMCFG1);
	//WriteRegister(REG_MDMCFG1,0xA2); // Enable Forward Error Correction (FEC) with interleaving for packet payload
	WriteRegister(REG_MDMCFG0,VAL_MDMCFG0);
	WriteRegister(REG_DEVIATN,VAL_DEVIATN);
	WriteRegister(REG_MCSM2,VAL_MCSM2);
	WriteRegister(REG_MCSM1,VAL_MCSM1);
	WriteRegister(REG_MCSM0,VAL_MCSM0);
	WriteRegister(REG_FOCCFG,VAL_FOCCFG);

	WriteRegister(REG_BSCFG,VAL_BSCFG);
	WriteRegister(REG_AGCCTRL2,VAL_AGCCTRL2);
	WriteRegister(REG_AGCCTRL1,VAL_AGCCTRL1);
	WriteRegister(REG_AGCCTRL0,VAL_AGCCTRL0);
	WriteRegister(REG_WOREVT1,VAL_WOREVT1);
	WriteRegister(REG_WOREVT0,VAL_WOREVT0);
	WriteRegister(REG_WORCTRL,VAL_WORCTRL);
	WriteRegister(REG_FREND1,VAL_FREND1);
	WriteRegister(REG_FREND0,VAL_FREND0);
	WriteRegister(REG_FSCAL3,VAL_FSCAL3);
	WriteRegister(REG_FSCAL2,VAL_FSCAL2);
	WriteRegister(REG_FSCAL1,VAL_FSCAL1);
	WriteRegister(REG_FSCAL0,VAL_FSCAL0);
	WriteRegister(REG_RCCTRL1,VAL_RCCTRL1);
	WriteRegister(REG_RCCTRL0,VAL_RCCTRL0);
	WriteRegister(REG_FSTEST,VAL_FSTEST);
	WriteRegister(REG_PTEST,VAL_PTEST);
	WriteRegister(REG_AGCTEST,VAL_AGCTEST);
	WriteRegister(REG_TEST2,VAL_TEST2);
	WriteRegister(REG_TEST1,VAL_TEST1);
	WriteRegister(REG_TEST0,VAL_TEST0);
}

void resetDevice(void) {
	SendStrobe(CC2500_CMD_SRES);
}

int listenForPacket(uint8_t *buf, int8_t blen, uint8_t *rssi, uint8_t *lqi) {
	WriteRegister(REG_IOCFG1,0x06);

	int PacketLength = 0;
	SendStrobe(CC2500_CMD_SRX);
	// Switch MISO to output if a packet has been received or not
	WriteRegister(REG_IOCFG1,0x01);
	delay(20);
	if (gpio_get_level(CONFIG_MISO_GPIO)) {
		PacketLength = ReadRegister(CC2500_RX_FIFO) - 1;
		ESP_LOGD(TAG, "Packet Received!");
		ESP_LOGD(TAG, "Packet Length:%d", PacketLength);
		if (PacketLength > blen) PacketLength = blen;
		//printf("Data: ");
		for(int i = 0; i < PacketLength; i++){
			buf[i] = ReadRegister(CC2500_RX_FIFO);
			//printf("%02x ", buf[i]);
		}
		//printf("\n");

		uint8_t pktctrl1 = ReadRegister(REG_PKTCTRL1);
		ESP_LOGD(TAG, "pktctrl1:%02x", pktctrl1);
		if (( pktctrl1 & 0x04) == 0x04) {
			// APPEND_STATUS
			// When enabled, two status bytes will be appended to the payload of the packet.
			// The status bytes contain RSSI and LQI values, as well as the CRC OK flag.
			*rssi = ReadRegister(CC2500_RX_FIFO);
			*lqi = ReadRegister(CC2500_RX_FIFO);
			ESP_LOGD(TAG, "RSSI: 0x%02x", *rssi);
			ESP_LOGD(TAG, "LQI: 0x%02x", *lqi);
		}
		    
		// Make sure that the radio is in IDLE state before flushing the FIFO
		// (Unless RXOFF_MODE has been changed, the radio should be in IDLE state at this point) 
		SendStrobe(CC2500_CMD_SIDLE);
		// Flush RX FIFO
		SendStrobe(CC2500_CMD_SFRX);
	} else {

	}
	return PacketLength;
}

void sendPacket(uint8_t *buf, int blen) {
	WriteRegister(REG_IOCFG1,0x06);
	// Make sure that the radio is in IDLE state before flushing the FIFO
	SendStrobe(CC2500_CMD_SIDLE);
	// Flush TX FIFO
	SendStrobe(CC2500_CMD_SFTX);
	// prepare Packet
	int length = blen + 1;
	uint8_t packet[length];
	// First Byte = Length Of Packet
	packet[0] = length;
	for (int i=0;i<blen;i++) {
		packet[i+1] = buf[i];
	}

	// SIDLE: exit RX/TX
	SendStrobe(CC2500_CMD_SIDLE);

	//Serial.println("Transmitting ");
	for(int i = 0; i < length; i++)
	{    
		WriteRegister(CC2500_TX_FIFO,packet[i]);
	}
	// STX: enable TX
	SendStrobe(CC2500_CMD_STX);
	// Wait for GDO0 to be set -> sync transmitted
	unsigned long previousTXTimeoutMillis = millis();
	while(1) {
		if (gpio_get_level(CONFIG_MISO_GPIO)) break;
		ESP_LOGD(TAG, "wait MISO HIGH");
		delayMicroseconds(10);
	}
	unsigned long elaspedTXTimeoutMillis = millis() - previousTXTimeoutMillis;
	ESP_LOGD(TAG, "elaspedTXTimeoutMillis=%lu", elaspedTXTimeoutMillis);
	   
	// Wait for GDO0 to be cleared -> end of packet
	previousTXTimeoutMillis = millis();
	while(1) {
		if (!gpio_get_level(CONFIG_MISO_GPIO)) break;
		ESP_LOGD(TAG, "wait MISO LOW");
		delayMicroseconds(10);
	}
	elaspedTXTimeoutMillis = millis() - previousTXTimeoutMillis;
	ESP_LOGD(TAG, "elaspedTXTimeoutMillis=%lu", elaspedTXTimeoutMillis);

	ESP_LOGD(TAG, "Finished sending");
	SendStrobe(CC2500_CMD_SIDLE);
}


void WriteRegister(char addr, char value) {
	gpio_set_level(CONFIG_CSN_GPIO, LOW);
	//SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
#if 0
	while (gpio_get_level(CONFIG_MISO_GPIO) == HIGH) {
	};
#endif
	while(1) {
		if (!gpio_get_level(CONFIG_MISO_GPIO)) break;
		delayMicroseconds(10);
	}

	spi_transfer(addr|CC2500_WRITE_SINGLE);
	//delay(10);
	spi_transfer(value);
	gpio_set_level(CONFIG_CSN_GPIO, HIGH);
	//SPI.endTransaction();
}

uint8_t ReadRegister(char addr) {
	gpio_set_level(CONFIG_CSN_GPIO, LOW);
	//SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
#if 0
	while (gpio_get_level(CONFIG_MISO_GPIO) == HIGH) {
	};
#endif
	while(1) {
		if (!gpio_get_level(CONFIG_MISO_GPIO)) break;
		delayMicroseconds(10);
	}

	spi_transfer(addr|CC2500_READ_SINGLE);
	//delay(10);
	uint8_t result = spi_transfer(0);
	gpio_set_level(CONFIG_CSN_GPIO, HIGH);
	//SPI.endTransaction();
	return result;  
}

uint8_t ReadStatus(uint8_t addr) {
	gpio_set_level(CONFIG_CSN_GPIO, LOW);
	//SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
#if 0
	while (gpio_get_level(CONFIG_MISO_GPIO) == HIGH) {
	};
#endif
	while(1) {
		if (!gpio_get_level(CONFIG_MISO_GPIO)) break;
		delayMicroseconds(10);
	}

	spi_transfer(addr|CC2500_READ_STATUS);
	//delay(10);
	uint8_t result = spi_transfer(0);
	gpio_set_level(CONFIG_CSN_GPIO, HIGH);
	//SPI.endTransaction();
	return result;  
}


uint8_t SendStrobe(char strobe) {
	gpio_set_level(CONFIG_CSN_GPIO, LOW);
	//SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
#if 0
	while (gpio_get_level(CONFIG_MISO_GPIO) == HIGH) {
	};
#endif
	while(1) {
		if (!gpio_get_level(CONFIG_MISO_GPIO)) break;
		delayMicroseconds(10);
	}

	uint8_t result =  spi_transfer(strobe);
	gpio_set_level(CONFIG_CSN_GPIO, HIGH);
	//SPI.endTransaction();
	//delay(10);
	return result;
}

uint8_t getChipPart(void) {
	return ReadStatus(REG_PARTNUM);
}

uint8_t getChipVersion(void) {
	return ReadStatus(REG_VERSION);
}

