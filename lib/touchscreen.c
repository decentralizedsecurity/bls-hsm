#include "touchscreen.h"

struct k_sem sem;

char pwd[100];

const struct device *display_dev;

/**
 * @brief Initialize touchscreen
*/
void ts_init(){
	k_sem_init(&sem, 0, 1);

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		printk("[ts_init] Device not ready, aborting test\n");
		return;
	}

	ts_createLabel("ETH 2.0 Hardware Remote Signer");
	printk("[ts_init] Touchscreen is ready\n");
}

/**
 * @brief Text area event callback
*/
static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);
	
	if(code == LV_EVENT_VALUE_CHANGED){
		// printk("Value changed\n");
	}

	if(code == LV_EVENT_READY){
		char * msg = lv_textarea_get_text(ta);
		strcpy(pwd, msg);

		k_sem_give(&sem);
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

/**
 * @brief Get text written by user via touchscreen keyboard
 * 
 * @param buffer Text entered by user
 * @param displayed_text (Optional) Text displayed on the top. If NULL, no text is displayed
*/
void ts_request_text_kb(const char * displayed_text){
	ts_clean();

	// Create a keyboard to use it with the text area
    lv_obj_t * kb = lv_keyboard_create(lv_scr_act());

    // Create label
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, displayed_text);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 10);

    // Create a text area. The keyboard will write here
    lv_obj_t * ta;
    ta = lv_textarea_create(lv_scr_act());
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(ta, "...");
    lv_obj_set_size(ta, 280, 80);

    lv_keyboard_set_textarea(kb, ta);

	// Display on screen
	lv_task_handler();
	display_blanking_off(display_dev);

	while (k_sem_take(&sem, K_MSEC(10))) {
		lv_task_handler();
	}
}

/**
 * @brief Displays text on the touchscreen. It can be re-colored by commands
 * 
 * @param buffer Text displayed
*/
void ts_createLabel(char * text){
	ts_clean();

    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP); //Break the long lines
    lv_label_set_recolor(label, true); //Enable re-coloring by commands in the text
    lv_label_set_text(label, text);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    // Display on screen
	lv_task_handler();
	display_blanking_off(display_dev);
}

/**
 * @brief Thread that manages touchscreen behavior
*/
void ts_display_manager(){
	ts_init();
}

K_THREAD_DEFINE(thread_id_2, THREAD_STACK_SIZE, ts_display_manager, NULL, NULL, NULL, THREAD_PRIORITY, 0, 0);

void ts_clean(){
	lv_obj_clean(lv_scr_act());
}

void ts_display_text(const char * buffer){
	ts_createLabel(buffer);
}

void ts_get_text_kb(char * buffer, const char * displayed_text){
	ts_request_text_kb(displayed_text);
	strcpy(buffer, pwd);
	ts_createLabel("ETH 2.0 Hardware Remote Signer");
}