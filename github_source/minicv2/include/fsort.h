/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013-2016 Kwabena W. Agyeman <kwagyeman@openmv.io>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Fast 9 and 25 bin sort.
 *
 */
#ifndef __FSORT_H__
#define __FSORT_H__
#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif
void fsort(int *data, int n);
#ifdef __cplusplus
}
#endif
#endif /* __FSORT_H__ */
