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
 * Copyright 2004 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */
/* Copyright (c) 1990 Mentat Inc. */

#ifndef	_RAWIP_IMPL_H
#define	_RAWIP_IMPL_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef _KERNEL

#include <sys/types.h>

#include <netinet/in.h>
#include <netinet/icmp6.h>
#include <netinet/ip6.h>

#include <inet/common.h>
#include <inet/ip.h>

/* Internal icmp control structure, one per open stream */
typedef	struct icmp_s {
	uint_t		icmp_state;	/* TPI state */
	in6_addr_t	icmp_v6src;	/* Source address of this stream */
	in6_addr_t	icmp_bound_v6src; /* Explicitely bound to address */
	in6_addr_t 	icmp_v6dst;	/* Connected destination */
	uint32_t	icmp_flowinfo;	/* Connected flow id and tclass */
	uint32_t 	icmp_max_hdr_len; /* For write offset in stream head */
	sa_family_t	icmp_family;	/* Family from socket() call */
	/*
	 * IP format that packets transmitted from this struct should use.
	 * Value can be IP4_VERSION or IPV6_VERSION.
	 */
	uchar_t	icmp_ipversion;

	uint_t		icmp_proto;
	uint_t		icmp_ip_snd_options_len; /* Len of IPv4 options */
	uint8_t		*icmp_ip_snd_options;	/* Ptr to IPv4 options */
	uint8_t		icmp_multicast_ttl;	/* IP*_MULTICAST_TTL/HOPS */
	ipaddr_t	icmp_multicast_if_addr; /* IP_MULTICAST_IF option */
	uint_t		icmp_multicast_if_index; /* IPV6_MULTICAST_IF option */
	int		icmp_bound_if;		/* IP*_BOUND_IF option */
	int		icmp_xmit_if;		/* IP_XMIT_IF option */

	uint_t
	    icmp_debug : 1,		/* SO_DEBUG "socket" option. */
	    icmp_dontroute : 1,		/* SO_DONTROUTE "socket" option. */
	    icmp_broadcast : 1,		/* SO_BROADCAST "socket" option. */
	    icmp_reuseaddr : 1,		/* SO_REUSEADDR "socket" option. */

	    icmp_useloopback : 1,	/* SO_USELOOPBACK "socket" option. */
	    icmp_multicast_loop : 1,	/* IP_MULTICAST_LOOP option */
	    icmp_hdrincl : 1,		/* IP_HDRINCL option + RAW and IGMP */
	    icmp_dgram_errind : 1,	/* SO_DGRAM_ERRIND option */

	    icmp_discon_pending : 1,	/* T_DISCON_REQ in progress */
	    icmp_unspec_source : 1,	/* IP*_UNSPEC_SRC option */
	    icmp_raw_checksum : 1,	/* raw checksum per IPV6_CHECKSUM */
	    icmp_no_tp_cksum : 1,	/* icmp_proto is UDP or TCP */

	    icmp_ipv6_recvpktinfo : 1,	/* IPV6_RECVPKTINFO option  */
	    icmp_ipv6_recvhoplimit : 1,	/* IPV6_RECVHOPLIMIT option */
	    icmp_ipv6_recvhopopts : 1,	/* IPV6_RECVHOPOPTS option */
	    icmp_ipv6_recvdstopts : 1,	/* IPV6_RECVDSTOPTS option */

	    icmp_ipv6_recvrthdr : 1,	/* IPV6_RECVRTHDR option */
	    icmp_ipv6_recvpathmtu : 1,	/* IPV6_RECVPATHMTU option */
	    icmp_recvif:1,		/* IP_RECVIF for raw sockets option */
	    icmp_ipv6_recvtclass : 1,	/* IPV6_RECVTCLASS option */

	    icmp_restricted : 1,	/* opened by non-privileged user */
	    icmp_ipv6_recvrtdstopts : 1, /* Obsolete IPV6_RECVRTHDRDSTOPTS */
	    icmp_old_ipv6_recvdstopts : 1, /* Old ver of IPV6_RECVDSTOPTS */

	    icmp_pad_to_bit_31: 9;

	uint8_t		icmp_type_of_service;
	uint8_t		icmp_ttl;		/* TTL or hoplimit */
	uint32_t	icmp_checksum_off; /* user supplied checksum offset */
	icmp6_filter_t	*icmp_filter;		/* ICMP6_FILTER option */
	cred_t		*icmp_credp;		/* Opener's credentials */

	ip6_pkt_t	icmp_sticky_ipp;	/* Sticky options */
	uint8_t		*icmp_sticky_hdrs;	/* Prebuilt IPv6 hdrs */
	uint_t		icmp_sticky_hdrs_len;	/* Incl. ip6h and any ip6i */
	zoneid_t	icmp_zoneid;		/* ID of owning zone */
} icmp_t;

#endif	/* _KERNEL */

#ifdef	__cplusplus
}
#endif

#endif	/* _RAWIP_IMPL_H */
