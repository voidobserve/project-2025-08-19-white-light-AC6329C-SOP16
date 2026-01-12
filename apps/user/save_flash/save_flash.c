
#include "system/includes.h"
#include "syscfg_id.h"
#include "save_flash.h"

#define CFG_USER_LED_LEDGTH_DATA 3

/*
    大致功能：
    需要写入flash时，调用：
    os_taskq_post("msg_task", 1, MSG_USER_SAVE_INFO);

    用户消息处理线程：
    save_user_data_enable() 使能延时写入flash的操作

    主循环：
    save_user_data_time_count_down() 倒计时
    save_user_data_handle() 延时写入flash的操作使能，并且倒计时到来，执行写入flash操作
*/

#define FLASH_CRC_DATA 0xC5

static volatile u16 time_count_down = 0; // 存放当前的倒计时
static volatile u8 flag_is_enable_count_down = 0;
static volatile u8 flag_is_enable_to_save = 0; // 标志位，是否使能了保存
 
u8 ble_state; // 默认开启BLE模块 

/*******************************************************************************************************
**函数名：上电读取FLASH里保存的指令数据
**输  出：
**输  入：读取CFG_USER_COMMAND_BUF_DATA里保存的最后一条接收到的指令，
**描  述：读取CFG_USER_LED_LEDGTH_DATA里保存的第一次上电标志，灯带长度，顺序是：：第1字节：第一次上电标志位，第2、3字节：灯带长度
**说  明：
**版  本：
**修改日期：
*******************************************************************************************************/
void read_flash_device_status_init(void)
{
    extern void set_custom_index(u8 m);
    extern void set_sensitive(u8 s);

#if 1
    int ret;
    save_flash_t save_flash3;

    memset((u8 *)&save_flash3, 0, sizeof(save_flash_t));

    ret = syscfg_read(CFG_USER_LED_LEDGTH_DATA, (u8 *)(&save_flash3), sizeof(save_flash_t));
    if (ret != sizeof(save_flash_t))
    {
        // 如果读取到的数据个数不一致
        // printf("read save info error \n");
        memset((u8 *)&save_flash3, 0, sizeof(save_flash_t));
    }

    if (save_flash3.header != FLASH_CRC_DATA) // 第一次上电
    {
        fc_data_init();
        ble_state = 1; // 默认开启BLE模块
        os_taskq_post("msg_task", 1, MSG_USER_SAVE_INFO);
    }
    else
    {
        memcpy((u8 *)(&fc_effect), (u8 *)(&save_flash3.fc_save), sizeof(fc_effect_t));
        set_custom_index(fc_effect.custom_index); // 设置动画索引
        set_sensitive(fc_effect.music.s);         // 设置灵敏度
        ble_state = save_flash3.ble_state; 
    }

#endif
}

/**
 * @brief 写入flash倒计时
 *      10ms调用一次，不需要特别准确
 *
 *      如果 flag_is_enable_count_down == 1，表示使能倒计时
 *      如果 flag_is_enable_count_down == 0，表示未使能倒计时
 *
 *      计时结束，将 flag_is_enable_to_save 置一
 */
void save_user_data_time_count_down(void)
{
    if (0 == flag_is_enable_count_down)
    {
        return;
    }

    if (time_count_down > 0)
    {
        time_count_down--;
    }

    if (0 == time_count_down)
    {
        flag_is_enable_count_down = 0;
        flag_is_enable_to_save = 1;
    }
}

// 把用户数据写到区域3 （对外隐藏，只在当前.c 文件中使用）
static void save_user_data_area3(void)
{
    extern u8 get_sensitive(void);
    extern u8 get_custom_index(void);

    int ret = 0;
    save_flash_t save_data;
    save_data.header = FLASH_CRC_DATA;
    // 不保存开关机状态，默认开机
    // fc_effect.on_off_flag = DEVICE_ON;
    fc_effect.sensitive = get_sensitive();
    fc_effect.custom_index = get_custom_index();
    save_data.ble_state = ble_state; 
    memcpy((u8 *)(&save_data.fc_save), (u8 *)(&fc_effect), sizeof(fc_effect_t));
    os_time_dly(1); // 先让出cpu，处理其他任务，防止看门狗复位

    ret = syscfg_write(CFG_USER_LED_LEDGTH_DATA, (u8 *)(&save_data), sizeof(save_flash_t));
    printf("save info done \n");
}

/**
 * @brief 使能延时写入flash的操作
 *
 */
void save_user_data_enable(void)
{
    flag_is_enable_count_down = 0;
    time_count_down = DELAY_SAVE_FLASH_TIMES / 10; // DELAY_SAVE_FLASH_TIMES / 10 ms计时，实现 DELAY_SAVE_FLASH_TIMES ms延时
    flag_is_enable_count_down = 1;
}

/**
 * @brief 保存用户数据
 *          需要放到主循环执行
 *
 * @return * void
 */
void save_user_data_handle(void)
{
    if (flag_is_enable_to_save)
    {
        flag_is_enable_to_save = 0;
        save_user_data_area3();
    }
}
