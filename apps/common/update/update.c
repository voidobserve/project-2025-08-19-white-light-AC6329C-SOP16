#include "update.h"
#include "update_loader_download.h"
#include "asm/crc16.h"
#include "asm/wdt.h"
#include "os/os_api.h"
#include "app_config.h"
#include "cpu.h"
#include "syscfg_id.h"
#include "btcontroller_modules.h"
#include "system/includes.h"
#include "uart_update.h"
#include "dual_bank_updata_api.h"
#include "btstack/avctp_user.h"

#if (SMART_BOX_EN && JL_SMART_BOX_EXTRA_FLASH_OPT)
#include "smartbox_extra_flash_opt.h"
#endif


#if TCFG_UI_ENABLE
#include "ui/ui_api.h"
#endif

#if RCSP_BTMATE_EN
#include "rcsp_user_update.h"
#elif RCSP_ADV_EN
#include "rcsp_adv_user_update.h"
#endif

#include "custom_cfg.h"

#include "update_tws.h"


#define LOG_TAG "[APP-UPDATE]"
#define LOG_INFO_ENABLE
#define LOG_ERROR_ENABLE
#include "system/debug.h"

extern void *dev_update_get_parm(int type);
extern u8 get_ota_status();
extern void get_nor_update_param(void *buf);
extern bool get_tws_phone_connect_state(void);
extern void tws_sniff_controle_check_disable(void);
extern void tws_tx_unsniff_req(void);
void sys_auto_sniff_controle(u8 enable, u8 *addr);

extern const int support_norflash_update_en;


#ifdef DEV_UPDATE_SUPPORT_JUMP
extern void __JUMP_TO_MASKROM();
extern void save_spi_port();
extern s32 sd1_unmount(void);
extern void usb_sie_close_all(void);
#endif      //endif DEV_UPDATE_SUPPORT_JUMP


#ifdef UPDATE_VOICE_REMIND
#include "tone_player.h"
#include "audio_config.h"
#endif

#ifdef UPDATE_LED_REMIND
#include "asm/pwm_led.h"
#endif

#define DEVICE_UPDATE_KEY_ERR  BIT(30)
#define DEVICE_FIRST_START     BIT(31)
//升级文件路径必须是短文件名（8+3）结构，仅支持２层目录
/* const char updata_file_name[] = "/UPDATA/JL_692X.BFU"; */
const char updata_file_name[] = "/*.UFW";
u32 g_updata_flag = 0;
u16 update_result_get(void)
{
    u16 ret = UPDATA_NON;

    if (!UPDATE_SUPPORT_DEV_IS_NULL()) {
        UPDATA_PARM *p = UPDATA_FLAG_ADDR;
        u16 crc_cal;
        crc_cal = CRC16(((u8 *)p) + 2, sizeof(UPDATA_PARM) - 2);	//2 : crc_val
        if (crc_cal && crc_cal == p->parm_crc) {
            ret =  p->parm_result;
        }
        g_updata_flag = ret;
        g_updata_flag |= ((u32)(p->magic)) << 16;

        memset(p, 0x00, sizeof(UPDATA_PARM));
    }

    return ret;
}

void update_result_set(u16 result)
{
    if (!UPDATE_SUPPORT_DEV_IS_NULL()) {
        UPDATA_PARM *p = UPDATA_FLAG_ADDR;

        memset(p, 0x00, sizeof(UPDATA_PARM));
        p->parm_result = result;
        p->parm_crc = CRC16(((u8 *)p) + 2, sizeof(UPDATA_PARM) - 2);
    }
}

bool update_success_boot_check(void)
{
    if (!UPDATE_SUPPORT_DEV_IS_NULL()) {
        u16 result = g_updata_flag & 0xffff;
        u16 up_tpye = g_updata_flag >> 16;
        if ((UPDATA_SUCC == result) && ((SD0_UPDATA == up_tpye) || (USB_UPDATA == up_tpye))) {
            return true;
        }
    }
    return false;
}

bool device_is_first_start()
{
    log_info("g_updata_flag=0x%x\n", g_updata_flag);
    if ((g_updata_flag & DEVICE_FIRST_START) || (g_updata_flag & DEVICE_UPDATE_KEY_ERR) || (g_updata_flag == UPDATA_SUCC)) {
        puts("\n=================device_is_first_start=========================\n");
        return true;
    }
    return false;
}

