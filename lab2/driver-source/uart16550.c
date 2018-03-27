#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include "uart16550.h"
#include "uart16550_hw.h"

MODULE_DESCRIPTION("Uart16550 driver");
MODULE_LICENSE("GPL");

#ifdef __DEBUG
        #define dprintk(fmt, ...)     printk(KERN_DEBUG "%s:%d " fmt, \
                                        __FILE__, __LINE__, ##__VA_ARGS__)
#else
        #define dprintk(fmt, ...)     do { } while (0)
#endif

static struct class *uart16550_class = NULL;
/*
 * Module parameters
 */
static int major = 42;
static int behavior = 0x3;
module_param(major, int, 0);
module_param(behavior, int, 0);

/*
 * Flags used in module init/clean_up
 */
static int have_com1, have_com2, dev_count, first_minor;

/*
 * File operation structure
 */
static int uart16550_open(struct inode *inode, struct file *filp);
static int uart16550_release(struct inode *inode, struct file *filp);
static ssize_t uart16550_read(struct file *filp, char __user *user_buffer,
        size_t size, loff_t *offset);
static ssize_t uart16550_write(struct file *filp, const char __user *user_buffer,
        size_t size, loff_t *offset);

static const struct file_operations uart16550_fops = {
        .owner = THIS_MODULE,
        .open = uart16550_open,
        .read = uart16550_read,
        .write = uart16550_write,
        .release = uart16550_release
};

/*
 * Device specific structure
 */
struct uart16550_dev {
        struct cdev cdev;
};

static struct uart16550_dev uart16550_dev_COM1, uart16550_dev_COM2;

/*
 * Setup cdev
 */
static int uart16550_setup_cdev(struct uart16550_dev *dev, int minor)
{
        int err;
        dev_t dev_num = MKDEV(major, minor);

        cdev_init(&dev->cdev, &uart16550_fops);
        dev->cdev.owner = THIS_MODULE;

        err = cdev_add(&dev->cdev, dev_num, 1);
        if (err) {
                dprintk("Fail add cdev, device minor number: %d\n", minor);
                return err;
        }

        return 0;
}

/*
 * File operations
 */
static int uart16550_open(struct inode *inode, struct file *filp)
{
        dprintk("open()\n");
        return 0;
}

static int uart16550_release(struct inode *inode, struct file *filp)
{
        dprintk("release()\n");
        return 0;
}

static ssize_t uart16550_read(struct file *filp, char __user *user_buffer,
        size_t size, loff_t *offset)
{
        dprintk("read()\n");
        return 0;
}

static ssize_t uart16550_write(struct file *filp, const char __user *user_buffer,
        size_t size, loff_t *offset)
{
        dprintk("write()\n");
        return size;
}

/* TODO: unlocked_ioctl */

#ifdef UART16550_WRITE
static ssize_t uart16550_write(struct file *file, const char __user *user_buffer,
        size_t size, loff_t *offset)
{
        int bytes_copied;
        u32 device_port;
        /*
         * TODO: Write the code that takes the data provided by the
         *      user from userspace and stores it in the kernel
         *      device outgoing buffer.
         * TODO: Populate bytes_copied with the number of bytes
         *      that fit in the outgoing buffer.
         */

        uart16550_hw_force_interrupt_reemit(device_port);

        return bytes_copied;
}
#endif

irqreturn_t interrupt_handler(int irq_no, void *data)
{
        int device_status;
        u32 device_port;
        /*
         * TODO: Write the code that handles a hardware interrupt.
         * TODO: Populate device_port with the port of the correct device.
         */

        device_status = uart16550_hw_get_device_status(device_port);

        while (uart16550_hw_device_can_send(device_status)) {
                u8 byte_value;
                /*
                 * TODO: Populate byte_value with the next value
                 *      from the kernel device outgoing buffer.
                 * NOTE: If the outgoing buffer is empty, the interrupt
                 *       will not occur again. When data becomes available,
                 *       the driver must either:
                 *   a) force the hardware to reissue the interrupt.
                 *      OR
                 *   b) send the data separately.
                 */
                uart16550_hw_write_to_device(device_port, byte_value);
                device_status = uart16550_hw_get_device_status(device_port);
        }

        while (uart16550_hw_device_has_data(device_status)) {
                u8 byte_value;
                byte_value = uart16550_hw_read_from_device(device_port);
                /*
                 * TODO: Store the read byte_value in the kernel device
                 *      incoming buffer.
                 */
                device_status = uart16550_hw_get_device_status(device_port);
        }

        return IRQ_HANDLED;
}

