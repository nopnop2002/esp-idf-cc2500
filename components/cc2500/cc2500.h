#ifndef cc2500_h
#define cc2500_h

#include <driver/spi_master.h>

#include "cc2500_REG.h"
#include "cc2500_VAL.h"

//#include <SPI.h>

// access types
#define CC2500_WRITE_SINGLE	0x00
#define CC2500_WRITE_BURST	0x40
#define CC2500_READ_SINGLE	0x80
#define CC2500_READ_BURST	0xC0
#define CC2500_READ_STATUS	0xC0


// command strobes
#define CC2500_CMD_SRES		0x30	// Reset chip
#define CC2500_CMD_SFSTXON	0x31	// Enable and calibrate frequency synthesizer
#define CC2500_CMD_SXOFF	0x32	// Turn off crystal oscillator
#define CC2500_CMD_SCAL		0x33	// Calibrate frequency synthesizer and turn it off
#define CC2500_CMD_SRX		0x34	// Enable RX
#define CC2500_CMD_STX		0x35	// Enable TX
#define CC2500_CMD_SIDLE	0x36	// Exit Radio mode
#define CC2500_CMD_SWOR		0x38	// Start automatic RX polling
#define CC2500_CMD_SPWD		0x39	// Enter power down mode when CSn goes high.
#define CC2500_CMD_SFRX		0x3A	// Flush the RX FIFO buffer.
#define CC2500_CMD_SFTX		0x3B	// Flush the TX FIFO buffer.
#define CC2500_CMD_SWORRST	0x3C	// Reset real time clock
#define CC2500_CMD_SNOP		0x3D	// No operation. May be used to get access to the chip status byte.

// multi-byte registers
#define CC2500_PATABLE		0x3E	// power output table
#define CC2500_TX_FIFO		0x3F	// transmit FIFO
#define CC2500_RX_FIFO		0x3F	// receive FIFO

#define delayMicroseconds(microsec) esp_rom_delay_us(microsec)
#define delay(millsec) esp_rom_delay_us(millsec*1000)
#define millis() xTaskGetTickCount()*portTICK_PERIOD_MS

#define LOW  0
#define HIGH 1

#ifndef __MAIN__
	extern spi_device_handle_t _handle;
#else
	spi_device_handle_t _handle;
#endif

bool    begin(uint8_t);
void    sendPacket(uint8_t *buf, int blen);
int	    listenForPacket(uint8_t *buf, int8_t blen, uint8_t *rssi, uint8_t *lqi);


uint8_t getChipPart(void);
uint8_t getChipVersion(void);
void    init(uint8_t channel);
void    resetDevice(void);
void    WriteRegister(char addr, char value);
uint8_t ReadRegister(char addr);
uint8_t ReadStatus(uint8_t addr);
uint8_t SendStrobe(char strobe);

#endif