void led_update_start(void)
{
#ifdef UPDATE_LED_REMIND
    puts("led_update_start\n");
    pwm_led_mode_set(PWM_LED_ALL_OFF);
#endif
}

void led_update_finish(void)
{
#ifdef UPDATE_LED_REMIND
    puts("led_update_finish\n");
    pwm_led_mode_set(PWM_LED0_LED1_FAST_FLASH);
#endif
}

static inline void dev_update_close_ui()
{

#if (TCFG_UI_ENABLE&&(CONFIG_UI_STYLE == STYLE_JL_LED7))
    u8 count = 0;
    UI_SHOW_WINDOW(ID_WINDOW_POWER_OFF);
__retry:
    if (UI_GET_WINDOW_ID() != ID_WINDOW_POWER_OFF) {
        os_time_dly(10);//增加延时防止没有关显示
        if (count < 3) {
            goto __retry;
        }
        count++;
    }
#endif
}

extern void delay_2ms(int cnt);
extern void app_audio_set_wt_volume(s16 volume);

int update_result_deal()
{
#ifdef CONFIG_FPGA_ENABLE
    return 0;
#endif

    u8 key_voice_cnt = 0;
    u16 result = 0;
    result = (g_updata_flag & 0xffff);
    log_info("<--------update_result_deal=0x%x %x--------->\n", result, g_updata_flag >> 16);
#ifdef  CONFIG_DEBUG_ENABLE
#if TCFG_APP_BT_EN
    u8 check_update_param_len(void);
    ASSERT(check_update_param_len(), "UPDATE_PARAM_LEN ERROR");
#endif
#endif
    if (result == UPDATA_NON || 0 == result) {
        return 0;
    }
#ifdef UPDATE_VOICE_REMIND
#endif
    if (result == UPDATA_SUCC) {
#if(JL_EARPHONE_APP_EN && RCSP_UPDATE_EN)
        u8 clear_update_flag = 0;
        syscfg_write(VM_UPDATE_FLAG, &clear_update_flag, 1);
#endif
#if(JL_SMART_BOX_EXTRA_FLASH_OPT && RCSP_UPDATE_EN)
        smartbox_eflash_update_flag_set(0);
        smartbox_eflash_flag_set(0);
#endif
#ifdef UPDATE_LED_REMIND
        led_update_finish();
#endif
    }

    extern u8 get_max_sys_vol(void);
    while (1) {
        wdt_clear();
        key_voice_cnt++;
#ifdef UPDATE_VOICE_REMIND
        if (result == UPDATA_SUCC) {
            puts("<<<<<<UPDATA_SUCC");
            app_audio_set_volume(APP_AUDIO_STATE_WTONE, get_max_sys_vol() / 2, 1);
            //tone_play_index(IDEX_TONE_NORMAL, 1);
            tone_play(TONE_SIN_NORMAL, 1);
            /* os_time_dly(25); */
            /* tone_stop(); */
            //delay_2ms(500);
            os_time_dly(25);
            puts(">>>>>>>>>>>\n");
        } else {
            log_info("!!!!!!!!!!!!!!!updata waring !!!!!!!!!!!=0x%x\n", result);
            app_audio_set_volume(APP_AUDIO_STATE_WTONE, get_max_sys_vol() / 2, 1);
            tone_play(TONE_SIN_NORMAL, 1);
            //tone_play_index(IDEX_TONE_NORMAL, 1);
            /* os_time_dly(50); */
            /* tone_stop(); */
            os_time_dly(25);
            //delay_2ms(500);
        }
#endif
        if (key_voice_cnt > 5) {
            key_voice_cnt = 0;
            //delay_2ms(500);
            //os_time_dly(100);
            puts("enter_sys_soft_poweroff\n");
            break;
            //注:关机要慎重,要设置开机键
            //enter_sys_soft_poweroff();
        }
    }

    return 1;
}

void clr_update_ram_info(void)
{
    UPDATA_PARM *p = UPDATA_FLAG_ADDR;
    memset(p, 0x00, sizeof(UPDATA_PARM));
}

static u32 loader_start_addr = 0;
void set_loader_start_addr(u32 addr)
{
    loader_start_addr = addr;
}

void update_close_hw(void *filter_name)
{
    const struct update_target *p;
    list_for_each_update_target(p) {
        if (memcmp(filter_name, p->name, strlen(filter_name) != 0)) {
            printf("close Hw Name : %s\n", p->name);
            p->driver_close();
        }
    }
}

