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

#include "system/includes.h"

#include "task.h"
#include "event.h"
#include "rf24g.h"
#include "led_strip_sys.h"
#include "board_ac632n_demo_cfg.h"
#include "led_strand_effect.h"
#include "rf24g_app.h"

// #if TCFG_RF24GKEY_ENABLE
#if 1
extern rf24g_ins_t rf24g_ins;
#define PAIR_MAX 2

#pragma pack(1)
typedef struct
{
    u8 pair[3];
    u8 flag; // 0:表示该数组没使用，0xAA：表示改数组已配对使用
} rf24g_pair_t;

#pragma pack()

rf24g_pair_t rf24g_pair[PAIR_MAX]; // 需要写flash
/***********************************************************移植须修改****************************************************************/
extern void save_user_data_area3(void);
#define PAIR_TIME_OUT 5 * 1000 // 3秒
static u16 pair_tc = 0;

// 配对计时，10ms计数一次
void rf24g_pair_tc(void)
{
    if (pair_tc <= PAIR_TIME_OUT)
    {
        pair_tc += 10;
    }
}

/***********************************************************移植须修改 END****************************************************************/

/***********************************************************API*******************************************************************/
#define CFG_USER_PAIR_DATA 4 // 保存2.4G遥控客户码
void save_rf24g_pair_data(void)
{
    syscfg_write(CFG_USER_PAIR_DATA, (u8 *)(&rf24g_pair[0]), sizeof(rf24g_pair_t));
}

void read_rf24g_pair_data(void)
{
    syscfg_read(CFG_USER_PAIR_DATA, (u8 *)(&rf24g_pair[0]), sizeof(rf24g_pair_t));
    printf_buf((u8 *)(&rf24g_pair[0]), sizeof(rf24g_pair_t));
}

//-------------------------------------------------效果

// -----------------------------------------------声控

// -----------------------------------------------灵敏度

/***********************************************************APP*******************************************************************/

// pair_handle是长按执行，长按时会被执行多次
// 所以执行一次后，要把pair_tc = PAIR_TIME_OUT，避免误触发2次
extern void ls_lenght_add(u8 l);
extern void ls_lenght_sub(u8 l);

static void pair_handle(void)
{
    extern void save_rf24g_pair_data(void);
    u8 op = 0; // 1:配对，2：解码
    u8 i;

    // 开机3秒内
    if (pair_tc < PAIR_TIME_OUT)
    {
        // printf("\n pair_tc=%d",pair_tc);
        pair_tc = PAIR_TIME_OUT; // 避免误触发2次
        memcpy((u8 *)(&rf24g_pair[0].pair), (u8 *)(&rf24g_ins.pair), 3);
        rf24g_pair[0].flag = 0xaa;
        save_rf24g_pair_data();
        // printf("\n pair");
        // printf_buf(&rf24g_pair[0].pair, 3);
        extern void fc_24g_pair_effect(void); // 配对效果
        fc_24g_pair_effect();
        // 查找表是否存在
    }
}

extern u8 ble_state;

u8 off_long_cnt = 0;
extern u8 Ble_Addr[6];
extern u8 auto_set_led_num_f;
u8 pexil_switch = 1;

u8 yaokong_or_banzai = 0;
uint16_t need_to_set_num;
extern void set_ls_lenght(u16 l);

#define _3V_12jian_head1 0x34
#define _3V_12jian_head2 0x12

#define _12V_18jian_head1 0x01
#define _12V_18jian_head2 0x23

extern u16 rf24_T0;
extern u16 rf24_T1;
extern u8 last_key_v;
u8 key_value = 0;
extern void ls_switch(void);
extern u8 stepmotpor_speed_cnt;
u8 long_press_f = 0;
u8 long_press_cnt = 0;
extern void add_meteor_mode(void);
extern void sub_meteor_mode(void);
extern void ls_speed_sub(void);
extern void ls_speed_plus(void);
extern void meteor_p_pul(void);
extern void meteor_p_sub(void);
extern void bright_plus(void);

