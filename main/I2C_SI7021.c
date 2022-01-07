/*
 * I2C_SI7021.c
 *
 *  Created on: Apr 13, 2020
 *      Author: Bakeromso
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"

#include "I2C_user.h"
#include "I2C_SI7021.h"

// static const char *TAG = "I2C_SI7021";

/**
 * @code to read data from SI7021
 *
 * 1. set which register to read from
 * ______________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write reg_address + ack | stop |
 * --------|---------------------------|-------------------------|------|
 *
 * 2. wait for results
 *
 * 3. read data from said register
 * ______________________________________________________________________________
 * | start | slave_addr + r_bit + ack | data_MSB + ack | data_LSB + nack | stop |
 * --------|--------------------------|----------------|-----------------|------|
 *
 * @param i2c_num I2C port number
 * @param reg_address slave reg address
 * @param data data to send
 * @param data_len data length
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */


esp_err_t i2c_master_SI7021_read_humidity(double* pRH)
{
	esp_err_t ret = ESP_OK;
	uint8_t sensor_data[2];
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, I2C_SI7021_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, MSR_RELHUM_NHM, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	if(ret != ESP_OK){
		return ret;
	}
	cmd = i2c_cmd_link_create();
	vTaskDelay(30 / portTICK_RATE_MS); // wait so the measurement can be performed
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, I2C_SI7021_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
	i2c_master_read_byte(cmd, &sensor_data[0], I2C_MASTER_ACK);
	i2c_master_read_byte(cmd, &sensor_data[1], I2C_MASTER_NACK);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	if(ret == ESP_OK){
		uint16_t relhum_code = (sensor_data[0] << 8) + sensor_data[1];
		*pRH = (double)(((double)(125*relhum_code))/65536) - 6; // calculation to get relative humidity from register value
	}
	i2c_cmd_link_delete(cmd);
	return ret;
}

esp_err_t i2c_master_SI7021_read_tempAfterHumidity(double* pTemp)
{
	esp_err_t ret;
	uint8_t sensor_data[2];
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, I2C_SI7021_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, 0xE0, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, I2C_SI7021_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
	i2c_master_read_byte(cmd, &sensor_data[0], I2C_MASTER_ACK);
	i2c_master_read_byte(cmd, &sensor_data[1], I2C_MASTER_NACK);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	if(ret == ESP_OK){
		uint16_t temp_code = (sensor_data[0] << 8) + sensor_data[1];
		*pTemp = (175.72*temp_code)/65536 - 46.85; // calculation to get relative humidity from register value
	}
	i2c_cmd_link_delete(cmd);
	return ret;
}


esp_err_t i2c_master_SI7021_read_temp()
{
	esp_err_t ret;
	uint8_t sensor_data[2];
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, I2C_SI7021_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, MSR_TEMP_NHM, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	if(ret != ESP_OK){
		return ret;
	}
	cmd = i2c_cmd_link_create();
	vTaskDelay(30 / portTICK_RATE_MS); // wait so the measurement can be performed
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, I2C_SI7021_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
	i2c_master_read_byte(cmd, &sensor_data[0], I2C_MASTER_ACK);
	i2c_master_read_byte(cmd, &sensor_data[1], I2C_MASTER_NACK);
	i2c_master_stop(cmd);

	ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

esp_err_t i2c_master_SI7021_read_register()
{
	esp_err_t ret;
	uint8_t register_data[1];
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, I2C_SI7021_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, R_USR_REG, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, I2C_SI7021_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
	i2c_master_read_byte(cmd, &register_data[0], I2C_MASTER_NACK);
	i2c_master_stop(cmd);

	ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}


esp_err_t i2c_master_SI7021_firmware_rev()
{
	esp_err_t ret = ESP_OK;
	uint8_t firmrev[8];
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, I2C_SI7021_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, 0x84, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, 0xB8, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, I2C_SI7021_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
	i2c_master_read_byte(cmd, &firmrev[0], I2C_MASTER_NACK);
	i2c_master_stop(cmd);

	ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}