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

#ifndef __LOG_H__
#define  __LOG_H__

#include <dlog.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "NUBISON"

#define ERR(fmt, args...) dlog_print(DLOG_ERROR, LOG_TAG, "%s : %s(%d) > " fmt" \n", rindex(__FILE__, '/') + 1, __func__, __LINE__, ##args)
#define WARN(fmt, args...) dlog_print(DLOG_WARN, LOG_TAG, "%s : %s(%d) > " fmt" \n", rindex(__FILE__, '/') + 1, __func__, __LINE__, ##args)
#define INFO(fmt, args...)  dlog_print(DLOG_INFO, LOG_TAG, "%s : %s(%d) > " fmt" \n", rindex(__FILE__, '/') + 1, __func__, __LINE__, ##args)
#define DBG(fmt, args...) dlog_print(DLOG_DEBUG, LOG_TAG, "%s : %s(%d) > " fmt" \n", rindex(__FILE__, '/') + 1, __func__, __LINE__, ##args)

#define FN_CALL dlog_print(DLOG_DEBUG, LOG_TAG, ">>>>>>>> called")
#define FN_END dlog_print(DLOG_DEBUG, LOG_TAG, "<<<<<<<< ended")

#define retv_if(expr, val) do { \
    if(expr) { \
		dlog_print(DLOG_ERROR, LOG_TAG, "%s : %s(%d) > (%s) -> %s() return", rindex(__FILE__, '/') + 1, __func__, __LINE__, #expr, __FUNCTION__, __LINE__); \
        return (val); \
    } \
} while (0)

#define ret_if(expr) do { \
    if(expr) { \
		dlog_print(DLOG_ERROR, LOG_TAG, "%s : %s(%d) > (%s) -> %s() return ", rindex(__FILE__, '/') + 1, __func__, __LINE__, #expr, __FUNCTION__, __LINE__); \
        return; \
    } \
} while (0)



#ifdef __cplusplus
}
#endif

#endif /* __LOG_H__ */
