/*
 * I2C_user.c
 * 
 * This file contains the initialisation of the I2C and the tasks. 
 * Functions are defined in the specific I2C_xxx.h where xxx is the I2C device.
 * 
 * Created on: Apr 13, 2020
 *      Author: Bakeromso
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "driver/i2c.h"

#include "esp_log.h"

#include "user_defines.h"
#include "I2C_user.h"
#include "I2C_SI7021.h"
#include "I2C_CCS811.h"
#include "MQTT_user.h"


double tempr = 0;
double relhum = 0;

/**
 * @brief i2c master initialization
 */
esp_err_t i2c_master_init()
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_D2;
    conf.sda_pullup_en = 1;
    conf.scl_io_num = I2C_MASTER_SCL_D1;
    conf.scl_pullup_en = 1;
    conf.clk_stretch_tick = 300; // 300 ticks (default)
    ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port, conf.mode));
    ESP_ERROR_CHECK(i2c_param_config(i2c_master_port, &conf));
    return ESP_OK;
}


void i2c_task_SI7021(void *arg)
{
        static const char *TAG = "I2C_USER:SI7021";
    int msg_id;
    char str_tempr[sizeof(tempr) + 1];
    char str_relhum[sizeof(relhum) + 1];
    uint8_t ccs811_stat = 0;

    for(;;){
    	ESP_ERROR_CHECK(i2c_master_SI7021_read_humidity(&relhum));
    	ESP_ERROR_CHECK(i2c_master_SI7021_read_tempAfterHumidity(&tempr));
    	ESP_LOGI(TAG, "Humidity is %f %%RH, temperature is %f degrees Celsius ", relhum, tempr);
        
    	snprintf(str_tempr, sizeof(tempr), "%2.2f", tempr); str_tempr[sizeof(tempr)-1]=0;
    	snprintf(str_relhum, sizeof(relhum), "%2.2f", relhum); str_tempr[sizeof(relhum)-1]=0;
    	if(isConnected()){
    		msg_id = mqtt_publish(TOPIC_TEMPR, str_tempr, 0, 1, 0);
    		msg_id = mqtt_publish(TOPIC_RELHUM, str_relhum, 0, 1, 0);
            mqtt_publish_tempr_and_relhum(tempr, relhum, 0, 0);
    	}
    	vTaskDelay(10000 / portTICK_RATE_MS); // every 10 seconds
    }

    i2c_driver_delete(I2C_MASTER_NUM);
}

void i2c_task_CCS811(void *arg)
{
    static const char *TAG = "I2C_USER:CCS811";
    int msg_id;
    char str_tempr[sizeof(tempr) + 1];
    char str_relhum[sizeof(relhum) + 1];
    uint8_t ccs811_stat = 0;
    uint8_t mode = 0;

    ESP_ERROR_CHECK(i2c_master_CCS811_read_status(&ccs811_stat));
    ESP_LOGI(TAG, "The status register before init is 0x%x", ccs811_stat);
    ESP_ERROR_CHECK(i2c_master_CCS811_init());
    ESP_ERROR_CHECK(i2c_master_CCS811_read_status(&ccs811_stat));
    ESP_LOGI(TAG, "The status register after init is 0x%x", ccs811_stat);    

    for(;;){
        ESP_ERROR_CHECK(i2c_master_CCS811_read_status(&ccs811_stat));
        ESP_LOGI(TAG, "The status register is 0x%x", ccs811_stat);

        ESP_ERROR_CHECK(i2c_master_CCS811_read_mode(&mode));
        ESP_LOGI(TAG, "The mode register is 0x%x", mode);
        
        ESP_ERROR_CHECK(i2c_master_CCS811_update_env(tempr,relhum));
    	vTaskDelay(10000 / portTICK_RATE_MS); // every 10 seconds
    }
    i2c_driver_delete(I2C_MASTER_NUM);
}

