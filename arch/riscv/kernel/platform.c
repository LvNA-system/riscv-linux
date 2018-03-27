/*
 * The RISC-V Platform requires only one device: the config ROM
 *
 * Copyright (C) 2016 SiFive, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 */

#include <linux/platform_device.h>
#include <asm/sbi.h>

static struct resource config_string_resources[] = {
	[0] = {
		.flags = IORESOURCE_MEM,
	},
};

static struct platform_device config_string = {
	.name		= "config-string",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(config_string_resources),
	.resource	= config_string_resources,
};

static struct platform_device *riscv_devices[] __initdata = {
	&config_string,
};

static char config_buffer[4096];
static int __init riscv_platform_init(void)
{
  long size;
  long i;

  /* We need to query SBI for the ROM's location */
  size = sbi_config_string_size();
  Log("Obtaining configure string, size = %ld", size);
  BUG_ON(size >= 4095);
  for(i = 0; i < size; i ++) {
    config_buffer[i] = (char)sbi_config_string_base(i);
    if (config_buffer[i] == '\0') {
      Log("find null byte in configure string!");
    }
  }
  config_buffer[i] = '\0';
  config_string_resources[0].start = (uintptr_t)config_buffer;
  config_string_resources[0].end   = (uintptr_t)config_buffer + size;

  platform_add_devices(riscv_devices, ARRAY_SIZE(riscv_devices));
  return 0;
}

arch_initcall(riscv_platform_init)