//write(buf,len);
void updata_parm_set(UPDATA_TYPE up_type, void *priv, u32 len)
{
    UPDATA_PARM *p = UPDATA_FLAG_ADDR;
    u8 addr[6];
#ifdef UPDATE_LED_REMIND
    led_update_start();
#endif
    memset((u8 *)p, 0x00, sizeof(UPDATA_PARM));
    p->parm_type = (u16)up_type;
    p->parm_result = (u16)UPDATA_READY;
    memcpy(p->file_path, updata_file_name, strlen(updata_file_name));
    if (priv) {
        if (support_norflash_update_en) {
            printf("p->parm_priv:0x%x\n len:%d\n", p->parm_priv, len);
            memcpy(p->parm_priv + 12, priv, len);
        } else {
            printf("p->parm_priv:0x%x priv:0x%x len:%d\n", p->parm_priv, priv, len);
            memcpy(p->parm_priv, priv, len);
        }
    } else {
        memset(p->parm_priv, 0x00, sizeof(p->parm_priv));
        if ((up_type == BLE_TEST_UPDATA) && UPDATE_MODULE_IS_SUPPORT(UPDATE_BLE_TEST_EN)) {
            extern int le_controller_get_mac(void *addr);
            le_controller_get_mac(addr);
            memcpy(p->parm_priv, addr, 6);
            puts("ble addr:\n");
            put_buf(p->parm_priv, 6);
        }
    }
#if (USE_SDFILE_NEW == 1)
    p->magic = UPDATE_PARAM_MAGIC;
    p->ota_addr = loader_start_addr;
#endif

#ifdef CONFIG_SD_UPDATE_ENABLE
    if ((up_type == SD0_UPDATA) || (up_type == SD1_UPDATA)) {
        int sd_start = (u32)p + sizeof(UPDATA_PARM);
        void *sd = NULL;
        sd = dev_update_get_parm(up_type);
        if (sd) {
            memcpy((void *)sd_start, sd, UPDATE_PRIV_PARAM_LEN);
        } else {
            memset((void *)sd_start, 0, UPDATE_PRIV_PARAM_LEN);
        }
    }
#endif

#ifdef CONFIG_USB_UPDATE_ENABLE
    if (up_type == USB_UPDATA) {
        log_info("usb updata ");
        int usb_start = (u32)p + sizeof(UPDATA_PARM);
        memset((void *)usb_start, 0, UPDATE_PRIV_PARAM_LEN);
    }
#endif

    /* #if RCSP_UPDATE_EN */
    /*     extern u32 ex_cfg_get_start_addr(void); */
    /*     if (support_norflash_update_en) { */
    /*         *((u32 *)(p->parm_priv + 12)) = ex_cfg_get_start_addr();//12个字节是Norflash升级参数占用 */
    /*     } else { */
    /*         *((u32 *)(p->parm_priv)) = ex_cfg_get_start_addr();     //为了兼容之前的程序升级 */
    /*     } */
    /* #endif */
    if (support_norflash_update_en) {
        get_nor_update_param(p->parm_priv);
        p->parm_type = NORFLASH_UPDATA;
        *((u16 *)((u8 *)p + sizeof(UPDATA_PARM) + 32)) = up_type;         //将实际的升级类型保存到ram
    }

    p->parm_crc = CRC16(((u8 *)p) + 2, sizeof(UPDATA_PARM) - 2);	//2 : crc_val
    printf("UPDATA_PARM_ADDR = 0x%x\n", p);
    printf_buf((void *)p, sizeof(UPDATA_PARM));
    printf("exif_addr:0x%x\n", *(u32 *)(p->parm_priv));
}

//reset
void updata_enter_reset(UPDATA_TYPE up_type)
{
    log_info("updata_enter_reset\n\n\n");
    cpu_reset();
    //reset
    //JL_POWER->CON |= BIT(4);
    /* JL_CLOCK->PWR_CON |= BIT(4); */
}


#define LOADER_NAME		"LOADER.BIN"
const u8 loader_file_path[] = "mnt/norflash/C/"LOADER_NAME"";

extern void ll_hci_destory(void);
extern void hci_controller_destory(void);
extern const int support_norflash_update_en;
extern void ram_protect_close(void);

