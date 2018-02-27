#ifndef __SMSSENDING_H__
#define __SMSSENDING_H__

int fd;//串口符号句柄
extern bool SMSsending_init();//初始化
extern int send_SMS(char* data, int data_num);
extern int send_MMS();

#endif

