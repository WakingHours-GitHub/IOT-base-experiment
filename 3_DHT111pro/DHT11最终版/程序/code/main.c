#include "reg52.H"
#include "stdio.h"
#include "intrins.h"
#include <math.h>

#define uchar unsigned char
#define uint unsigned int

#define DB P0//液晶并行数据接口
sbit RS = P2 ^ 0;//液晶指令数据信号
sbit RW = P2 ^ 1;//液晶读写信号
sbit E = P2 ^ 2;//液晶使能信号

sbit T_IO = P1 ^ 2;     //clk为DS1302的时钟信号线
sbit T_CLK = P1 ^ 7;    //DAT为DS1302的I/O数据线
sbit T_RST = P2 ^ 3;    //RST为DS1302的RST信号线  

uchar bdata
datbyte;
sbit datbyte0 = datbyte ^ 0;
sbit datbyte7 = datbyte ^ 7;

uchar time_1302[7] = {0x00, 0x00, 0x00, 0x03, 0x07, 0x03, 0x08};//设置时间初始值数组
//  秒   分   时   号  月份 星期  年
//DS1302函数声明
void get_1302(uchar time[]);

void write_ds1302(uchar dat);

uchar r_1302(uchar add);

void w_1302(uchar add, uchar dat);

uchar read_ds1302(void);

void init_1302(uchar *time);

void change();

uchar times[9];//保存时间
uchar date[11];//保存日期

unsigned char key_value;//获取到按键的值

sbit Data = P1 ^ 1;       //温湿度IO
signed char humi_value;//湿度
signed char temp_value;//温度

unsigned char now_window;//当前显示窗口
unsigned char curr_menu;
#define  normal_mode  0x10//显示模式
#define  set_mode  0x20//设置模式

signed char AlarmTL = 10;                // 温度下限报警值
signed char AlarmTH = 30;                // 温度上限报警值
signed char AlarmHL = 40;          // 湿度下限报警值
signed char AlarmHH = 80;                // 湿度上限报警值

unsigned char i;
unsigned char cnt_100ms, cnt_500ms = 0;
unsigned char time_100ms_flag;
//unsigned char open_del;//开锁成功延时计数
//unsigned char open_flag;//锁状态标志
sbit beep = P1 ^ 0;

sbit LedTL_P = P1 ^ 3;            // 温度过低报警指示灯
sbit LedTH_P = P1 ^ 4;            // 温度过高报警指示灯
sbit LedHL_P = P1 ^ 5;            // 湿度过低报警指示灯
sbit LedHH_P = P1 ^ 6;            // 湿度过高报警指示灯

/*******************************************************************************
* 函 数 名         : delay_ms
* 函数功能		   : 延时函数，延时1ms
* 输    入         : cnt
* 输    出         : 无
* 说    名         : 该函数是在12MHZ晶振下，12分频单片机的延时。
*******************************************************************************/
void delay_ms(unsigned int cnt)   //
{
    unsigned int x;
    for (; cnt > 0; cnt--) {
        for (x = 110; x > 0; x--);//软件延时为1MS
    }
}

#if 0
void delay_us(unsigned int cnt)   //
{
    while(cnt--);
}
#endif

/******************************************************
** 函数名：time_init
** 描述  ：定时器初始化
** 输入  ：无
** 输出  ：无
******************************************************/
void time_init(void) {
    TMOD |= 0x01;//time0 工作方式为1
    TH0 = 0xf8;//装载初值
    TL0 = 0x2f;//装载初值，为2ms(65535-63535)
    TR0 = 1;//开启定时器
    ET0 = 1;//打开中断
    EA = 1;
}

