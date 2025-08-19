#include "system/includes.h"
#include "app_config.h"
#include "app_action.h"
#include "app_main.h"
#include "update.h"
#include "update_loader_download.h"
#include "app_charge.h"
#include "app_power_manage.h"
#include "system/includes.h"
#include "system/event.h"
#include "asm/mcpwm.h"
#include "le_trans_data.h"
#include "btstack/bluetooth.h"
#include "key_app.h"
//��������ɨ������б�
struct key_driver_para irkey_scan_para = {
    .scan_time 	  	  = 10,				//����ɨ��Ƶ��, ��λ: ms
    .last_key 		  = NO_KEY,  		//��һ��get_value����ֵ, ��ʼ��ΪNO_KEY;
    .filter_time  	  = 2,				//����������ʱ;
    .long_time 		  = 75,  			//�����ж���������
    .hold_time 		  = (75 + 15),  	//�����ж�HOLD����
    .click_delay_time = 2,//20,				//������̧���ȴ�������ʱ����
    .key_type		  = KEY_DRIVER_TYPE_IR,
    .get_value 		  = ir_get_key_value,
};

typedef struct _IR_KEY {
    u32 ir_addr;
	u16 ir_code;
	bool ir_flag;
	u32 ir_timeout;
}IR_KEY;

static IR_KEY ir_key;

//___interrupt
AT_VOLATILE_RAM_CODE
void ir_detect_isr(void)
{
	static bool last_status = 1,ir_data_bit;
	static u8 soft_ir_cnt = 0, time_cnt = 250;
	static u32 ir_data_temp;
	bool new_status;

	if(gpio_read(TCFG_IRKEY_PORT))
	{
		new_status = 1;
	}
	else
	{
		new_status = 0;
	}

	if(new_status!=last_status)
	{
		last_status = new_status;
		if(new_status==0)
		{
			//if((time_cnt>100)&&(time_cnt<250))
			//printf("%d\n",time_cnt);
			if(((time_cnt>100)&&(time_cnt<150))//13.5ms ����ͷ //11.25ms ������
			// ||((time_cnt>112)&&(time_cnt<123))
			)
			{
				soft_ir_cnt = 0;
				time_cnt = 1;
				ir_data_temp = 0;
				//printf("-");
				ir_key.ir_timeout = 0;
				return;
			}
			else if((time_cnt>8)&&(time_cnt<15))// 1.125ms	 bit0
			{
				ir_data_bit = 0;
				//printf("0");
			}
			else if((time_cnt>18)&&(time_cnt<30))// 2.25ms	 bit1
			{
				ir_data_bit = 1;
				//printf("1");
			}
			else
			{
				time_cnt =1;
				return;
			}
			time_cnt = 1;

			ir_data_temp >>= 1;
			soft_ir_cnt++;

			if(ir_data_bit)
			{
				ir_data_temp |= 0x80000000;
				//printf("1");
			}
			else
			{
				//printf("0");
			}

			if (soft_ir_cnt == 32)
			{
				ir_key.ir_addr= ir_data_temp&0xffff;
				ir_key.ir_code= (ir_data_temp>>16)&0xff;
				ir_key.ir_flag = 1;
//				printf("code = %x\n",ir_key.ir_code);
//				printf("%02x ",ir_cb.cmd);
			}
		}
	}
	else
	{
		if(time_cnt<250)
		{
			time_cnt++;
		}
	}
}

void user_ir_init(u8 port)
{
	gpio_set_die(port, 1);
	gpio_set_direction(port, 1);
	gpio_set_pull_up(port, 1);
}

/*----------------------------------------------------------------------------*/
/**@brief   ir������ʼ��
   @param   void
   @param   void
   @return  void
   @note    void ir_key_init(void)
*/
/*----------------------------------------------------------------------------*/
extern const struct irkey_platform_data irkey_data;

int irkey_init(const struct irkey_platform_data *irkey_data)
{
    printf("------------irkey_init---------------");
	user_ir_init(irkey_data->port);
    return 0;
}
const u8 ir_tbl_FF00[][2] =
{
	{ 0x2d, IRKEY_AUTO},
	{ 0x26, IRKEY_ON},
	{ 0x25, IRKEY_OFF},
	{ 0xF, IRKEY_LIGHT_PLUS},
	{ 0x6E, IRKEY_LIGHT_SUB},
	{ 0x5B, IRKEY_SPEED_PLUS},
	{ 0x5C, IRKEY_SPEED_SUB},
	{ 0x6, IRKEY_R},
	{ 0x8, IRKEY_G},
	{ 0x5A, IRKEY_B},
	{ 0x9, IRKEY_YELLOW},
	{ 0x10, IRKEY_CYAN},
	{ 0x1C, IRKEY_PURPLE},
	{ 0xC, IRKEY_ORANGE},
	{ 0x8A, IRKEY_B1},
	{ 0x52, IRKEY_B2},
	{ 0x53, IRKEY_G1},
	{ 0x1A, IRKEY_W},
	{ 0xB, IRKEY_MODE_ADD},
	{ 0xA, IRKEY_MODE_DEC},
	{ 0x1, IRKEY_MUSIC1},
	{ 0x2, IRKEY_MUSIC2},
	{ 0x1f, IRKEY_COLOR},
	{ 0x0, IRKEY_LOCK},
	{ 0xff, NO_KEY},

};

u8 ir_key_get(const u8 ir_table[][2], u8 ir_data)
{
    u8 keyval = NO_KEY;
    for(u8 i=0;;i++)
    {
        if((ir_table[i][1] == ir_data) || (ir_table[i][0] == 0xFF))
        {
            keyval = ir_table[i][1];
            break;
        }
    }
    return keyval;
}

u8 ir_get_key_value(void)
{
    u8 tkey = NO_KEY;

	if(ir_key.ir_timeout< 16)
	{
		ir_key.ir_timeout++;
	}
	else
	{
		ir_key.ir_addr= 0;
		ir_key.ir_code = 0;
		ir_key.ir_timeout = 0xff;//time out
		ir_key.ir_flag = 0;
	}

	if(ir_key.ir_flag)
	{
		tkey = ir_key_get(ir_tbl_FF00, ir_key.ir_code);
		printf("tkey = %d\n",tkey);
	}

    return tkey;
}

