#include "reg51.h"
#include "uart.h"

#define uchar unsigned char
uchar code table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d, 0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
	
//串口初始化，设置波特率为9600，使用定时器1,晶振频率11.0592Mhz
void UART_Init(){
	//设置波特率9600
	TMOD|=0x20;  //定时器1工作于自动重装载模式
	TH1=0xfd;
	TH0=0xfd;
	TR1=1; //启动定时器1
	ET1=0; //禁止定时器1中断
	PCON&=0x7f; //设置SMOD=0，波特率不加倍
	SCON=0x50; //工作于方式1  8位无校验异步通信的收发模式，并清空收发中断标志位
	EA=1;  //总中断允许
	ES=0;//禁止串口收发中断（本程序中不使用串口中断）
}

void UART_Send_Data(uchar byte){ // 发送数据
	SBUF=byte; // 将SUBF缓冲区设置为byte
	while(!TI);//等待串口数据发送完毕
	TI=0;//将发送中断标志位清0，为下次发送数据做准备
}


uchar UART_Receive_Data(){
	uchar temp;
	while(!RI);//等待串口接收到数据（RI被置1）
	temp=SBUF; // 从SBUF中取出
	RI=0;//清空接收中断标志位，为下次接收做准备
	return temp;
}




int main(){
	uchar temp = 0;
	// 初始化。
	UART_Init();
	while(1){
		// 测试1
		temp = UART_Receive_Data();
// 		
		// 
		if (temp >= '0' && temp <= '9'){ 
			// P1 = 1 << temp - '0';
			P1 = table[temp - '0'];
		}
		
		// 测试2
		UART_Send_Data(UART_Receive_Data());
			
	}
	
	return 0;
	
}