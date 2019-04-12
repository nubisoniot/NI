/*****************************************************************
 *
 * Copyright (c) 2019 Sim Platform Co., Ltd. All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include <peripheral_io.h>

#include "log.h"

// 하드웨어 제어용 전역변수
static peripheral_gpio_h g_sensor_h = NULL;
static int g_pin_num = -1;
static int g_write_value = 0;

void resource_close_led(void)
{
	if (!g_sensor_h) return;

	INFO("LED is finishing...");

	peripheral_gpio_close(g_sensor_h);
	g_sensor_h = NULL;
	g_pin_num = -1;
}

// GPIO제어 코드 LED 제어
int resource_write_led(int pin_num, uint32_t write_value)
{
	int ret = PERIPHERAL_ERROR_NONE;

	if (g_write_value == write_value) {
		ERR("Same value as before : %s", write_value ? "ON":"OFF");
		return -1;
	}

	g_write_value = write_value;

	if (!g_sensor_h) {
		peripheral_gpio_h temp = NULL;

		ret = peripheral_gpio_open(pin_num, &temp);
		retv_if(ret, -1);

		DBG("GPIO %d opened", pin_num);
		g_sensor_h = temp;
		g_pin_num = pin_num;
	}

	if (g_pin_num != pin_num) {
		peripheral_gpio_close(g_sensor_h);
		ERR("Invalid pin number");
		return -1;
	}

	ret = peripheral_gpio_set_direction(g_sensor_h, PERIPHERAL_GPIO_DIRECTION_OUT_INITIALLY_LOW);
	if (ret) {
		peripheral_gpio_close(g_sensor_h);
		ERR("Failed to peripheral_gpio_set_direction");
		return -1;
	}

	ret = peripheral_gpio_write(g_sensor_h, write_value);
	if (ret != 0) {
		ERR("Failed to write led : %d(%s)", ret, get_error_message(ret));
		return -1;
	}

	DBG("LED Write Value : %s", write_value ? "ON":"OFF");

	return 0;
}

// GPIO Sensor Read
int resource_read_led(int pin_num, uint32_t *read_value)
{
	int ret = PERIPHERAL_ERROR_NONE;

	if (!g_sensor_h) {
		peripheral_gpio_h temp = NULL;

		ret = peripheral_gpio_open(pin_num, &temp);
		retv_if(ret, -1);

		DBG("GPIO %d opened", pin_num);
		g_sensor_h = temp;
		g_pin_num = pin_num;
	}

	if (g_pin_num != pin_num) {
		peripheral_gpio_close(g_sensor_h);
		ERR("Invalid pin number");
		return -1;
	}

	/*ret = peripheral_gpio_set_direction(g_sensor_h, PERIPHERAL_GPIO_DIRECTION_IN);
	if (ret) {
		peripheral_gpio_close(g_sensor_h);
		ERR("Failed to peripheral_gpio_set_direction");
		return -1;
	}*/

	ret = peripheral_gpio_read(g_sensor_h, (uint32_t*)read_value);
	retv_if(ret < 0, -1);

	DBG("LED Read : %d", *read_value);

	return 0;
}

