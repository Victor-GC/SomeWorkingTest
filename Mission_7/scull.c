#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev>
#include <linux/kernel.h>	/*container_of() */
#include <asm/uaccess.h>	/*copy_to_user(), copy_from_user() */
#include <linux/errno.h>	/*错误代码 */
#include <linux/types.h>	/*size_t */
#include <linux/slab.h>		/*kmalloc(),kfree() */

#include "scull.h"

MODULE_AUTHOR("Victor Geng, Dji");
MODULE_LICENSE("Dual BSD/GPL");

module_param(scull_major, int, S_IRUGO);/*类似于main函数中的参数传递，用来向驱动传递函数 */
module_param(scull_minor, int, S_IRUGO);
module_param(scull_nr_devs, int, S_IRUGO);
module_param(scull_quantum, int, S_IRUGO);
module_param(scull_qset, int, S_IRUGO);

/*设备打开操作*/
int scull_open(struct inode *inode, struct file *filp)
{
	struct scull_dev *dev;	/*设备信息指针 */

	dev = container_of(inode->i_cdev, struct scull_dev, cdev);
	filp->private_data = dev;	/*将获得设备信息指针保存起来 */

	if ((filp->f_flags & O_ACCMODE) == O_WRONLY) {	/*如果打开模式是只写入模式，则将dev清空为0 */
		scull_trim(dev);
	}
	return 0;
}

/*设备关闭操作*/
int scull_release(struct inode *inode, struct file *filp)
{
	return 0;
}

/*从scull_dev的数据内存起始处开始寻找，如果中途有来内存为空则分配对应内存，直到找到指定位置。当中无法分配内存则返回空指针*/
struct scull_qset *scull_follow(struct scull_dev *dev, int n)
{
	struct scull_qset *qs = dev->data;

	if (!qs) {
		qs = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
		if (qs == NULL)
			return NULL;

		memset(qs, 0, sizeof(struct scull_qset));
	}

	while (--n) {
		if (!qs->next) {
			qs->next = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
		if (qs->next == NULL)
			return NULL;
		memset(qs->next, 0, sizeof(struct scull_qset));
		}
		qs = qs->next;
	}
	return qs;
}

/*设备读操作函数*/
ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
		   loff_t *f_pos)
{
	struct scull_dev *dev = filp->private_data;
	struct scull_qset *dptr;
	int quantum = dev->quantum;
	int qset = dev->qset;
	int itemsize = quantum * qset;
	int item, s_pos, q_pos, rest;
	ssize_t retval = 0;

	/*开启互斥信号量 */
	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	/*如果读取位置大于设备已有数据的大小，则退出 */
	if (*f_pos >= dev->size)
		goto out;

	if (f_pos + count > dev->size)	/*读取位置加读取数据量超过已有数据的大小，则只读取已有数据中剩余大小的数据 */
		count = dev->size - *f_pos;

	/*在量子集中寻找链表项，qset索引以及偏移量 */
	item = (long)*f_pos / itemsize;	/*第几个scull_qset开始 */
	rest = (long)*f_pos % itemsize;	/*开始scull_qset的哪个位置开始读取 */
	s_pos = rest / quantum;	/*开始scull_qset中的第几个量子开始 */
	q_pos = rest % quantum;	/*开始量子中哪个位置开始读取 */

	/*沿该链表前行，直到正确的读取位置 */
	dptr = scull_follow(dev, item);	/*获得应该开始的scull_qset的指针 */

	if (dptr == NULL || !dptr->data || !dptr->data[s_pos])	/*如果选定scull_qset为空，或者其指向内容为空，或者应该开始的量子内容为空 */
		goto out;

	/*读取该量子的数据，一直到结尾 */
	if (count > quantum - q_pos)
		count = quantum - q_pos;
	/*将量子中指定位置的数据读取到用户空间 */
	if (copy_to_user(buf, dptr->data[s_pos] + q_pos, count)) {
		retval = -EFAULT;
		goto out;
	}
	*f_pos += count;
	retval = count;

out:
	up(&dev->sem);		/*释放互斥量 */
	return retval;		/*返回已读取数据的值或者错误值 */
}

