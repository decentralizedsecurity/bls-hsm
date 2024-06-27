
#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include <stdio.h>
#include <string.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
//#include <tfm_ns_interface.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#include <zephyr/drivers/display.h>
#include <lvgl.h>

// Threads
#include <zephyr/sys/__assert.h>
#define THREAD_STACK_SIZE 17408 // Same value as CONFIG_MAIN_STACK_SIZE
#define THREAD_PRIORITY 7

// LVGL
#define LV_HOR_RES_MAX 320
#define LV_VER_RES_MAX 240

/**
 * @brief Removes touchscreen content
*/
void ts_clean();

/**
 * @brief Displays text on the touchscreen
 * 
 * @param buffer Text displayed
*/
void ts_display_text(const char * buffer);

/**
 * @brief Get text written by user via touchscreen keyboard
 * 
 * @param buffer Text entered by user
 * @param displayed_text (Optional) Text displayed on the top. If NULL, no text is displayed
*/
void ts_get_text_kb(char * buffer, const char * displayed_text);

#endif
