/*
 * I2C_CCS811.c
 *
 *  Created on: Apr 13, 2020
 *      Author: Bakeromso
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"

#include "I2C_user.h"
#include "I2C_CCS811.h"

// static const char *TAG = "I2C_CCS811";

/**
 * @code to initialize CCS811
 *
 * 1. wait 20 ms to make sure the device is ready for I2C after power on
 * 
 * 2. write to register APP_START
 * ______________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write reg_address + ack | stop |
 * --------|---------------------------|-------------------------|------|
 *
 * 3. wait 1 ms to make sure app is started
 *
 * 4. set mode 2
 * ___________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write reg_address + ack | data + ack | stop |
 * --------|---------------------------|-------------------------|------------|------|
 * 
 * 5. wait 20 minutes before results are reliable
 * 
 *  
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */

esp_err_t i2c_master_CCS811_init()
{
	esp_err_t ret;
	i2c_cmd_handle_t cmd;
    
    // 1. make sure device is ready after power on
    vTaskDelay(20 / portTICK_RATE_MS);

    // 2. write to APP_START
    cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, APP_START, ACK_CHECK_EN);
	i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	if(ret != ESP_OK){
		return ret;
	}
	i2c_cmd_link_delete(cmd);
    
    // 3. wait so that app is started. waiting 10 ms is smallest wait unit possible
    vTaskDelay(10 / portTICK_RATE_MS);

	// 4. set mode 2
    cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, MEAS_MODE, ACK_CHECK_EN);
    // Drive mode 2, interrupt disabled
    i2c_master_write_byte(cmd, 0b00010000, ACK_CHECK_EN);
	i2c_master_stop(cmd);
    i2c_cmd_link_delete(cmd);

    // // 5. wait 20 minutes
    // vTaskDelay(20*60000 / portTICK_RATE_MS); 

	return ret;
}

esp_err_t i2c_master_CCS811_read_status(uint8_t* stat)
{
	esp_err_t ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, STATUS, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ADDR << 1 | READ_BIT, ACK_CHECK_EN);
	i2c_master_read_byte(cmd, stat, I2C_MASTER_NACK);
	i2c_master_stop(cmd);

	ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

esp_err_t i2c_master_CCS811_read_mode(uint8_t* mode)
{
	esp_err_t ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, MEAS_MODE, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ADDR << 1 | READ_BIT, ACK_CHECK_EN);
	i2c_master_read_byte(cmd, mode, I2C_MASTER_NACK);
	i2c_master_stop(cmd);

	ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

/**
 * @code to initialize CCS811
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */

esp_err_t i2c_master_CCS811_update_env(double tempr, double relhum)
{
	esp_err_t ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    /*  
        conversion of temperature double tempr 127.998046875 relates to register being
        65535 (=0xFFFF, byte 0 and byte 1 of ENV_DATA)
        So double tempr * 65535 / 127.998046875 = 16 bit register value
    */

   if((tempr < -25) || (tempr > 127.999)){
       return ESP_ERR_INVALID_ARG;
   }
   uint16_t tempr_16 = (uint16_t) (((double)(tempr * 0xFFFF)) / 127.998046875);
   uint8_t tempr_MSB = (uint8_t) (tempr_16 >> 8);
   uint8_t tempr_LSB = (uint8_t) tempr_16;

    /*  
        conversion of relative humidity. 127.998046875 % relates to register being 
        65535 (=0xFFFF, byte 0 and byte 1 of ENV_DATA)
        So double relhum * 65535 / 127.998046875 = 16 bit register value
    */
    if((relhum < (double) 0 ) || (relhum > (double) 100)){
        return ESP_ERR_INVALID_ARG;
    }
    uint16_t relhum_16 = (uint16_t) (((double)(relhum * 0xFFFF)) / 127.998046875);
    uint8_t relhum_MSB = (uint8_t) (relhum_16 >> 8);
    uint8_t relhum_LSB = (uint8_t) relhum_16;

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, tempr_MSB, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, tempr_LSB, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, relhum_MSB, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, relhum_LSB, ACK_CHECK_EN);
	i2c_master_stop(cmd);

	ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

