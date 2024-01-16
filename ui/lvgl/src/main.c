/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

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

	while (1) {
		lv_task_handler();
		k_sleep(K_MSEC(5));
	}

}
