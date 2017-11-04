#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/time.h>
#include <linux/rtc.h>

#define proc_file_name	"myclock"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Daniel Johnsson <danne_j87@hotmail.com>");
MODULE_DESCRIPTION("Kernel clock - test");
MODULE_VERSION("1.0");

void do_gettimeofday(struct timeval *tv);
static ssize_t proc_file_read(struct file *fp, char *buf, size_t len, loff_t *off);

struct proc_dir_entry *psProc_File;


static const struct file_operations proc_file_fops = {
	.owner = THIS_MODULE,
	.read  = proc_file_read,
};


static ssize_t proc_file_read(struct file *fp, char *buf, size_t count, loff_t *offp) {
	int len;
	struct timeval time;
	unsigned long local_time, local_time_swe;
	struct rtc_time tmUtc;
	struct rtc_time tmSwe;

	printk(KERN_INFO "Read file /proc/%s", proc_file_name);
	if (0 == *offp) {

		do_gettimeofday(&time);
		local_time = (u32)(time.tv_sec - (sys_tz.tz_minuteswest * 60));
		local_time_swe = (u32)(time.tv_sec - (-60 * 60));

		rtc_time_to_tm(local_time, &tmUtc);
		rtc_time_to_tm(local_time_swe, &tmSwe);

		len = sprintf(buf, "The UTC time is: @ (%04d-%02d-%02d %02d:%02d:%02d)\nThe GTM+1 time is: @ (%04d-%02d-%02d %02d:%02d:%02d)\n", 
			tmUtc.tm_year + 1900, tmUtc.tm_mon + 1, tmUtc.tm_mday, tmUtc.tm_hour, tmUtc.tm_min, tmUtc.tm_sec,
			tmSwe.tm_year + 1900, tmSwe.tm_mon + 1, tmSwe.tm_mday, tmSwe.tm_hour, tmSwe.tm_min, tmSwe.tm_sec);
		if (count < len)
			return -EINVAL;
	}
	else
		return 0;

	*offp = len;
	return len;
}


int proc_file_start(void) {
	if (!(psProc_File = proc_create(proc_file_name, 0666, NULL, &proc_file_fops)))
		return -ENOMEM;

	printk(KERN_INFO "/proc/%s created\n", proc_file_name);
	return 0;
}


void proc_file_end(void) {
	remove_proc_entry(proc_file_name, NULL);
	printk(KERN_INFO "/proc/%s removed\n", proc_file_name);
}

module_init(proc_file_start);
module_exit(proc_file_end);
