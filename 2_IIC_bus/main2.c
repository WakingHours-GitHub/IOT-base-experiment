#include"reg51.h"
#include"intrins.h"

#define uc unsigned char

sbit SDA = P1 ^ 0;
sbit SCL = P1 ^ 1;

bit NackFlag;      //定义非应答标志

void delay()       //微秒延时，大概5us
{
	int i = 1000;
	for(; i > 0; i--)
		;
}

void delayxms(uc count)    //毫秒延时函数
{
    uc i, j;
    for (i = 0; i < count; i++) {
        for (j = 0; j < 120; j++);
    }
}


void i2_start(void)      //启动I2C通信
{
    SDA = 1;
    SCL = 1;
    delay();
    SDA = 0;
    delay();
    SCL = 0;
}

void i2_stop(void)          //停止I2C通信
{
    SDA = 0;
    SCL = 1;
    delay();
    SDA = 1;
    delay();
    SCL = 0;
}

void checkACK(void)       //应答检查
{
    SDA = 1;
    SCL = 1;
    NackFlag = 0;
    if (SDA == 1) {
        NackFlag = 1;
    }
    SCL = 0;
}

void send8Bit(uc num)         //发送一个8位数据子函数
{
    uc i = 8;
    uc temp;
    temp = num;
    for (i = 8; i > 0; i--) {
        SDA = temp & 0x80;      //根据IIC协议，应该从最高位开始发
        SCL = 1;                //&是与运算符。temp&0x80：表示取temp的最高位。0x80 = 1000 0000
        delay();
        SCL = 0;
        temp = temp << 1;        //temp左移移位，0补空位。如temp=1010 0010 左移后temp=0100 0100
    }
}

void sendByte(uc addr, uc num)     //写数据函数
{
    i2_start();
    send8Bit(0xa0);        //单片机寻找芯片
    checkACK();            //检查应答信号
    send8Bit(addr);        //确定器件存储位地址
    checkACK();
    send8Bit(num);        //写数据
    checkACK();
    i2_stop();
    delayxms(10);
}

uc read8Bit()             //读8位数据子函数
{
    uc i, k;
    SCL = 0;
    delay();
    SDA = 1;
    for (i = 0; i < 8; i++) {
        SCL = 1;
        delay();
        k = (k << 1) | SDA;      //从最高位读取。(k<<1)|SDA：k先左移一位，然后与SDA按位或
        SCL = 0;
        delay();
    }
    delay();
    return k;
}

uc readByte(uc addr)     //读取数据函数
{
    uc data_;
    i2_start();
    send8Bit(0xa0);        //单片机寻找芯片
    checkACK();

    send8Bit(addr);
    checkACK();

    i2_start();

    send8Bit(0xa1);        //器件地址+1 ，这是一个读取数据的控制命令字
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
		
		// 置0
    for (i = 0; i < 0xff; i++) {
        sendByte(i, 0);     
        delayxms(5);
    }
	
		// 发送数据到存储器的0地址上
    sendByte(0, 0x0b);     //在第0个位置写入0x0B
    delayxms(5);

		// 将字节
    read_data = readByte(0);     //读取第0个位置的数据
    delayxms(5);

    sendByte(3, read_data);     //将读取的数据写入到第3个位置
    delayxms(5);

    sendByte(5, read_data);     //将读取的数据写入到第5个位置
    delayxms(5);
		
	}

    
}