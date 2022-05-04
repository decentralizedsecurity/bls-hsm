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
#include <device.h>
#include <drivers/uart.h>
#include <zephyr.h>
#include <picohttpparser.h>
#include <common.h>
#include <bls_hsm_ns.h>
#include <httpRemote.h>

#include <usb/usb_device.h>
#include <logging/log.h>
LOG_MODULE_REGISTER(remote_signer, LOG_LEVEL_INF);

uint8_t buf[MAXBUF];
int len = 0;

void main(void)
{
	const struct device *dev;
	uint32_t baudrate, dtr = 0U;
	int ret;

	dev = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);
	if (!device_is_ready(dev)) {
		LOG_ERR("CDC ACM device not ready");
		return;
	}

	ret = usb_enable(NULL);
	if (ret != 0) {
		LOG_ERR("Failed to enable USB");
		return;
	}

	LOG_INF("Wait for DTR");

	while (true) {
		uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
		if (dtr) {
			break;
		} else {
			/* Give CPU resources to low priority threads. */
			k_sleep(K_MSEC(100));
		}
	}

	LOG_INF("DTR set");

	/* They are optional, we use them to test the interrupt endpoint */
	ret = uart_line_ctrl_set(dev, UART_LINE_CTRL_DCD, 1);
	if (ret) {
		LOG_WRN("Failed to set DCD, ret code %d", ret);
	}

	ret = uart_line_ctrl_set(dev, UART_LINE_CTRL_DSR, 1);
	if (ret) {
		LOG_WRN("Failed to set DSR, ret code %d", ret);
	}

	/* Wait 1 sec for the host to do all settings */
	k_busy_wait(1000000);

	ret = uart_line_ctrl_get(dev, UART_LINE_CTRL_BAUD_RATE, &baudrate);
	if (ret) {
		LOG_WRN("Failed to get baudrate, ret code %d", ret);
	} else {
		LOG_INF("Baudrate detected: %d", baudrate);
	}

	struct boardRequest reply;
	import("3604f89dbd5161c5b214dd15afab350048a9c74ca1f657d93b02d14088f55ad0", buf);
	import("4604f89dbd5161c5b214dd15afab350048a9c74ca1f657d93b02d14088f55ad0", buf);

	while(1){
		uart_irq_rx_enable(dev);
		while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
			if (uart_irq_rx_ready(dev)) {
				int recv_len;
				recv_len = uart_fifo_read(dev, buf + len, MAXBUF - len);
				len += recv_len;
			}
		}
		uart_irq_rx_disable(dev);
		int pr = parseRequest(buf, len, &reply);
		if(pr == 0){
			len = dumpHttpResponse(buf, &reply);
			if(len == -1){
				memset(buf, 0, MAXBUF);
				strcpy(buf, badRequest);
				len = strlen(badRequest);
			}
			uart_irq_tx_enable(dev);
			while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
				if (uart_irq_tx_ready(dev)) {
					int send_len = uart_fifo_fill(dev, buf, len);
					LOG_INF("len: %d | send_len: %d | %d", len, send_len, len == send_len);
					len -= send_len;
					if(len == 0){
						uart_irq_tx_disable(dev);
						memset(buf, 0, MAXBUF);
					}
				}
			}
		}else if(pr == -1){
			len = 0;
		}else if(pr == -3){
			memset(buf, 0, MAXBUF);
			strcpy(buf, badRequest);
			len = strlen(badRequest);
			uart_irq_tx_enable(dev);
			while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
				if (uart_irq_tx_ready(dev)) {
					int send_len = uart_fifo_fill(dev, buf, len);
					LOG_INF("len: %d | send_len: %d | %d", len, send_len, len == send_len);
					len -= send_len;
					if(len == 0){
						uart_irq_tx_disable(dev);
						memset(buf, 0, MAXBUF);
					}
				}
			}
		}
	}
}
