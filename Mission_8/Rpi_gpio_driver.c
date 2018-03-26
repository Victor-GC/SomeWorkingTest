#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
MODULE_LICENSE("Dual BSD/GPL");

/*树莓派GPIO引脚总数*/
#define MAX_GPIO_NUMBER 40
#define NUM_GPIO_PINS 21   //设备编号的大小范围
#define DEVICE_NAME "raspi-gpio"

/*定义GPIO的状态和方向*/
enum stata {low, high};
enum direction {in, out};

/*函数的前向声明*/
static int rpi_gpio_init(void);mZstatic int rpi_gpio_exit(void);

/*全局变量*/
static dev_t first;
static struct class *raspi_gpio_class;//class结构体指针

static int __init rpi_gpio_init(void)
{
    int i, ret, index = 0;
    if (alloc_chrdev_region(&first, 0, NUM_GPIO_PINS, DEVICE_NAME) < 0) {
        printk(KERN_DEBUG "Cannot register device\n");
        return -1;
    }
    if ((raspi_gpio_class = class_create( THIS_MODULE, DEVICE_NAME)) == NULL) {
        printk(KERN_DEBUG "Cannot create class%s\n", DEVICE_NAME);
        unregister_chrdev_region(first, NUM_GPIO_PINS);
        return -EINVAL;
    }
    
    return 0;
}

static int __exit rpi_gpio_exit(void)
{
    return 0;
}

module_init(rpi_gpio_init);
module_exit(rpi_gpio_exit);
