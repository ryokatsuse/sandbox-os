#include "kernel.h"
#include "common.h"

typedef unsigned char unit8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

extern char __bss[], __bss_end[], __stack_top[];

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4,
                       long arg5, long fid, long eid) {
    register long a0 __asm__("x10") = arg0;
    register long a1 __asm__("x11") = arg1;
    register long a2 __asm__("x12") = arg2;
    register long a3 __asm__("x13") = arg3;
    register long a4 __asm__("x14") = arg4;
    register long a5 __asm__("x15") = arg5;
    register long a6 __asm__("x16") = fid;
    register long a7 __asm__("x17") = eid;

    __asm__ __volatile__("ecall"
                         : "=r"(a0), "=r"(a1)
                         : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                           "r"(a6), "r"(a7)
                         : "memory");
    return (struct sbiret){.error = a0, .value = a1};
}

void my_printf(const char *fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    for (int i = 0; buf[i] != '\0'; i++) {
        putchar(buf[i]);
    }
}

void putchar(char ch) {
    sbi_call(ch, 0, 0, 0, 0, 0, 0, 1 /* Console Putchar */);
}
void *memset(void *buf, char c, size_t n) {
  unit8_t * p = (unit8_t *) buf;
  while(n--)
  *p++ = c;
  return buf;
}

void kernel_main(void) {
    const char *s = "\n\nHello World!\n";
    my_printf("\n\nHello %s\n", "World!");
    my_printf("1 + 2 = %d, %x\n", 1 + 2, 0x1234abcd);
    for (int i = 0; s[i] != '\0'; i++) {
        putchar(s[i]);
    }

    for (;;) {
        __asm__ __volatile__("wfi");
    }
}

__attribute__((section(".text,boot")))
__attribute__((naked))
void boot(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n"
        "j kernel_main\n"
        :
        : [stack_top] "r" (__stack_top)
    );
}