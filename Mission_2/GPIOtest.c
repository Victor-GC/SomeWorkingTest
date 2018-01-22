#include <stdio.h>
#include <wiringPi.h>
using namespace std;

int main()
{
	wiringPiSetup();
	pinMode(1,PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetRange(1024);
	pwmSetClock(16);

	printf("pwm begin\n");
	pwmWrite(1,0.5);
	delay(500);

	pinMode(2,OUTPUT);
	printf("Set 2 high\n");
	digitalWrite(2,HIGH);
	delay(500);
	
	printf("Set 2 low\n");
	digitalWrite(2,LOW);
	delay(500);

	printf("test finished!\n");
	}
