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
#ifndef __IRQSYNC_H__
#define __IRQSYNC_H__

#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/spinlock.h>
#include <linux/list.h>

struct managed_irq
{
	struct list_head list;
	unsigned number;
	struct irq_desc *irq;
};

struct irqsync_manager
{
	struct list_head irqs;
	struct timer_list timer;
	spinlock_t lock;
	bool irqs_disabled;
	bool suspend_active;
};

struct irqsync_manager *irqsync_create(void);
void irqsync_free(struct irqsync_manager *mgr);

bool irqsync_add_managed_irq(struct irqsync_manager *mgr, unsigned number,
			     struct irq_desc *irq);

void irqsync_suspend_irqs(struct irqsync_manager *mgr);
void irqsync_resume_irqs(struct irqsync_manager *mgr);

#endif /* __IRQSYNC_H__ */