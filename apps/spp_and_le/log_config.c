/*********************************************************************************************
    *   Filename        : log_config.c

    *   Description     : Optimized Code & RAM (编译优化Log配置)

    *   Author          : Bingquan

    *   Email           : caibingquan@zh-jieli.com

    *   Last modifiled  : 2019-03-18 14:45

    *   Copyright:(c)JIELI  2011-2019  @ , All Rights Reserved.
*********************************************************************************************/
#include "system/includes.h"

/**
 * @brief Bluetooth Controller Log
 */
/*-----------------------------------------------------------*/
const char log_tag_const_v_SETUP AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_SETUP AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_SETUP AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_SETUP AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_SETUP AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_BOARD AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_BOARD AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_BOARD AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_BOARD AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_BOARD AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_UI AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_UI AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_d_UI AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_w_UI AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_UI AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_KEY_EVENT_DEAL AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_KEY_EVENT_DEAL AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_KEY_EVENT_DEAL AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_KEY_EVENT_DEAL AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_KEY_EVENT_DEAL AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_APP_IDLE AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_APP_IDLE AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_APP_IDLE AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_APP_IDLE AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_APP_IDLE AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_APP AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_APP AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_APP AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_APP AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_APP AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_AT_CMD AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_AT_CMD AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_AT_CMD AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_AT_CMD AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_AT_CMD AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_USER_CFG AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_USER_CFG AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_USER_CFG AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_USER_CFG AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_USER_CFG AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_SPP_AND_LE AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_SPP_AND_LE AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_SPP_AND_LE AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_SPP_AND_LE AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_SPP_AND_LE AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_AT_COM AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_AT_COM AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_AT_COM AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_AT_COM AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_AT_COM AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_AT_CHAR_COM AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_AT_CHAR_COM AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_AT_CHAR_COM AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_AT_CHAR_COM AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_AT_CHAR_COM AT(.LOG_TAG_CONST) = 1;


const char log_tag_const_v_LE_CTL AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_LE_CTL AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_LE_CTL AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_LE_CTL AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_LE_CTL AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_OPTICAL_MOUSE_SENSOR AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_OPTICAL_MOUSE_SENSOR AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_d_OPTICAL_MOUSE_SENSOR AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_OPTICAL_MOUSE_SENSOR AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_OPTICAL_MOUSE_SENSOR AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_USBSTACK AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_USBSTACK AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_d_USBSTACK AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_USBSTACK AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_USBSTACK AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_HID_KEY AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_HID_KEY AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_HID_KEY AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_HID_KEY AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_HID_KEY AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_APP_CHARGE AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_APP_CHARGE AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_APP_CHARGE AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_w_APP_CHARGE AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_APP_CHARGE AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_DONGLE AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_DONGLE AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_DONGLE AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_DONGLE AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_DONGLE AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_APP_POWER AT(.LOG_TAG_CONST) = FALSE;
const char log_tag_const_i_APP_POWER AT(.LOG_TAG_CONST) = TRUE;
const char log_tag_const_d_APP_POWER AT(.LOG_TAG_CONST) = FALSE;
const char log_tag_const_w_APP_POWER AT(.LOG_TAG_CONST) = TRUE;
const char log_tag_const_e_APP_POWER AT(.LOG_TAG_CONST) = TRUE;

const char log_tag_const_v_APP_TONE AT(.LOG_TAG_CONST) = FALSE;
const char log_tag_const_i_APP_TONE AT(.LOG_TAG_CONST) = TRUE;
const char log_tag_const_d_APP_TONE AT(.LOG_TAG_CONST) = FALSE;
const char log_tag_const_w_APP_TONE AT(.LOG_TAG_CONST) = TRUE;
const char log_tag_const_e_APP_TONE AT(.LOG_TAG_CONST) = TRUE;

const char log_tag_const_v_MULTI_CONN AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_MULTI_CONN AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_MULTI_CONN AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_MULTI_CONN AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_MULTI_CONN AT(.LOG_TAG_CONST) = 1;