extern void hwi_all_close(void);
void wifi_det_close();
static void update_before_jump_common_handle(UPDATA_TYPE up_type)
{
    dev_update_close_ui();

#if TCFG_AUDIO_ANC_ENABLE
    extern void audio_anc_hw_close();
    audio_anc_hw_close();
#endif

    local_irq_disable();
    hwi_all_close();

    /*跳转的时候遇到死掉的情况很可能是硬件模块没关导致，加上保护可以判断哪个异常，保护的地址根据不同SDK而定*/
    /* u8 inv = 0; */
    /* mpu_set(1, (u32)&test_pro_addr, (u32)test_pro_addr, inv, "0r", DBG_FM); */
#ifdef CONFIG_SUPPORT_WIFI_DETECT
    wifi_det_close();
#endif
    /*跳转的时候遇到死掉的情况很可能是硬件模块没关导致，加上保护可以判断哪个异常，保护的地址根据不同SDK而定*/
    /* u8 inv = 0; */
    /* mpu_set(1, (u32)&test_pro_addr, (u32)test_pro_addr, inv, "0r", DBG_FM); */

}

extern void __bt_updata_reset_bt_bredrexm_addr(void);
extern int __bt_updata_save_connection_info(void);
void update_mode_api(UPDATA_TYPE up_type, ...)
{
    u8 i;
    u32 addr;
    dev_update_close_ui();
#if RCSP_ADV_EN || RCSP_BTMATE_EN || SMART_BOX_EN
    if (up_type == BLE_APP_UPDATA || up_type == SPP_APP_UPDATA) {
        addr = ex_cfg_fill_content_api();
    }
#endif
    //step 1: disable irq
#if TCFG_USER_TWS_ENABLE || TCFG_USER_BLE_ENABLE
    if (up_type == BT_UPDATA) {
        log_info("close ble hw\n");
        ll_hci_destory();
    }
#endif

#if TCFG_AUDIO_ANC_ENABLE
    extern void audio_anc_hw_close();
    audio_anc_hw_close();
#endif
    local_irq_disable();
    for (i = 0; i < 64; i++) {
        bit_clr_ie(i);
    }
    //step 2: prepare parm

    printf("update_type:0x%x\n", up_type);
    switch (up_type) {
#ifdef CONFIG_SD_UPDATE_ENABLE
    case SD0_UPDATA:
    case SD1_UPDATA:
        updata_parm_set(up_type, (u8 *)loader_file_path, sizeof(loader_file_path));
        break;
#endif
#ifdef CONFIG_USB_UPDATE_ENABLE
    case USB_UPDATA:
        updata_parm_set(up_type, (u8 *)loader_file_path, sizeof(loader_file_path));
        break;
#endif

    case UART_UPDATA:
        /* uart_updata_io_ctrl(&parm); */
        //log_info("up_io = %x\nup_baud = %d\nup_timeout = %dms\n", ((UPDATA_UART *)parm)->control_io, ((UPDATA_UART *)parm)->control_baud, ((UPDATA_UART *)parm)->control_timeout * 10);
        /* updata_parm_set(up_type, (u8 *)loader_file_path, sizeof(loader_file_path)); */
    {
        va_list argptr;
        va_start(argptr, up_type);
        u32 baudrate = va_arg(argptr, int);
        u32 uart_update_io_tx = va_arg(argptr, int);
        u32 uart_update_io_rx = va_arg(argptr, int);
        va_end(argptr);
        UPDATA_UART uart_param = {.control_baud = baudrate, .control_io_tx = uart_update_io_tx, .control_io_rx = uart_update_io_rx};
        updata_parm_set(up_type, (u8 *)&uart_param, sizeof(UPDATA_UART));
    }
    break;

    case BT_UPDATA:
        if (UPDATE_MODULE_IS_SUPPORT(UPDATE_BT_LMP_EN)) {
            if (__bt_updata_save_connection_info()) {
                log_error("bt save conn info fail!\n");
                break;
            }
            updata_parm_set(up_type, (u8 *)loader_file_path, sizeof(loader_file_path));
            ram_protect_close();
            __bt_updata_reset_bt_bredrexm_addr();
            //note:last func no return;
        }
        break;

    case BLE_TEST_UPDATA:
        if (BT_MODULES_IS_SUPPORT(BT_MODULE_LE)) {
            ll_hci_destory();
            updata_parm_set(up_type, NULL, 0);
        }
        break;

#if RCSP_UPDATE_EN
    case BLE_APP_UPDATA:
        updata_parm_set(up_type, (u8 *)&addr, sizeof(addr));
        break;

    case SPP_APP_UPDATA:
        updata_parm_set(BLE_APP_UPDATA, (u8 *)&addr, sizeof(addr)); //暂时不支持SPP
        break;
#endif

    default:
        updata_parm_set(up_type, NULL, 0);
        break;
    }


#ifdef DEV_UPDATE_SUPPORT_JUMP
#if TCFG_BLUETOOTH_BACK_MODE			//后台模式需要把蓝牙关掉
    /* if (BT_MODULES_IS_SUPPORT(BT_MODULE_LE)) { */
    /*     ll_hci_destory(); */
    /* } */
    hci_controller_destory();
#endif
    update_close_hw("null");
    ram_protect_close();
    save_spi_port();

    printf("update jump to mask...\n");
    /* JL_UART0->CON0 = 0; */
    /* JL_UART1->CON0 = 0; */
    __JUMP_TO_MASKROM();
#else
    //step 3: enter updata
    updata_enter_reset(up_type);
    /* updata_enter_jump(up_type); */
#endif      //DEV_UPDATE_SUPPORT_JUMP
}

