#include<reg51.h>


sbit p10 = P1^0;
sbit p11 = P1^1;
sbit p12 = P1^2;


int time = 50000; // ��ʱʱ��, 50ms�ж�һ��
int count = 0; // ������¼, �ڼ��ζ�ʱ
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
	// ʹ�ö�ʱ��0, ��ʽ2, ��ʱģʽ
	TMOD = 0x01; // 0000 0001
	// ��ֵ
	TH0 = (65535 - time)/256;
	TL0 = (65535- time) % 256;
	
	ET0 = 1; // ����timer0�ж�
	EA = 1; // �������ж�
	TR0 = 1;
	
	
	
}



void time0() interrupt 1{
	
	TH0 = (65535 - time) / 256;
	TL0 = (65535 - time) % 256;
	count++;
	if (count >= 20){ // 1s��ʱ
		p10 = ~p10;
		count = 0;
		num++; // ��ʱ
	}

	

	
}