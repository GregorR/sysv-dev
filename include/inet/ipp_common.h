/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright 2002, 2003 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef	_INET_IPP_COMMON_H
#define	_INET_IPP_COMMON_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef _KERNEL
#include <ipp/ipp.h>

/*
 * IPP  -- IP Policy -- interface.
 *
 * WARNING: Everything in this file is private, belonging to the IPP
 * subsystem.  The interfaces and declarations made here are subject
 * to change.
 */

extern uint32_t ipp_action_count;

/* Whether ip policy is enabled at callout position proc */
#define	IPP_ENABLED(proc)	((ipp_action_count != 0) && \
	(~(ip_policy_mask) & (proc)))

/* Apply IPQoS policies for inbound traffic? */
#define	IP6_IN_IPP(flags) (IPP_ENABLED(IPP_LOCAL_IN) &&	\
	(!((flags) & (IP6_NO_IPPOLICY|IP6_IN_NOCKSUM))))

/* Apply IPQoS policies for oubound traffic? */
#define	IP6_OUT_IPP(flags)	\
	(IPP_ENABLED(IPP_LOCAL_OUT) && (!((flags) & IP6_NO_IPPOLICY)))

/* Extracts 8 bit traffic class from IPV6 flow label field */
#ifdef  _BIG_ENDIAN
#define	__IPV6_TCLASS_FROM_FLOW(n)	(((n)>>20) & 0xff)
#else
#define	__IPV6_TCLASS_FROM_FLOW(n)	((((n)<<4) | (((n)>>12) & 0xf)) & 0xff)
#endif /* _BIG_ENDIAN */

typedef	enum {
	IPP_LOCAL_IN =	0x01,
	IPP_LOCAL_OUT =	0x02,
	IPP_FWD_IN =	0x04,
	IPP_FWD_OUT =	0x08
} ip_proc_t;

/* IP private data structure */
typedef	struct ip_priv {
	ip_proc_t	proc;
	uint32_t	ill_index;
} ip_priv_t;

/* The entry point for ip policy processing */
extern void ip_process(ip_proc_t, mblk_t **, uint32_t);
extern void ip_priv_free(void *);
#endif /* _KERNEL */


#ifdef	__cplusplus
}
#endif

#endif	/* _INET_IPP_COMMON_H */
