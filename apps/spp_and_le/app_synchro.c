/*********************************************************************************************
    *   Filename        : app_synchro.c

    *   Description     :

    *   Author          :

    *   Email           :

    *   Last modifiled  :

    *   Copyright:(c)
*********************************************************************************************/
#include "system/includes.h"
#include "app_config.h"
#include "app_action.h"
#include "app_main.h"
#include "update.h"
#include "update_loader_download.h"
#include "app_charge.h"
#include "app_power_manage.h"
#include "system/event.h"
#include "asm/mcpwm.h"
#include "le_trans_data.h"
#include "btstack/bluetooth.h"


extern u8 Ble_Addr[];     //BLE��ַ

/****************************************************************************************
**����:ͬ����̬ģʽ
**����:
**˵��:
**��ע:
**����:
*****************************************************************************************/
void Mode_Synchro(u8 mod)
{
    u8 buf[9];

    memcpy(buf,Ble_Addr, 6);

    buf[6] = 0x04;
    buf[7] = 0x02;
    buf[8] = mod;
    // app_send_user_data(ATT_CHARACTERISTIC_fff1_01_VALUE_HANDLE, buf,9, ATT_OP_AUTO_READ_CCC);
    os_time_dly(1);
}
/****************************************************************************************
**����:ͬ�����ػ�
**����:
**˵��:
**��ע:
**����:
*****************************************************************************************/
void OnOff_Synchro(u8 onoff)
{
    u8 buf[9];

    memcpy(buf,Ble_Addr, 6);

    buf[6] = 0x01;
    buf[7] = 0x01;
    buf[8] = onoff;
    // app_send_user_data(ATT_CHARACTERISTIC_fff1_01_VALUE_HANDLE, buf,9, ATT_OP_AUTO_READ_CCC);
    os_time_dly(1);
}
/****************************************************************************************
**����:ͬ���ٶ�
**����:
**˵��:
**��ע:
**����:
*****************************************************************************************/
void Speed_Synchro(u8 spe)
{
    u8 buf[9];

    memcpy(buf,Ble_Addr, 6);

    buf[6] = 0x04;
    buf[7] = 0x04;
    buf[8] = spe;
    // app_send_user_data(ATT_CHARACTERISTIC_fff1_01_VALUE_HANDLE, buf,9, ATT_OP_AUTO_READ_CCC);
    os_time_dly(1);
}
/****************************************************************************************
**����:ͬ������
**����:
**˵��:
**��ע:
**����:
*****************************************************************************************/
void Light_Synchro(u8 lig)
{
    u8 buf[9];

    memcpy(buf,Ble_Addr, 6);

    buf[6] = 0x04;
    buf[7] = 0x03;
    buf[8] = lig;
    // app_send_user_data(ATT_CHARACTERISTIC_fff1_01_VALUE_HANDLE, buf,9, ATT_OP_AUTO_READ_CCC);
    os_time_dly(1);
}
/****************************************************************************************
**����:ͬ��RGB
**����:
**˵��:
**��ע:
**����:
*****************************************************************************************/
void RGB_Synchro(u8 r_flag,u8 g_flag,u8 b_flag)
{
    u8 buf[12];

    memcpy(buf,Ble_Addr, 6);
    buf[6] = 0x04;
    buf[7] = 0x01;
    buf[8] = 0x1e;
    buf[9] = r_flag;
    buf[10] = g_flag;
    buf[11] = b_flag;
    // app_send_user_data(ATT_CHARACTERISTIC_fff1_01_VALUE_HANDLE, buf,12, ATT_OP_AUTO_READ_CCC);
    os_time_dly(1);
}
