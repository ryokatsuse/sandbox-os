#!/bin/bash
set -xue

# QEMUのファイルパス
# QEMU_PATH=/usr/local/bin/qemu-system-x86_64
QEMU=qemu-system-riscv32

# QMENUを起動
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot
