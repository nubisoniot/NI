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

#ifndef NUBISONIF_h
#define NUBISONIF_h




#include "nubisonif.hpp"




#define TYPE_STRING "STRING"
#define TYPE_DEC     "DEC"

#define IF_QUERY_CALLBACK_FN void (*query_nubicb)(char *, char *,char *)
#define IF_INVOKE_CALLBACK_FN void (*invoke_nubicb)(char *, char *,char *)
#define IF_SETTING_CALLBACK_FN void (*setting_nubicb)(char *, char *,char *)
#define IF_CHECK_CALLBACK_FN void (*check_nubicb)(char *, char *,char *)
#define IF_AUTHO_CALLBACK_FN void (*auth_nubicb)(int )


//Nubison Interface Protocol Define

#define USE_MOSQUITTO_MQTT 1
//#define USE_INTER_MQTT



/**
 * @ Nubison IoT Cloud 에서 인증  Option
 * @
 */
typedef enum {
	NUBISONIOT_AUTHO_OK = 0,              			/** [정상] 클라우드 사용 정상 인증  */
	NUBISONIOT_AUTHO_ERROR_TESTTIME_EXPIRE,  		/** [에러] 테스트 사용시간 만료    */
	NUBISONIOT_AUTHO_ERROR_UNAUTHORIZED_USE   		/** [에러] 비인가 사용  		*/
} nubison_autho_e;




class NubisonIF
{
	public:
		NubisonIF () ;
		~NubisonIF();
	void Loop();
    int Init(char *cloudaddress,int port,char* tokenkey );
    void SetCBFuntion(IF_QUERY_CALLBACK_FN,IF_INVOKE_CALLBACK_FN,IF_SETTING_CALLBACK_FN,IF_CHECK_CALLBACK_FN,IF_AUTHO_CALLBACK_FN);
	void SendtoCloud(char *sdata,char *type,char* api,char* uniqKey);
	void NotitoCloud(char *sdata, char *type, char* uniqKey,int min_term);


    IF_QUERY_CALLBACK_FN;
    IF_INVOKE_CALLBACK_FN;
    IF_SETTING_CALLBACK_FN;
    IF_CHECK_CALLBACK_FN;
    IF_AUTHO_CALLBACK_FN;

    int _connectstate;
    
	private:
    char _tokenkey[256]; // 기기에 발급되는 유니크한 key
	char _uniqkey[256];  // 클라우드에 호출시 일시 생성되는 유니크한 key
	char _cloudaddress[64];


	int _port;
	int _pretime;

	//USE MQTT
	struct mosquitto *mosq;
   
};

#endif
