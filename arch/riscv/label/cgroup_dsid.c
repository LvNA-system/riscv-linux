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
enum {
	/* Access current dsid */
	CP_HART_DSID            = 0x41,

	/* Access dsid selector */
	CP_HART_SEL             = 0x42,

	/* Read total dsid count */
	CP_DSID_COUNT           = 0x43,

	/* Access mem base with current dsid */
	CP_MEM_BASE_LO          = 0x44,

	CP_MEM_BASE_HI          = 0x45,

	/* Access mem mask with current dsid */
	CP_MEM_MASK_LO          = 0x46,

	CP_MEM_MASK_HI          = 0x47,

	CP_BUCKET_FREQ          = 0x48,

	CP_BUCKET_SIZE          = 0x49,

	CP_BUCKET_INC           = 0x4a,

	CP_TRAFFIC              = 0x4b,

	CP_WAYMASK              = 0x4c,

	CP_L2_CAPACITY          = 0x4d,

	CP_DSID_SEL             = 0x4e,

	CP_LIMIT_INDEX          = 0x4f,

	CP_LIMIT                = 0x50,

	CP_LOW_THRESHOLD        = 0x51,

	CP_HIGH_THRESHOLD       = 0x52,

	CP_MAX_QUOTA            = 0x53,

	CP_MIN_QUOTA            = 0x54,

	CP_QUOTA_STEP           = 0x55,

	CP_TIMER_LO             = 0x56,

	CP_TIMER_HI             = 0x57,

	CORE_PC_HI              = 0x70,

	CORE_PC_LO              = 0x71,

	CORE_PC_SNAP            = 0x72,

	CORE_PC_READ_DONE       = 0x73,

	CORE_PC_READ            = 0x74,

	CORE_INT_DEBUG          = 0x75,

	CORE_N_INT_DEBUG        = 0x76,

	CORE_N_INT_DEBUG_LOCAL  = 0x77,

	CORE_CSR_INT_VALID      = 0x78,

	CORE_CSR_PENDING_INT_LO = 0x79,

	CORE_CSR_PENDING_INT_HI = 0x7a,

	CP_HART_ID              = 0x7b,
};

int splitted_cp_reg_lo = {
    CP_MEM_BASE_LO,
    CP_MEM_MASK_LO,
    CP_TIMER_LO
};

const char *cp_reg_name[] = {
	/* Access current dsid */
	[CP_HART_DSID            - CP_HART_DSID] = "hart_dsid",

	/* Access dsid selector */
	[CP_HART_SEL             - CP_HART_DSID] = "hart_sel",

	/* Read total dsid count */
	[CP_DSID_COUNT           - CP_HART_DSID] = "dsid_count",

	/* Access mem base with current dsid */
	[CP_MEM_BASE_LO          - CP_HART_DSID] = "mem_base",

	[CP_MEM_BASE_HI          - CP_HART_DSID] = "mem_base",

	/* Access mem mask with current dsid */
	[CP_MEM_MASK_LO          - CP_HART_DSID] = "mem_mask",

	[CP_MEM_MASK_HI          - CP_HART_DSID] = "mem_mask",

	[CP_BUCKET_FREQ          - CP_HART_DSID] = "bucket_freq",

	[CP_BUCKET_SIZE          - CP_HART_DSID] = "bucket_size",

	[CP_BUCKET_INC           - CP_HART_DSID] = "bucket_inc",

	[CP_TRAFFIC              - CP_HART_DSID] = "l1_traffic",

	[CP_WAYMASK              - CP_HART_DSID] = "l2_waymask",

	[CP_L2_CAPACITY          - CP_HART_DSID] = "l2_capacity",

	[CP_DSID_SEL             - CP_HART_DSID] = "dsid_sel",

	[CP_LIMIT_INDEX          - CP_HART_DSID] = "N/A",

	[CP_LIMIT                - CP_HART_DSID] = "N/A",

	[CP_LOW_THRESHOLD        - CP_HART_DSID] = "N/A",

	[CP_HIGH_THRESHOLD       - CP_HART_DSID] = "N/A",

	[CP_MAX_QUOTA            - CP_HART_DSID] = "N/A",

	[CP_MIN_QUOTA            - CP_HART_DSID] = "N/A",

	[CP_QUOTA_STEP           - CP_HART_DSID] = "N/A",

	[CP_TIMER_LO             - CP_HART_DSID] = "timestamp",

	[CP_TIMER_HI             - CP_HART_DSID] = "timestamp",

	[CORE_PC_HI              - CP_HART_DSID] = "N/A",

	[CORE_PC_LO              - CP_HART_DSID] = "N/A",

	[CORE_PC_SNAP            - CP_HART_DSID] = "N/A",

	[CORE_PC_READ_DONE       - CP_HART_DSID] = "N/A",

	[CORE_PC_READ            - CP_HART_DSID] = "N/A",

	[CORE_INT_DEBUG          - CP_HART_DSID] = "N/A",

	[CORE_N_INT_DEBUG        - CP_HART_DSID] = "N/A",

	[CORE_N_INT_DEBUG_LOCAL  - CP_HART_DSID] = "N/A",

	[CORE_CSR_INT_VALID      - CP_HART_DSID] = "N/A",

	[CORE_CSR_PENDING_INT_LO - CP_HART_DSID] = "N/A",

	[CORE_CSR_PENDING_INT_HI - CP_HART_DSID] = "N/A",

	[CP_HART_ID              - CP_HART_DSID] = "vhartid",
};

#define NR(arr) (sizeof(arr) / sizeof(arr[0]))

// waymask,access,miss,usage,sizes,freq,incs,read,write
// cpbase[idx * (1 << proc_dsid_width) + proc_dsid]

extern uint32_t cp_reg_r(uint32_t dm_reg);
extern void cp_reg_w(uint32_t dm_reg, uint32_t val);

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
	while(*val != ' ') {
		if(*val++ == '\0')
		{
			printk("error, input format is: table_name hex_val \n");
			return -EINVAL;
		}
    }
	*val='\0';
	err	= kstrtou32(++val, 16, &num);
	if (err < 0) {
		return -EINVAL;
    }

    for (int i = 0; i < NR(cp_reg_name); i++) {
        const char *name = cp_reg_name[i];
        // TODO membase/memmask/hartid uses hartsel for indexing
        if (name && strcmp(name, val) == 0) {
            cp_reg_w(CP_DSID_SEL, dsid_ptr->dsid);
            cp_reg_w(CP_HART_DSID + i, num);
            return nbytes;
        }
    }

    printk("please input correct table name:\n");
    for (int i = 0; i < NR(cp_reg_name); i++) {
        const char *name = cp_reg_name[i];
        if (name && name != "N/A") {
            printk("%s ", name);
        }
    }
    printk("\n");
    return -EINVAL;
}

static int dsid_mem_show(struct seq_file *sf, void *v)
{
	struct cgroup_subsys_state *css = seq_css(sf);
	struct dsid_cgroup *dsid_ptr = css_dsid(css);

	int proc_dsid = dsid_ptr->dsid;

    cp_reg_w(CP_DSID_SEL, dsid_ptr->dsid);
	seq_printf(sf,"dsid of this group:%d\n",proc_dsid);

    for (int i = 0; i < NR(cp_reg_name); i++) {
        const char *name = cp_reg_name[i];
        if (name && name != "N/A") {
            seq_printf(sf, "%s: %x\n", name, cp_reg_r(CP_HART_SEL + i));
        }
    }

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
