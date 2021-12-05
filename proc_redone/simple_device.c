/* Optimizations: 
1. Make methods static - methods cannot be seen outside of this file.
2. add __init before the proc init, and __exit before the exit. Both are macros that 
   improve efficiency. __init for example takes the function out of memory once used.




ADD seek function call!!


 */

#include <linux/init.h>   //initilization
#include <linux/module.h> //Lots of stuff
#include <linux/kernel.h>  //lots of stuff
#include <linux/fs.h>  //Contains useful linux data structures.
#include <linux/uaccess.h>  //Moves kernel code to user space. (Lets you print to user space)

#include <linux/cdev.h> //Allows you to reigster the device from code.
#include <linux/types.h> //Major and minor numbers (dev_t)

#include <linux/proc_fs.h>  //open_proc and close proc


#include <linux/stat.h>    //For changing the file permissions properly.



#define PROC_NAME "simple_device"
#define BUFFER_SIZE 128

ssize_t proc_write(struct file *filep, const char *usr_buffer, size_t len, loff_t *offset);
ssize_t proc_read(struct file *file, char *usr_buf, size_t count, loff_t *pos);

loff_t proc_seek (struct file *filep, loff_t, int);
loff_t proc_seek (struct file *filep, loff_t offset, int type);   //Offset will be used to change directories.



static int location = 0;
static char buffer[6][128] = {0};      //6 registers for now.
static int buffer_fill[6] = {0};


static struct proc_dir_entry *proc_file;    //If we want to chang the file permissions correctly.


static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read,
    .write = proc_write,
    .llseek = proc_seek
};



loff_t proc_seek (struct file *filep, loff_t offset, int type){   //not going to use many of these.


    if (offset <= 5 && offset >=0) {

	location = offset;
	printk("Changing current register to: %d\n",location);
	return location;
	
    }
    else{
	printk("%d is not a valid register\n",(int)offset);
	return -1;   //tells user that the seek failed
    }  
}



ssize_t proc_read(struct file *file, char *usr_buf,
		  size_t count, loff_t *pos)
{

    printk("READING!\n");

    copy_to_user(usr_buf, buffer[location], buffer_fill[location]);
    return buffer_fill[location];
}



ssize_t proc_write(struct file *filep, const char *usr_buffer, size_t len, loff_t *offset){

    printk("WRITING!\n"); 
    buffer_fill[location] = 0;   
   
    copy_from_user(buffer[location], usr_buffer ,len);     //its gonna write the result into the kernel buffer...
    buffer_fill[location] = len;
    
    printk("%s\n",buffer[location]);
    
    return 0;    
}


/* This function is called when the module is loaded. */
int proc_init(void)
{


    
//    proc_file -> mode = s_IFREG | S_IRUGO;                        //Also try 0660
    proc_file = proc_create(PROC_NAME, 00444, NULL, &proc_ops);     //MAX permissions power is this 00444
    
    return 0;
}
/* This function is called when the module is removed. */
void proc_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
}






module_init(proc_init);
module_exit(proc_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("simple file read");
MODULE_AUTHOR("SGG");
