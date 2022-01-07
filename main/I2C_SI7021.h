/*
 * I2C_SI7021.h
 *
 *  Created on: Apr 13, 2020
 *      Author: Bakeromso
 */

#ifndef MAIN_I2C_SI7021_H_
#define MAIN_I2C_SI7021_H_

#define I2C_SI7021_ADDR		0x40

#define MSR_RELHUM_HM		0xE5			// Measure Relative Humidity, Hold Master Mode
#define MSR_RELHUM_NHM		0xF5			// Measure Relative Humidity, No Hold Master Mode
#define MSR_TEMP_HM			0xE3			// Measure Temperature, Hold Master Mode
#define MSR_TEMP_NHM		0xF3 			// Measure Temperature, No Hold Master Mode
#define W_USR_REG			0xE6			// Write RH/T User Register 1
#define R_USR_REG			0xE7			// Write RH/T User Register 1

esp_err_t i2c_master_SI7021_firmware_rev();
esp_err_t i2c_master_SI7021_read_register();
esp_err_t i2c_master_SI7021_read_temp();
esp_err_t i2c_master_SI7021_read_tempAfterHumidity(double* pTemp);
esp_err_t i2c_master_SI7021_read_humidity(double* pRH);


#endif /* MAIN_I2C_SI7021_H_ */