/* void update_param_setting()
{
	u32 saddr
	void (*
} */

void update_parm_set_and_get_buf(int type, u32 loader_saddr, void **buf_addr, u16 *len)
{
    u32 exif_addr;

    int total_len = sizeof(UPDATA_PARM);
    if ((type == SD0_UPDATA) || (type == SD1_UPDATA) || (type == USB_UPDATA)) {
        total_len += UPDATE_PRIV_PARAM_LEN;
    }

    set_loader_start_addr(loader_saddr);
    if ((BLE_APP_UPDATA == type) || (SPP_APP_UPDATA == type)) {
#if RCSP_UPDATE_EN
        extern u32 ex_cfg_get_start_addr(void);
        exif_addr = ex_cfg_get_start_addr();
        printf("exif_addr:0x%x\n", exif_addr);
        updata_parm_set(type, (u8 *)&exif_addr, sizeof(exif_addr));
#endif
    } else if (UART_UPDATA == type) {
#if(USER_UART_UPDATE_ENABLE) && (UART_UPDATE_ROLE == UART_UPDATE_SLAVE)
        sava_uart_update_param();
#endif
    } else if (BLE_TEST_UPDATA == type) {
        updata_parm_set(type, NULL, 0);
    } else {
        updata_parm_set(type, (u8 *)loader_file_path, sizeof(loader_file_path));
    }

    *buf_addr = UPDATA_FLAG_ADDR;
    *len = total_len;
}
//fill common content
//fill private content
//fill crc16

static succ_report_t succ_report;
/* static void (*update_param_private_fill_handle)(UPDATA_PARM *p) = NULL;
static void (*update_before_jump_private_handle)(void) = NULL;
void update_param_private_fill_handle_register(void (*handle)(UPDATA_PARM *p))
{
	update_param_private_fill_handle = handle;
}

void update_before_jump_private_handle_register(void (*handle)(u16 type))
{
	update_before_jump_private_handle = handle;
} */

static void update_param_content_fill(int type, UPDATA_PARM *p, void (*priv_param_fill_hdl)(UPDATA_PARM *P))
{
    memset((u8 *)p, 0x00, sizeof(UPDATA_PARM));
    p->parm_type = (u16)type;
    p->parm_result = (u16)UPDATA_READY;
    p->magic = UPDATE_PARAM_MAGIC;
    p->ota_addr = succ_report.loader_saddr;

    if (0 == p->ota_addr) {
        log_error("ota addr err\n");
        return;
    }

    if (priv_param_fill_hdl) {
        priv_param_fill_hdl(p);
    }

    p->parm_crc = CRC16(((u8 *)p) + 2, sizeof(UPDATA_PARM) - 2);	//2 : crc_val
}

static void update_param_ram_set(u8 *buf, u16 len)
{
    u8 *update_ram = UPDATA_FLAG_ADDR;
    memcpy(update_ram, (u8 *)buf, len);
}

void update_mode_api_v2(UPDATA_TYPE type, void (*priv_param_fill_hdl)(UPDATA_PARM *p), void (*priv_update_jump_handle)(int type))
{
    u16 update_param_len = sizeof(UPDATA_PARM) + UPDATE_PRIV_PARAM_LEN;

    UPDATA_PARM *p = malloc(update_param_len);

    if (p) {
        update_param_content_fill(type, p, priv_param_fill_hdl);

        if (succ_report.update_param_write_hdl) {
            succ_report.update_param_write_hdl(succ_report.priv_param, p, update_param_len);
        }

#ifdef UPDATE_LED_REMIND
        led_update_start();
#endif
        update_before_jump_common_handle(type);

        update_param_ram_set((u8 *)p, update_param_len);

        if (priv_update_jump_handle) {
            priv_update_jump_handle(type);
        }

        free(p);
    } else {
        ASSERT(p, "malloc update param err \n");
    }
}

