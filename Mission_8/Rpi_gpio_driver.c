#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
MODULE_LICENSE("Dual BSD/GPL");

/*树莓派GPIO引脚总数*/
#define MAX_GPIO_NUMBER 40

/*定义GPIO的状态和方向*/
enum stata {low, high};
enum direction {in, out};

/*函数的前向声明*/
static int rpi_gpio_init(void);
static int rpi_gpio_exit(void);

static int __init rpi_gpio_init(void)
{
    return 0;
}

static int __exit rpi_gpio_exit(void)
{
    return 0;
}

module_init(rpi_gpio_init);
module_exit(rpi_gpio_exit);