static int uart16550_init(void)
{
        int err;
        struct device *dev_ret;
        /*
         * Parsing module parameters
         */
        switch (behavior) {
                case 0x1 :
                        have_com1 = 1;
                        have_com2 = 0;
                        dev_count = 1;
                        first_minor = 0; 
                        break;
                case 0x2 :
                        have_com1 = 0;
                        have_com2 = 1;
                        dev_count = 1;
                        first_minor = 1; 
                        break;
                case 0x3 :
                        have_com1 = 1;
                        have_com2 = 1;
                        dev_count = 2;
                        first_minor = 0; 
                        break;
                default :
                        err = -EINVAL;
                        dprintk("Bad module parameters\n");
                        goto nothing_to_undo;
        }

        /*
         * Register character device numbers
         */
        err = register_chrdev_region(MKDEV(major, first_minor), dev_count,
                "uart16550");
        if (err) {
                dprintk("Fail registering chrdev region\n");
                goto nothing_to_undo; 
        }

        /*
         * TODO: Write driver initialization code here.
         * TODO: have_com1 & have_com2 need to be set according to the
         *      module parameters.
         * TODO: Check return values of functions used. Fail gracefully.
         */

        /*
         * Setup a sysfs class & device to make /dev/com1 & /dev/com2 appear.
         */
        uart16550_class = class_create(THIS_MODULE, "uart16550");
        if (IS_ERR(uart16550_class)) {
                dprintk("Fail creating sysfs class\n");
                err = PTR_ERR(uart16550_class);
                goto undo_reg_chrdev_region;
        }

        if (have_com1) {
                /* Setup the hardware device for COM1 */
                err = uart16550_hw_setup_device(COM1_BASEPORT, THIS_MODULE->name);
                if (err) {
                        dprintk("Fail setting up hw device COM1\n");
                        goto undo_sysfs_class_create; 
                }

                /* Create the sysfs info for /dev/com1 */
                dev_ret = device_create(uart16550_class, NULL, MKDEV(major, 0),
                        NULL, "com1");
                if (IS_ERR(dev_ret)) {
                        dprintk("Fail device_create COM1\n");
                        err = PTR_ERR(dev_ret);
                        goto undo_hw_setup_COM1;
                }

                /* Register COM1 to the kernel */ 
                err = uart16550_setup_cdev(&uart16550_dev_COM1, 0);
                if (err)
                        goto undo_dev_create_COM1;
        }
        if (have_com2) {
                /* Setup the hardware device for COM2 */
                err = uart16550_hw_setup_device(COM2_BASEPORT, THIS_MODULE->name);
                if (err) {
                        dprintk("Fail setting up hw device COM2\n");
                        goto undo_dev_reg_COM1; 
                }

                /* Create the sysfs info for /dev/com2 */
                dev_ret = device_create(uart16550_class, NULL, MKDEV(major, 1),
                        NULL, "com2");
                if (IS_ERR(dev_ret)) {
                        dprintk("Fail device_create COM2\n");
                        err = PTR_ERR(dev_ret);
                        goto undo_hw_setup_COM2;
                }

                /* Register COM2 to the kernel */ 
                err = uart16550_setup_cdev(&uart16550_dev_COM2, 1);
                if (err)
                        goto undo_dev_create_COM2;
        }

        /*
         * Success
         */
        return 0;

        /*
         * Error handling
         * Undo things
         */
undo_dev_create_COM2:
        if (have_com2)
                device_destroy(uart16550_class, MKDEV(major, 1));       
undo_hw_setup_COM2:
        if (have_com2)
                uart16550_hw_cleanup_device(COM2_BASEPORT);       
undo_dev_reg_COM1:
        if (have_com1)
                cdev_del(&uart16550_dev_COM1.cdev);
undo_dev_create_COM1:
        if (have_com1)
                device_destroy(uart16550_class, MKDEV(major, 0));       
undo_hw_setup_COM1:
        if (have_com1)
                uart16550_hw_cleanup_device(COM1_BASEPORT);       
undo_sysfs_class_create:
        class_destroy(uart16550_class);
undo_reg_chrdev_region:      
        unregister_chrdev_region(MKDEV(major, first_minor), dev_count);
nothing_to_undo:
        return err;
}

static void uart16550_cleanup(void)
{
        /*
         * TODO: Write driver cleanup code here.
         * TODO: have_com1 & have_com2 need to be set according to the
         *      module parameters.
         */
        if (have_com1) {
                /* Unregister COM1 from kernel */
                cdev_del(&uart16550_dev_COM1.cdev);
                /* Reset the hardware device for COM1 */
                uart16550_hw_cleanup_device(COM1_BASEPORT);
                /* Remove the sysfs info for /dev/com1 */
                device_destroy(uart16550_class, MKDEV(major, 0));
        }
        if (have_com2) {
                /* Unregister COM2 from kernel */
                cdev_del(&uart16550_dev_COM2.cdev);
                /* Reset the hardware device for COM2 */
                uart16550_hw_cleanup_device(COM2_BASEPORT);
                /* Remove the sysfs info for /dev/com2 */
                device_destroy(uart16550_class, MKDEV(major, 1));
        }

        /*
         * Cleanup the sysfs device class.
         */
        class_destroy(uart16550_class);

        /*
         * unregister character device numbers
         */
        unregister_chrdev_region(MKDEV(major, first_minor), dev_count);
}

module_init(uart16550_init);
module_exit(uart16550_cleanup);
