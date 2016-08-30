#include <Servo\src\Servo.h>

Servo handao, tuigan, duoji;

const int photogate = 2;//���������
const int photoswitch_1 = 3; //��翪������#1
const int photoswitch_2 = 4; //��翪������#2
const int photoswitch_3 = 5; //��翪������#3
const int photoswitch_4 = 6; //��翪������#4

const int photogate_delay = 30;//���ִ������ӳ�

int sensor_0; //���ִ�����
int sensor_1; //���д�����#1
int sensor_2; //���д�����#2
int sensor_3; //���д�����#3
int sensor_4; //���д�����#4

int statue; //������״̬

int sensor_1_count = 0;//���ִ���������

void diantuigan(int b)
{
	tuigan.write(b); //1500ֹͣ   1000��  2000��
}
void handaoo(int c)
{
	handao.write(c);
}
void songdan(int a)
{
	//70__120
	duoji.write(a);
}

//���ӵ�

void supply()
{
	Serial.begin(9600);
	handao.attach(A0);
	tuigan.attach(A1);
	duoji.attach(A2);
	songdan(120);//�͵��պ�
	handao.writeMicroseconds(1000);
	handaoo(10); //��������
	delay(3500);//��������ʱ��
				// tuigan.writeMicroseconds(1000);
	diantuigan(2000);
	delay(3000);
	diantuigan(1500);  //���Ƹ�ֹͣ
	delay(2000);
	diantuigan(1000); //���Ƹ��ƣ�1000��
	delay(6000); //���Ƹ���6s
	diantuigan(1500); //���Ƹ�ֹͣ��1500��
	handaoo(100); //������ʼ
	delay(2500); //������2.5s
	handaoo(10); //����ֹͣ
	songdan(70); //�͵�70 ����� 
	delay(2000);
	songdan(120); //�͵�120 ����պ�
	delay(1000);
	handaoo(100); //������
	delay(2500); //������2.5s
	handaoo(10); //����ֹͣ
	delay(2000);
	songdan(70); //�͵�70 ����� 
	delay(2000);
	diantuigan(2000); //���Ƹ��գ�2000��
	delay(6000); //���Ƹ���6s
	songdan(120); //�͵�120 ����պ�
	delay(1500);
	handaoo(100); //������
	delay(2500); //������2.5s��������
	handaoo(10);  //����ͣ
	delay(1000);
	songdan(70); //�͵�70 �����
	delay(1000);
	handaoo(100); //������
	delay(2500); //������2.5s������+�͵�����
	handaoo(10); //�����ر�
	delay(1500);
	songdan(120); //�͵��ر�
	delay(2000);
}

// ��ʼ����
void setup() {
	Serial.begin(115200);
	sensor_0 = 0;
	sensor_1 = 0;
	sensor_2 = 0;
	sensor_3 = 0;
	sensor_4 = 0;
}

//������źŶ�ȡ�����ڵ����0�����ڵ����1
int photogate_get(int pin)
{
	int pinread = digitalRead(pin);
	if (pinread == 0)
	{
		sensor_1_count++;//���ڵ�
	}
	else
	{
		sensor_1_count--;//���ڵ�
	}
	if (sensor_1_count > 0)
	{
		if (sensor_1_count > photogate_delay)
		{
			sensor_1_count = photogate_delay;
		}
		return 0;//���ڵ�
	}
	else
	{
		sensor_1_count = 0;
		return 1;//���ڵ�
	}
}

//��ȡ������״̬
int sensor_get()
{
	sensor_0 = photogate_get(photogate);
	sensor_1 = digitalRead(photoswitch_1);
	sensor_2 = digitalRead(photoswitch_2);
	sensor_3 = digitalRead(photoswitch_3);
	sensor_4 = digitalRead(photoswitch_4);
	if (sensor_0 == 0)
	{
		return 0;//���ִ�����״̬Ϊ���ӵ������貹��
	}
	else
	{
		if (sensor_1 == 0 && sensor_2 == 0 && sensor_3 == 0 && sensor_4 == 0)
		{
			return 1;//����������Ŀ��
		}
		else if (sensor_1 == 0 && sensor_2 == 0 && sensor_3 == 0 && sensor_4 == 1)
		{
			return 2;//������������Ŀ��
		}
		else if (sensor_1 == 1 && sensor_2 == 0 && sensor_3 == 0 && sensor_4 == 0)
		{
			return 3;//������������Ŀ��
		}
		else if ((sensor_1 == 0 || sensor_2 == 0) && sensor_3 == 1 && sensor_4 == 1)
		{
			return 4;//Ŀ���ڴ��������
		}
		else if (sensor_1 == 1 && sensor_2 == 0 && (sensor_3 == 0 || sensor_4 == 0))
		{
			return 5;//Ŀ���ڴ������Ҳ�
		}
		else if (sensor_1 == 1 && sensor_4 == 1 && (sensor_3 == 0 || sensor_2 == 0))
		{
			return 6;//���ڵ���Ǵ�
		}
		else
		{
			return 7;//���ӿ��Լ���ǰ��
		}
	}
}

// ������ѭ��
void loop() {
	statue = sensor_get();
	Serial.print(statue);
	if (statue == 1)
	{
		supply();
	}
}