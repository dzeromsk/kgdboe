/*
 * Copyright (c) 2014 Sysprogs OU <sysprogs@sysprogs.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#ifndef __TIMERHOOK_H__
#define __TIMERHOOK_H__

#include <linux/module.h>
#include <linux/spinlock.h>

struct timer_hook
{
	spinlock_t lock;
	struct module *module;
};

struct timer_hook *timerhook_create(struct module *moduleToHook);
void timerhook_free(struct timer_hook *hook);

static inline spinlock_t *timerhook_get_spinlock(struct timer_hook *hook)
{
	BUG_ON(!hook);
	return &hook->lock;
}

#endif /* __TIMERHOOK_H__ */