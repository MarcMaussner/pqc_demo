/**
 * syscalls.c - Minimal newlib syscall stubs for bare-metal STM32
 * Required with --specs=nano.specs since libc_nano.a needs these stubs
 * Note: _exit is NOT defined here — it comes from -lnosys
 */
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdint.h>

/* External symbols from linker script */
extern uint8_t _end;
extern uint8_t _estack;

/* Heap management - _sbrk is NOT in libnosys for nano */
void *_sbrk(ptrdiff_t incr)
{
    static uint8_t *heap_end = NULL;
    uint8_t *prev_heap_end;

    if (heap_end == NULL) {
        heap_end = &_end;
    }

    prev_heap_end = heap_end;

    if ((heap_end + incr) > &_estack) {
        errno = ENOMEM;
        return (void *)-1;
    }

    heap_end += incr;
    return (void *)prev_heap_end;
}

int _read(int file, char *ptr, int len)
{
    (void)file; (void)ptr; (void)len;
    return 0;
}

int _write(int file, char *ptr, int len)
{
    (void)file; (void)ptr;
    return len;
}

int _close(int file)
{
    (void)file;
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    (void)file; (void)ptr; (void)dir;
    return 0;
}

int _fstat(int file, struct stat *st)
{
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    (void)file;
    return 1;
}
