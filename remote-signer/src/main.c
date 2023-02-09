/*
 * Copyright (c) 2019 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Sample echo app for CDC ACM class
 *
 * Sample app for USB CDC ACM class driver. The received data is echoed back
 * to the serial port.
 */

#include <stdio.h>
#include <string.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <picohttpparser.h>
#include <common.h>
#include <bls_hsm_ns.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(remote_signer, LOG_LEVEL_INF);
#include <httpRemote.h>

#ifndef NET
#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>
#else
#include <zephyr/net/socket.h>
#include <modem/nrf_modem_lib.h>
#include <nrf_modem_at.h>
#include <modem/lte_lc.h>
#include <zephyr/net/tls_credentials.h>
#include <modem/modem_key_mgmt.h>
#include "tcp.h"

#ifdef THINGY91
#include <zephyr/drivers/gpio.h>
#include <dk_buttons_and_leds.h>
#include <time.h>

#define RED_NODE DT_ALIAS(led0)
#define GREEN_NODE DT_ALIAS(led1)
#define BLUE_NODE DT_ALIAS(led2)

#define LED0	DT_GPIO_LABEL(RED_NODE, gpios)
#define PIN0	DT_GPIO_PIN(RED_NODE, gpios)
#define FLAGS0	DT_GPIO_FLAGS(RED_NODE, gpios)

#define LED1	DT_GPIO_LABEL(GREEN_NODE, gpios)
#define PIN1	DT_GPIO_PIN(GREEN_NODE, gpios)
#define FLAGS1	DT_GPIO_FLAGS(GREEN_NODE, gpios)

#define LED2	DT_GPIO_LABEL(BLUE_NODE, gpios)
#define PIN2	DT_GPIO_PIN(BLUE_NODE, gpios)
#define FLAGS2	DT_GPIO_FLAGS(BLUE_NODE, gpios)

static const struct device *leds[3];

enum { RED, GREEN, BLUE };

bool pressed = false;
bool on = false;

bool red, green, blue = false;

int leds_init(){
	int err;

	leds[RED] = device_get_binding(LED0);
	if (leds[RED] == NULL) {
		return -1;
	}

	leds[GREEN] = device_get_binding(LED1);
	if (leds[GREEN] == NULL) {
		return -1;
	}

	leds[BLUE] = device_get_binding(LED2);
	if (leds[BLUE] == NULL) {
		return -1;
	}

	err = gpio_pin_configure(leds[RED], PIN0, GPIO_OUTPUT_ACTIVE | FLAGS0);
	if (err < 0) {
		return -1;
	}

	err = gpio_pin_configure(leds[GREEN], PIN1, GPIO_OUTPUT_ACTIVE | FLAGS1);
	if (err < 0) {
		return -1;
	}

	err = gpio_pin_configure(leds[BLUE], PIN2, GPIO_OUTPUT_ACTIVE | FLAGS2);
	if (err < 0) {
		return -1;
	}


	gpio_pin_set(leds[RED], PIN0, false);
	gpio_pin_set(leds[GREEN], PIN1, false);
	gpio_pin_set(leds[BLUE], PIN2, false);

	return 0;
}

void button_callback(uint32_t button_state, uint32_t has_changed){
	if (((has_changed & DK_BTN1_MSK) != 0) && (button_state)){
		pressed = true;
	}
}

void blink_callback(struct k_timer *timer){

	gpio_pin_set(leds[RED], PIN0, on && red);
	gpio_pin_set(leds[GREEN], PIN1, on && green);
	gpio_pin_set(leds[BLUE], PIN2, on && blue);

	on = !on;
}

void set_leds(bool r, bool g, bool b){
	gpio_pin_set(leds[RED], PIN0, r);
	gpio_pin_set(leds[GREEN], PIN1, g);
	gpio_pin_set(leds[BLUE], PIN2, b);
}

void set_blink(bool r, bool g, bool b){
	red = r;
	green = g;
	blue = b;
}
#endif

#define TCP_PORT 9090
#define HOSTNAME "188.86.154.35"
#endif

uint8_t buf[MAXBUF];
int len = 0;

