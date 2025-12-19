#ifndef __RF24G_H__
#define __RF24G_H__

// 硬件按键值定义
// 3V 2.4G遥控
#define RF24_K01 0x11
#define RF24_K02 0x12
#define RF24_K03 0x13
#define RF24_K04 0x14

#define RF24_K05 0x21 // 声控按键 没有单击
#define RF24_K06 0x22
#define RF24_K07 0x23
#define RF24_K08 0x24

#define RF24_K09 0x31
#define RF24_K10 0x32
#define RF24_K11 0x33
#define RF24_K12 0x34

#define RF24_K13 0x41
#define RF24_K14 0x42
#define RF24_K15 0x43
#define RF24_K16 0x44

#define RF24_K17 0x51
#define RF24_K18 0x52
#define RF24_K19 0x53
#define RF24_K20 0x54

#define RF24_K21 0x61
#define RF24_K22 0x62
#define RF24_K23 0x63
#define RF24_K24 0x64

#define RF24_K25 0x01
#define RF24_K26 0x02
#define RF24_K27 0x03
#define RF24_K28 0x04

enum
{
    RF24G_KEY_VAL_R1C1 = RF24_K01, // 第一行第一列
    RF24G_KEY_VAL_R1C2 = RF24_K02, // 第一行第二列
    RF24G_KEY_VAL_R1C3 = RF24_K03,
    RF24G_KEY_VAL_R1C4 = RF24_K04,

    RF24G_KEY_VAL_R2C1 = RF24_K05,
    RF24G_KEY_VAL_R2C2 = RF24_K06,
    RF24G_KEY_VAL_R2C3 = RF24_K07,
    RF24G_KEY_VAL_R2C4 = RF24_K08,

    RF24G_KEY_VAL_R3C1 = RF24_K09,
    RF24G_KEY_VAL_R3C2 = RF24_K10,
    RF24G_KEY_VAL_R3C3 = RF24_K11,
    RF24G_KEY_VAL_R3C4 = RF24_K12,

    RF24G_KEY_VAL_R4C1 = RF24_K13,
    RF24G_KEY_VAL_R4C2 = RF24_K14,
    RF24G_KEY_VAL_R4C3 = RF24_K15,
    RF24G_KEY_VAL_R4C4 = RF24_K16,

    RF24G_KEY_VAL_R5C1 = RF24_K17,
    RF24G_KEY_VAL_R5C2 = RF24_K18,
    RF24G_KEY_VAL_R5C3 = RF24_K19,
    RF24G_KEY_VAL_R5C4 = RF24_K20,

    RF24G_KEY_VAL_R6C1 = RF24_K21,
    RF24G_KEY_VAL_R6C2 = RF24_K22,
    RF24G_KEY_VAL_R6C3 = RF24_K23,
    RF24G_KEY_VAL_R6C4 = RF24_K24,

    RF24G_KEY_VAL_R7C1 = RF24_K25,
    RF24G_KEY_VAL_R7C2 = RF24_K26,
    RF24G_KEY_VAL_R7C3 = RF24_K27,
    RF24G_KEY_VAL_R7C4 = RF24_K28,
};

// 17键按键遥控器对应的键值
#define _17_key1 0x01
#define _17_key2 0x05
#define _17_key3 0x07
#define _17_key4 0x08
#define _17_key5 0x09
#define _17_key6 0x0a
#define _17_key7 0x0b
#define _17_key8 0x0c
#define _17_key9 0x0d
#define _17_key10 0x0e
#define _17_key11 0x0f
#define _17_key12 0x10
#define _17_key13 0x11
#define _17_key14 0x12
#define _17_key15 0x13
#define _17_key16 0x14
#define _17_key17 0x15

#pragma pack(1) // 控制结构体成员的内存对齐方式
typedef struct
{
#if 0
    u8 header1;
    u8 header2;
   
    u8 pair[3];         //客户码
    u8 key_v;
    u8 dynamic_code;    //  动态码
#endif

#if 1 // 803 2.4G遥控器 数据包：

    u8 header1;
    u8 header2;

    u8 key_v;        // 键值
    u8 fix_val_1;    // 固定值
    u8 fix_val_2;    // 固定值
    u8 fix_val_3;    // 固定值
    u8 dynamic_code; // 滚动码

    // u8 fix_val_4; // 固定值
    //

#endif

} rf24g_ins_t; // 指令数据
#pragma pack()

// void rf24g_scan(unsigned char *pBuf);
void rf24g_scan(u8 *pBuf, u8 len);
void rf24g_long_timer(void);
u8 get_rf24g_long_state(void);

#endif
