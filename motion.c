/*
 * motion.c: simple userspace driver for the MOTION click board 
 *
 * This driver sets up the GPIO defined by MOTION_GPIO and monitors it
 * for an interrupt which means motion has been detected.
 *
 * This driver requires the libsoc library version 0.6.5
 *
 * Jack Mitchell <jack@embed.me.uk>, Tuxable Ltd, Embedded Linux Consultancy
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <libsoc_gpio.h>

// Linux GPIO number on which the MOTION click interrupt is connected to
#define MOTION_GPIO 27

int main() {

	// Turn libsoc debug off (0) or on (1)
	libsoc_set_debug(0);

	printf("Requesting GPIO\n");
	gpio* motion_int = libsoc_gpio_request(MOTION_GPIO, LS_SHARED);

	printf("Setting direction\n");
	libsoc_gpio_set_direction(motion_int, INPUT);

	printf("Setting edge\n");
	libsoc_gpio_set_edge(motion_int, RISING);

	printf("Waiting for interrupt\n");

	int ret;

	while (1) {

		libsoc_gpio_wait_interrupt(motion_int, -1);
		printf("Motion detected!\n");

		do {
			ret = libsoc_gpio_wait_interrupt(motion_int, 1000);
		}
		while(ret != EXIT_FAILURE); 
	}

	return 0;
}
