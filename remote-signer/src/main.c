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

#include <zephyr/drivers/display.h>
#include <lvgl.h>

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
#include <tiny-json.h>

//#include <tfm_ns_interface.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

// Threads
#include <zephyr/sys/__assert.h>
#define THREAD_STACK_SIZE 17408 // Same value as CONFIG_MAIN_STACK_SIZE
#define THREAD_PRIORITY 7

K_THREAD_STACK_DEFINE(my_thread_stack, THREAD_STACK_SIZE);

#ifndef NET
#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>
#endif

uint8_t buf[MAXBUF];
int len = 0;

#include <stdlib.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/conn_mgr_monitor.h>
#include <zephyr/net/conn_mgr_connectivity.h>
#include <zephyr/net/tls_credentials.h>
#include <net/wifi_credentials.h>
#include <zephyr_wifi_mgmt.h>
#include "net_private.h"

#define WIFI_SHELL_MGMT_EVENTS (NET_EVENT_WIFI_CONNECT_RESULT |		\
				NET_EVENT_WIFI_DISCONNECT_RESULT)

#define MAX_SSID_LEN        32
#define STATUS_POLLING_MS   300

#define CONFIG_STA_CONN_TIMEOUT_SEC 70

//! TCP Server port
#define TCP_SERVER_PORT 4242
//! TCP server connection check interval in miliseconds
#define TCP_SERVER_SLEEP_TIME_MS 10

static struct net_mgmt_event_callback wifi_shell_mgmt_cb;
static struct net_mgmt_event_callback net_shell_mgmt_cb;

static struct {
	const struct shell *sh;
	union {
		struct {
			uint8_t connected	: 1;
			uint8_t connect_result	: 1;
			uint8_t disconnect_requested	: 1;
			uint8_t _unused		: 5;
		};
		uint8_t all;
	};
} context;

//! Receiver buffer
static uint8_t receiverBuffer[128];

#define HTTPS_PORT		"443"
#define HTTPS_HOSTNAME		"example.com"

#define RECV_BUF_SIZE		2048
#define TLS_SEC_TAG		42

static const char cert[] = {
"TODO"
};

BUILD_ASSERT(sizeof(cert) < KB(4), "Certificate too large");

/* Provision certificate to modem */
int cert_provision(void)
{
	int err;

	printk("Provisioning certificate\n");

	err = tls_credential_add(TLS_SEC_TAG,
				 TLS_CREDENTIAL_CA_CERTIFICATE,
				 cert,
				 sizeof(cert));
	if (err < 0) {
		printk("Failed to register CA certificate: %d\n", err);
		return err;
	}

	return 0;
}


/* Setup TLS options on a given socket */
int tls_setup(int fd)
{
	int err;
	int verify;

	/* Security tag that we have provisioned the certificate with */
	const sec_tag_t tls_sec_tag[] = {
		TLS_SEC_TAG,
	};

	/* Set up TLS peer verification */
	enum {
		NONE = 0,
		OPTIONAL = 1,
		REQUIRED = 2,
	};

	verify = REQUIRED;

	err = setsockopt(fd, SOL_TLS, TLS_PEER_VERIFY, &verify, sizeof(verify));
	if (err) {
		printk("Failed to setup peer verification, err %d\n", errno);
		return err;
	}

	/* Associate the socket with the security tag
	 * we have provisioned the certificate with.
	 */
	err = setsockopt(fd, SOL_TLS, TLS_SEC_TAG_LIST, tls_sec_tag, sizeof(tls_sec_tag));
	if (err) {
		printk("Failed to setup TLS sec tag, err %d\n", errno);
		return err;
	}

	err = setsockopt(fd, SOL_TLS, TLS_HOSTNAME, HTTPS_HOSTNAME, sizeof(HTTPS_HOSTNAME) - 1);
	if (err) {
		printk("Failed to setup TLS hostname, err %d\n", errno);
		return err;
	}
	return 0;
}

