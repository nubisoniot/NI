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
#include <peripheral_io.h>
#include <Ecore.h>

#include <unistd.h>

#include "log.h"

/*
 * Nubison IoT 연계 모듈 헤더파일
 * */
#include "nubisonif.hpp"

//==========================
// 서버에서 발급한 key입니다. 수정하지 마세요.
char mytoken[32] = "54s8-t812-7H8y-1068";

//Nubison IoT 클라우드 서비스 주소(테스트서버 주소)
char cloudifaddress[64]="nubisoniot.com";
// 클라우드 접속 포트 번
int  cloudifport=1883;



//==========================

//==========================

//Nubison IoT 연계 모듈 클래스 인스턴
static NubisonIF *cloudif=NULL;

/*
 *하드웨어 제어관련 전역변
 * */
static peripheral_gpio_h g_sensor_h = NULL;

static int g_pin_num = -1;

static int button_state=0;

/*
 *GPIO제어 코드 LED제
 * */
int resource_write_led(int pin_num, int write_value)
{
	int ret = PERIPHERAL_ERROR_NONE;

	if (!g_sensor_h) {
		peripheral_gpio_h temp = NULL;

		dlog_print(DLOG_INFO, "NUBISON", "GPIO OPEN");
		ret = peripheral_gpio_open(pin_num, &temp);
		dlog_print(DLOG_INFO, "NUBISON", "State %d",ret);

		retv_if(ret, -1);

		ret = peripheral_gpio_set_direction(temp, PERIPHERAL_GPIO_DIRECTION_OUT_INITIALLY_LOW);
		if (ret) {
			peripheral_gpio_close(temp);
			dlog_print(DLOG_INFO, "NUBISON", "peripheral_gpio_set_direction failed.");
			return -1;
		}

		g_sensor_h = temp;
		g_pin_num = pin_num;
	}

	if (g_pin_num != pin_num) {
		dlog_print(DLOG_INFO, "NUBISON", "Invalid pin number.");
		return -1;
	}

	ret = peripheral_gpio_write(g_sensor_h, write_value);
	retv_if(ret < 0, -1);

	dlog_print(DLOG_INFO, "NUBISON", "LED Value : %s", write_value ? "ON":"OFF");

	button_state=write_value;

	return 0;
}


/*
 *GPIO Sensor Read
 * */
int resource_read_sensor(int pin_num, uint32_t *read_value)
{
	int ret = PERIPHERAL_ERROR_NONE;
		peripheral_gpio_h temp = NULL;

		dlog_print(DLOG_INFO, "NUBISON", "GPIO OPEN");
		ret = peripheral_gpio_open(pin_num, &temp);
		dlog_print(DLOG_INFO, "NUBISON", "State %d",ret);

		retv_if(ret, -1);

		ret = peripheral_gpio_set_direction(temp, PERIPHERAL_GPIO_DIRECTION_IN);
		if (ret) {
			peripheral_gpio_close(temp);
			dlog_print(DLOG_INFO, "NUBISON", "peripheral_gpio_set_direction failed.");

			return -1;
		}






	ret = peripheral_gpio_read(temp, (uint32_t*)read_value);
	retv_if(ret < 0, -1);

	dlog_print(DLOG_INFO, "NUBISON", "Sensor Read : %d", *read_value );
	peripheral_gpio_close(temp);

	return 0;
}


/*
 * Nubison Cloud과 통신하는 콜백 함수
 */