void rf24_key_handle(void)
{

    if (rf24_T0 < 0xFFFF)
        rf24_T0++;

    if (rf24_T0 > 150 && rf24_T1 < 10)
    {
        key_value = last_key_v;
        if (key_value == NO_KEY)
            return;
        last_key_v = NO_KEY;
        rf24_T0 = 0;
        rf24_T1 = 0;

        /*CODE*/

        if (rf24g_ins.header1 == 0XDC && rf24g_ins.header2 == 0XDC)
        {

#if 0
            
            //软关灯
            if(key_value == RF24_K03) {
               
                soft_rurn_off_lights();    
            
            //软开灯
            } else if(key_value == RF24_K04){

                soft_turn_on_the_light();   
            }


            if(get_on_off_state() == DEVICE_ON) 
            {

                //1-10种模式加
                if(key_value == RF24_K01 ) {
                
                    extern void add_meteor_mode(void);
                    add_meteor_mode();
                  //  1-10种模式减
                } else if(key_value == RF24_K02 ) {
         
                 

                     extern void sub_meteor_mode(void);
                    sub_meteor_mode();

                    //流星速度加
                } else if(key_value == RF24_K05 ) {
                   
                    extern void ls_speed_plus(void);
                    ls_speed_plus();
                    fc_meteor_speed();

                    //流星速度减
                } else if(key_value == RF24_K06 ) {

                    extern void ls_speed_sub(void);
                    ls_speed_sub();
                    fc_meteor_speed();

                    //周期加
                } else if(key_value == RF24_K07 ) {

                    extern void meteor_p_pul(void);
                    meteor_p_pul();
                    fc_meteor_p();


                    //周期减
                } else if(key_value == RF24_K08 ) {
                   

                    extern void meteor_p_sub(void);
                    meteor_p_sub();
                    fc_meteor_p();

                    //亮度加
                } else if(key_value == RF24_K09 ) {
                    extern void bright_plus(void);
                    bright_plus();
                    //亮度减
                } else if(key_value == RF24_K10 ) {
                    extern void bright_sub(void);
                    bright_sub();
                    //拖尾加
                } else if(key_value == RF24_K11 ) {
                    u8 temp1 = get_custom_index();
                    if(temp1 < 4) {
                        temp1++;
                    } else if (temp1 > 4 && temp1 < 8) {
                        temp1++;
                    } 
                    printf("temp1 = %d",temp1);
                    set_mereor_mode(temp1);
                    set_fc_effect();
                    //拖尾减
                } else if(key_value == RF24_K12 ) {
                    u8 temp2 = get_custom_index();
                    
                    if(temp2 > 0 && temp2 <= 4) {
                        temp2--;
                    } else if (temp2 >= 8 && temp2 < 5) {
                        temp2--;
                    } 


                    printf("temp2 = %d",temp2);
                    set_mereor_mode(temp2);
                    set_fc_effect();
                    //正反流星
                } else if(key_value == RF24_K13 ) {
                    u8 temp = get_custom_index();
                    if(temp == 1 || temp == 2 || temp == 3 || temp == 4 || temp == 9)
                    {
                        switch(temp)
                        {
                            case 1: temp = 5; break;
                            case 2: temp = 6; break;
                            case 3: temp = 7; break;
                            case 4: temp = 8; break;
                            case 9: temp = 10; break;

                        }
                    }
                    else if(temp == 5 || temp == 6 || temp == 7 || temp == 8 || temp == 10)
                    {
                        switch(temp)
                        {
                            case 5: temp = 1; break;
                            case 6: temp = 2; break;
                            case 7: temp = 3; break;
                            case 8: temp = 4; break;
                            case 10: temp = 9; break;

                        }
                    }
                    else
                    {
                        temp = 1;
                    }
                  
                    set_mereor_mode(temp);
                    set_fc_effect();
                    //周期最短
                } else if(key_value == RF24_K14 ) {
                    set_meteor_p(2);
                    fc_meteor_p();
                    //定周期 10s
                } else if(key_value == RF24_K15 ) {
                    set_meteor_p(10);
                    fc_meteor_p();
                    //周期最长
                } else if(key_value == RF24_K16 ) {
                    set_meteor_p(20);
                    fc_meteor_p();
                    //音乐律动1
                } else if(key_value == RF24_K17 ) {
                    set_mereor_mode(11);
                    //音乐律动2
                } else if(key_value == RF24_K18 ) {
                    set_mereor_mode(12);
                    //音乐律动3
                } else if(key_value == RF24_K19 ) {
                    set_mereor_mode(13);
                    //流星效果1
                } else if(key_value == RF24_K20 ) {
                    set_mereor_mode(1);
                    //流星效果2
                } else if(key_value == RF24_K21 ) {
                    set_mereor_mode(2);
                    //流星效果3
                } else if(key_value == RF24_K22 ) {
                    set_mereor_mode(3);
                    //流星效果4
                } else if(key_value == RF24_K23 ) {
                    set_mereor_mode(4);
                    //流星效果5
                } else if(key_value == RF24_K24 ) {
                    set_mereor_mode(5);
                    //流星效果6
                } else if(key_value == RF24_K25 ) {
                    set_mereor_mode(6);
                    //流星效果7
                } else if(key_value == RF24_K26 ) {
                    set_mereor_mode(7);
                    //流星效果8
                } else if(key_value == RF24_K27 ) {
                    set_mereor_mode(8);
                    //流星效果9
                } else if(key_value == RF24_K28 ) {
                    set_mereor_mode(9);
                    
                }
                
            }

#endif
        }
        else if (rf24g_ins.header1 == 0XCC && rf24g_ins.header2 == 0X02)
        {

            // 软开关灯
            if (key_value == _17_key1)
            {

                ls_switch();
            }

            if (get_on_off_state() == DEVICE_ON)
            {

                if (key_value == _17_key2)
                {
                    // MODE+
                    // 模式加

                    add_meteor_mode();
                }
                else if (key_value == _17_key3)
                {
                    // 速度减
                    // SPEED-

                    ls_speed_sub();
                    fc_meteor_speed();
                }
                else if (key_value == _17_key4)
                {
                    // 恢复出厂
                    fc_data_init();
                    set_fc_effect();
                    // demo
                }
                else if (key_value == _17_key5)
                {
                    // 速度加
                    ls_speed_plus();
                    fc_meteor_speed();
                    // SPEED+
                }
                else if (key_value == _17_key6)
                {
                    // COLOR+
                    //  流水时间间隔+

                    meteor_p_pul();
                    fc_meteor_p();
                }

                else if (key_value == _17_key7)
                {
                    // MODE-
                    //  模式-

                    sub_meteor_mode();
                }
                else if (key_value == _17_key8)
                {
                    // BRIGHT+
                    // 亮度加

                    bright_plus();
                }
                else if (key_value == _17_key9)
                {
                    // COLOR-
                    // 流水时间间隔-
                    meteor_p_sub();
                    fc_meteor_p();
                }
                else if (key_value == _17_key10)
                {
                    // A
                }
                else if (key_value == _17_key11)
                {
                    // BRIGHT-
                    //    流水切换方向
                    u8 temp = get_custom_index();
                    if (temp == 1 || temp == 2 || temp == 3 || temp == 4 || temp == 9)
                    {
                        switch (temp)
                        {
                        case 1:
                            temp = 5;
                            break;
                        case 2:
                            temp = 6;
                            break;
                        case 3:
                            temp = 7;
                            break;
                        case 4:
                            temp = 8;
                            break;
                        case 9:
                            temp = 10;
                            break;
                        }
                    }
                    else if (temp == 5 || temp == 6 || temp == 7 || temp == 8 || temp == 10)
                    {
                        switch (temp)
                        {
                        case 5:
                            temp = 1;
                            break;
                        case 6:
                            temp = 2;
                            break;
                        case 7:
                            temp = 3;
                            break;
                        case 8:
                            temp = 4;
                            break;
                        case 10:
                            temp = 9;
                            break;
                        }
                    }
                    else
                    {
                        temp = 1;
                    }

                    set_mereor_mode(temp);
                    set_fc_effect();
                }
                else if (key_value == _17_key12)
                {
                    // B
                    //   时间间隔最短
                    set_meteor_p(2);
                    fc_meteor_p();
                }
                else if (key_value == _17_key13)
                {
                    // C
                }
                else if (key_value == _17_key14)
                {
                    // D
                }
                else if (key_value == _17_key15)
                {
                    // E
                }
                else if (key_value == _17_key16)
                {
                    // F
                    //    6种流水时间间隔固定键
                    set_meteor_p(10);
                    fc_meteor_p();
                }
                else if (key_value == _17_key17)
                {
                    // G
                    // 时间间隔最长

                    set_meteor_p(20);
                    fc_meteor_p();
                }
            }
        }
        save_user_data_area3();
    }
    else if (rf24_T0 > 150)
    {
        rf24_T1 = 0;
        long_press_f = 0;
        long_press_cnt = 0;
        printf("cled");
    }

    if (get_rf24g_long_state() == KEY_EVENT_LONG && rf24_T1 > 30) // 长按 rf24_T1 100ms+1
    {
        long_press_f = 1;
        key_value = last_key_v;
        last_key_v = NO_KEY;

        // code
    }
}

void long_press_handle(void)
{

    extern u8 single_flow_flag;
    if ((get_on_off_state() == DEVICE_ON) && long_press_f)
    {
        long_press_cnt++;
    }
}

#endif
