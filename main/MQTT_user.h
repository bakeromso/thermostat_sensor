/*
 * MQTT_user.h
 *
 *  Created on: Apr 13, 2020
 *      Author: Bakeromso
 */

#ifndef MAIN_MQTT_USER_H_
#define MAIN_MQTT_USER_H_

#define TEMPANDHUM "/env/tempandhum/study" 
#define STR_LEN_TEMPANDHUM 90

bool isConnected(void);
void setConnected(bool conn);
int mqtt_publish(char* topic, char* data, int len, int qos, int retain);
void wifi_init(void);
void mqtt_app_start(void);
int mqtt_publish_tempr_and_relhum(double tempr, double relhum, int qos, int retain);


#endif /* MAIN_MQTT_USER_H_ */