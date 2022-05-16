#include<reg51.h>


sbit p10 = P1^0;
sbit p11 = P1^1;
sbit p12 = P1^2;


int time = 50000; // 定时时间, 50ms中断一次
int count = 0; // 用来记录, 第几次定时
int num = 1;
void init();


int main(){
	init();
	
	while(1){
		
		if (num % 2 == 0){
			p11 = ~p11;
			num = 3;
		}
		if (num % 4  == 0){
			p12 = ~p12;
			num = 1;
		}

		/*
				if (count % 20 == 0){
			p10 = ~p10;
		}if (count % 40 == 0){
			p11 = ~p11;
		}
		if (count % 80 == 0){
			p12 = ~p12;
		}
			count++; 
		*/
	}
	
	
	return 1;
	
}

void init(){
	P1 = 0x00;
	// 使用定时器0, 方式2, 定时模式
	TMOD = 0x01; // 0000 0001
	// 初值
	TH0 = (65535 - time)/256;
	TL0 = (65535- time) % 256;
	
	ET0 = 1; // 开启timer0中断
	EA = 1; // 开启总中断
	TR0 = 1;
	
	
	
}



void time0() interrupt 1{
	
	TH0 = (65535 - time) / 256;
	TL0 = (65535 - time) % 256;
	count++;
	if (count >= 20){ // 1s定时
		p10 = ~p10;
		count = 0;
		num++; // 定时
	}

	

	
}