static int cmd_wifi_status(void)
{
	struct net_if *iface = net_if_get_default();
	struct wifi_iface_status status = { 0 };

	if (net_mgmt(NET_REQUEST_WIFI_IFACE_STATUS, iface, &status,
				sizeof(struct wifi_iface_status))) {
		LOG_INF("Status request failed");

		return -ENOEXEC;
	}

	LOG_INF("==================");
	LOG_INF("State: %s", wifi_state_txt(status.state));

	if (status.state >= WIFI_STATE_ASSOCIATED) {
		uint8_t mac_string_buf[sizeof("xx:xx:xx:xx:xx:xx")];

		LOG_INF("Interface Mode: %s",
		       wifi_mode_txt(status.iface_mode));
		LOG_INF("Link Mode: %s",
		       wifi_link_mode_txt(status.link_mode));
		LOG_INF("SSID: %-32s", status.ssid);
		LOG_INF("BSSID: %s",
		       net_sprint_ll_addr_buf(
				status.bssid, WIFI_MAC_ADDR_LEN,
				mac_string_buf, sizeof(mac_string_buf)));
		LOG_INF("Band: %s", wifi_band_txt(status.band));
		LOG_INF("Channel: %d", status.channel);
		LOG_INF("Security: %s", wifi_security_txt(status.security));
		LOG_INF("MFP: %s", wifi_mfp_txt(status.mfp));
		LOG_INF("RSSI: %d", status.rssi);
	}
	return 0;
}

static void handle_wifi_connect_result(struct net_mgmt_event_callback *cb)
{
	const struct wifi_status *status =
		(const struct wifi_status *) cb->info;

	if (context.connected) {
		return;
	}

	if (status->status) {
		LOG_ERR("Connection failed (%d)", status->status);
	} else {
		LOG_INF("Connected");
		context.connected = true;
	}

	context.connect_result = true;
}

static void handle_wifi_disconnect_result(struct net_mgmt_event_callback *cb)
{
	const struct wifi_status *status =
		(const struct wifi_status *) cb->info;

	if (!context.connected) {
		return;
	}

	if (context.disconnect_requested) {
		LOG_INF("Disconnection request %s (%d)",
			 status->status ? "failed" : "done",
					status->status);
		context.disconnect_requested = false;
	} else {
		LOG_INF("Received Disconnected");
		context.connected = false;
	}

	cmd_wifi_status();
}

static void wifi_mgmt_event_handler(struct net_mgmt_event_callback *cb,
				     uint32_t mgmt_event, struct net_if *iface)
{
	switch (mgmt_event) {
	case NET_EVENT_WIFI_CONNECT_RESULT:
		handle_wifi_connect_result(cb);
		break;
	case NET_EVENT_WIFI_DISCONNECT_RESULT:
		handle_wifi_disconnect_result(cb);
		break;
	default:
		break;
	}
}

bool ipSet = false;

static void print_dhcp_ip(struct net_mgmt_event_callback *cb)
{
	/* Get DHCP info from struct net_if_dhcpv4 and print */
	const struct net_if_dhcpv4 *dhcpv4 = cb->info;
	const struct in_addr *addr = &dhcpv4->requested_ip;
	char dhcp_info[128];

	net_addr_ntop(AF_INET, addr, dhcp_info, sizeof(dhcp_info));

	LOG_INF("DHCP IP address: %s", dhcp_info);
	ipSet = true;
}
static void net_mgmt_event_handler(struct net_mgmt_event_callback *cb,
				    uint32_t mgmt_event, struct net_if *iface)
{
	switch (mgmt_event) {
	case NET_EVENT_IPV4_DHCP_BOUND:
		print_dhcp_ip(cb);
		break;
	default:
		break;
	}
}

static int __wifi_args_to_params(struct wifi_connect_req_params *params)
{

	params->timeout =  CONFIG_STA_CONN_TIMEOUT_SEC * MSEC_PER_SEC;

	if (params->timeout == 0) {
		params->timeout = SYS_FOREVER_MS;
	}

	/* SSID */
	params->ssid = CONFIG_STA_SAMPLE_SSID;
	params->ssid_length = strlen(params->ssid);

#if defined(CONFIG_STA_KEY_MGMT_WPA2)
	params->security = 1;
#elif defined(CONFIG_STA_KEY_MGMT_WPA2_256)
	params->security = 2;
#elif defined(CONFIG_STA_KEY_MGMT_WPA3)
	params->security = 3;
#else
	params->security = 0;
#endif

#if !defined(CONFIG_STA_KEY_MGMT_NONE)
	params->psk = CONFIG_STA_SAMPLE_PASSWORD;
	params->psk_length = strlen(params->psk);
#endif
	params->channel = WIFI_CHANNEL_ANY;

	/* MFP (optional) */
	params->mfp = WIFI_MFP_OPTIONAL;

	return 0;
}

