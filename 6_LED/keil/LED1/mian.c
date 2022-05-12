#include<reg51.h>

sbit p10 = P1^0;
sbit p11 = P1^1;
sbit p12 = P1^2;

void init();
void delay_1s();

int main(){
	int i = 0;
	init();
	while(1){
		
		delay_1s();
		i++;
		p10 = ~p10; // 取反
		
		if(i % 2 == 0){
			p11 = ~p11;
		}
		
		if(i % 4 == 0){
			p12 = ~p12;
		}
		
	}
	
	return 1;
	
}

void init(){
	P1 = 0x00; // 初始化
}

void delay_1s(){
	int i = 100, j = 1000;
	for(; i > 0; i--){
		for (; j > 0; j--){
				;
		}
	}
}