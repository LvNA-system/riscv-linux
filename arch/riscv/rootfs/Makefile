# check RISCV environment variable
ifndef RISCV
$(error Please set environment variable RISCV. Please take a look at README)
endif

all: rootfs

#--------------------------------------------------------------------
# Build tools
#--------------------------------------------------------------------

RISCV_PREFIX=riscv64-unknown-elf-
RISCV_LIBC_PREFIX=riscv64-unknown-linux-gnu-
CC = $(RISCV_PREFIX)gcc
CC_LIBC = $(RISCV_LIBC_PREFIX)gcc
LD = $(RISCV_PREFIX)ld
STRIP = $(RISCV_PREFIX)strip
RISCV_DUMP = $(RISCV_PREFIX)objdump
RISCV_COPY = $(RISCV_PREFIX)objcopy
RISCV_READELF = $(RISCV_PREFIX)readelf
CFLAGS = -static -Wa,-march=RVIMAFD -ffast-math -fno-builtin-printf -O2 #-fPIC
RISCV_DUMP_OPTS = -D
RISCV_LINK_OPTS = -nostdlib -nostartfiles -ffast-math #-lc -lgcc

#--------------------------------------------------------------------
# Busybox variables
#--------------------------------------------------------------------

BUSYBOX_REPO_PATH = repo/busybox
BUSYBOX_ELF_BUILD = $(BUSYBOX_REPO_PATH)/busybox
BUSYBOX_ELF = build/busybox

#--------------------------------------------------------------------
# Stream variables
#--------------------------------------------------------------------

STREAM_REPO_PATH = repo/stream
STREAM_INT_ELF = build/stream.int


#--------------------------------------------------------------------
# Busybox rules
#--------------------------------------------------------------------

$(BUSYBOX_REPO_PATH):
	mkdir -p $@
	git clone -b 1_25_stable git://git.busybox.net/busybox $@
	cp config/busybox.config $@/.config

$(BUSYBOX_ELF): $(BUSYBOX_ELF_BUILD) 
	mkdir -p $(@D)
	ln -sf $(abspath $<) $@

$(BUSYBOX_ELF_BUILD): | $(BUSYBOX_REPO_PATH)
	$(MAKE) -C $(@D)

busybox: $(BUSYBOX_ELF)

busybox-clean:
	-$(MAKE) clean -C $(BUSYBOX_REPO_PATH)
	-rm $(BUSYBOX_ELF)

.PHONY: busybox $(BUSYBOX_ELF_BUILD) busybox-clean

#--------------------------------------------------------------------
# Stream rules
#--------------------------------------------------------------------

$(STREAM_REPO_PATH):
	mkdir -p $@
	git clone https://github.com/jeffhammond/STREAM $@

$(STREAM_REPO_PATH)/stream.c: | $(STREAM_REPO_PATH)

$(STREAM_INT_ELF): $(STREAM_REPO_PATH)/stream.c
	mkdir -p $(@D)
	$(CC) -static -DSTREAM_ARRAY_SIZE=10000 -DNTIMES=2 -DSTREAM_TYPE=int $(STREAM_REPO_PATH)/stream.c -o $@
	$(STRIP) -g $@

stream: $(STREAM_INT_ELF)

stream-clean:
	-rm $(STREAM_INT_ELF)

.PHONY: stream stream-clean

#--------------------------------------------------------------------
# Hello rules
#--------------------------------------------------------------------

HELLO_ELF = build/hello

$(HELLO_ELF): hello/hello.c
	mkdir -p $(@D)
	$(CC) -static $< -o $@

hello: $(HELLO_ELF)

.PHONY: hello

#--------------------------------------------------------------------
# Minimal init
#--------------------------------------------------------------------

MININIT_ELF = build/mininit

$(MININIT_ELF): mininit.c
	mkdir -p $(@D)
	$(CC_LIBC) -DEXECUTE=\"/stream.int\" -static $< -o $@
	$(STRIP) -g $@

mininit: $(MININIT_ELF)

.PHONY: mininit


#--------------------------------------------------------------------
# Sort variables
#--------------------------------------------------------------------

SORT_PATH = sort-openmp
SORT_ELF_BUILD = $(SORT_PATH)/multi_thread
SORT_ELF = build/multi_thread

#--------------------------------------------------------------------
# Sort rules
#--------------------------------------------------------------------

$(SORT_ELF): $(SORT_ELF_BUILD)
	mkdir -p $(@D)
	ln -sf $(abspath $<) $@
	ln -sf $(abspath $(<D))/single_thread $(@D)/single_thread

$(SORT_ELF_BUILD):
	$(MAKE) all -C $(SORT_PATH)

sort: $(SORT_ELF)

sort-clean:
	-$(MAKE) clean -C $(SORT_PATH)
	-rm $(SORT_ELF) build/single_thread

.PHONY: sort $(SORT_ELF_BUILD) sort-clean

#--------------------------------------------------------------------
# Redis variables
#--------------------------------------------------------------------

REDIS_PATH = redis-4.0.2
REDIS_ELF_BUILD = $(REDIS_PATH)/src/redis-server
REDIS_ELF = build/redis-server

#--------------------------------------------------------------------
# Redis rules
#--------------------------------------------------------------------

$(REDIS_ELF): $(REDIS_ELF_BUILD)
	mkdir -p $(@D)
	ln -sf $(abspath $<) $@
	ln -sf $(abspath $(<D))/redis-benchmark $(@D)/redis-benchmark
	ln -sf $(abspath $(<D))/redis-cli $(@D)/redis-cli

$(REDIS_ELF_BUILD):
	$(MAKE) -C $(REDIS_PATH)

redis: $(REDIS_ELF)

redis-clean:
	-$(MAKE) clean -C $(REDIS_PATH)
	-rm $(REDIS_ELF)

.PHONY: redis $(REDIS_ELF_BUILD) redis-clean

#--------------------------------------------------------------------
# Rootfs top-level rules
#--------------------------------------------------------------------

rootfs: busybox redis sort stream hello mininit

clean: busybox-clean stream-clean redis-clean sort-clean
	rm $(HELLO_ELF)

.PHONY: all rootfs rootfs-clean
