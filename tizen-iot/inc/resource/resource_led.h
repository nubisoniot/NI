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

#ifndef __RESOURCE_LED_H__
#define  __RESOURCE_LED_H__

#ifdef __cplusplus
extern "C" {
#endif

extern int resource_write_led(int pin_num, int write_value);
extern void resource_close_led(void);
extern int resource_read_led(int pin_num, uint32_t *read_value);

#ifdef __cplusplus
}
#endif

#endif /* __RESOURCE_LED_H__ */
