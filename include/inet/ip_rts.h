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
 * Copyright 1992-2003 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef	_INET_IP_RTS_H
#define	_INET_IP_RTS_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef _KERNEL
extern	void	ip_rts_change(int, ipaddr_t, ipaddr_t,
    ipaddr_t, ipaddr_t, ipaddr_t, int, int,
    int);

extern	void	ip_rts_change_v6(int, const in6_addr_t *, const in6_addr_t *,
    const in6_addr_t *, const in6_addr_t *, const in6_addr_t *, int, int, int);

extern	void	ip_rts_ifmsg(ipif_t *);

extern	void	ip_rts_newaddrmsg(int, int, ipif_t *);

extern	int	ip_rts_request(queue_t *, mblk_t *, cred_t *);

extern	void	ip_rts_rtmsg(int, ire_t *, int);

extern	mblk_t	*rts_alloc_msg(int, int, sa_family_t);

extern	size_t	rts_data_msg_size(int, sa_family_t);

extern	void	rts_fill_msg_v6(int, int, const in6_addr_t *,
    const in6_addr_t *, const in6_addr_t *, const in6_addr_t *,
    const in6_addr_t *, const in6_addr_t *, ipif_t *, mblk_t *);

extern	size_t	rts_header_msg_size(int);

extern	void	rts_queue_input(mblk_t *, queue_t *, sa_family_t);
#endif /* _KERNEL */

#ifdef	__cplusplus
}
#endif

#endif	/* _INET_IP_RTS_H */
