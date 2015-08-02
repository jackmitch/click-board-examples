/*
 * proximity.c: simple userspace driver for the Proximity click board 
 *
 * Reads the full set of registers from I2C device and triggers an ambient
 * light sensor reading, followed by a proximity sensor reading. 
 *
 * This driver requires the libsoc library version 0.6.5
 *
 * Jack Mitchell <jack@embed.me.uk>, Tuxable Ltd, Embedded Linux Consultancy
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <libsoc_i2c.h>

#define PROXIMITY_I2C_BUS 2
#define PROXIMITY_I2C_ADDR 0x13

#define NUM_REGISTERS 17
#define REG_START_ADDR 0x80

#define VCNL4000_COMMAND        0x80 /* Command register */
#define VCNL4000_PROD_REV       0x81 /* Product ID and Revision ID */
#define VCNL4000_LED_CURRENT    0x83 /* IR LED current for proximity mode */
#define VCNL4000_AL_PARAM       0x84 /* Ambient light parameter register */
#define VCNL4000_AL_RESULT_HI   0x85 /* Ambient light result register, MSB */
#define VCNL4000_AL_RESULT_LO   0x86 /* Ambient light result register, LSB */
#define VCNL4000_PS_RESULT_HI   0x87 /* Proximity result register, MSB */
#define VCNL4000_PS_RESULT_LO   0x88 /* Proximity result register, LSB */

/* Bit masks for COMMAND register */
#define VCNL4000_AL_RDY         0x40 /* ALS data ready? */
#define VCNL4000_PS_RDY         0x20 /* proximity data ready? */
#define VCNL4000_AL_OD          0x10 /* start on-demand ALS measurement */
#define VCNL4000_PS_OD          0x08 /* start on-demand proximity measurement */

int main() {

	// Turn libsoc debug off (0) or on (1)
	libsoc_set_debug(0);

	i2c* prox_i2c = libsoc_i2c_init(PROXIMITY_I2C_BUS, PROXIMITY_I2C_ADDR);

	libsoc_i2c_set_timeout(prox_i2c, 1);

	int i;
	uint8_t buffer[NUM_REGISTERS];

	buffer[0] = 0x81;
 
	libsoc_i2c_write(prox_i2c, buffer, 1);
	libsoc_i2c_read(prox_i2c, buffer, 1);

	if (buffer[0] == 0x21) {

		printf("Found Proximity Click board\n");

	}

	buffer[0] = REG_START_ADDR;

	libsoc_i2c_write(prox_i2c, buffer, 1);

	printf("Current register status::\n");
	
	for (i = 0; i < NUM_REGISTERS; i++) {

		libsoc_i2c_read(prox_i2c, buffer, 1);
		printf("0x%02X: 0x%02X\n", (REG_START_ADDR + i), buffer[0]);
	}	

	printf("Requesting ambient light sensor value\n");	

	buffer[0] = VCNL4000_COMMAND;
	buffer[1] = VCNL4000_AL_OD;

	libsoc_i2c_write(prox_i2c, buffer, 2);

	buffer[1] = VCNL4000_AL_RDY; 

	while (1) {

		libsoc_i2c_write(prox_i2c, buffer, 1);
		libsoc_i2c_read(prox_i2c, &buffer[1], 1);

		if (buffer[1] & VCNL4000_AL_RDY) {
			break;
		}

		usleep(20);
	}	

	buffer[0] = VCNL4000_AL_RESULT_HI;
	libsoc_i2c_write(prox_i2c, buffer, 1);
	libsoc_i2c_read(prox_i2c, &buffer[1], 2);

	uint16_t light_value = (buffer[1] << 8) | buffer[2];
	printf("val: %04X (%d)\n", light_value, light_value);

	printf("Requesting proximity sensor value\n");	

	buffer[0] = VCNL4000_COMMAND;
	buffer[1] = VCNL4000_PS_OD;

	libsoc_i2c_write(prox_i2c, buffer, 2);

	buffer[1] = VCNL4000_PS_RDY; 

	while (1) {

		libsoc_i2c_write(prox_i2c, buffer, 1);
		libsoc_i2c_read(prox_i2c, &buffer[1], 1);

		if (buffer[1] & VCNL4000_PS_RDY) {
			break;
		}

		usleep(20);
	}	

	buffer[0] = VCNL4000_PS_RESULT_HI;
	libsoc_i2c_write(prox_i2c, buffer, 1);
	libsoc_i2c_read(prox_i2c, &buffer[1], 2);

	uint16_t prox_value = (buffer[1] << 8) | buffer[2];
	printf("val: %04X (%d)\n", prox_value, prox_value);


	return 0;
}