void main(void)
{
#ifndef NET
	const struct device *dev;
	uint32_t baudrate, dtr = 0U;
	int ret;

	dev = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);
	if (!device_is_ready(dev)) {
		//LOG_ERR("CDC ACM device not ready");
		return;
	}

	ret = usb_enable(NULL);
	if (ret != 0) {
		//LOG_ERR("Failed to enable USB");
		return;
	}

	//LOG_INF("Wait for DTR");

	while (true) {
		uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
		if (dtr) {
			break;
		} else {
			/* Give CPU resources to low priority threads. */
			k_sleep(K_MSEC(100));
		}
	}

	//LOG_INF("DTR set");

	/* They are optional, we use them to test the interrupt endpoint */
	ret = uart_line_ctrl_set(dev, UART_LINE_CTRL_DCD, 1);
	if (ret) {
		//LOG_WRN("Failed to set DCD, ret code %d", ret);
	}

	ret = uart_line_ctrl_set(dev, UART_LINE_CTRL_DSR, 1);
	if (ret) {
		//LOG_WRN("Failed to set DSR, ret code %d", ret);
	}

	/* Wait 1 sec for the host to do all settings */
	k_busy_wait(1000000);

	ret = uart_line_ctrl_get(dev, UART_LINE_CTRL_BAUD_RATE, &baudrate);
	if (ret) {
		//LOG_WRN("Failed to get baudrate, ret code %d", ret);
	} else {
		//LOG_INF("Baudrate detected: %d", baudrate);
	}
#else
#ifdef THINGY91
	//Inicialización de los leds RGB
	if (leds_init() != 0) {
		LOG_ERR("Failed to initialize leds");
		return 0;
	}
#endif
	modem_configure();

	struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
	};

	struct addrinfo *sin;

	int err = getaddrinfo(HOSTNAME, NULL, &hints, &sin);
	if (err) {
		set_leds(true, false, false);
		printk("getaddrinfo() failed, err %d\n", errno);
		return 0;
	}

	((struct sockaddr_in *)sin->ai_addr)->sin_port = htons(TCP_PORT);

	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TLS_1_2);

	if (fd < 0) {
		set_leds(true, false, false);
		printk("socket() failed\n");
		return 0;
	}

	printk("Connecting to the bridge...\n");
	err = connect(fd, sin->ai_addr, sizeof(struct sockaddr_in));
	if ( err ) {
		set_leds(true, false, false);
		printk("connect() failed.\n");
		return 0;
	}
	
	printk("Connected\n");
#endif

	struct boardRequest reply;
	InitZeroHashes(64);
	//tfm_init();

	while(1){
#ifndef NET
		uart_irq_rx_enable(dev);
		while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
			if (uart_irq_rx_ready(dev)) {
				int recv_len;
				recv_len = uart_fifo_read(dev, buf + len, MAXBUF - len);
				len += recv_len;
			}
		}
		uart_irq_rx_disable(dev);
#else
		int recv_len = recv_tcp(fd, buf + len, MAXBUF - len);
		len += recv_len;
#endif
		int pr = parseRequest(buf, len, &reply);
		if(pr == 0){
			len = dumpHttpResponse(buf, &reply);
			if(len == -1){
				memset(buf, 0, MAXBUF);
				strcpy(buf, badRequest);
				len = strlen(badRequest);
			}
			//LOG_INF("REQUEST PROCESSED");
#ifndef NET
			uart_irq_tx_enable(dev);
			while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
				if (uart_irq_tx_ready(dev)) {
					int send_len = uart_fifo_fill(dev, buf, len);
					len -= send_len;
					if(len == 0){
						uart_irq_tx_disable(dev);
						memset(buf, 0, MAXBUF);
					}
				}
			}
#else
			int send_len = send_tcp(fd, buf, len);
			if(send_len != 0){
				LOG_INF("FAILED SENDING RESPONSE");
			} else {
				memset(buf, 0, MAXBUF);
			}
#endif
		}else if(pr == -1){
			len = 0;
		}else if(pr == -3){
			memset(buf, 0, MAXBUF);
			strcpy(buf, badRequest);
			len = strlen(badRequest);
#ifndef NET
			uart_irq_tx_enable(dev);
			while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
				if (uart_irq_tx_ready(dev)) {
					int send_len = uart_fifo_fill(dev, buf, len);
					len -= send_len;
					if(len == 0){
						uart_irq_tx_disable(dev);
						memset(buf, 0, MAXBUF);
					}
				}
			}
#else
			int send_len = send_tcp(fd, buf, len);
			if(send_len != 0){
				LOG_INF("FAILED SENDING RESPONSE");
			} else {
				memset(buf, 0, MAXBUF);
			}
#endif
		}
	}
}
