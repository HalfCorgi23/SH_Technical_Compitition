#include <Servo\src\Servo.h>

Servo handao, tuigan, duoji;

const int photogate = 2;//光电门引脚
const int photoswitch_1 = 3; //光电开关引脚#1
const int photoswitch_2 = 4; //光电开关引脚#2
const int photoswitch_3 = 5; //光电开关引脚#3
const int photoswitch_4 = 6; //光电开关引脚#4

const int photogate_delay = 30;//弹仓传感器延迟

int sensor_0; //弹仓传感器
int sensor_1; //弹盒传感器#1
int sensor_2; //弹盒传感器#2
int sensor_3; //弹盒传感器#3
int sensor_4; //弹盒传感器#4

int statue; //传感器状态

int sensor_1_count = 0;//弹仓传感器计数

void diantuigan(int b)
{
	tuigan.write(b); //1500停止   1000推  2000收
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

//吸子弹

void supply()
{
	Serial.begin(9600);
	handao.attach(A0);
	tuigan.attach(A1);
	duoji.attach(A2);
	songdan(120);//送弹闭合
	handao.writeMicroseconds(1000);
	handaoo(10); //涵道解锁
	delay(3500);//涵道解锁时间
				// tuigan.writeMicroseconds(1000);
	diantuigan(2000);
	delay(3000);
	diantuigan(1500);  //电推杆停止
	delay(2000);
	diantuigan(1000); //电推杆推（1000）
	delay(6000); //电推杆推6s
	diantuigan(1500); //电推杆停止（1500）
	handaoo(100); //涵道开始
	delay(2500); //涵道吸2.5s
	handaoo(10); //涵道停止
	songdan(70); //送弹70 舵机开 
	delay(2000);
	songdan(120); //送弹120 舵机闭合
	delay(1000);
	handaoo(100); //涵道吸
	delay(2500); //涵道吸2.5s
	handaoo(10); //涵道停止
	delay(2000);
	songdan(70); //送弹70 舵机开 
	delay(2000);
	diantuigan(2000); //电推杆收（2000）
	delay(6000); //电推杆收6s
	songdan(120); //送弹120 舵机闭合
	delay(1500);
	handaoo(100); //涵道吸
	delay(2500); //涵道吸2.5s（空吸）
	handaoo(10);  //涵道停
	delay(1000);
	songdan(70); //送弹70 舵机开
	delay(1000);
	handaoo(100); //涵道吸
	delay(2500); //涵道吸2.5s（空吸+送弹开）
	handaoo(10); //涵道关闭
	delay(1500);
	songdan(120); //送弹关闭
	delay(2000);
}

// 初始设置
void setup() {
	Serial.begin(115200);
	sensor_0 = 0;
	sensor_1 = 0;
	sensor_2 = 0;
	sensor_3 = 0;
	sensor_4 = 0;
}

//光电门信号读取，有遮挡输出0，无遮挡输出1
int photogate_get(int pin)
{
	int pinread = digitalRead(pin);
	if (pinread == 0)
	{
		sensor_1_count++;//有遮挡
	}
	else
	{
		sensor_1_count--;//无遮挡
	}
	if (sensor_1_count > 0)
	{
		if (sensor_1_count > photogate_delay)
		{
			sensor_1_count = photogate_delay;
		}
		return 0;//有遮挡
	}
	else
	{
		sensor_1_count = 0;
		return 1;//无遮挡
	}
}

//获取传感器状态
int sensor_get()
{
	sensor_0 = photogate_get(photogate);
	sensor_1 = digitalRead(photoswitch_1);
	sensor_2 = digitalRead(photoswitch_2);
	sensor_3 = digitalRead(photoswitch_3);
	sensor_4 = digitalRead(photoswitch_4);
	if (sensor_0 == 0)
	{
		return 0;//弹仓传感器状态为有子弹，无需补弹
	}
	else
	{
		if (sensor_1 == 0 && sensor_2 == 0 && sensor_3 == 0 && sensor_4 == 0)
		{
			return 1;//传感器正对目标
		}
		else if (sensor_1 == 0 && sensor_2 == 0 && sensor_3 == 0 && sensor_4 == 1)
		{
			return 2;//传感器左正对目标
		}
		else if (sensor_1 == 1 && sensor_2 == 0 && sensor_3 == 0 && sensor_4 == 0)
		{
			return 3;//传感器右正对目标
		}
		else if ((sensor_1 == 0 || sensor_2 == 0) && sensor_3 == 1 && sensor_4 == 1)
		{
			return 4;//目标在传感器左侧
		}
		else if (sensor_1 == 1 && sensor_2 == 0 && (sensor_3 == 0 || sensor_4 == 0))
		{
			return 5;//目标在传感器右侧
		}
		else if (sensor_1 == 1 && sensor_4 == 1 && (sensor_3 == 0 || sensor_2 == 0))
		{
			return 6;//车在弹箱角处
		}
		else
		{
			return 7;//车子可以继续前进
		}
	}
}

// 程序自循环
void loop() {
	statue = sensor_get();
	Serial.print(statue);
	if (statue == 1)
	{
		supply();
	}
}