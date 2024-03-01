#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fcntl.h>

static int ref_count = 1;
static void* kptr = NULL;
static struct class *devClass;
static struct cdev cdev;
static dev_t kheap_dev_no;
static spinlock_t sp_lock;

static long kheap_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
static int kheap_open(struct inode *i, struct file *f);
static int kheap_close(struct inode *i, struct file *f);

static struct file_operations kheap_fops = {
	.owner = THIS_MODULE,
	.open = kheap_open,
	.release = kheap_close,
	.unlocked_ioctl = kheap_ioctl
};


static int __init kheap_init(void) {
	if (alloc_chrdev_region(&kheap_dev_no, 0, 1, "kheap") < 0) {
		return -1;
	}
 
	if ((devClass = class_create(THIS_MODULE, "chardrv")) == NULL) {
		unregister_chrdev_region(kheap_dev_no, 1);
		return -1;
	}

	if (device_create(devClass, NULL, kheap_dev_no, NULL, "kheap") == NULL) {
		class_destroy(devClass);
		unregister_chrdev_region(kheap_dev_no, 1);
		return -1;
	}

	cdev_init(&cdev, &kheap_fops);

	if (cdev_add(&cdev, kheap_dev_no, 1) == -1) {
		device_destroy(devClass, kheap_dev_no);
		class_destroy(devClass);
		unregister_chrdev_region(kheap_dev_no, 1);
		return -1;
	}

	return 0;
}

static void __exit kheap_exit(void) {
	unregister_chrdev_region(kheap_dev_no, 1);
	cdev_del(&cdev);
}

static long kheap_ioctl(struct file *filp, unsigned int cmd, unsigned long size) {

	long retval = -1;
	spin_lock(&sp_lock);
	switch (cmd) {
		case 0xdead:
			if (kptr) {
				printk(KERN_INFO "Bad Guy!\n");
				break;
			}
			kptr = kmalloc(64, GFP_KERNEL);
			ref_count++;
			retval = 0;
			break;
		case 0xbeef:
			if (!ref_count || !kptr) {
				printk(KERN_INFO "Bad Guy!\n");
				break;
			}
			ref_count--;
			kfree(kptr);
			retval = 0;
			break;
		default:
			printk(KERN_INFO "Bad Guy!\n");
			break;
	}
	spin_unlock(&sp_lock);
	return retval;
}

static int kheap_open(struct inode *i, struct file *f) {
	printk(KERN_INFO "OPEN\n");
	return 0;
}

static int kheap_close(struct inode *i, struct file *f) {
	printk(KERN_INFO "CLOSE\n");
	return 0;
}

module_init(kheap_init);
module_exit(kheap_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("XiaozaYa");