//1.Device 의 상태를 조회하는 콜백
void NubisonCB_Query(char* rdata, char* api, char* uniqkey)
{


	// 아래 함수는 서버에 값을 전송하는 코드입니다.
	// sendData를 String 자료형 변수로 저장하여 보내주세요.
	char tmp[32];


	uint32_t sensor_value=0;
	resource_read_sensor(17,&sensor_value);

	sprintf(tmp,"%d",sensor_value);
	// 클라우드에서 조회 요청이 왔을때 관련된내용을 담아서 전달 함
	// 관련해서 정확히 DB에 Unit 별로 들어 게 하는 것은 클라우드 서버에서 Driver로 셋팅함//
	cloudif->SendtoCloud(tmp, TYPE_STRING, api, uniqkey);
}
//2.Device 의 제어 하는 콜백
void NubisonCB_Invoke(char* rdata, char* api, char* uniqkey)
{
	printf("InvokeCB:%s %s %s\n",rdata,api,uniqkey);
	// 아래 함수는 서버에 값을 전송하는 코드입니다.
	// sendData를 String 자료형 변수로 저장하여 보내주세요.



	/*
	 * 하드웨어 제어 코드 작성
	 * ---->
	 * */

	uint32_t value = atoi(rdata);

	resource_write_led(26,value);

	/*
	 * <----
	 * */

	// 클라우드에서 조회 요청이 왔을때 관련된내용을 담아서 전달 함
    // 제어가 성공적으로 되었는지확인해서 값을 전달함//
	cloudif->SendtoCloud((char*)"ok", TYPE_STRING, api, uniqkey);


}
//3.Device 의 하드웨어 설정 하는 콜백
void NubisonCB_Setting(char* rdata, char* api, char* uniqkey)
{
	printf("SettingCB:%s %s %s\n",rdata,api,uniqkey);


	// 클라우드에서 조회 요청이 왔을때 관련된내용을 담아서 전달 함
	// 설정 성공적으로 되었는지확인해서 값을 전달함//
	cloudif->SendtoCloud((char*)"ok", TYPE_STRING, api, uniqkey);
}
//4.Device 의 하드웨어 상태를 체크 하는 콜백
void NubisonCB_Check(char* rdata, char* api, char* uniqkey)
{
	printf("CheckCB:%s %s %s\n",rdata,api,uniqkey);


	// 클라우드에서 조회 요청이 왔을때 관련된내용을 담아서 전달 함
	// 상태체크 성공적으로 되었는지확인해서 값을 전달함//
	cloudif->SendtoCloud((char*)"ok", TYPE_STRING, api, uniqkey);
}
//5.Device 의 클라우드와의 인증 관련  콜백
void NubisonCB_AUTHO(int authocode)
{

	  //1.인증에 정상적으로
	  if(authocode==NUBISONIOT_AUTHO_OK)
	  {

	  }//2.테스트 계정기간이 만료
	  else if(authocode==NUBISONIOT_AUTHO_ERROR_TESTTIME_EXPIRE)
	  {

	  }//3.인증되지 않은 계정
	  else if(authocode==NUBISONIOT_AUTHO_ERROR_UNAUTHORIZED_USE)
	  {

	  }

}

/*
 *
 */


Eina_Bool app_idler(void *data)
{
	NubisonIF * nubif=(NubisonIF * )data;
		if(nubif!=NULL)
		{
			nubif->Loop();
		}
    return ECORE_CALLBACK_RENEW;
}





static bool service_app_create(void *user_data)
{
	FN_CALL;


			/*
			 * Nubison Cloud 연계 관련 코드 시작
			 */
			        		//1. Nubison IoT 연계 모듈 생


						cloudif=new NubisonIF();

						//2.콜백 함수를 설정
						cloudif->SetCBFuntion(NubisonCB_Query, NubisonCB_Invoke, NubisonCB_Setting,NubisonCB_Check,NubisonCB_AUTHO);

						//3. 클라우드 접속 정보를 설정해서 초기화
						int ret=cloudif->Init(cloudifaddress,cloudifport,mytoken);
						if(ret!=0){
							dlog_print(DLOG_ERROR, "NUBISON", "Nubison IoT 연결 에러 발생 ");
						}



				ecore_idler_add(app_idler, cloudif);





	return true;
}

static void service_app_terminate(void *user_data)
{
	FN_CALL;


	 if(g_sensor_h!=NULL)
		 peripheral_gpio_close(g_sensor_h);
}

static void service_app_control(app_control_h app_control, void *user_data)
{
	FN_CALL;

}




int main(int argc, char *argv[])
{
	FN_CALL;

	char ad[50] = {0,};
	service_app_lifecycle_callback_s event_callback;


	event_callback.create = service_app_create;
	event_callback.terminate = service_app_terminate;
	event_callback.app_control = service_app_control;





	return service_app_main(argc, argv, &event_callback, ad);
}
