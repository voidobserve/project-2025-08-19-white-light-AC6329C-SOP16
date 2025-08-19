/*********************************************************************************************
    *   Filename        : led_music.c

    *   Description     :

    *   Author          :

    *   Email           :

    *   Last modifiled  :

    *
*********************************************************************************************/
#include "system/app_core.h"
#include "system/includes.h"

#include "app_config.h"
#include "app_action.h"

u32 triac_turn_on_timer=10;
u32 count_val=0;


/****************************************************************************************
**����:������3�жϷ������
**����:
**˵��:
**��ע:
**����:
*****************************************************************************************/
___interrupt
AT_VOLATILE_RAM_CODE
void capture_cycle_timer_isr(void)
{
//    printf("triac_turn_on_timer = %d\n",triac_turn_on_timer);

    JL_PORTB->OUT &= ~ BIT(7);

    count_val = JL_TIMER3->PRD;
    JL_TIMER3->CNT = 0;			    //�����ֵ
    JL_TIMER3->PRD = 0;
    JL_TIMER3->CON |= (1<<14);      //����жϱ�־

    JL_TIMER0->CNT = 0;
    JL_TIMER0->PRD = count_val*(1-triac_turn_on_timer/100);
    JL_TIMER0->CON |= (0b01 << 0);	//������ʱ��
    JL_TIMER0->CON |= (1<<14);      //�����ʱ���жϱ�־
}

/****************************************************************************************
**����:������3��ʼ��
**����:�½��ز���
**˵��:
**��ע:
**����:
*****************************************************************************************/
void capture_cycle_time_init(void)
{
    u32 u_clk = 24000000;

    gpio_set_die(IO_PORTB_06, 1);
	gpio_set_direction(IO_PORTB_06, 1);
	gpio_set_pull_up(IO_PORTB_06,1);
    gpio_set_pull_down(IO_PORTB_06, 0);

    //��ʼ��timer
    JL_TIMER3->CON = 0;
    JL_TIMER3->CON |= (0b110 << 10);					//ʱ��Դѡ��STD_24Mʱ��Դ
    JL_TIMER3->CON |= (0b0001 << 4);					//ʱ��Դ��4��Ƶ
    JL_TIMER3->CNT = 0;									//�����ֵ
    JL_TIMER3->PRD = 0;				                    //�����ֵ
    JL_TIMER3->CON |= (0b11 << 0);						//�½��ز���ģʽ
    JL_TIMER3->CON |= (1<<14);                          //����жϱ�־

    request_irq(IRQ_TIME3_IDX, 1, capture_cycle_timer_isr, 0);  //�����жϷ������
    gpio_set_fun_input_port(IO_PORTB_06, PFI_TMR3_CAP); //PB6�������
}

/****************************************************************************************
**����:��ʱ��0�жϷ������
**����:
**˵��:
**��ע:
**����:
*****************************************************************************************/
___interrupt
AT_VOLATILE_RAM_CODE
void triac_turn_on_timer_isr(void)
{
//    printf("triac_turn_on\n");

    JL_PORTB->OUT |= BIT(7);

    JL_TIMER0->CON |= (1<<14);                          //�����ʱ���жϱ�־
    JL_TIMER0->CON &= 0xfffc;                           //��ͣ����ģʽ
    JL_TIMER0->PRD = 0;
}
/****************************************************************************************
**����:��ʱ��0��ʼ��
**����:���ڶ�ʱ����
**˵��:
**��ע:
**����:
*****************************************************************************************/
void buty_time(void)
{
    //��ʼ��timer
    JL_TIMER0->CON = 0;
    JL_TIMER0->CON |= (0b110 << 10);					//ʱ��Դѡ��STD_24Mʱ��Դ
    JL_TIMER0->CON |= (0b0001 << 4);					//ʱ��Դ��4��Ƶ
//    JL_TIMER0->CON &= 0xfffc;                           //��ͣ����ģʽ
    JL_TIMER0->CON |= (1<<14);                          //�����ʱ���жϱ�־
    JL_TIMER0->CNT = 0;                                 //

    request_irq(IRQ_TIME0_IDX, 1, triac_turn_on_timer_isr, 0);
}





