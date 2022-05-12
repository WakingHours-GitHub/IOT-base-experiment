#include"reg51.h"
#include"intrins.h"

#define uc unsigned char

sbit SDA = P1 ^ 0;
sbit SCL = P1 ^ 1;

bit NackFlag;      //�����Ӧ���־

void delay()       //΢����ʱ�����5us
{
	int i = 1000;
	for(; i > 0; i--)
		;
}

void delayxms(uc count)    //������ʱ����
{
    uc i, j;
    for (i = 0; i < count; i++) {
        for (j = 0; j < 120; j++);
    }
}


void i2_start(void)      //����I2Cͨ��
{
    SDA = 1;
    SCL = 1;
    delay();
    SDA = 0;
    delay();
    SCL = 0;
}

void i2_stop(void)          //ֹͣI2Cͨ��
{
    SDA = 0;
    SCL = 1;
    delay();
    SDA = 1;
    delay();
    SCL = 0;
}

void checkACK(void)       //Ӧ����
{
    SDA = 1;
    SCL = 1;
    NackFlag = 0;
    if (SDA == 1) {
        NackFlag = 1;
    }
    SCL = 0;
}

void send8Bit(uc num)         //����һ��8λ�����Ӻ���
{
    uc i = 8;
    uc temp;
    temp = num;
    for (i = 8; i > 0; i--) {
        SDA = temp & 0x80;      //����IICЭ�飬Ӧ�ô����λ��ʼ��
        SCL = 1;                //&�����������temp&0x80����ʾȡtemp�����λ��0x80 = 1000 0000
        delay();
        SCL = 0;
        temp = temp << 1;        //temp������λ��0����λ����temp=1010 0010 ���ƺ�temp=0100 0100
    }
}

void sendByte(uc addr, uc num)     //д���ݺ���
{
    i2_start();
    send8Bit(0xa0);        //��Ƭ��Ѱ��оƬ
    checkACK();            //���Ӧ���ź�
    send8Bit(addr);        //ȷ�������洢λ��ַ
    checkACK();
    send8Bit(num);        //д����
    checkACK();
    i2_stop();
    delayxms(10);
}

uc read8Bit()             //��8λ�����Ӻ���
{
    uc i, k;
    SCL = 0;
    delay();
    SDA = 1;
    for (i = 0; i < 8; i++) {
        SCL = 1;
        delay();
        k = (k << 1) | SDA;      //�����λ��ȡ��(k<<1)|SDA��k������һλ��Ȼ����SDA��λ��
        SCL = 0;
        delay();
    }
    delay();
    return k;
}

uc readByte(uc addr)     //��ȡ���ݺ���
{
    uc data_;
    i2_start();
    send8Bit(0xa0);        //��Ƭ��Ѱ��оƬ
    checkACK();

    send8Bit(addr);
    checkACK();

    i2_start();

    send8Bit(0xa1);        //������ַ+1 ������һ����ȡ���ݵĿ���������
    checkACK();

    data_ = read8Bit();
    checkACK();

    i2_stop();
    delayxms(10);
    return data_;
}

void main() {
while (1){
    uc read_data, i; 
		
		// ��0
    for (i = 0; i < 0xff; i++) {
        sendByte(i, 0);     
        delayxms(5);
    }
	
		// �������ݵ��洢����0��ַ��
    sendByte(0, 0x0b);     //�ڵ�0��λ��д��0x0B
    delayxms(5);

		// ���ֽ�
    read_data = readByte(0);     //��ȡ��0��λ�õ�����
    delayxms(5);

    sendByte(3, read_data);     //����ȡ������д�뵽��3��λ��
    delayxms(5);

    sendByte(5, read_data);     //����ȡ������д�뵽��5��λ��
    delayxms(5);
		
	}

    
}