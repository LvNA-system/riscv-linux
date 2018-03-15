#include <linux/cgroup.h>   
#include <linux/slab.h>
#include <linux/kernel.h>

struct dsid_cgroup 
{
	struct cgroup_subsys_state css;
	int dsid;
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
		struct dsid_cgroup *dsid = css_dsid(dst_css);
		task->dsid = dsid->dsid;
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

static struct cftype dsid_files[] = 
{
	{
		.name = "dsid-set",
		.write = dsid_set_write,
		.seq_show = dsid_set_show,
	},
	{
		.name = "dsid-cache",
		.write = dsid_cache_write,
		.seq_show = dsid_cache_show,
	},
	{
		.name = "dsid-mem",
		.write = dsid_mem_write,
		.seq_show = dsid_mem_show,
	}
	/*
	{
		.name = "dsid-core",
		.write = dsid_core_write,
		.seq_show = dsid_core_show,
	}*/
};

/*
static void dsid_free(struct task_struct *task)
{
	struct dsid_cgroup *dsid = css_dsid(task_css(task, dsid_cgrp_id));
}
*/

struct cgroup_subsys dsid_cgrp_subsys = 
{
	.css_alloc = dsid_css_alloc,
	.css_free = dsid_css_free,
//	.free = dsid_free,
	.legacy_cftypes = dsid_files,
	.dfl_cftypes = dsid_files,
	.can_attach = dsid_can_attach,
};
