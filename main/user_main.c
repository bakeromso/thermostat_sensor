/* Air Quality Sensor

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   Based on Espressif GPIO example, rest by author: Bakeromso 
*/


#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "driver/gpio.h"
#include "driver/i2c.h"

#include "esp_log.h"

#include "user_defines.h"
#include "I2C_user.h"
#include "MQTT_user.h"

static const char *TAG = "MAIN";

void app_main(void)
{
////// --------------- START GPIO PART ----------------------/////
//	// configure gpio output
//    gpio_config_t io_conf;
//    //disable interrupt
//    io_conf.intr_type = GPIO_INTR_DISABLE;
//    //set as output mode
//    io_conf.mode = GPIO_MODE_OUTPUT;
//    //bit mask of the pins that you want to set,e.g.GPIO15/16
//    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
//    //disable pull-down mode
//    io_conf.pull_down_en = 0;
//    //disable pull-up mode
//    io_conf.pull_up_en = 0;
//    //configure GPIO with the given settings
//    gpio_config(&io_conf);
//
//    //bit mask of the pins
//    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
//    //set as input mode
//    io_conf.mode = GPIO_MODE_INPUT;
//    //disable pull-up mode
//    io_conf.pull_up_en = 0;
//
//    int cnt = 0;
//
//    printf("GPIO has been configured, hello world!\n");
//
////    xTaskCreate(vTaskScrollerLeft, "vtask function", 2048, NULL, 10, &xHandlePulseOutput);
////    xTaskCreate(vTaskScroller, "vtask scroller function", 2048, NULL, 10, &xHandlePulseOutput);
//
////// --------------- END GPIO PART ----------------------/////

//// --------------- START MQTT PART ----------------------/////
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

    nvs_flash_init();
    wifi_init();
    mqtt_app_start();

//// --------------- END MQTT PART ----------------------/////

    //start i2c task
    // initialize I2C
	ESP_ERROR_CHECK(i2c_master_init());
    xTaskCreate(i2c_task_SI7021, "i2c_task_SI7021", 2048, NULL, 10, NULL);
    xTaskCreate(i2c_task_CCS811, "i2c_task_SI7021", 2048, NULL, 10, NULL);

    printf("Task I2C has been configured, hello world!\n");

    while (1) {
//        ESP_LOGI(TAG, "cnt: %d\n", cnt++);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}