ssize_t scull_write(struct file *filp, const char __user buf, size_t count,
		    loff_t f_pos)
{
	struct scull_dev *dev = filp->private_data;
	struct scull_qset *dptr;
	int quantum = dev->quantum;
	int qset = dev->qset;
	int itemsize = quantum * qset;
	int item, s_pos, q_pos, rest;
	ssize_t retval = -ENOMEM;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	/*在量子集中寻找链表项，qset索引以及偏移量 */
	item = (long)*f_pos / itemsize;	/*第几个scull_qset开始 */
	rest = (long)*f_pos % itemsize;	/*开始scull_qset的哪个位置开始写入 */
	s_pos = rest / quantum;	/*开始scull_qset中的第几个量子开始 */
	q_pos = rest % quantum;	/*开始量子中哪个位置开始写入 */

	/*沿该链表前行，直到正确的写入位置 */
	dptr = scull_follow(dev, item);	/*获得应该开始的scull_qset的指针 */
	if (dptr == NULL)
		goto out;

	if (!dptr->data) {	/*在当前数组为空的情况下，分配当前数组大小的内存 */
		dptr->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
		if (!dptr->data)
			goto out;
		memset(dptr->data, 0, qset * sizeof(char *));
	}
	if (!dptr->data[s_pos]) {	/*为最后一个量子分配内存 */
		dptr->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
		if (!dptr->data[s_pos])
			goto out;
	}

	/*将数据写入该量子，直到结尾 */
	if (count > quantum - q_pos)
		count = quantum - q_pos;

	if (copy_from_user(dptr->data[s_pos] + q_pos, buf, count)) {
		retval = -EFAULT;
		goto out;
	}
	*f_pos += count;
	retval = count;

	/*更新文件大小 */
	if (dev->size < *f_pos)
		dev->size = *f_pos;

out:
	up(&dev->sem);
	return retval;
}

const struct file_operations scull_fops = {
	.owner = THIS_MODULE,
	/*.llseek = scull_llseek, */
	.read = scull_read,
	.write = scull_write,
	/*.ioctl =  scull_ioctl, */
	.open = scull_open,
	.release = scull_release,
};

/*设备注册*/
static void scull_setup_cdev(struct scull_dev *dev, int index)
{
	int err = 0;
	dev_t devno = MKDEV(scull_major, scull_minor + index);

	cdev_init(&dev->cdev, &scull_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &scull_fops;
	err = cdev_add(&dev->cdev, devno, 1);
	if (err)
		printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}

static int scull_init_module(void)
{
	int result;
	dev_t dev = 0;

	/*在装载驱动的时候获得主设备号,此处次设备号是随即设置的么？ */
	if (scull_major) {	/*如果默认主设备号非零，则按此设备号进行分配 */
		dev = MKDEV(scull_major, scull_minor);
		result = register_chrdev_region(dev, scull_nr_devs, "scull");
	} else {		/*如果主设备号为零，则进行动态分配 */
		result =
		    alloc_chrdev_region(&dev, scull_minor, scull_nr_devs,
					"scull");
		scull_dev = MAJOR(dev);
	}
	if (result < 0) {
		printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
		return result;
	}

	/*为每个设备分配内存，并初始化为0 */
	scull_devices =
	    kmalloc(scull_nr_devs * sizeof(struct scull_dev), GFP_KERNEL);
	if (!scull_devices) {
		result = -ENOMEM;
		goto fail;
	}
	memset(scull_devices, 0, scull_nr_devs * sizeof(struct scull_dev));
	/*以相应的值初始化scull_devices */
	for (int i; i < scull_nr_devs; ++i) {
		scull_devices[i].quantum = scull_quantum;
		scull_devices[i].qset = scull_qset;
		init_MUTEX(&scull_devices[i].sem);
		scull_setup_cdev(&scull_devices[i], i);
	}

	return 0;

fail:
	scull_exit_module();
	return result;
}

int scull_trim(struct scull_dev *dev)
{
	struct scull_qset *next, *dptr;
	int qset = dev->qset;

	for (dptr = dev->data; dptr; dptr = next) {
		if (dptr->data) {
			for (int i = 0; i < qset; ++i)
				kfree(dptr->data[i]);
			kfree(dptr->data);
			dptr->data = NULL;
		}
		next = dptr->next;
		kfree(dptr);
	}
	dev->size = 0;
	dev->quantum = scull_quantum;
	dev->qset = scull_qset;
	dev->data = NULL;
	return 0;
}

static int scull_exit_module(void)
{
	dev_t devno = MKDEV(scull_major, scull_minor);

	/*释放scull_devices设备内存，并且删除每个设备的cdev */
	if (scull_devices) {
		for (int i = 0; i < scull_nr_devices; ++i) {
			scull_trim(scull_devices + i);
			cdev_del(&scull_devices[i].cdev);
		}
		kfree(scull_devices);
	}

	/*释放设备号 */
	unregister_chrdev_region(devno, scull_nr_devs);
}

module_init(scull_init_module);
module_exit(scull_exit_module);
