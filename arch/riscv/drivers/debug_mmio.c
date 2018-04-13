#include <asm/io.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/platform_device.h>

volatile uint64_t *cpbase;
extern int debug_install;
/*enum{
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
*/
uint32_t cp_reg_r(uint32_t idx,uint32_t proc_id)
{
	return (uint32_t)readq( cpbase+(idx * (1<<3) + proc_id) );
}
void cp_reg_w(uint32_t idx,uint32_t proc_id, uint32_t val)
{
	writeq( val, cpbase+(idx * (1<<3) + proc_id) );
}

static int riscv_debug_probe(struct platform_device *pdev)
{
	cpbase=ioremap_nocache(0x900,0x700);
	debug_install = 1;
	return 0;
}

static int riscv_debug_remove(struct platform_device *pdev)
{
	debug_install=0;
	iounmap(cpbase);
	return 0;
}

static struct platform_driver riscv_debug_driver = {
	.driver = {
		.name = "debug",
	},
	.probe = riscv_debug_probe,
	.remove = riscv_debug_remove,
};

module_platform_driver(riscv_debug_driver);
