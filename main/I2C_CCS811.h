/*
 * I2C_CCS811.h
 *
 *  Created on: Apr 13, 2020
 *      Author: Bakeromso
 */

#ifndef MAIN_I2C_CCS811_H_
#define MAIN_I2C_CCS811_H_

#define ADDR                0x5A            // ADDR is set to GND, otherwise 0x5B

#define STATUS              0x00            // Status register
#define MEAS_MODE           0x01            // Measurement mode and conditions register
#define ALG_RESULT_DATA     0x02            // Algorithm result. The most significant 2 bytes contain 
                                                // a ppm estimate of the equivalent CO 2 (eCO 2 ) level, 
                                                // and the next two bytes contain a ppb estimate of the total
                                                // VOC level.
#define RAW_DATA            0x03            // Raw ADC data values for resistance and current source used.
#define ENV_DATA            0x05            // Temperature and humidity data can be written to enable compensation
#define THRESHOLDS          0x10            // Thresholds for operation when interrupts are only generated when eCO 2 ppm crosses a threshold
#define BASELINE            0x11            // The encoded current baseline value can be read. A previously saved encoded baseline can be written.
#define HW_ID               0x20            // Hardware ID. The value is 0x81
#define HW_VERSION          0x21            // Hardware Version. The value is 0x1X
#define FW_BOOT_VERSION     0x23            // Firmware Boot Version. The first 2 bytes contain the firmware version number for the boot code.
#define FW_APP_VERSION      0x24            // Firmware Application Version. The first 2 bytes contain the firmware version number for the application code
#define INTERNAL_STATE      0xA0            // Internal Status register
#define ERROR_ID            0xE0            // Error ID. When the status register reports an error its source is located in this register
#define APP_ERASE           0xF1            // If the correct 4 bytes (0xE7 0xA7 0xE6 0x09) are written to this register in a single sequence 
                                                // the device will start the application erase
#define APP_DATA            0xF2            // Transmit flash code for the bootloader to write to the application flash code space.
#define APP_VERIFY          0xF3            // Starts the process of the bootloader checking though the application to make sure a full image is valid.
#define APP_START           0xF4            // Application start. Used to transition the CCS811 state from boot to application mode,
                                                // a write with no data is required. Before performing a write to APP_START the
                                                // Status register should be accessed to check if there is a valid application present.
#define SW_RESET            0xFF            // If the correct 4 bytes (0x11 0xE5 0x72 0x8A) are written to this register in a single sequence
                                            // the device will reset and return to BOOT mode.

esp_err_t i2c_master_CCS811_init();
esp_err_t i2c_master_CCS811_read_status(uint8_t* stat);
esp_err_t i2c_master_CCS811_read_mode(uint8_t* mode);
esp_err_t i2c_master_CCS811_update_env(double tempr, double relhum);


#endif /* MAIN_I2C_CCS811_H_ */