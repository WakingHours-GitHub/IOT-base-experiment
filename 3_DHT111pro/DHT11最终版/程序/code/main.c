#include "reg52.H"
#include "stdio.h"
#include "intrins.h"
#include <math.h>

#define uchar unsigned char
#define uint unsigned int

#define DB P0//Һ���������ݽӿ�
sbit RS = P2 ^ 0;//Һ��ָ�������ź�
sbit RW = P2 ^ 1;//Һ����д�ź�
sbit E = P2 ^ 2;//Һ��ʹ���ź�

sbit T_IO = P1 ^ 2;     //clkΪDS1302��ʱ���ź���
sbit T_CLK = P1 ^ 7;    //DATΪDS1302��I/O������
sbit T_RST = P2 ^ 3;    //RSTΪDS1302��RST�ź���  

uchar bdata
datbyte;
sbit datbyte0 = datbyte ^ 0;
sbit datbyte7 = datbyte ^ 7;

uchar time_1302[7] = {0x00, 0x00, 0x00, 0x03, 0x07, 0x03, 0x08};//����ʱ���ʼֵ����
//  ��   ��   ʱ   ��  �·� ����  ��
//DS1302��������
void get_1302(uchar time[]);

void write_ds1302(uchar dat);

uchar r_1302(uchar add);

void w_1302(uchar add, uchar dat);

uchar read_ds1302(void);

void init_1302(uchar *time);

void change();

uchar times[9];//����ʱ��
uchar date[11];//��������

unsigned char key_value;//��ȡ��������ֵ

sbit Data = P1 ^ 1;       //��ʪ��IO
signed char humi_value;//ʪ��
signed char temp_value;//�¶�

unsigned char now_window;//��ǰ��ʾ����
unsigned char curr_menu;
#define  normal_mode  0x10//��ʾģʽ
#define  set_mode  0x20//����ģʽ

signed char AlarmTL = 10;                // �¶����ޱ���ֵ
signed char AlarmTH = 30;                // �¶����ޱ���ֵ
signed char AlarmHL = 40;          // ʪ�����ޱ���ֵ
signed char AlarmHH = 80;                // ʪ�����ޱ���ֵ

unsigned char i;
unsigned char cnt_100ms, cnt_500ms = 0;
unsigned char time_100ms_flag;
//unsigned char open_del;//�����ɹ���ʱ����
//unsigned char open_flag;//��״̬��־
sbit beep = P1 ^ 0;

sbit LedTL_P = P1 ^ 3;            // �¶ȹ��ͱ���ָʾ��
sbit LedTH_P = P1 ^ 4;            // �¶ȹ��߱���ָʾ��
sbit LedHL_P = P1 ^ 5;            // ʪ�ȹ��ͱ���ָʾ��
sbit LedHH_P = P1 ^ 6;            // ʪ�ȹ��߱���ָʾ��

/*******************************************************************************
* �� �� ��         : delay_ms
* ��������		   : ��ʱ��������ʱ1ms
* ��    ��         : cnt
* ��    ��         : ��
* ˵    ��         : �ú�������12MHZ�����£�12��Ƶ��Ƭ������ʱ��
*******************************************************************************/
void delay_ms(unsigned int cnt)   //
{
    unsigned int x;
    for (; cnt > 0; cnt--) {
        for (x = 110; x > 0; x--);//�����ʱΪ1MS
    }
}

#if 0
void delay_us(unsigned int cnt)   //
{
    while(cnt--);
}
#endif

/******************************************************
** ��������time_init
** ����  ����ʱ����ʼ��
** ����  ����
** ���  ����
******************************************************/
void time_init(void) {
    TMOD |= 0x01;//time0 ������ʽΪ1
    TH0 = 0xf8;//װ�س�ֵ
    TL0 = 0x2f;//װ�س�ֵ��Ϊ2ms(65535-63535)
    TR0 = 1;//������ʱ��
    ET0 = 1;//���ж�
    EA = 1;
}

