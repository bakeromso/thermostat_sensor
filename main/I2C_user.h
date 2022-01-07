/*
 * I2C_user.h
 *
 *  Created on: Apr 13, 2020
 *      Author: Bakeromso
 */

#include "driver/i2c.h"

#ifndef MAIN_I2C_user_H_
#define MAIN_I2C_user_H_
 
/**
 * Brief:
 * 
 * GPIO status:
 * D1 / GPIO5: 	SCL
 * D2 / GPIO4: 	SDA
 */
#define I2C_MASTER_SCL_D1	5
#define I2C_MASTER_SDA_D2	4
#define I2C_MASTER_NUM		I2C_NUM_0

#define WRITE_BIT			I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT            I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN        0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS       0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL             0x0              /*!< I2C ack value */
#define NACK_VAL            0x1              /*!< I2C nack value */
#define LAST_NACK_VAL       0x2              /*!< I2C last_nack value */

#define TOPIC_TEMPR         "/env/tempr/qos1"
#define TOPIC_RELHUM        "/env/relhum/qos1"

esp_err_t i2c_master_init();
void i2c_task_SI7021(void *arg);
void i2c_task_CCS811(void *arg);

#endif /* MAIN_I2C_user_H_ */