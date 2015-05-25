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
#ifndef __TRACEWRAPPER_H__
#define __TRACEWRAPPER_H__

#include <linux/version.h>
#include <linux/kallsyms.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 15, 0)

#define register_tracepoint_wrapper(tp, func, ctx)                             \
	register_trace_##tp(func, ctx)

#define unregister_tracepoint_wrapper(tp, func, ctx)                           \
	unregister_trace_##tp(func, ctx)

#else

#define register_tracepoint_wrapper(tp, func, ctx)                             \
	tracepoint_probe_register(                                             \
	    (struct tracepoint *)kallsyms_lookup_name("__tracepoint_" #tp),    \
	    func, ctx)

#define unregister_tracepoint_wrapper(tp, func, ctx)                           \
	tracepoint_probe_unregister(                                           \
	    (struct tracepoint *)kallsyms_lookup_name("__tracepoint_" #tp),    \
	    func, ctx)

#endif

#endif /* __TRACEWRAPPER_H__ */