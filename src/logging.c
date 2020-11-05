#include "logging.h"

#include <linux/ktime.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>

static __le64         buf_log_packet[LOG_BUFSIZE];
static unsigned short buf_log_step[LOG_BUFSIZE];
static __le64         buf_log_time[LOG_BUFSIZE];

static unsigned short log_index = 0;

spinlock_t wg_log_spinlock;

/* Log the timestamp and step for an individual packet identified by its counter.
 * Steps are defined in the logging.h header file.
 *
 * When the internal buffer is full, it'll be written to a log file in the /tmp/
 * directory. The log contains the packet id, step and time in nanoseconds
 * since boot, each separated by a whitespace.
 */
void wg_log_packet(__le64 message_id, unsigned short log_step)
{
	int i;
	struct file *fp = NULL;
	mm_segment_t old_fs;

	// get kernel time with nanosecond precision
	u64 ktime = ktime_get_boottime_ns();

	spin_lock_bh(&wg_log_spinlock);

	// save in buffer
	buf_log_packet[log_index] = message_id;
	buf_log_step[log_index]   = log_step;
	buf_log_time[log_index]   = ktime;

	// when buffer is full, write to log file
	if (LOG_BUFSIZE == ++log_index) {
		old_fs = get_fs();
		set_fs(KERNEL_DS);

		fp = filp_open(LOG_FILENAME, O_RDWR|O_CREAT|O_APPEND, 0644);
		if (IS_ERR(fp)) {
			// error, skip and force overwrite of last log entry
			printk(KERN_ALERT "wireguard: filp_open failed with %ld", PTR_ERR(fp));

			set_fs(old_fs);
			log_index--;

			spin_unlock_bh(&wg_log_spinlock);
			return;
		}

		for (i = 0; i < LOG_BUFSIZE; i++) {
			char msg[100];
			snprintf(msg, sizeof(msg), "%lld %d %llu\n",
					buf_log_packet[i], buf_log_step[i], buf_log_time[i]);

			kernel_write(fp, msg, strlen(msg), &fp->f_pos);
		}

		set_fs(old_fs);

		log_index = 0;
		filp_close(fp, NULL);
	}

	spin_unlock_bh(&wg_log_spinlock);
}
