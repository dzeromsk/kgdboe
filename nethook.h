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
#ifndef __NETHOOK_H__
#define __NETHOOK_H__

#include <linux/netdevice.h>

bool nethook_initialize(struct net_device *dev);
void nethook_cleanup(void);

// Other cores should NOT be stopped at this point. Nethook will grab relevant
// spinlocks and ensure other cores won't.
void nethook_take_relevant_resources(void);

// Other cores may be still frozen. Nethook should not do anything that may
// block.
void nethook_release_relevant_resources(void);

// Other cores SHOULD be stopped at this point and up until a call to
// work_done().
// Nethook will release non-recursive locks and it's the caller's responsibility
// to ensure that no other core grabs them.
void nethook_netpoll_work_starting(void);

// Other cores SHOULD be stopped at this point.
void nethook_netpoll_work_done(void);

#endif /* __NETHOOK_H__ */