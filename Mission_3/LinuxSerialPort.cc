#include "LinuxSerialPort.h"
#include "termios.h"
#include "stdlib.h"
#include "stdio.h"
#include "fcntl.h"
#include "unistd.h"
#include "strings.h"

LinuxSerialPort::LinuxSerialPort()
{
	serialport_fd=-1;
	nBits=8;
	nEvent='N';
	nSpeed=9600;
	nStop=1;
	bzero(portName,sizeof(portName));
}

LinuxSerialPort::~LinuxSerialPort()
{

}

bool LinuxSerialPort::readConfigData(char* cfg_filename,int filename_size)
{
	FILE *serial_fp=NULL;//文件操作指针
	char j[10];//用来存放cfg配置文件的临时缓存空间

	serial_fp = fopen(cfg_filename,"r");//打开文件

	if(NULL == serial_fp)//打开失败则返回
	{
		fclose(serial_fp);
		return false;
	}
	else//打开成功则读取数据
	{
		fscanf(serial_fp, "DEV=%s\n", portName);//将串口名读取到portNmae中

		fscanf(serial_fp, "SPEED=%s\n", j);//读取波特率，存储到nSpeed中
		nSpeed = atoi(j);

		fscanf(serial_fp, "DATABITS=%s\n", j);//读取数据位，存储到nBits中
		nBits = atoi(j);

		fscanf(serial_fp, "STOPBITS=%s\n", j);//读取停止位，存储到nStop中
		nStop = atoi(j);

		fscanf(serial_fp, "PARITY=%s\n", j);//读取奇偶校验位，存储到nEvent中
		nEvent = j[0];
	}

	fclose(serial_fp);
	return true;
}


bool LinuxSerialPort::openPort()
{
	serialport_fd=open(portName,O_RDWR|O_NOCTTY|O_NONBLOCK);//打开串口，模式读写，非阻塞
	if(serialport_fd==-1)
	{
		return false;
	}

	if(!configPort())//根据读取到的串口信息配置串口
	{
		return false;
	}

	return true;
}

bool LinuxSerialPort::closePort()
{
	close(serialport_fd);
	return true;
}

int LinuxSerialPort::writeData(char* data_buffer,int data_size)
{
	int writed=write(serialport_fd,data_buffer,data_size);
	if(writed==data_size)
	{//所有要写的字节全部写入了，则返回已经写入的字节数
		return writed;
	}
	else
	{//要写入的字节数并没有写全，则刷新缓冲区，并返回-1
		tcflush(serialport_fd,TCOFLUSH);
		return -1;
	}
}

int LinuxSerialPort::readData(char* data_buffer,int data_size)
{
	int readed=read(serialport_fd,data_buffer,data_size);	
	return readed;
}

bool LinuxSerialPort::configPort()
{
	struct termios newtio,oldtio;//termios是异步通信口的控制设置接口
	if(tcgetattr(serialport_fd,&oldtio)!=0)//判断是否能成功获得串口数据，如果获得串口数据都不成功，则无法设置数据
	{
		return false;
	}

	bzero(&newtio,sizeof(newtio));//将newtio清零,全是‘\0’

	//c_cflag为控制模式状态设定字
	newtio.c_cflag |= (CLOCAL|CREAD);//设置本地连接(不改变端口所有者)，接收使能
	newtio.c_cflag &= ~CRTSCTS;//取消硬件流控
	newtio.c_cflag &= ~CSIZE;//取消数据位的位掩码


	switch(nBits)//设置串口通信数据位数
	{
		case 7:
			newtio.c_cflag |= CS7;//设置为7位
			break;
		case 8:
			newtio.c_cflag |= CS8;//设置为8位
			break;
		default:
			newtio.c_cflag |= CS8;
			break;
	}
	switch(nEvent)
	{
		case 'O'://使用奇校验
			newtio.c_cflag |= PARENB;//使能奇偶校验
			newtio.c_cflag |= PARODD;//设置为奇校验
			newtio.c_iflag |= (INPCK|ISTRIP);//使能输入奇偶校验，去除第8位
			break;
		case 'E'://使用偶校验
			newtio.c_cflag |= PARENB;//使能奇偶校验
			newtio.c_cflag &= ~PARODD;//设置为偶校验
			newtio.c_iflag |= (INPCK|ISTRIP);//使能输入奇偶校验，去除第8位
			break;
		case 'N'://不使用奇偶校验
			newtio.c_cflag &= ~PARENB;//不使用奇偶校验
			break;
		default://不使用奇偶校验
			newtio.c_cflag &= ~PARENB;//不使用奇偶校验
			break;
	}

	switch(nSpeed)//设置波特率
	{
		case 1200:
			cfsetispeed(&newtio,B1200);//设置输入波特率
			cfsetospeed(&newtio,B1200);//设置输出波特率
			break;
		case 2400:
			cfsetispeed(&newtio,B2400);//设置输入波特率
			cfsetospeed(&newtio,B2400);//设置输出波特率
			break;
		case 4800:
			cfsetispeed(&newtio,B4800);//设置输入波特率
			cfsetospeed(&newtio,B4800);//设置输出波特率
			break;
		case 9600:
			cfsetispeed(&newtio,B9600);//设置输入波特率
			cfsetospeed(&newtio,B9600);//设置输出波特率
			break;
		case 19200:
			cfsetispeed(&newtio,B19200);//设置输入波特率
			cfsetospeed(&newtio,B19200);//设置输出波特率
			break;
		case 38400:
			cfsetispeed(&newtio,B38400);//设置输入波特率
			cfsetospeed(&newtio,B38400);//设置输出波特率
			break;
		case 115200:
			cfsetispeed(&newtio,B115200);//设置输入波特率
			cfsetospeed(&newtio,B115200);//设置输出波特率
			break;
		default:
			cfsetispeed(&newtio,B9600);//设置输入波特率
			cfsetispeed(&newtio,B9600);//设置输入波特率
			break;
	}

	if(nStop==1)//设置停止位
	{
		newtio.c_cflag &= ~CSTOPB;//无停止位
	}
	else
	{
		newtio.c_cflag |= CSTOPB;//1位停止位
	}

	newtio.c_oflag &= ~OPOST;//使能非正是模式

	//只有设置为阻塞的时候下面参数才有效果，且只针对读操作
	newtio.c_cc[VTIME] = 0;//设定在不满足VMIN的情况或者读缓存区剩余数据在多少ms后读出
	newtio.c_cc[VMIN] = 0;//设定读缓冲区字节大于几个时才读操作

	tcflush(serialport_fd,TCIFLUSH);//清空输入缓冲区

	if(tcsetattr(serialport_fd,TCSANOW,&newtio)!=0)//将设置好的串口参数设定给串口
	{
		return false;
	}

	return true;
}
