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

#include <tfm_veneers.h>
#include <tfm_ns_interface.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <lvgl.h>

// Threads
#include <zephyr/sys/__assert.h>
#define THREAD_STACK_SIZE 17408 // Same value as CONFIG_MAIN_STACK_SIZE
#define THREAD_PRIORITY 7

K_THREAD_STACK_DEFINE(my_thread_stack, THREAD_STACK_SIZE);

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

lv_obj_t *textArea;

static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);
	
	if(code == LV_EVENT_VALUE_CHANGED){
		printk("Value changed\n");
	}

	if(code == LV_EVENT_READY){
		printk("OK\n");
		char * msg = lv_textarea_get_text(ta);
		printk("txt: %s\n",msg);

		char buff [2048];
    	memset(buff, 0, 2048);
		keygen("", buff);
		printk("buff = %s\n", buff);

		char pk[96+2];
		memcpy(pk, buff, 98);
		printk("pk = %s\n");
    	memset(buff, 0, 2048);
		signature(pk, msg, buff);
		printk("signature = %s\n", buff);
	}

    if(code == LV_EVENT_FOCUSED) {
        lv_keyboard_set_textarea(kb, ta);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }

    if(code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

void lv_example_keyboard_1(void)
{
    /*Create a keyboard to use it with an of the text areas*/
    lv_obj_t * kb = lv_keyboard_create(lv_scr_act());

    /*Create a text area. The keyboard will write here*/
    lv_obj_t * ta;
    ta = lv_textarea_create(lv_scr_act());
    lv_obj_align(ta, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(ta, "Hello");
    lv_obj_set_size(ta, 140, 80);

    ta = lv_textarea_create(lv_scr_act());
    lv_obj_align(ta, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_set_size(ta, 140, 80);

    lv_keyboard_set_textarea(kb, ta);
}

void createTextArea(void){
	textArea = lv_textarea_create(lv_scr_act());
	lv_obj_set_size(textArea, 320, 240);
	lv_textarea_set_text(textArea, "New TextArea created");
}

void addText(char * text){
	//lv_style_t *style = lv_textarea_get_style(textArea);
	//style->text.color = LV_COLOR_RED;       // Establecer el color de texto en rojo

	lv_textarea_add_text(textArea, text);
}

uint8_t buf[MAXBUF];
int len = 0;

#define LV_HOR_RES_MAX 320
#define LV_VER_RES_MAX 240

static void event_handler_button(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        printk("Clicked");
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        printk("Toggled");
    }
}

static void signature_event_handler(lv_event_t * e){
	if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        printk("Signature\n");
    }
}

void lvgl_cli(){
	lv_obj_t * label;

    lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn1, event_handler_button, LV_EVENT_ALL, NULL);
    lv_obj_align(btn1, LV_ALIGN_TOP_LEFT, 5, 5);

    label = lv_label_create(btn1);
    lv_label_set_text(label, "Keygen");
    lv_obj_center(label);

    lv_obj_t * btn2 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn2, event_handler_button, LV_EVENT_ALL, NULL);
    lv_obj_align(btn2, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height(btn2, LV_SIZE_CONTENT);

    label = lv_label_create(btn2);
    lv_label_set_text(label, "Keys");
    lv_obj_center(label);

	lv_obj_t * btn3 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn3, signature_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn3, LV_ALIGN_TOP_RIGHT, -5, 5);

    label = lv_label_create(btn3);
    lv_label_set_text(label, "Signature");
    lv_obj_center(label);
}

void request_password(){
	int cont = 1;
	const struct device *display_dev;

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		printk("[request_password]: Device not ready, aborting test\n");
		return;
	}

	printk("[request_password]: Requesting password via touchscreen\n");

	// Create a keyboard to use it with the text area
    lv_obj_t * kb = lv_keyboard_create(lv_scr_act());

    // Create a text area. The keyboard will write here
    lv_obj_t * ta;
    ta = lv_textarea_create(lv_scr_act());
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(ta, "Hello");
    lv_obj_set_size(ta, 280, 80);

    lv_keyboard_set_textarea(kb, ta);

	// Display on screen
	lv_task_handler();
	display_blanking_off(display_dev);

	while (1) {
		lv_task_handler();
		cont++;
		k_sleep(K_MSEC(10));
	}
}

void lvgl_foo(){
	// ---------------------------------------------------------------------------

	printk("LVGL\n");
	int cont = 1;
	const struct device *display_dev;

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		printk("Device not ready, aborting test\n");
		return;
	}

	//lv_example_keyboard_1();
	//createTextArea();
	//addText("\n\nsignature 86a722b1f5c1cb1420ff0766cf5205b023de2e9c69efc65dbf976af2d710c3d12f937cf7104c9cd51bb4c62ff185d07f 5656565656565656565656565656565656565656565656565656565656565656\nSignature: 0xb912c912616709f6a190b03db1a259ca21f535abe51f88d6c95407a81fd8648b067c5e0548587f6a84f2dea9afd2098812bb1d7fb188f1d04411a04f25042b627c5f8d60dcef6416072cfef40b799b3c89397bcddf69ae62611484bfc6e83689\n");
	//addText("\nverify 0x86a722b1f5c1cb1420ff0766cf5205b023de2e9c69efc65dbf976af2d710c3d12f937cf7104c9cd51bb4c62ff185d07f 5656565656565656565656565656565656565656565656565656565656565656 0xb912c912616709f6a190b03db1a259ca21f535abe51f88d6c95407a81fd8648b067c5e0548587f6a84f2dea9afd2098812bb1d7fb188f1d04411a04f25042b627c5f8d60dcef6416072cfef40b799b3c89397bcddf69ae62611484bfc6e83689\nSuccess");
	//lvgl_cli();
	request_password();

	lv_task_handler();
	display_blanking_off(display_dev);

	while (1) {
		/*if ((cont % 400) == 0U) {
			printf("tic\n");
		}else if((cont % 200) == 0U){
			printf("tac\n");
			cont = 1;
		}*/
		lv_task_handler();
		cont++;
		k_sleep(K_MSEC(10));
	}

	printk("End of LVGL\n");

// -------------
}

void display_manager(){
	printk("[myThread]: I'm alive!\n");
	request_password();
}

K_THREAD_DEFINE(thread_id, THREAD_STACK_SIZE, display_manager, NULL, NULL, NULL, THREAD_PRIORITY, 0, 0);

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
	tfm_init();

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
