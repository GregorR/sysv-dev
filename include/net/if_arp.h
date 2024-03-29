/*
 * Copyright 1997-2003 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#ifndef	_NET_IF_ARP_H
#define	_NET_IF_ARP_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"
/* if_arp.h 1.5 88/08/19 SMI; from UCB 7.1 1/24/86	*/

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * Address Resolution Protocol.
 *
 * See RFC 826 for protocol description.  ARP packets are variable
 * in size; the arphdr structure defines the fixed-length portion.
 * Protocol type values are the same as those for 10 Mb/s Ethernet.
 * It is followed by the variable-sized fields ar_sha, arp_spa,
 * arp_tha and arp_tpa in that order, according to the lengths
 * specified.  Field names used correspond to RFC 826.
 */
struct	arphdr {
	ushort_t ar_hrd;	/* format of hardware address */
#define	ARPHRD_ETHER 	1	/* ethernet hardware address */
#define	ARPHRD_IB 	32	/* IPoIB hardware address */
	ushort_t ar_pro;	/* format of protocol address */
	uchar_t	ar_hln;		/* length of hardware address */
	uchar_t	ar_pln;		/* length of protocol address */
	ushort_t ar_op;		/* one of: */
#define	ARPOP_REQUEST	1	/* request to resolve address */
#define	ARPOP_REPLY	2	/* response to previous request */
#define	REVARP_REQUEST	3	/* Reverse ARP request */
#define	REVARP_REPLY	4	/* Reverse ARP reply */
	/*
	 * The remaining fields are variable in size,
	 * according to the sizes above, and are defined
	 * as appropriate for specific hardware/protocol
	 * combinations.  (E.g., see <netinet/if_ether.h>.)
	 */
#ifdef	notdef
	uchar_t	ar_sha[];	/* sender hardware address */
	uchar_t	ar_spa[];	/* sender protocol address */
	uchar_t	ar_tha[];	/* target hardware address */
	uchar_t	ar_tpa[];	/* target protocol address */
#endif	/* notdef */
};

/*
 * Extended ARP ioctl request
 */
struct xarpreq {
	struct	sockaddr_storage xarp_pa;	/* protocol address */
	struct	sockaddr_dl	 xarp_ha;	/* hardware address */
	int	xarp_flags;			/* flags */
};

/*
 * BSD ARP ioctl request
 */
struct arpreq {
	struct	sockaddr arp_pa;		/* protocol address */
	struct	sockaddr arp_ha;		/* hardware address */
	int	arp_flags;			/* flags */
};
/*  arp_flags and at_flags field values */
#define	ATF_INUSE	0x01	/* entry in use */
#define	ATF_COM		0x02	/* completed entry (enaddr valid) */
#define	ATF_PERM	0x04	/* permanent entry */
#define	ATF_PUBL	0x08	/* publish entry (respond for other host) */
#define	ATF_USETRAILERS	0x10	/* has requested trailers */

/*
 * This data structure is used by kernel protocol modules to register
 * their interest in a particular packet type with the Ethernet drivers.
 * For example, other kinds of ARP would use this, XNS, ApleTalk, etc.
 */
struct ether_family {
	int		ef_family;	/* address family */
	ushort_t	ef_ethertype;	/* ethernet type field */
	struct ifqueue *(*ef_infunc)();	/* input function */
	int		(*ef_outfunc)();	/* output function */
	int		(*ef_netisr)();	/* soft interrupt function */
	struct ether_family *ef_next;	/* link to next on list */
};

#ifdef	__cplusplus
}
#endif

#endif	/* _NET_IF_ARP_H */
