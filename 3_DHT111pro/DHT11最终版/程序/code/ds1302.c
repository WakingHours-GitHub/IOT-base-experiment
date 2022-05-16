#include "ds1302.h"

uchar bdata datbyte;
sbit datbyte0=datbyte^0;
sbit datbyte7=datbyte^7;


uchar time_1302[7]={0x00,0x00,0x00,0x03,0x07,0x03,0x08};//设置时间初始值数组
                 //  秒   分   时   号  月份 星期  年

/*==========================================
       D S 1 3 0 2  初 始 化
===========================================*/

void init_1302(uchar *time)
{uchar i, add;
 //uchar time_bcd[7];
 add=0x80;//0写入，1读出
 w_1302(0x8e,0x00);
 for(i=0;i<7;i++)
  {w_1302(add,*time);
   add+=2;
   time++;
   }
 w_1302(0x8e,0x80);
}

//===========================
//    读取当前时间
//===========================

void get_1302(uchar time[])
{uchar i;
 uchar add=0x81;
 w_1302(0x8e,0x00);
 for(i=0;i<7;i++)
 { 
   time[i]=r_1302(add); 
   add+=2;
   }
 w_1302(0x8e,0x80);
}    
/*=================================
   DS1302写入一个字节（上升沿有效）
=================================*/

void write_ds1302(uchar dat)
{uchar i;
 datbyte=dat;
 for(i=0;i<8;i++)
 {T_IO=datbyte0;
  T_CLK=1;
  T_CLK=0;
  datbyte=datbyte>>1;
  }
}

/*=======================================
   DS1302读取一个字节（下降沿有效）
=======================================*/

uchar read_ds1302(void)
{uchar i;
 
 for(i=0;i<8;i++)
  {datbyte=datbyte>>1;
   datbyte7=T_IO;
   T_CLK=1;
   T_CLK=0;
   }
 return(datbyte);
}

/*=========================================
         指定位置读取数据
=========================================*/

uchar r_1302(uchar add)
{uchar temp,dat1,dat2;
 T_RST=0;
 T_CLK=0;
 T_RST=1;
 write_ds1302(add);
 temp=read_ds1302();
 T_CLK=1;
 T_RST=0;
 dat1=temp/16;
 dat2=temp%16;
 temp=dat1*10+dat2;
 return(temp);
}

/*==========================================
        指定位置写入数据
==========================================*/

void w_1302(uchar add,uchar dat)
{
 T_RST=0;
 T_CLK=0;
 T_RST=1;
 write_ds1302(add);
 write_ds1302(dat/10<<4|dat%10);
 T_CLK=1;
 T_RST=0;
}