/******************************************************
** 函数名：key_scan
** 描述  ：按键扫描
** 输入  ：无
** 输出  ：无
******************************************************/
void key_scan(void) {
    static unsigned char key_in_flag = 0;//按键按下标志
    unsigned char key_l;//存储扫描到行列值。
    key_value = 20;//按键值清除
    if ((P3 & 0x0f) != 0x0f)//按键按下
    {
        delay_ms(1);//按键消抖动
        if (((P3 & 0x0f) != 0x0f) && (key_in_flag == 1)) {
            key_in_flag = 0;//松手检测防止一直触发
            P3 = 0x0f;
            //delay_ms(1);//按键消抖动
            key_l = P3;//扫描得到按键值
            switch (key_l) {
                //获取按键值
                case 0x0e: {
                    key_value = 1;
                }
                    break;
                case 0x0d: {
                    key_value = 2;
                }
                    break;
                case 0x0b: {
                    key_value = 3;
                }
                    break;
                    //case 0x70:
                    //break;
            }
        }
    } else {
        key_in_flag = 1;//(按键松开标志)
    }

}

/*******************************************************************************
* 函 数 名         : LcdWriteCom
* 函数功能		   : 向LCD写入一个字节的命令
* 输    入         : u8com
* 输    出         : 无
*******************************************************************************/
void lcd_wri_com(unsigned char com)      //写入命令
{
    E = 0;     //使能清零
    RS = 0;     //选择写入命令
    RW = 0;     //选择写入

    DB = com;
    delay_ms(1);

    E = 1;     //写入时序
    delay_ms(5);
    E = 0;
}

/*******************************************************************************
* 函 数 名         : LcdWriteData
* 函数功能		   : 向LCD写入一个字节的数据
* 输    入         : u8dat
* 输    出         : 无
*******************************************************************************/

void lcd_wri_data(unsigned char dat)//写入数据
{
    E = 0;      //使能清零
    RS = 1;      //选择写入数据
    RW = 0;      //选择写入
    DB = dat;
    delay_ms(1);
    E = 1;      //写入时序
    delay_ms(5);
    E = 0;
}

/*******************************************************************************
* 函 数 名         : WriString
* 函数功能		   : 刷新屏幕显示
* 输    入         : hang，add，*p
* 输    出         : 无
*******************************************************************************/
void wri_string(unsigned char y, unsigned char x, unsigned char *p) {
    if (y == 1)//如果选择第一行
        lcd_wri_com(0x80 + x);//选中地址
    else
        lcd_wri_com(0xc0 + x);//选中地址
    while (*p) {
        lcd_wri_data(*p);//写入数据
        p++;
    }
}

/*******************************************************************************
* 函 数 名         : lcd_write_char
* 函数功能		   :
* 输    入         :
* 输    出         : 无
*******************************************************************************/
void lcd_write_char(unsigned char y, unsigned char x, unsigned char dat) //列x=0~15,行y=0,1
{
    unsigned char temp_l, temp_h;
    if (y == 1)//如果选择第一行
        lcd_wri_com(0x80 + x);//选中地址
    else
        lcd_wri_com(0xc0 + x);//选中地址
    temp_l = dat % 10;
    temp_h = dat / 10;
    lcd_wri_data(temp_h + 0x30);          //convert to ascii
    lcd_wri_data(temp_l + 0x30);
}

/*********************光标控制***********************/
void lcd1602_guanbiao(unsigned char y, unsigned char x, unsigned char on_off) {
    if (on_off == 1)   //开光标
    {
        if (y == 1)//如果选择第一行
            lcd_wri_com(0x80 + x);
        else
            lcd_wri_com(0xc0 + x);//将光标移动到秒个位
        lcd_wri_com(0x0f);//显示光标并且闪烁
    } else {
        if (y == 1)//如果选择第一行
            lcd_wri_com(0x80 + x);
        else
            lcd_wri_com(0xc0 + x);//将光标移动到秒个位
        lcd_wri_com(0x0c);   //关光标
    }
}

/*******************************************************************************
* 函 数 名       : LcdInit()
* 函数功能		 : 初始化LCD屏
* 输    入       : 无
* 输    出       : 无
*******************************************************************************/
void lcd_init(void)                          //LCD初始化子程序
{
    lcd_wri_com(0x38);//置功能8位双行
    lcd_wri_com(0x0c);//显示开关光标
    lcd_wri_com(0x06);//字符进入模式屏幕不动字符后移
    delay_ms(5);//延时5ms
    lcd_wri_com(0x01);  //清屏
    wri_string(1, 0, times);//初始化显示
    wri_string(1, 6, date);
    wri_string(2, 0, "H:   %RH T:   C ");//初始化显示
}

