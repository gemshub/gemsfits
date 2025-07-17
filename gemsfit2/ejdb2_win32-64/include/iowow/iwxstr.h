#pragma once
#ifndef IWXSTR_H
#define IWXSTR_H

/**************************************************************************************************
 * IOWOW library
 *
 * MIT License
 *
 * Copyright (c) 2012-2022 Softmotions Ltd <info@softmotions.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *************************************************************************************************/

#include "basedefs.h"

#include <stdarg.h>

IW_EXTERN_C_START

typedef struct _IWXSTR IWXSTR;

IW_EXPORT IW_ALLOC IWXSTR* iwxstr_new(void);

IW_EXPORT IWXSTR* iwxstr_new2(size_t siz);

IW_EXPORT IWXSTR* iwxstr_new_printf(const char *format, ...) __attribute__ ((format(__printf__, 1, 2)));

IW_EXPORT IWXSTR* iwxstr_new_clone(const IWXSTR *xstr);

IW_EXPORT IWXSTR* iwxstr_wrap(char *buf, size_t size, size_t asize);

IW_EXPORT void iwxstr_destroy(IWXSTR *xstr);

IW_EXPORT IW_ALLOC char* iwxstr_destroy_keep_ptr(IWXSTR *xstr);

IW_EXPORT iwrc iwxstr_cat(IWXSTR *xstr, const void *buf, size_t size);

IW_EXPORT iwrc iwxstr_cat2(IWXSTR *xstr, const char *buf);

IW_EXPORT iwrc iwxstr_unshift(IWXSTR *xstr, const void *buf, size_t size);

IW_EXPORT iwrc iwxstr_printf_va(IWXSTR *xstr, const char *format, va_list va);

IW_EXPORT iwrc iwxstr_printf(IWXSTR *xstr, const char *format, ...) __attribute__((format(__printf__, 2, 3)));

IW_EXPORT void iwxstr_shift(IWXSTR *xstr, size_t shift_size);

IW_EXPORT void iwxstr_pop(IWXSTR *xstr, size_t pop_size);

IW_EXPORT iwrc iwxstr_insert(IWXSTR *xstr, size_t pos, const void *buf, size_t size);

IW_EXPORT iwrc iwxstr_insert_vaprintf(IWXSTR *xstr, size_t pos, const char *format, va_list va);

IW_EXPORT iwrc iwxstr_insert_printf(IWXSTR *xstr, size_t pos, const char *format, ...) __attribute__((format(__printf__, 3, 4)));

IW_EXPORT char* iwxstr_ptr(IWXSTR *xstr);

IW_EXPORT iwrc iwxstr_set_size(IWXSTR *xstr, size_t size);

/**
 * Returns actual size of data stored in @a xstr buffer.
 */
IW_EXPORT size_t iwxstr_size(IWXSTR *xstr);

/**
 * @brief Returns allocated size of @a xstr buffer.
 */
IW_EXPORT size_t iwxstr_asize(IWXSTR *xstr);

IW_EXPORT void iwxstr_user_data_set(IWXSTR *xstr, void *data, void (*free_fn)(void*));

IW_EXPORT void* iwxstr_user_data_get(IWXSTR *xstr);

IW_EXPORT void* iwxstr_user_data_detach(IWXSTR *xstr);

IW_EXPORT void iwxstr_clear(IWXSTR *xstr);

IW_EXTERN_C_END
#endif
