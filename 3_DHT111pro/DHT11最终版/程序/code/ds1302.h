#ifndef _DS1302_H_
#define _DS1302_H_

#include <reg52.h> 
#include "main.h"


sbit T_IO=P1^1;           //与硬件相关的连线  clk为DS1302的时钟信号线
sbit T_CLK=P1^2;           //DAT为DS1302的I/O数据线
sbit T_RST=P1^3;           //RST为DS1302的RST信号线  

extern uchar bdata datbyte;
extern uchar  time_1302[7];

void get_1302(uchar time[]);
void write_ds1302(uchar dat);
uchar r_1302(uchar add);
void w_1302(uchar add,uchar dat);
uchar read_ds1302(void);
void init_1302(uchar *time);


#endif