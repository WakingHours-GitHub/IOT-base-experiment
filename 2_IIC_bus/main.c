#include<reg51.h>

#define uchar unsigned char
#define uint unsigned int
#define write_ADD 0xa0 // д��ĵ�ַ
#define read_ADD 0xa1 // ��ȡ�ĵ�ַ



void SomeNop();     //����ʱ
void init();    //��ʼ��
void check_ACK(void);
void I2CStart(void);
void I2cStop(void);
void write_byte(uchar dat);//д�ֽ�
void delay();
uchar read_byte();     //���ֽ�
uchar read_add(uchar address);
void write_add(uchar address,uchar date);
uchar read(uchar addr);       //ָ����ַ��

sbit sda=P1^0;
sbit scl=P1^1;

uchar a;  

bit flag;  //Ӧ���־λ


void main(){
    init();
    write_add(0x10,0x00); //���ַ5д��0xaa
    delay(); // ��ʱ, ��Ҫ�ȴ����ݵ�д��
    P1=read_add(0x10);      //��ȡ��ַ5��ֵ
    while(1); // ԭ�صȴ�
}
 
 
 
void delay(){
	; ;
}

//��ʼ�ź� SCL�ڸߵ�ƽ�ڼ䣬SDAһ���½������ʾ�����ź� 
void start() {     
    sda=1; //�ͷ�SDA����  
    delay();  
    scl=1;  
    delay();  
    sda=0;  
    delay();  
}  

//ֹͣ SCL�ڸߵ�ƽ�ڼ䣬SDAһ�����������ʾֹͣ�ź�  
void stop() {  
    sda=0;  
    delay();  
    scl=1;  
    delay();  
    sda=1;  
    delay();  
}

 //Ӧ�� SCL�ڸߵ�ƽ�ڼ䣬SDA�����豸��Ϊ�͵�ƽ��ʾӦ��  
void respons() {
    uchar i;  
    scl=1;  
    delay(); 
    //����ȴ�250��CPUʱ������ 
    while((sda==1)&&(i<250))i++;  
    scl=0;  
    delay();  
}  

//���߳�ʼ�� �����߶�����һ�ͷ�����  ���������ź�ǰ��Ҫ�ȳ�ʼ�����ߡ������м�⵽���߿��вſ�ʼ���������ź�  
void init(){  
    sda=1;  
    delay();  
    scl=1;  
    delay();  
}  
  
void write_byte(uchar date){
    uchar i,temp;  
    temp=date;  
  
  
    for(i=0;i<8;i++)  {  
        temp=temp<<1;  // ����
        scl=0;//����SCL����Ϊֻ����ʱ���ź�Ϊ�͵�ƽ�ڼ䰴�������ϵĸߵ͵�ƽ״̬������仯�����ڴ�ʱ����һ��ѭ����scl=1һ���γ�һ��������  
        delay();  
        sda=CY;  
        delay();  
        scl=1;//����SCL����ʱSDA�ϵ������ȶ�  
        delay();  
    }  
    scl=0;//����SCL��Ϊ�´����ݴ�������׼��  
    delay();  
    sda=1;//�ͷ�SDA���ߣ��������ɴ��豸���ƣ�������豸���������ݺ���SCLΪ��ʱ������SDA��ΪӦ���ź�  
    delay();  
}  

//��һ���ֽ� 
uchar read_byte() {  
    uchar i,k;  
    scl=0;  
    delay();  
    sda=1;  
    delay();  
    for(i=0;i<8;i++)  
    {  
        scl=1;//������ʱ��IIC�豸�����ݷ���sda���ϣ����ڸߵ�ƽ�ڼ������Ѿ��ȶ������Խ�����  
        delay();      
        k=(k<<1)|sda;  
        scl=0;//����SCL��ʹ���Ͷ˿��԰����ݷ���SDA��  
        delay();      
    }  
    return k;  
}  

//�����ַдһ���ֽ�  
void write_add(uchar address,uchar date){  
    start();//����  
    write_byte(0xa0);//���ʹ��豸��ַ  
    respons();//�ȴ����豸����Ӧ  
    write_byte(address);//����оƬ�ڵ�ַ  
    respons();//�ȴ����豸����Ӧ  
    write_byte(date);//��������  
    respons();//�ȴ����豸����Ӧ  
    stop();//ֹͣ  
}  
//��ȡһ���ֽ�  
uchar read_add(uchar address){  
    uchar date;  
    start();//����  
    write_byte(0xa0);//���ͷ��ʹ��豸��ַ д����  
    respons();//�ȴ����豸����Ӧ  
    write_byte(address);//����оƬ�ڵ�ַ  
    respons();//�ȴ����豸����Ӧ  
    start();//����  
    write_byte(0xa1);//���ͷ��ʹ��豸��ַ ������  
    respons();//�ȴ����豸����Ӧ  
    date=read_byte();//��ȡ����  
    stop();//ֹͣ  
    return date;//��������  
}