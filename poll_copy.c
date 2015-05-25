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
#include <linux/rcupdate.h>
#include <linux/list.h>
#include <linux/netpoll.h>
#include "netpoll_wrapper.h"

/*
	This file contains copies of netpoll_poll_dev() and all related
   functions that are no longer usable
	in kernel 3.15.
*/

#ifndef NETPOLL_POLL_DEV_USABLE

static int poll_one_napi(struct napi_struct *napi, int budget)
{
	int work;

	/* net_rx_action's ->poll() invocations and our's are
	* synchronized by this test which is only made while
	* holding the napi->poll_lock.
	*/
	if (!test_bit(NAPI_STATE_SCHED, &napi->state))
		return budget;

	set_bit(NAPI_STATE_NPSVC, &napi->state);

	work = napi->poll(napi, budget);
	WARN_ONCE(work > budget, "%pF exceeded budget in poll\n", napi->poll);

	clear_bit(NAPI_STATE_NPSVC, &napi->state);

	return budget - work;
}

static void poll_napi(struct net_device *dev, int budget)
{
	struct napi_struct *napi;

	list_for_each_entry(napi, &dev->napi_list, dev_list)
	{
		if (napi->poll_owner != smp_processor_id() &&
		    spin_trylock(&napi->poll_lock)) {
			budget = poll_one_napi(napi, budget);
			spin_unlock(&napi->poll_lock);
		}
	}
}

void netpoll_poll_dev_copy(struct net_device *dev,
			   void (*zap_completion_queue)(void))
{
	const struct net_device_ops *ops;
	struct netpoll_info *ni = rcu_dereference_bh(dev->npinfo);
	int budget = 16;

	/* Don't do any rx activity if the dev_lock mutex is held
	* the dev_open/close paths use this to block netpoll activity
	* while changing device state
	*/
	if (down_trylock(&ni->dev_lock))
		return;

	if (!netif_running(dev)) {
		up(&ni->dev_lock);
		return;
	}

	ops = dev->netdev_ops;
	if (!ops->ndo_poll_controller) {
		up(&ni->dev_lock);
		return;
	}

	/* Process pending work on NIC */
	ops->ndo_poll_controller(dev);

	poll_napi(dev, budget);

	up(&ni->dev_lock);

	zap_completion_queue();
}
#endif