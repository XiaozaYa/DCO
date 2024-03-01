#!/bin/sh
qemu-system-x86_64 \
    -m 4G \
    -kernel ./bzImage \
    -initrd  ./rootfs.cpio \
    -monitor /dev/null \
    -append "root=/dev/ram rdinit=/sbin/init console=ttyS0 oops=panic panic=1 loglevel=3 quiet pti=on kaslr" \
    -cpu kvm64,+smep,+smap \
    -smp cores=4,threads=2 \
    -nographic \
    -s
