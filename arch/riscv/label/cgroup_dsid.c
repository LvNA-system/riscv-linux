#include <linux/cgroup.h>   
#include <linux/slab.h>
#include <linux/kernel.h>

#include <linux/smp.h>

struct dsid_cgroup 
{
	struct cgroup_subsys_state css;
	unsigned int dsid;
	unsigned int sizes;
	unsigned int freq;
	unsigned int inc;
};

static struct dsid_cgroup *css_dsid(struct cgroup_subsys_state *css)
{
	return css ? container_of(css,struct dsid_cgroup, css) : NULL;
}
			  
static struct cgroup_subsys_state * dsid_css_alloc(struct cgroup_subsys_state *parent)
{
	struct dsid_cgroup *dsid;
	dsid = kzalloc(sizeof(struct dsid_cgroup), GFP_KERNEL);
	if(!dsid)
		return ERR_PTR(-ENOMEM);
	return &dsid->css;      
}

static void dsid_css_free(struct cgroup_subsys_state *css)
{		
	kfree(css_dsid(css));
}

static int dsid_can_attach(struct cgroup_taskset *tset)
{
	struct task_struct *task;
	struct cgroup_subsys_state *dst_css;

	cgroup_taskset_for_each(task, dst_css, tset)
	{
		struct dsid_cgroup *dsid_ptr = css_dsid(dst_css);
		task->dsid = dsid_ptr->dsid;

		task->sizes = dsid_ptr-> sizes;
		task->freq = dsid_ptr-> freq;
		task->inc = dsid_ptr-> inc;
	}
	return 0;
}

static ssize_t dsid_set_write(struct kernfs_open_file *of, char *buf, size_t nbytes, loff_t off)
{
	struct cgroup_subsys_state *css = of_css(of);
	struct dsid_cgroup *dsid = css_dsid(css);
	int err;
	long num;
	struct css_task_iter it;
	struct task_struct *task;
	buf = strstrip(buf);
	err	= kstrtol(buf,10,&num);
	if(err < 0)
		return -EINVAL;
	dsid->dsid = num;
	/*
	struct list_head head = css->cgroup->cset_links;
	struct list_head *cset_link;
	list_for_each(cset_link, &head)
	{
		struct css_set *cset = container_of(cset_link, struct cgrp_cset_link, cset_link)->;

	}*/
	css_task_iter_start(css,&it);
	while((task = css_task_iter_next(&it)))
	{
		task->dsid = dsid->dsid;
	}
	css_task_iter_end(&it);

	return nbytes;
}

static int dsid_set_show(struct seq_file *sf, void *v)
{
	struct cgroup_subsys_state *css = seq_css(sf);
	struct dsid_cgroup *dsid = css_dsid(css);
	seq_printf(sf,"dsid of this group is %d\n",dsid->dsid);
	return 0;
}


extern uint64_t *cpbase;
enum{
	//cache p
	WAYMASK=0,
	//cache s
	ACCESS,
	MISS,
	USAGE,
	//mem p
	SIZES,
	FREQ,
	INC,
	DSID,
	//mem s
	MEM_READ,
	MEM_WRITE
};
// waymask,access,miss,usage,sizes,freq,incs,read,write
// cpbase[idx * (1 << proc_dsid_width) + proc_dsid]

extern uint32_t cp_reg_r(uint32_t idx,uint32_t proc_id);
extern void cp_reg_w(uint32_t idx,uint32_t proc_id, uint32_t val);

static ssize_t dsid_mem_write(struct kernfs_open_file *of, char *buf, size_t nbytes, loff_t off)
{
	struct cgroup_subsys_state *css = of_css(of);
	struct dsid_cgroup *dsid_ptr = css_dsid(css);
	struct css_task_iter it;
	struct task_struct *task;
	int err;
	uint32_t num;
	char *tab = buf;
	char *val = buf;
	while(*val != ' ')
		if(*val++ == '\0')
		{
			printk("error, input format is: table_name,hex_val \n");
			return -EINVAL;
		}
	*val='\0';
	err	= kstrtou32(++val,16,&num);
	if(err < 0)
		return -EINVAL;
	int flag=-1;

	if(strcmp(tab,"sizes")==0)
		flag=SIZES;
	else if(strcmp(tab,"freq")==0)
		flag=FREQ;
	else if(strcmp(tab,"inc")==0)
		flag=INC;
	else if(strcmp(tab,"dsid")==0)
		flag=DSID;
	else if(strcmp(tab,"read")==0)
		flag=MEM_READ;
	else if(strcmp(tab,"write")==0)
		flag=MEM_WRITE;
	
	if(flag<0)
	{
		printk("please input correct table name: sizes,freq,inc,read,write\n");
		return -EINVAL;
	}
	else if(flag>=MEM_READ)
		cp_reg_w(flag,dsid_ptr->dsid,num);
	else{
		(&(dsid_ptr->sizes))[flag-SIZES]= num;

		css_task_iter_start(css,&it);
		while((task = css_task_iter_next(&it)))
		{
			(&(task->sizes))[flag-SIZES] = (&(dsid_ptr->sizes))[flag-SIZES];
			//task->freq = dsid_ptr->freq;
			//task->inc = dsid_ptr->inc;
		}
		css_task_iter_end(&it);
	}

	return nbytes;
}

