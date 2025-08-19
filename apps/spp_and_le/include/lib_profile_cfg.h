
#ifndef _LIB_PROFILE_CFG_H_
#define _LIB_PROFILE_CFG_H_

#include "app_config.h"
#include "btcontroller_modules.h"

#define BT_BTSTACK_CLASSIC                   BIT(0)
#define BT_BTSTACK_LE_ADV                    BIT(1)
#define BT_BTSTACK_LE                        BIT(2)

extern const int config_stack_modules;
#define STACK_MODULES_IS_SUPPORT(x)         (config_stack_modules & (x))

///---sdp service record profile- 用户选择支持协议--///

//ble demo的例子
#define DEF_BLE_DEMO_NULL                 0 //ble 没有使能
#define DEF_BLE_DEMO_TRANS_DATA           1 //
#define DEF_BLE_DEMO_CLIENT               3 //
#define DEF_BLE_DEMO_AT_COM               4 //
#define DEF_BLE_DEMO_AT_CLIENT            5 //
#define DEF_BLE_DEMO_AT_CHAR_COM          6
#define DEF_BLE_DEMO_MI					  9
#define DEF_BLE_DEMO_MULTI                10 //
#define DEF_BLE_DEMO_ADV                  11 //
#define DEF_BLE_DEMO_NONCONN_24G          12 //
#define DEF_BLE_DEMO_LL_SYNC              13 //
#define DEF_BLE_DEMO_TUYA_TRANS           14

//配置选择的demo
#if TCFG_USER_BLE_ENABLE

#if TRANS_DATA_EN
#define TCFG_BLE_DEMO_SELECT          DEF_BLE_DEMO_TRANS_DATA

#elif LL_SYNC_EN
#define TCFG_BLE_DEMO_SELECT          DEF_BLE_DEMO_LL_SYNC

#elif TUYA_TRANS_EN
#define TCFG_BLE_DEMO_SELECT          DEF_BLE_DEMO_TUYA_TRANS

#elif (TRANS_CLIENT_EN || TRANS_DONGLE_EN)
#define TCFG_BLE_DEMO_SELECT          DEF_BLE_DEMO_CLIENT

#elif TRANS_MULTI_BLE_EN
#define TCFG_BLE_DEMO_SELECT          DEF_BLE_DEMO_MULTI

#elif TRANS_AT_COM
#define TCFG_BLE_DEMO_SELECT          DEF_BLE_DEMO_AT_COM

#elif TRANS_AT_CLIENT
#define TCFG_BLE_DEMO_SELECT          DEF_BLE_DEMO_AT_CLIENT

#elif CONFIG_APP_AT_CHAR_COM
#define TCFG_BLE_DEMO_SELECT          DEF_BLE_DEMO_AT_CHAR_COM

#elif XM_MMA_EN
#define TCFG_BLE_DEMO_SELECT	      DEF_BLE_DEMO_MI

#elif BEACON_MODE_EN
#define TCFG_BLE_DEMO_SELECT          DEF_BLE_DEMO_ADV

#elif TRANS_NONCON_24G_EN
#define TCFG_BLE_DEMO_SELECT          DEF_BLE_DEMO_NONCONN_24G

#else
#define TCFG_BLE_DEMO_SELECT          DEF_BLE_DEMO_NULL//ble is closed
#endif /* TCFG_USER_BLE_ENABLE */

//配对加密使能
#define TCFG_BLE_SECURITY_EN          config_le_sm_support_enable

#endif

#endif

