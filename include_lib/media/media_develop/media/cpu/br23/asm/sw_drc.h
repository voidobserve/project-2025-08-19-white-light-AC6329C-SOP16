#ifndef __SW_DRC_H
#define __SW_DRC_H

#include "generic/typedef.h"
#include "generic/list.h"
#include "os/os_api.h"

#define DRC_TYPE_LIMITER		1
#define DRC_TYPE_COMPRESSOR		2


struct drc_limiter {
    int attacktime;
    int releasetime;
    int threshold[2];//threshold[1]是固定值32768, threshold[0]为界面参数
};

struct drc_compressor {
    int attacktime;
    int releasetime;
    int threshold[3];//threshold[2]是固定值32768
    int ratio[3];// ratio[0]为固定值100， ratio[1] ratio[2]为界面参数
};

struct drc_ch {
    u8 nband;//max<=3，1：全带 2：两段 3：三段
    u8 type;//0:没有使能限幅和压缩器，1:限幅器   2:压缩器
    u8 reserved[2];//保留,未用
    int low_freq;//中低频分频点
    int high_freq;//中高频分频点
    int order;//分频器阶数, 2或者4
    union {
        struct drc_limiter		limiter[3];
        struct drc_compressor	compressor[3];
    } _p;
};



struct sw_drc {
    void *work_buf[3];
    void *crossoverBuf;
    int *run_tmp[3];
    int run_tmp_len;
    u8 nband;
    u8 type;
    u8 channel;
	u8 run32bit;
    struct audio_eq *crossover[3];
    u16 sample_rate;
    u8 nsection;

};
extern void *get_low_sosmatrix();
extern void *get_high_sosmatrix();
extern void *get_band_sosmatrix();
extern int get_crossover_nsection();


extern void *audio_sw_drc_open(struct drc_ch *ch_tmp, u16 sample_rate, u8 channel, u8 drc_name, u8 run32bit);
extern void audio_sw_drc_close(void *hdl);

extern int audio_sw_drc_update(void *hdl, struct drc_ch *ch_tmp, u16 sample_rate, u8 channel);

extern int audio_sw_drc_run(void *hdl, s16 *in_buf, s16 *out_buf, int npoint_per_channel);

#endif
