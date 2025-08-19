/* 
适用用2.4G遥控
基于中道版本2.4G遥控
1、app_config.h,把宏CONFIG_BT_GATT_CLIENT_NUM设置1
2、@bt_ble_init() 加入multi_client_init()
3、@le_gatt_client.c    
   __resolve_adv_report（）
   HCI_EIR_DATATYPE_MORE_16BIT_SERVICE_UUIDS 加入键值处理函数
4、在key_driver.c 注册rf24g_scan_para
5、board_ac632n_demo_cfg.h 使能TCFG_RF24GKEY_ENABLE
6、@app_tuya.c tuya_key_event_handler()加入上层应用的键值处理函数
7、底层无法判断长按，需要靠上层应用实现
 */


/**该遥控框架，无需*/
#include "system/includes.h"
#include "app_config.h"
#include "board_ac632n_demo_cfg.h"
#include "rf24g.h"
// #if TCFG_RF24GKEY_ENABLE
#if 1
u8 rf24g_get_key_value(void);


struct key_driver_para rf24g_scan_para = {
    .scan_time 	  	  = 5,				//按键扫描频率, 单位: ms
    .last_key 		  = NO_KEY,  		//上一次get_value按键值, 初始化为NO_KEY;
    .filter_time  	  = 0,				//按键消抖延时;
    .long_time 		  = 500,  			//按键判定长按数量
    .hold_time 		  = 500,  	        //按键判定HOLD数量
    .click_delay_time = 0,//20,				//按键被抬起后等待连击延时数量
    .key_type		  = KEY_DRIVER_TYPE_RF24GKEY,
    .get_value 		  = rf24g_get_key_value,
};

#define HEADER1 0XDC
#define HEADER2 0XDC

#define HEADER1_1 0XCC
#define HEADER2_1 0X02

rf24g_ins_t rf24g_ins;

u8 rf24g_rx_flag = 0;

// 底层按键扫描，由__resolve_adv_report()调用
void rf24g_scan(u8 *pBuf)
{
    rf24g_ins_t *p = (rf24g_ins_t *) pBuf;
    // if(p->header1 == HEADER1 && p->header2 == HEADER2)
    // {
    //     // printf_buf(pBuf, sizeof(rf24g_ins_t));
    //     memcpy((u8*)&rf24g_ins, pBuf, sizeof(rf24g_ins_t));
    //     rf24g_rx_flag = 1;
    // }
    // else 
    
    if(p->header1 == HEADER1_1 && p->header2 == HEADER2_1)
    {
        // printf_buf(pBuf, sizeof(rf24g_ins_t));
        memcpy((u8*)&rf24g_ins, pBuf, sizeof(rf24g_ins_t));
        rf24g_rx_flag = 1;
    }



}

static u16 long_press_cnt;      /* 定时10ms++ */
#define LONG_PRESS_T    1000    /* 定时1秒返回long press */

/* 长按计数器 */
void rf24g_long_timer(void)
{
    // if(LONG_PRESS_T > long_press_cnt)
    long_press_cnt+=10;
    long_press_cnt%=LONG_PRESS_T + 200;
}

u8 last_dynamic_code;   //记录上次的滚码，和当前的滚码对比，实现长按判定
u8 last_key_v;
u8 last_key_v2;
u8 rf24g_key_state;     /* 用完需要手动清状态,值KEY_EVENT_LONG，  */

u16 rf24_T0 = 0;
u16 rf24_T1 = 0;


static u8 rf24g_get_key_value(void)
{
   
    if(rf24g_rx_flag == 1)  //收到2.4G广播
    {
        rf24g_rx_flag = 0;
        rf24_T0 = 0;
        
        // 上次的动态码不等于这次收到的动态码，则判定为按键按下
        if(last_dynamic_code != rf24g_ins.dynamic_code)// || last_key_v != rf24g_ins.key_v)
        {
            long_press_cnt = 0;
            // printf("\n dynamic_code=%d", rf24g_ins.dynamic_code);
            last_key_v = rf24g_ins.key_v;
     
            last_dynamic_code = rf24g_ins.dynamic_code;
            return rf24g_ins.key_v;
        }
        else // 上次的动态码等于这次收到的动态码，则判定为按键长按
        {
            if(long_press_cnt >= LONG_PRESS_T)
            {
                 rf24_T1++;
                long_press_cnt = 0;
                rf24g_key_state = KEY_EVENT_LONG;
                // printf("\n KEY_EVENT_LONG");
                last_key_v = rf24g_ins.key_v;
          
                return rf24g_ins.key_v;
            }
        }
        
    }

    
    // return NO_KEY;
}

/* 返回长按状态，阅后即焚 */
u8 get_rf24g_long_state(void)
{
    u8 tmp ;
    tmp = rf24g_key_state;
    rf24g_key_state = NO_KEY;
    return tmp;
}


#endif