/******************************************************
** ��������key_scan
** ����  ������ɨ��
** ����  ����
** ���  ����
******************************************************/
void key_scan(void) {
    static unsigned char key_in_flag = 0;//�������±�־
    unsigned char key_l;//�洢ɨ�赽����ֵ��
    key_value = 20;//����ֵ���
    if ((P3 & 0x0f) != 0x0f)//��������
    {
        delay_ms(1);//����������
        if (((P3 & 0x0f) != 0x0f) && (key_in_flag == 1)) {
            key_in_flag = 0;//���ּ���ֹһֱ����
            P3 = 0x0f;
            //delay_ms(1);//����������
            key_l = P3;//ɨ��õ�����ֵ
            switch (key_l) {
                //��ȡ����ֵ
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
        key_in_flag = 1;//(�����ɿ���־)
    }

}

/*******************************************************************************
* �� �� ��         : LcdWriteCom
* ��������		   : ��LCDд��һ���ֽڵ�����
* ��    ��         : u8com
* ��    ��         : ��
*******************************************************************************/
void lcd_wri_com(unsigned char com)      //д������
{
    E = 0;     //ʹ������
    RS = 0;     //ѡ��д������
    RW = 0;     //ѡ��д��

    DB = com;
    delay_ms(1);

    E = 1;     //д��ʱ��
    delay_ms(5);
    E = 0;
}

/*******************************************************************************
* �� �� ��         : LcdWriteData
* ��������		   : ��LCDд��һ���ֽڵ�����
* ��    ��         : u8dat
* ��    ��         : ��
*******************************************************************************/

void lcd_wri_data(unsigned char dat)//д������
{
    E = 0;      //ʹ������
    RS = 1;      //ѡ��д������
    RW = 0;      //ѡ��д��
    DB = dat;
    delay_ms(1);
    E = 1;      //д��ʱ��
    delay_ms(5);
    E = 0;
}

/*******************************************************************************
* �� �� ��         : WriString
* ��������		   : ˢ����Ļ��ʾ
* ��    ��         : hang��add��*p
* ��    ��         : ��
*******************************************************************************/
void wri_string(unsigned char y, unsigned char x, unsigned char *p) {
    if (y == 1)//���ѡ���һ��
        lcd_wri_com(0x80 + x);//ѡ�е�ַ
    else
        lcd_wri_com(0xc0 + x);//ѡ�е�ַ
    while (*p) {
        lcd_wri_data(*p);//д������
        p++;
    }
}

/*******************************************************************************
* �� �� ��         : lcd_write_char
* ��������		   :
* ��    ��         :
* ��    ��         : ��
*******************************************************************************/
void lcd_write_char(unsigned char y, unsigned char x, unsigned char dat) //��x=0~15,��y=0,1
{
    unsigned char temp_l, temp_h;
    if (y == 1)//���ѡ���һ��
        lcd_wri_com(0x80 + x);//ѡ�е�ַ
    else
        lcd_wri_com(0xc0 + x);//ѡ�е�ַ
    temp_l = dat % 10;
    temp_h = dat / 10;
    lcd_wri_data(temp_h + 0x30);          //convert to ascii
    lcd_wri_data(temp_l + 0x30);
}

/*********************������***********************/
void lcd1602_guanbiao(unsigned char y, unsigned char x, unsigned char on_off) {
    if (on_off == 1)   //�����
    {
        if (y == 1)//���ѡ���һ��
            lcd_wri_com(0x80 + x);
        else
            lcd_wri_com(0xc0 + x);//������ƶ������λ
        lcd_wri_com(0x0f);//��ʾ��겢����˸
    } else {
        if (y == 1)//���ѡ���һ��
            lcd_wri_com(0x80 + x);
        else
            lcd_wri_com(0xc0 + x);//������ƶ������λ
        lcd_wri_com(0x0c);   //�ع��
    }
}

/*******************************************************************************
* �� �� ��       : LcdInit()
* ��������		 : ��ʼ��LCD��
* ��    ��       : ��
* ��    ��       : ��
*******************************************************************************/
void lcd_init(void)                          //LCD��ʼ���ӳ���
{
    lcd_wri_com(0x38);//�ù���8λ˫��
    lcd_wri_com(0x0c);//��ʾ���ع��
    lcd_wri_com(0x06);//�ַ�����ģʽ��Ļ�����ַ�����
    delay_ms(5);//��ʱ5ms
    lcd_wri_com(0x01);  //����
    wri_string(1, 0, times);//��ʼ����ʾ
    wri_string(1, 6, date);
    wri_string(2, 0, "H:   %RH T:   C ");//��ʼ����ʾ
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
    DHT11_delay_ms(30);   //��ʱ18ms����
    Data = 1;
    DHT11_delay_us(30);
}

unsigned char DHT11_rec_byte(){      //����һ���ֽ�
    unsigned char i, dat = 0;
    for (i = 0; i < 8; i++){    //�Ӹߵ������ν���8λ����
        while (!Data);   ////�ȴ�50us�͵�ƽ��ȥ
        DHT11_delay_us(8);     //��ʱ60us�������Ϊ��������Ϊ1������Ϊ0
        dat <<= 1;           //��λʹ��ȷ����8λ���ݣ�����Ϊ0ʱֱ����λ
        if (Data == 1)    //����Ϊ1ʱ��ʹdat��1����������1
            dat += 1;
        while (Data);  //�ȴ�����������
    }
    return dat;
}

void DHT11_receive(){      //����40λ������
    unsigned char R_H, R_L, T_H, T_L, RH, RL, TH, TL, revise;
    DHT11_start();
    if (Data == 0) {
        while (Data == 0);   //�ȴ�����
        DHT11_delay_us(40);  //���ߺ���ʱ80us
        R_H = DHT11_rec_byte();    //����ʪ�ȸ߰�λ
        R_L = DHT11_rec_byte();    //����ʪ�ȵͰ�λ
        T_H = DHT11_rec_byte();    //�����¶ȸ߰�λ
        T_L = DHT11_rec_byte();    //�����¶ȵͰ�λ
        revise = DHT11_rec_byte(); //����У��λ

        DHT11_delay_us(25);    //����

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
    if (temp_value > AlarmTH)// �¶��Ƿ����
    {
        LedTH_P = 0;
        LedTL_P = 1;
    } else if (temp_value < AlarmTL)// �¶��Ƿ����
    {
        LedTL_P = 0;
        LedTH_P = 1;
    } else// �¶�����
    {
        LedTH_P = 1;
        LedTL_P = 1;
    }

    if (humi_value > AlarmHH)// ʪ���Ƿ����
    {
        LedHH_P = 0;
        LedHL_P = 1;
    } else if (humi_value < AlarmHL)    // ʪ���Ƿ����
    {
        LedHL_P = 0;
        LedHH_P = 1;
    } else    // ʪ������
    {
        LedHH_P = 1;
        LedHL_P = 1;
    }

    if ((LedHH_P == 0) || (LedHL_P == 0) || (LedTH_P == 0) || (LedTL_P == 0))    // �������жϣ�ֻҪ����1�������������������ͱ���
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
 ** ��������key_service
 ** ����  ������������
 ** ����  ����
 ** ���  ����
 ** ����  ��������
******************************************************/
void key_service(void) {
    switch (now_window) {
        case normal_mode: {
            if (key_value == 1) {
                now_window = set_mode;
                curr_menu = 0;
                wri_string(1, 0, "T:  -           ");//��ʼ����ʾ
                wri_string(2, 0, "H:  -           ");//��ʼ����ʾ
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
                    wri_string(1, 0, times);//��ʼ����ʾ
                    wri_string(1, 6, date);
                    wri_string(2, 0, "H:   %RH T:   C ");//��ʼ����ʾ
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
 ** ��������alm
 ** ����  ����ʱ��˸����
 ** ����  : ��
 ** ���  ����
 ** ����  ���жϵ���
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
                AlarmJudge();//��������
            }
        }

    }

}

/******************************************************
 ** ��������init_all_hardware
 ** ����  ����ʼ������Ӳ�����������������
 ** ����  ����
 ** ���  ����
 ** ����  ��������
 ******************************************************/
void init_all_hardware(void) {

    time_init();//��ʱ����ʼ��
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
    get_1302(time_1302);//��ȡʱ��
    change(); //ʱ��ת��
    init_all_hardware();//��ʼ��Ӳ����IO�Ͷ�ʱ��
    while (1) {
        get_1302(time_1302);//��ȡʱ��
        change(); //ʱ��ת��
        key_scan();//����ɨ��
        key_service();//������������
        time_service();//ʱ�䴦����
    }
}

/******************************************************
** ��������time0_interrupt
** ����  ������ɨ�躯��
** ����  ����
** ���  ����
******************************************************/
void time0_interrupt()

interrupt 1
{
TF0 = 0;//�����־
TR0 = 0;
if (++cnt_100ms>50)
{
cnt_100ms = 0;
time_100ms_flag = 1;
}
TR0 = 1;
TH0 = 0xf8;
TL0 = 0x2f;//װ�س�ֵ2ms(65535-63535)
}


//DS1302��غ���
/*==========================================
       D S 1 3 0 2  �� ʼ ��
===========================================*/

void init_1302(uchar *time) {
    uchar i, add;
    //uchar time_bcd[7];
    add = 0x80;//0д�룬1����
    w_1302(0x8e, 0x00);
    for (i = 0; i < 7; i++) {
        w_1302(add, *time);
        add += 2;
        time++;
    }
    w_1302(0x8e, 0x80);
}

//===========================
//    ��ȡ��ǰʱ��
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
   DS1302д��һ���ֽڣ���������Ч��
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
   DS1302��ȡһ���ֽڣ��½�����Ч��
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
         ָ��λ�ö�ȡ����
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
        ָ��λ��д������
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

//DS1302��ֵת��
void change() {
    // ʱ �� �� ת ��
    times[0] = time_1302[2] / 10 + '0';
    times[1] = time_1302[2] % 10 + '0';
    times[2] = ':';
    times[3] = time_1302[1] / 10 + '0';
    times[4] = time_1302[1] % 10 + '0';
//  times[5]=':';
//  times[6]=time_1302[0]/10+'0';
//  times[7]=time_1302[0]%10+'0';
    times[5] = '\0';
    // �� �� �� ת ��
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