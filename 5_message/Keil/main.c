#include "reg51.h"
#include "uart.h"

#define uchar unsigned char
uchar code table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d, 0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
	
//���ڳ�ʼ�������ò�����Ϊ9600��ʹ�ö�ʱ��1,����Ƶ��11.0592Mhz
void UART_Init(){
	//���ò�����9600
	TMOD|=0x20;  //��ʱ��1�������Զ���װ��ģʽ
	TH1=0xfd;
	TH0=0xfd;
	TR1=1; //������ʱ��1
	ET1=0; //��ֹ��ʱ��1�ж�
	PCON&=0x7f; //����SMOD=0�������ʲ��ӱ�
	SCON=0x50; //�����ڷ�ʽ1  8λ��У���첽ͨ�ŵ��շ�ģʽ��������շ��жϱ�־λ
	EA=1;  //���ж�����
	ES=0;//��ֹ�����շ��жϣ��������в�ʹ�ô����жϣ�
}

void UART_Send_Data(uchar byte){ // ��������
	SBUF=byte; // ��SUBF����������Ϊbyte
	while(!TI);//�ȴ��������ݷ������
	TI=0;//�������жϱ�־λ��0��Ϊ�´η���������׼��
}


uchar UART_Receive_Data(){
	uchar temp;
	while(!RI);//�ȴ����ڽ��յ����ݣ�RI����1��
	temp=SBUF; // ��SBUF��ȡ��
	RI=0;//��ս����жϱ�־λ��Ϊ�´ν�����׼��
	return temp;
}




int main(){
	uchar temp = 0;
	// ��ʼ����
	UART_Init();
	while(1){
		// ����1
		temp = UART_Receive_Data();
// 		
		// 
		if (temp >= '0' && temp <= '9'){ 
			// P1 = 1 << temp - '0';
			P1 = table[temp - '0'];
		}
		
		// ����2
		UART_Send_Data(UART_Receive_Data());
			
	}
	
	return 0;
	
}