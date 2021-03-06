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
#ifndef __NETPOLL_WRAPPER_H__
#define __NETPOLL_WRAPPER_H__

#include <linux/netpoll.h>
#include <linux/atomic.h>
#include <linux/version.h>

typedef void (*pnetpoll_wrapper_rx_handler)(void *pContext, int port, char *msg,
					    int len);

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 15, 0)
#define NETPOLL_POLL_DEV_USABLE
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 9, 0)
#define ip_addr_as_int(addr) (addr)
#else
#define ip_addr_as_int(addr) ((addr).ip)
#endif

struct queued_arp_reply
{
	__be32 local_ip;
	__be32 remote_ip;
	u8 remote_mac[ETH_ALEN];
	bool valid;
};

struct netpoll_wrapper
{
	struct netpoll netpoll_obj;
	struct net_device *pDeviceWithHandler;

	pnetpoll_wrapper_rx_handler pReceiveHandler;
	void *pUserContext;

#ifdef NETPOLL_POLL_DEV_USABLE
	void (*netpoll_poll_dev)(struct net_device *dev);
#else
	void (*zap_completion_queue)(void);
#endif

	bool netpoll_initialized, tracepoint_registered;
	bool drop_other_packets;
	bool handle_arp;
	// Theoretically this should be a queue with dynamic size as we don't
	// want to drop ARP requests, as otherwise someone
	// can execute a DoS attack by flooding us with fake ARP requests, but
	// we don't bother with this assuming that kernel
	// debugging over network is usually done in a controlled environment.
	struct queued_arp_reply pending_arp_replies[4];
};

struct netpoll_wrapper *netpoll_wrapper_create(const char *pDeviceName,
					       int localPort,
					       const char *pOptionalLocalIp);
void netpoll_wrapper_free(struct netpoll_wrapper *pWrapper);
void netpoll_wrapper_send_reply(struct netpoll_wrapper *pWrapper,
				const void *pData, int dataSize);
void netpoll_wrapper_poll(struct netpoll_wrapper *pWrapper);
void netpoll_wrapper_set_callback(struct netpoll_wrapper *pWrapper,
				  pnetpoll_wrapper_rx_handler pReceiveHandler,
				  void *pUserContext);

static inline void
netpoll_wrapper_set_drop_flag(struct netpoll_wrapper *pWrapper, bool flag)
{
	BUG_ON(!pWrapper);
	pWrapper->drop_other_packets = flag;
}

#ifndef NETPOLL_POLL_DEV_USABLE
void netpoll_poll_dev_copy(struct net_device *dev,
			   void (*zap_completion_queue)(void));
#endif

#endif /* __NETPOLL_WRAPPER_H__ */