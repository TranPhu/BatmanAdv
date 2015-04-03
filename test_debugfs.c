/********************************
mmap example code
found on http://people.ee.ethz.ch/~arkeller/linux/multi/kernel_user_space_howto-8.html
changed by bta on 27 feb 2014 based on http://lkml.iu.edu//hypermail/linux/kernel/0802.0/0672.html  mmap_nopage -> mmap_fault
********************************/ 

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/slab.h>

#include <linux/mm.h>  /* mmap related stuff */

#include "test_debugfs.h"

struct dentry  *file1;

struct mmap_info {
	char *data;	/* the data */
	int reference;       /* how many times it is mmapped */  	
};


/* keep track of how many times it is mmapped */

void mmap_open(struct vm_area_struct *vma)
{
	printk(KERN_ALERT "open\n");
	struct mmap_info *info = (struct mmap_info *)vma->vm_private_data;
	info->reference++;
}

void mmap_close(struct vm_area_struct *vma)
{
	printk(KERN_ALERT "close\n");
	struct mmap_info *info = (struct mmap_info *)vma->vm_private_data;
	info->reference--;
}

/* nopage is called the first time a memory area is accessed which is not in memory,
 * it does the actual mapping between kernel and user space memory
 */
//struct page *mmap_nopage(struct vm_area_struct *vma, unsigned long address, int *type)	--changed
static int mmap_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
	printk(KERN_ALERT "fault\n");
	struct page *page;
	struct mmap_info *info;
	/* is the address valid? */			//--changed
	/*if (address > vma->vm_end) {
		printk("invalid address\n");
		//return NOPAGE_SIGBUS;
		return VM_FAULT_SIGBUS;
	}
	/* the data is in vma->vm_private_data */
	info = (struct mmap_info *)vma->vm_private_data;
	if (!info->data) {
		printk(KERN_ALERT "no data\n");
		return NULL;	
	}

	/* get the page */
	page = virt_to_page(info->data);
	printk(KERN_ALERT "virt to page ngon\n");
	
	/* increment the reference count of this page */
	get_page(page);
	printk(KERN_ALERT "get page ngon\n");
	vmf->page = page;					//--changed
	/* type is the page fault type */
	/*if (type)
		*type = VM_FAULT_MINOR;
	*/
	printk(KERN_ALERT "qua het\n");
	return page;
}

struct vm_operations_struct mmap_vm_ops = {
	.open =     mmap_open,
	.close =    mmap_close,
	.fault =    mmap_fault,
	//.nopage =   mmap_nopage,				//--changed
};

int my_mmap(struct file *filp, struct vm_area_struct *vma)
{
	printk(KERN_ALERT "my mmap\n");
	vma->vm_ops = &mmap_vm_ops;
	vma->vm_flags |= VM_READ | VM_WRITE | VM_EXEC | VM_SHARED;
	/* assign the file private data to the vm private data */
	vma->vm_private_data = filp->private_data;
	mmap_open(vma);
	return 0;
}

int my_close(struct inode *inode, struct file *filp)
{
	printk(KERN_ALERT "my close\n");
	struct mmap_info *info = filp->private_data;
	/* obtain new memory */
	free_page((unsigned long)info->data);
    	kfree(info);
	filp->private_data = NULL;
	return 0;
}

int my_open(struct inode *inode, struct file *filp)
{
	printk(KERN_ALERT "my open\n");
	struct mmap_info *info = kmalloc(sizeof(struct mmap_info), GFP_KERNEL);
	/* obtain new memory */
    	info->data = (char *)get_zeroed_page(GFP_KERNEL);
	memcpy(info->data, "hello from kernel this is file: ", 32);
	printk(KERN_ALERT "%s\n",info->data);
	memcpy(info->data + 32, filp->f_dentry->d_name.name, strlen(filp->f_dentry->d_name.name));
	printk(KERN_ALERT "%s\n",info->data);
	/* assign this info struct to the file */
	filp->private_data = info;
	return 0;
}

static const struct file_operations my_fops = {
	.open = my_open,
	.release = my_close,
	.mmap = my_mmap,
};

static void mmapexample_module_init(void)
{
	file1 = debugfs_create_file("mmap_example", 0644, NULL, NULL, &my_fops);
	printk(KERN_ALERT "init success\n");
	return 0;
}

static void mmapexample_module_exit(void)
{
	printk(KERN_ALERT "exit success\n");
	debugfs_remove(file1);

}