void DHT11_delay_us(unsigned char n) {
    while (--n);
}

void DHT11_delay_ms(unsigned int z) {
    unsigned int i, j;
    for (i = z; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void DHT11_start() {
    Data = 1;
    DHT11_delay_us(2);
    Data = 0;
    DHT11_delay_ms(30);   //延时18ms以上
    Data = 1;
    DHT11_delay_us(30);
}

unsigned char DHT11_rec_byte(){      //接收一个字节
    unsigned char i, dat = 0;
    for (i = 0; i < 8; i++){    //从高到低依次接收8位数据
        while (!Data);   ////等待50us低电平过去
        DHT11_delay_us(8);     //延时60us，如果还为高则数据为1，否则为0
        dat <<= 1;           //移位使正确接收8位数据，数据为0时直接移位
        if (Data == 1)    //数据为1时，使dat加1来接收数据1
            dat += 1;
        while (Data);  //等待数据线拉低
    }
    return dat;
}

void DHT11_receive(){      //接收40位的数据
    unsigned char R_H, R_L, T_H, T_L, RH, RL, TH, TL, revise;
    DHT11_start();
    if (Data == 0) {
        while (Data == 0);   //等待拉高
        DHT11_delay_us(40);  //拉高后延时80us
        R_H = DHT11_rec_byte();    //接收湿度高八位
        R_L = DHT11_rec_byte();    //接收湿度低八位
        T_H = DHT11_rec_byte();    //接收温度高八位
        T_L = DHT11_rec_byte();    //接收温度低八位
        revise = DHT11_rec_byte(); //接收校正位

        DHT11_delay_us(25);    //结束

        if ((R_H + R_L + T_H + T_L) == revise){      //
            RH = R_H;
            RL = R_L;
            TH = T_H;
            TL = T_L;
        }
        humi_value = RH;
        temp_value = TH;
    }
}


void AlarmJudge(void) {
    if (temp_value > AlarmTH)// 温度是否过高
    {
        LedTH_P = 0;
        LedTL_P = 1;
    } else if (temp_value < AlarmTL)// 温度是否过低
    {
        LedTL_P = 0;
        LedTH_P = 1;
    } else// 温度正常
    {
        LedTH_P = 1;
        LedTL_P = 1;
    }

    if (humi_value > AlarmHH)// 湿度是否过高
    {
        LedHH_P = 0;
        LedHL_P = 1;
    } else if (humi_value < AlarmHL)    // 湿度是否过低
    {
        LedHL_P = 0;
        LedHH_P = 1;
    } else    // 湿度正常
    {
        LedHH_P = 1;
        LedHL_P = 1;
    }

    if ((LedHH_P == 0) || (LedHL_P == 0) || (LedTH_P == 0) || (LedTL_P == 0))    // 蜂鸣器判断，只要至少1个报警灯亮，蜂鸣器就报警
    {
        for (i = 0; i < 3; i++) {
            beep = 0;
            delay_ms(20);
            beep = 1;
            delay_ms(20);
        }
    }
}

/******************************************************
 ** 函数名：key_service
 ** 描述  ：按键服务函数
 ** 输入  ：无
 ** 输出  ：无
 ** 调用  ：主程序
******************************************************/
void key_service(void) {
    switch (now_window) {
        case normal_mode: {
            if (key_value == 1) {
                now_window = set_mode;
                curr_menu = 0;
                wri_string(1, 0, "T:  -           ");//初始化显示
                wri_string(2, 0, "H:  -           ");//初始化显示
                lcd_write_char(1, 2, AlarmTL);
                lcd_write_char(1, 6, AlarmTH);
                lcd_write_char(2, 2, AlarmHL);
                lcd_write_char(2, 6, AlarmHH);
                lcd1602_guanbiao(1, 3, 1);
            }
        }
            break;
        case set_mode: {
            if (key_value == 1) {
                ++curr_menu;
                if (curr_menu == 1) {
                    lcd1602_guanbiao(1, 7, 1);
                } else if (curr_menu == 2) {
                    lcd1602_guanbiao(2, 3, 1);
                } else if (curr_menu == 3) {
                    lcd1602_guanbiao(2, 7, 1);
                }
                if (curr_menu > 3) {
                    curr_menu = 0;
                    lcd1602_guanbiao(2, 7, 0);
                    now_window = normal_mode;
                    wri_string(1, 0, times);//初始化显示
                    wri_string(1, 6, date);
                    wri_string(2, 0, "H:   %RH T:   C ");//初始化显示
                    lcd_write_char(2, 2, humi_value);
                    lcd_write_char(2, 11, temp_value);
                    lcd_wri_com(0xcd);
                    lcd_wri_data(0xdf);
                }
            }
            if (key_value == 2) {
                if (curr_menu == 0) {
                    if (++AlarmTL > 99) {
                        AlarmTL = 0;
                    }
                    lcd_write_char(1, 2, AlarmTL);
                    lcd1602_guanbiao(1, 3, 1);
                } else if (curr_menu == 1) {
                    if (++AlarmTH > 99) {
                        AlarmTH = 0;
                    }
                    lcd_write_char(1, 6, AlarmTH);
                    lcd1602_guanbiao(1, 7, 1);
                } else if (curr_menu == 2) {
                    if (++AlarmHL > 99) {
                        AlarmHL = 0;
                    }
                    lcd_write_char(2, 2, AlarmHL);
                    lcd1602_guanbiao(2, 3, 1);
                } else if (curr_menu == 3) {
                    if (++AlarmHH > 99) {
                        AlarmHH = 0;
                    }
                    lcd_write_char(2, 6, AlarmHH);
                    lcd1602_guanbiao(2, 7, 1);
                }
            }
            if (key_value == 3) {
                if (curr_menu == 0) {
                    if (--AlarmTL < 0) {
                        AlarmTL = 99;
                    }
                    lcd_write_char(1, 2, AlarmTL);
                    lcd1602_guanbiao(1, 3, 1);
                } else if (curr_menu == 1) {
                    if (--AlarmTH < 0) {
                        AlarmTH = 99;
                    }
                    lcd_write_char(1, 6, AlarmTH);
                    lcd1602_guanbiao(1, 7, 1);
                } else if (curr_menu == 2) {
                    if (--AlarmHL < 0) {
                        AlarmHL = 99;
                    }
                    lcd_write_char(2, 2, AlarmHL);
                    lcd1602_guanbiao(2, 3, 1);
                } else if (curr_menu == 3) {
                    if (--AlarmHH < 0) {
                        AlarmHH = 99;

                    }
                    lcd_write_char(2, 6, AlarmHH);
                    lcd1602_guanbiao(2, 7, 1);
                }
            }
        }
            break;
    }
}

/******************************************************
 ** 函数名：alm
 ** 描述  ：定时闪烁函数
 ** 输入  : 无
 ** 输出  ：无
 ** 调用  ：中断调用
 ******************************************************/
void time_service(void) {
    if (time_100ms_flag) {
        time_100ms_flag = 0;
        if (++cnt_500ms > 5) {
            cnt_500ms = 0;
            if (now_window == normal_mode) {
                EA = 0;
                DHT11_receive();
                EA = 1;
                lcd_write_char(2, 2, humi_value);
                lcd_write_char(2, 11, temp_value);
                AlarmJudge();//报警函数
            }
        }

    }

}

/******************************************************
 ** 函数名：init_all_hardware
 ** 描述  ：初始化所有硬件，及其变量参数。
 ** 输入  ：无
 ** 输出  ：无
 ** 调用  ：主程序
 ******************************************************/
void init_all_hardware(void) {

    time_init();//定时器初始化
    lcd_init();
    lcd_write_char(2, 2, humi_value);
    lcd_write_char(2, 11, temp_value);
    lcd_wri_com(0xcd);
    lcd_wri_data(0xdf);
    key_value = 20;
    now_window = normal_mode;
    cnt_100ms = 0;
    time_100ms_flag = 0;
    curr_menu = 0;
}

void main(void) {
    get_1302(time_1302);//获取时间
    change(); //时间转换
    init_all_hardware();//初始化硬件，IO和定时器
    while (1) {
        get_1302(time_1302);//获取时间
        change(); //时间转换
        key_scan();//按键扫描
        key_service();//按键服务处理函数
        time_service();//时间处理函数
    }
}

/******************************************************
** 函数名：time0_interrupt
** 描述  ：按键扫描函数
** 输入  ：无
** 输出  ：无
******************************************************/
void time0_interrupt()

interrupt 1
{
TF0 = 0;//清除标志
TR0 = 0;
if (++cnt_100ms>50)
{
cnt_100ms = 0;
time_100ms_flag = 1;
}
TR0 = 1;
TH0 = 0xf8;
TL0 = 0x2f;//装载初值2ms(65535-63535)
}


//DS1302相关函数
/*==========================================
       D S 1 3 0 2  初 始 化
===========================================*/

void init_1302(uchar *time) {
    uchar i, add;
    //uchar time_bcd[7];
    add = 0x80;//0写入，1读出
    w_1302(0x8e, 0x00);
    for (i = 0; i < 7; i++) {
        w_1302(add, *time);
        add += 2;
        time++;
    }
    w_1302(0x8e, 0x80);
}

//===========================
//    读取当前时间
//===========================

void get_1302(uchar time[]) {
    uchar i;
    uchar add = 0x81;
    w_1302(0x8e, 0x00);
    for (i = 0; i < 7; i++) {
        time[i] = r_1302(add);
        add += 2;
    }
    w_1302(0x8e, 0x80);
}

/*=================================
   DS1302写入一个字节（上升沿有效）
=================================*/

void write_ds1302(uchar dat) {
    uchar i;
    datbyte = dat;
    for (i = 0; i < 8; i++) {
        T_IO = datbyte0;
        T_CLK = 1;
        T_CLK = 0;
        datbyte = datbyte >> 1;
    }
}

/*=======================================
   DS1302读取一个字节（下降沿有效）
=======================================*/

uchar read_ds1302(void) {
    uchar i;

    for (i = 0; i < 8; i++) {
        datbyte = datbyte >> 1;
        datbyte7 = T_IO;
        T_CLK = 1;
        T_CLK = 0;
    }
    return (datbyte);
}

/*=========================================
         指定位置读取数据
=========================================*/

uchar r_1302(uchar add) {
    uchar temp, dat1, dat2;
    T_RST = 0;
    T_CLK = 0;
    T_RST = 1;
    write_ds1302(add);
    temp = read_ds1302();
    T_CLK = 1;
    T_RST = 0;
    dat1 = temp / 16;
    dat2 = temp % 16;
    temp = dat1 * 10 + dat2;
    return (temp);
}

/*==========================================
        指定位置写入数据
==========================================*/

void w_1302(uchar add, uchar dat) {
    T_RST = 0;
    T_CLK = 0;
    T_RST = 1;
    write_ds1302(add);
    write_ds1302(dat / 10 << 4 | dat % 10);
    T_CLK = 1;
    T_RST = 0;
}

//DS1302数值转换
void change() {
    // 时 间 的 转 换
    times[0] = time_1302[2] / 10 + '0';
    times[1] = time_1302[2] % 10 + '0';
    times[2] = ':';
    times[3] = time_1302[1] / 10 + '0';
    times[4] = time_1302[1] % 10 + '0';
//  times[5]=':';
//  times[6]=time_1302[0]/10+'0';
//  times[7]=time_1302[0]%10+'0';
    times[5] = '\0';
    // 日 期 的 转 换
    date[0] = '2';
    date[1] = '0';
    date[2] = time_1302[6] / 10 + '0';
    date[3] = time_1302[6] % 10 + '0';
    date[4] = '/';
    date[5] = time_1302[4] / 10 + '0';
    date[6] = time_1302[4] % 10 + '0';
    date[7] = '/';
    date[8] = time_1302[3] / 10 + '0';
    date[9] = time_1302[3] % 10 + '0';
    date[10] = '\0';
}