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

#include <tizen.h>
#include <service_app.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Ecore.h>
#include <unistd.h>

#include "log.h"
#include "resource/resource_led.h"
// Nubison IoT 연계 모듈 헤더파일
#include "nubisonif.hpp"

#define BUFSIZE 32
#define SENSOR_LED_GPIO_NUMBER (26)

// 누비슨 클라우드에서 발급하는 인식키
// 연결장치의 제품 식별키를 입력하기
char mytoken[32] = "XXXX-XXXX-XXXX-XXXX";

// Nubison IoT 클라우드 서비스 주소(테스트서버 주소)
char cloudifaddress[64] = "nubisoniot.com";

// 클라우드 접속 포트 번
int  cloudifport = 1883;

//Nubison IoT 연계 모듈 클래스 인스턴스
static NubisonIF *cloudif = NULL;

static int led0 = 0;
static int led1 = 1;


// Nubison Cloud과 통신하는 콜백 함수
//1.Device 의 상태를 조회하는 콜백
void NubisonCB_Query(char* rdata, char* api, char* uniqkey)
{
	DBG("QueryCB : %s %s %s", rdata, api, uniqkey);

	// sendData를 String 자료형 변수로 저장하여 보내주세요.
	char tmp[BUFSIZE] = {0, };
	sprintf(tmp, "%d:%d", led0, led1);

	// 클라우드에서 조회 요청이 왔을때 관련된내용을 담아서 전달 함
	// 관련해서 정확히 DB에 Unit 별로 들어 게 하는 것은 클라우드 서버에서 Driver로 셋팅함
	cloudif->SendtoCloud(tmp, TYPE_STRING, api, uniqkey);
}

//2.Device 의 제어 하는 콜백
void NubisonCB_Invoke(char* rdata, char* api, char* uniqkey)
{
	DBG("InvokeCB : %s %s %s", rdata, api, uniqkey);

	char tmp[BUFSIZE] = {0, };
	char *word = NULL;

	int led_number = 0;
	int led_value = 0;

	strncpy(tmp, rdata, BUFSIZE);

	word = strtok(tmp, ":");
	led_number = atoi(word);

	word = strtok(NULL, ":");
	led_value = atoi(word);

	DBG("Led Number : %d, Value : %d", led_number, led_value);

	switch (led_number) {
	case 0:
		led0 = led_value;
		break;
	case 1:
		led1 = led_value;
		break;
	default:
		break;
	}

	// 클라우드에서 조회 요청이 왔을때 관련된내용을 담아서 전달 함
    // 제어가 성공적으로 되었는지확인해서 값을 전달함//
	cloudif->SendtoCloud(tmp, TYPE_STRING, api, uniqkey);
}

//3.Device 의 하드웨어 설정 하는 콜백
void NubisonCB_Setting(char* rdata, char* api, char* uniqkey)
{
	DBG("SettingCB : %s %s %s", rdata, api, uniqkey);

	// 클라우드에서 조회 요청이 왔을때 관련된내용을 담아서 전달 함
	// 설정 성공적으로 되었는지확인해서 값을 전달함//
	cloudif->SendtoCloud((char*)"ok", TYPE_STRING, api, uniqkey);
}

//4.Device 의 하드웨어 상태를 체크 하는 콜백
void NubisonCB_Check(char* rdata, char* api, char* uniqkey)
{
	DBG("CheckCB : %s %s %s", rdata, api, uniqkey);

	// 클라우드에서 조회 요청이 왔을때 관련된내용을 담아서 전달 함
	// 상태체크 성공적으로 되었는지확인해서 값을 전달함//
	cloudif->SendtoCloud((char*)"Success.", TYPE_STRING, (char*)"check", uniqkey);
}

//5.Device 의 클라우드와의 인증 관련 콜백
void NubisonCB_AUTHO(int authocode)
{
	DBG("AUTHOCB : %d", authocode);

	if (authocode == NUBISONIOT_AUTHO_OK) {
		//1.인증에 정상적으로
		DBG("NUBISONIOT_AUTHO_OK");
	} else if (authocode == NUBISONIOT_AUTHO_ERROR_TESTTIME_EXPIRE) {
		//2.테스트 계정기간이 만료
		DBG("NUBISONIOT_AUTHO_ERROR_TESTTIME_EXPIRE");
	} else if (authocode == NUBISONIOT_AUTHO_ERROR_UNAUTHORIZED_USE) {
		//3.인증되지 않은 계정
		DBG("NUBISONIOT_AUTHO_ERROR_UNAUTHORIZED_USE");
	}
}

Eina_Bool app_idler(void *data)
{
	NubisonIF *nubif = (NubisonIF * )data;
	char tmp[BUFSIZE] = {0, };

	if(nubif != NULL) {
		nubif->Loop();
	}

	sprintf(tmp, "%d:%d", led0, led1);
	nubif->NotitoCloud(tmp, TYPE_STRING, mytoken, 1);

    return ECORE_CALLBACK_RENEW;
}

static bool service_app_create(void *user_data)
{
	DBG(">>>>>>>>>>> App create");

	//1. Nubison IoT 연계 모듈 생
	cloudif = new NubisonIF();

	//2.콜백 함수를 설정
	cloudif->SetCBFuntion(NubisonCB_Query, NubisonCB_Invoke, NubisonCB_Setting, NubisonCB_Check, NubisonCB_AUTHO);

	//3. 클라우드 접속 정보를 설정해서 초기화
	int ret = cloudif->Init(cloudifaddress, cloudifport, mytoken);
	if(ret!=0) {
		ERR("Failed to connect with Nubison IoT");
	}

	ecore_idler_add(app_idler, cloudif);

	return true;
}

static void service_app_terminate(void *user_data)
{
	DBG("<<<<<<<<<<< App terminated");

	resource_close_led();
}

static void service_app_control(app_control_h app_control, void *user_data)
{
	DBG(">>>>>>>>>>> App control");
}

int main(int argc, char *argv[])
{
	char ad[50] = {0,};
	service_app_lifecycle_callback_s event_callback;

	event_callback.create = service_app_create;
	event_callback.terminate = service_app_terminate;
	event_callback.app_control = service_app_control;

	return service_app_main(argc, argv, &event_callback, ad);
}