static int wifi_connect(void)
{
	struct net_if *iface = net_if_get_default();
	static struct wifi_connect_req_params cnx_params;

	context.connected = false;
	context.connect_result = false;
	__wifi_args_to_params(&cnx_params);

	if (net_mgmt(NET_REQUEST_WIFI_CONNECT, iface,
		     &cnx_params, sizeof(struct wifi_connect_req_params))) {
		LOG_ERR("Connection request failed");

		return -ENOEXEC;
	}

	LOG_INF("Connection requested");

	return 0;
}

int bytes_from_str(const char *str, uint8_t *bytes, size_t bytes_len)
{
	size_t i;
	char byte_str[3];

	if (strlen(str) != bytes_len * 2) {
		LOG_ERR("Invalid string length: %zu (expected: %d)\n",
			strlen(str), bytes_len * 2);
		return -EINVAL;
	}

	for (i = 0; i < bytes_len; i++) {
		memcpy(byte_str, str + i * 2, 2);
		byte_str[2] = '\0';
		bytes[i] = strtol(byte_str, NULL, 16);
	}

	return 0;
}

static uint32_t count;

#ifdef CONFIG_GPIO
static struct gpio_dt_spec button_gpio = GPIO_DT_SPEC_GET_OR(
		DT_ALIAS(sw0), gpios, {0});
static struct gpio_callback button_callback;

static void button_isr_callback(const struct device *port,
				struct gpio_callback *cb,
				uint32_t pins)
{
	ARG_UNUSED(port);
	ARG_UNUSED(cb);
	ARG_UNUSED(pins);

	count = 0;
}
#endif

static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);
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

// ===================================================================================================
static lv_obj_t * list1;
static lv_obj_t * info;
static lv_obj_t * currentButton = NULL;

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        printk("Clicked: %s\n", lv_list_get_btn_text(list1, obj));

        if(currentButton == obj) {
            currentButton = NULL;
        }
        else {
            currentButton = obj;
        }
        lv_obj_t * parent = lv_obj_get_parent(obj);
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(parent); i++) {
            lv_obj_t * child = lv_obj_get_child(parent, i);
            if(child == currentButton) {
                lv_obj_add_state(child, LV_STATE_CHECKED);
            }
            else {
                lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }
    }
}

void menu1(void)
{
    /*Create a Tab view object*/
    lv_obj_t * tabview;
    tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 38);

	lv_obj_t * tab_btns = lv_tabview_get_tab_btns(tabview);
    lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
    lv_obj_set_style_text_color(tab_btns, lv_palette_lighten(LV_PALETTE_GREY, 5), 0);
    lv_obj_set_style_border_side(tab_btns, LV_BORDER_SIDE_BOTTOM, LV_PART_ITEMS | LV_STATE_CHECKED);

    /*Add 4 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Info");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Signature");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "Log");
    lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "Stats");

    /*Add content to the tabs*/
    lv_obj_t * label = lv_label_create(tab1);
    lv_label_set_text(label, "Remote-signer\n\n"
							 "The aim of this project is to develop a\n"
							 "working prototype for an Eth 2.0 remote\n"
							 "signer that follows EIP-3030/Web3Signer \n"
							 "ETH2 Api for the ARM Cortex-M33 architecture.\n"
							 "We take advantage of ARM TrustZone to protect\n"
							 "the private keys of the validator, critical\n"
							 "parts of the code that require access to private.\n"
							 "keys are run in the secure world. Our prototype is\n"
							 "developed specifically for the nRF9160DK and\n"
							 "nRF5340DK boards, although it should be easy\n"
							 "to port to other boards using an ARM Cortex-M33.\n"
							 "We use the blst library by suprational to implement\n"
							 "the BLS signature and related methods");

    //label = lv_label_create(tab2);
    //lv_label_set_text(label, "Signature?");

    label = lv_label_create(tab3);
    lv_label_set_text(label, "This is a log");

	label = lv_label_create(tab4);
    lv_label_set_text(label, "Number of pwd: 0");

    //lv_obj_scroll_to_view_recursive(label, LV_ANIM_ON);

	lv_obj_clear_flag(lv_tabview_get_content(tabview), LV_OBJ_FLAG_SCROLLABLE);

	// Create a list
    list1 = lv_list_create(tab2);
    lv_obj_set_size(list1, lv_pct(60), lv_pct(100));
    lv_obj_set_style_pad_row(list1, 5, 0);

    // Add buttons to the list
    lv_obj_t * btn;
    int i;
    for(i = 0; i < 10; i++) {
        btn = lv_btn_create(list1);
        lv_obj_set_width(btn, lv_pct(50));
		lv_obj_set_width(btn, lv_pct(100));
        lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

        lv_obj_t * lab = lv_label_create(btn);
        lv_label_set_text_fmt(lab, "Public key %d: [...]", i);
    }

    // Select the first button by default
    currentButton = lv_obj_get_child(list1, 0);
    lv_obj_add_state(currentButton, LV_STATE_CHECKED);/**/

	// Create info label
	info = lv_label_create(tab2);
    lv_obj_set_size(info, lv_pct(40), lv_pct(100));
    lv_obj_align(info, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_flex_flow(info, LV_FLEX_FLOW_COLUMN);
	lv_label_set_text(info, "Public key:\n"
							 "ae24....7eec2\n"
							 "\n"
							 "More info...");

}

