#include "LinuxSerialPort.h"
#include "stdio.h"
#include "string.h"

using namespace std;

LinuxSerialPort port1;


int print_core(char data)
{
  return  port1.writeData(&data,1);
}

int print_core(char* data_str)
{
	int data_size=strlen(data_str);
	return port1.writeData(data_str,data_size);
}

int print_core(int data_int)
{
	char data_str[256];
	int data_size=sprintf(data_str,"%d",data_int);
	return port1.writeData(data_str,data_size);
}

int print_core(float data_float)
{
	char data_str[256];
	int data_size=sprintf(data_str,"%f",data_float);
	return port1.writeData(data_str,data_size);
}

int main(int argc,char* argv[])
{
	if(!port1.readConfigData(argv[1],strlen(argv[1])))
	{
		printf("读取串口配置文件失败！\n");
		return -1;
	}
	if(!port1.openPort())
	{
		printf("打开串口或者配置串口数据失败！\n");
		return -1;
	}
	//要打印的数据
	int data1=123;
	float data2=123.45678;
	char data3='A';
	char data4[15]={0};
	strcpy(data4,"Hello Dji!");
	
	print_core(data1);
	print_core(data2);
	print_core(data3);
	print_core(data4);
	
	char inBuffer[10]={0};
	port1.readData(inBuffer,10);
	printf("%s\n",inBuffer);


	return 0;
}
