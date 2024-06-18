/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Memory allocation functions.
 */
#include <string.h>
// #include "py/runtime.h"
// #include "py/gc.h"
// #include "py/mphal.h"
#include "xalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include "imlib_io.h"

int Debug_s = 0;

#define LOG_PRINT_P(fmt, ...) do{\
	if(Debug_s)\
	{\
		printf(fmt"  [info:%s:%d] [%s]\n", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__);\
	}\
}while(0);



static void xalloc_fail(size_t size)
{
    imlib_printf(0, "MemoryError :memory allocation failed, allocating %d bytes", size);
    // mp_raise_msg_varg(&mp_type_MemoryError,
    //         MP_ERROR_TEXT("memory allocation failed, allocating %u bytes"), (uint)size);
}

// returns null pointer without error if size==0
void *xalloc(size_t size)
{
    void *mem = malloc(size);
    if (size && (mem == NULL)) {
        xalloc_fail(size);
    }
    LOG_PRINT_P("xalloc:%p", mem);
    return mem;
}

// returns null pointer without error if size==0
void *xalloc_try_alloc(size_t size)
{
    return malloc(size);
}

// returns null pointer without error if size==0
void *xalloc0(size_t size)
{
    void *mem = malloc(size);
    if (size && (mem == NULL)) {
        xalloc_fail(size);
    }
    memset(mem, 0, size);
    LOG_PRINT_P("xalloc0:%p", mem);
    return mem;
}

// returns without error if mem==null
void xfree(void *mem)
{
    free(mem);
    LOG_PRINT_P("xfree:%p", mem);
}

// returns null pointer without error if size==0
// allocs if mem==null and size!=0
// frees if mem!=null and size==0
void *xrealloc(void *mem, size_t size)
{
    mem = realloc(mem, size);
    if (size && (mem == NULL)) {
        xalloc_fail(size);
    }
    LOG_PRINT_P("xrealloc:%p", mem);
    return mem;
}
void *xcalloc(size_t nitems, size_t size)
{
    void *mem = xcalloc(nitems, size);
    if (size && (mem == NULL)) {
        xalloc_fail(size);
    }
    LOG_PRINT_P("xalloc_fail:%p", mem);
    return mem;
}