static int dsid_mem_show(struct seq_file *sf, void *v)
{
	struct cgroup_subsys_state *css = seq_css(sf);
	struct dsid_cgroup *dsid_ptr = css_dsid(css);
	int proc_dsid=dsid_ptr->dsid;
	//uint32_t tok_size = cp_reg_r(SIZES,proc_dsid);
	//uint32_t tok_freq = cp_reg_r(FREQ,proc_dsid);
	//uint32_t tok_inc = cp_reg_r(INC,proc_dsid);
	uint32_t tok_size = dsid_ptr->sizes;
	uint32_t tok_freq = dsid_ptr->freq;
	uint32_t tok_inc = dsid_ptr->inc;
	uint32_t mem_read = cp_reg_r(MEM_READ,proc_dsid);
	uint32_t mem_write = cp_reg_r(MEM_WRITE,proc_dsid);

	seq_printf(sf,"dsid of this group:%d\n",proc_dsid);
	seq_printf(sf,"memory p table:\ntok_size:%#x tok_freq:%#x tok_inc:%#x\n",tok_size,tok_freq,tok_inc);
	seq_printf(sf,"memory s table:\nread:%#x write:%#x\n",mem_read,mem_write);
	return 0;
}
static ssize_t dsid_cache_write(struct kernfs_open_file *of, char *buf, size_t nbytes, loff_t off)
{
	struct cgroup_subsys_state *css = of_css(of);
	struct dsid_cgroup *dsid_ptr = css_dsid(css);
	int err;
	uint32_t num;
	char *tab = buf;
	char *val = buf;
	while(*val != ' ')
		if(*val++ == '\0')
		{
			printk("error, input format is: table_name,hex_val \n");
			return -EINVAL;
		}
	*val='\0';
	err	= kstrtou32(++val,16,&num);
	if(err < 0)
		return -EINVAL;
	int flag=-1;

	if(strcmp(tab,"waymask")==0)
		flag=WAYMASK;
	else if(strcmp(tab,"access")==0)
		flag=ACCESS;
	else if(strcmp(tab,"miss")==0)
		flag=MISS;
	else if(strcmp(tab,"usage")==0)
		flag=USAGE;
	
	if(flag<0)
	{
		printk("please input correct table name: waymask,access,miss,usage\n");
		return -EINVAL;
	}
	cp_reg_w(flag,dsid_ptr->dsid,num);

	return nbytes;
}

static int dsid_cache_show(struct seq_file *sf, void *v)
{
	struct cgroup_subsys_state *css = seq_css(sf);
	struct dsid_cgroup *dsid_ptr = css_dsid(css);
	int proc_dsid=dsid_ptr->dsid;
	uint32_t waymask = cp_reg_r(WAYMASK,proc_dsid);
	uint32_t access_num = cp_reg_r(ACCESS,proc_dsid);
	uint32_t miss_num = cp_reg_r(MISS,proc_dsid);
	uint32_t usage_num = cp_reg_r(USAGE,proc_dsid);

	seq_printf(sf,"dsid of this group:%d\n",proc_dsid);
	seq_printf(sf,"cache p table:\nwaymask:%#x\n",waymask);
	seq_printf(sf,"cache s table:\naccess:%#x miss:%#x usage:%#x\n",access_num,miss_num,usage_num);
	return 0;
}

static struct cftype dsid_files[] = 
{
	{
		.name = "dsid-set",
		.write = dsid_set_write,
		.seq_show = dsid_set_show,
	},
	{
		.name = "dsid-mem",
		.write = dsid_mem_write,
		.seq_show = dsid_mem_show,
	},
	{
		.name = "dsid-cache",
		.write = dsid_cache_write,
		.seq_show = dsid_cache_show,
	},
	{}//null terminator
	
};



struct cgroup_subsys dsid_cgrp_subsys = 
{
	.css_alloc = dsid_css_alloc,
	.css_free = dsid_css_free,
	.can_attach = dsid_can_attach,
//	.free = dsid_free,
	.legacy_cftypes = dsid_files,
	.dfl_cftypes = dsid_files,
};
