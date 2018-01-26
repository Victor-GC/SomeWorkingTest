#ifndef _SCULL_H_
#define _SCULL_H_

#ifndef SCULL_MAJOR
#define SCULL_MAJOR 0
#endif

#ifndef SCULL_NR_DEVS
#define SCULL_NR_DEVS 4
#endif

/*
 *下面这两个宏是内存分配的。
 *驱动中每一个设备的内存是一段可变的内存区域，通过一系列的内存块连接组成。
 *"scull_dev->data"指针指向一个指针数组，数组中的每个指针都指向一个SCULL_QUANTUM大小的内存块。
 *数组的长度为SCULL_QSET。
 */
#ifndef SCULL_QUANTUM
#define SCULL_QUANTUM 4000
#endif

#ifndef SCULL_QSET
#define SCULL_QSET 1000
#endif


struct scull_qset {
	void **data;
	struct scull_qset *next;
};

/*表示驱动中一个设备的结构体*/
struct scull_dev {
	struct scull_qset *data;/*指向第一个量子集的指针*/
	int quantum;/*当前量子的大小*/
	int qset;/*当前数组的长度*/
	unsigned long size;/*保存在其中的数据总量*/
	unsigned int access_key;/*由sculluid，scullpriv使用*/
	struct semaphore sem;/*互斥信号量*/
	struct cdev cdev;/*字符设备结构*/
}

/*全局变量*/
int scull_major = SCULL_MAJOR;/*主设备号，默认值为0，表示动态分配*/
int scull_minor;
int scull_nr_devs = SCULL_NR_DEVS;/*空scull设备的个数*/
int scull_quantum = SCULL_QUANTUM;/*scull中量子的默认大小*/
int scull_qset = SCULL_QSET;/*scull中量子数组的默认个数*/
struct scull_dev *scull_devices;

/*这里声明函数原型，可以用来与其他文件共享*/
int scull_open(struct inode *inode, struct file *filp);
int scull_release(struct inode *inode, struct file *filp);
struct scull_qset *scull_follow(struct scull_dev *dev, int n);
ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t scull_write(struct file *filp, const char __user buf, size_t count, loff_t f_pos);



#endif