void main(void)
{
	int err;
	char count_str[11] = {0};
	const struct device *display_dev;
	lv_obj_t *hello_world_label;
	lv_obj_t *count_label;

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return;
	}

#ifdef CONFIG_GPIO
	if (device_is_ready(button_gpio.port)) {
		err = gpio_pin_configure_dt(&button_gpio, GPIO_INPUT);
		if (err) {
			LOG_ERR("failed to configure button gpio: %d", err);
			return;
		}

		gpio_init_callback(&button_callback, button_isr_callback,
				   BIT(button_gpio.pin));

		err = gpio_add_callback(button_gpio.port, &button_callback);
		if (err) {
			LOG_ERR("failed to add button callback: %d", err);
			return;
		}

		err = gpio_pin_interrupt_configure_dt(&button_gpio,
						      GPIO_INT_EDGE_TO_ACTIVE);
		if (err) {
			LOG_ERR("failed to enable button callback: %d", err);
			return;
		}
	}
#endif

	/*if (IS_ENABLED(CONFIG_LV_Z_POINTER_KSCAN)) {
		lv_obj_t *hello_world_button;

		hello_world_button = lv_btn_create(lv_scr_act());
		lv_obj_align(hello_world_button, LV_ALIGN_CENTER, 0, 0);
		hello_world_label = lv_label_create(hello_world_button);
	} else {
		hello_world_label = lv_label_create(lv_scr_act());
	}

	lv_label_set_text(hello_world_label, "Hello world!");
	lv_obj_align(hello_world_label, LV_ALIGN_CENTER, 0, 0);

	count_label = lv_label_create(lv_scr_act());
	lv_obj_align(count_label, LV_ALIGN_BOTTOM_MID, 0, 0);

	lv_task_handler();
	display_blanking_off(display_dev);

	while (1) {
		if ((count % 100) == 0U) {
			sprintf(count_str, "%d", count/100U);
			lv_label_set_text(count_label, count_str);
		}
		lv_task_handler();
		++count;
		k_sleep(K_MSEC(10));
	}*/

	/*lv_example_keyboard_1();
	lv_task_handler();
	display_blanking_off(display_dev);*/

	// -------------------------------------------------------------------------------------------------------------

	menu1();
	//menu2();
	lv_task_handler();
	display_blanking_off(display_dev);

	/*while (1) {
		lv_task_handler();
		k_sleep(K_MSEC(5));
	}*/

	nrf_cc3xx_platform_init();

	memset(&context, 0, sizeof(context));

	net_mgmt_init_event_callback(&wifi_shell_mgmt_cb,
				     wifi_mgmt_event_handler,
				     WIFI_SHELL_MGMT_EVENTS);

	net_mgmt_add_event_callback(&wifi_shell_mgmt_cb);


	net_mgmt_init_event_callback(&net_shell_mgmt_cb,
				     net_mgmt_event_handler,
				     NET_EVENT_IPV4_DHCP_BOUND);

	net_mgmt_add_event_callback(&net_shell_mgmt_cb);

	LOG_INF("Starting %s with CPU frequency: %d MHz", CONFIG_BOARD, SystemCoreClock/MHZ(1));
	k_sleep(K_SECONDS(1));

	LOG_INF("Static IP address (overridable): %s/%s -> %s",
		CONFIG_NET_CONFIG_MY_IPV4_ADDR,
		CONFIG_NET_CONFIG_MY_IPV4_NETMASK,
		CONFIG_NET_CONFIG_MY_IPV4_GW);

	int tcpServerSocket;
	int tcpClientSocket;
	struct sockaddr_in bindAddress;
	int connectionsNumber = 0;
	int bindingResult;
	struct sockaddr_in clientAddress;
	socklen_t clientAddressLength = sizeof( clientAddress );
	uint8_t addressString[32];
	int receivedBytes;
	uint8_t *pTransmitterBuffer;
	int sentBytes; 

	struct boardRequest reply;

	while (1) {
		wifi_connect();

		while (!context.connect_result) {
			cmd_wifi_status();
			k_sleep(K_MSEC(STATUS_POLLING_MS));
		}

		while (!ipSet) {
			LOG_INF("Waiting for DHCP IP");
			k_sleep(K_MSEC(STATUS_POLLING_MS));
		}

		if (context.connected) {

			// Server socket creation 
	tcpServerSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	if ( tcpServerSocket < 0 )	{
		LOG_ERR( "TCP Server error: socket: %d\n", errno );
		k_sleep( K_FOREVER );
	}

	// Binding 
	bindAddress.sin_family = AF_INET;				   
	bindAddress.sin_addr.s_addr = htonl(INADDR_ANY); 
	bindAddress.sin_port = htons(TCP_SERVER_PORT);	

	bindingResult = bind( 													\
						tcpServerSocket, 									\
						( struct sockaddr * )&bindAddress,					\
						sizeof( bindAddress ));

	if ( bindingResult < 0 )	{
		LOG_ERR( "TCP Server error: bind: %d\n", errno );
		k_sleep( K_FOREVER );
	}
    
	/* Listen */
	if ( listen( tcpServerSocket, 5 ) < 0 )	{
		LOG_ERR( "TCP Server error: listen: %d\n", errno );
		k_sleep( K_FOREVER );
	}

	printk( "TCP server waits for a connection on port %d...",				\
		   TCP_SERVER_PORT );

	while ( 1 ) {
		// Accept connection 
		tcpClientSocket = accept( 											\
								tcpServerSocket, 							\
								( struct sockaddr * )&clientAddress,		\
								&clientAddressLength );

		if ( tcpClientSocket < 0 ) {
			printk( "TCP Server error: accept: %d\n", errno );
			continue;
		}

		// Convert network address from internal to numeric ASCII form
		inet_ntop( 															\
				clientAddress.sin_family,									\
				&clientAddress.sin_addr,									\
				addressString, 												\
				sizeof( addressString ));
		LOG_INF( "TCP Server: Connection #%d from %s",						\
				connectionsNumber++, addressString );

		// Receiving loop
		while ( 1 ) {
			
			// Receive 
			receivedBytes = recv( 											\
								tcpClientSocket, 							\
								( char *) buf + len, 					\
								MAXBUF - len,					\
								0 );			

			if ( receivedBytes <= 0 ) {
				if ( receivedBytes < 0 ) { 
						LOG_ERR( "TCP Server error: recv: %d\n", errno );
				}
				break;
			}
			len += receivedBytes;

			int pr = parseRequest(buf, len, &reply);
			if(pr == 0){
				len = dumpHttpResponse(buf, &reply);
				if(len == -1){
					memset(buf, 0, MAXBUF);
					strcpy(buf, badRequest);
					len = strlen(badRequest);
				}
			}else if(pr == -1){
				len = 0;
			}else if(pr == -3){
				memset(buf, 0, MAXBUF);
				strcpy(buf, badRequest);
				len = strlen(badRequest);
				LOG_INF("REQUEST PROCESSED");
			}

			if(pr != -1 && pr != -2){
				// Send 
				int totalSent = 0;
				do {
					// Sending the received message by brusts
					sentBytes = send( 											\
									tcpClientSocket, 							\
									buf + totalSent, 						\
									len,								\
									0 );

					if ( sentBytes < 0 ) {
						LOG_ERR( "TCP Server error: send: %d\n", errno );
						close( tcpClientSocket );
						LOG_ERR( "TCP server: Connection from %s closed\n", 
								addressString );
					}
					totalSent += sentBytes;
					len -= sentBytes;
					
					LOG_INF( "TCP Server mode. Received and replied with %d "	\
							"bytes", sentBytes );
				} while ( len );
				if(len == 0){
					close(tcpClientSocket);
					break;
				}
			}
		}
	}
			k_sleep(K_FOREVER);
		}
	}

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

	/*struct boardRequest reply;
	InitZeroHashes(64);
	tfm_init();*/

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
			//LOG_INF("REQUEST PROCESSED");
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
					len -= send_len;
					if(len == 0){
						uart_irq_tx_disable(dev);
						memset(buf, 0, MAXBUF);
					}
				}
			}
		}
	}
#endif
}