int update_check_sniff_en(void)

{
    if (!UPDATE_SUPPORT_DEV_IS_NULL()) {
#if (OTA_TWS_SAME_TIME_ENABLE && RCSP_ADV_EN && !OTA_TWS_SAME_TIME_NEW)
        if (tws_ota_control(OTA_STATUS_GET) != OTA_OVER) {
            return 0;
        }
#endif
        if (get_ota_status()) {
            log_info("ota ing...");
            return 0;
        } else {
            return 1;
        }
    }
    return 1;
}

static volatile u8 ota_status = 0;

void set_ota_status(u8 stu)
{
    ota_status = stu;
}

u8 get_ota_status()
{
    return ota_status;
}

static u8 ota_idle_query(void)
{
    return !ota_status;
}

REGISTER_LP_TARGET(ota_lp_target) = {
    .name = "ota",
    .is_idle = ota_idle_query,
};

extern void tws_sync_update_api_register(const update_op_tws_api_t *op);
extern update_op_tws_api_t *get_tws_update_api(void);

extern const int support_dual_bank_update_en;
extern int tws_ota_init(void);
extern void sys_auto_shut_down_disable(void);
extern void sys_auto_shut_down_enable(void);
extern void tws_api_auto_role_switch_disable();
extern void tws_api_auto_role_switch_enable();

static void update_init_common_handle(int type)
{
    ota_status = 1;
    if (UPDATE_DUAL_BANK_IS_SUPPORT()) {
#if TCFG_AUTO_SHUT_DOWN_TIME
        sys_auto_shut_down_disable();
#endif

#if OTA_TWS_SAME_TIME_ENABLE
        tws_api_auto_role_switch_disable();
        tws_sync_update_api_register(get_tws_update_api());
        tws_ota_init();
#endif
    }
}

static void update_exit_common_handle(int type, void *priv)
{
    update_ret_code_t *ret_code = (update_ret_code_t *)priv;

#if TCFG_AUTO_SHUT_DOWN_TIME
    sys_auto_shut_down_enable();
#endif

#if OTA_TWS_SAME_TIME_ENABLE
    if (UPDATE_DUAL_BANK_IS_SUPPORT()) {
        tws_api_auto_role_switch_enable();
    }
#endif

    ota_status = 0;
}

static void update_common_state_cbk(update_mode_info_t *info, u32 state, void *priv)
{
    int type = info->type;

    log_info("type:%x state:%x code:%x\n", type, state, priv);

    switch (state) {
    case UPDATE_CH_INIT:
        memset((u8 *)&succ_report, 0x00, sizeof(succ_report_t));
        update_init_common_handle(info->type);
        break;

    case UPDATE_CH_SUCESS_REPORT:
        log_info("succ report stored\n");
        memcpy((u8 *)&succ_report, (u8 *)priv, sizeof(succ_report_t));
        break;
    }

    if (info->state_cbk) {
        info->state_cbk(type, state, priv);
    }

    switch (state) {
    case UPDATE_CH_EXIT:
        update_exit_common_handle(info->type, priv);
        break;
    }
}


extern void update_module_init(void (*cbk)(update_mode_info_t *, u32, void *));
extern void testbox_update_init(void);
static void app_update_init(void)
{
    update_module_init(update_common_state_cbk);
    testbox_update_init();
}

__initcall(app_update_init);


void update_start_exit_sniff(void)
{
#if TCFG_USER_TWS_ENABLE
    volatile u8 wait_tws_sniff_exit = 1;
    if (get_tws_phone_connect_state() == TRUE) {
        g_printf("exit sniff mode...\n");
        user_send_cmd_prepare(USER_CTRL_ALL_SNIFF_EXIT, 0, NULL);
    } else {
        tws_tx_unsniff_req();
    }
    tws_sniff_controle_check_disable();
#else
    user_send_cmd_prepare(USER_CTRL_ALL_SNIFF_EXIT, 0, NULL);
#endif
    sys_auto_sniff_controle(0, NULL);
}

