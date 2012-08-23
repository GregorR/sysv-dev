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

#ifndef	_INET_IP_IRE_H
#define	_INET_IP_IRE_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef	__cplusplus
extern "C" {
#endif

#define	IPV6_LL_PREFIXLEN	10	/* Number of bits in link-local pref */

#define	IP_FTABLE_HASH_SIZE	32	/* size of each hash table in ptrs */
#define	IP_CACHE_TABLE_SIZE	256
#define	IP_MRTUN_TABLE_SIZE	256	/* Mobile IP reverse tunnel table */
					/* size. Only used by mipagent */
#define	IP_SRCIF_TABLE_SIZE	256	/* Per interface routing table size */
#define	IP_MASK_TABLE_SIZE	(IP_ABITS + 1)		/* 33 ptrs */

#define	IP6_FTABLE_HASH_SIZE	32	/* size of each hash table in ptrs */
#define	IP6_CACHE_TABLE_SIZE	256
#define	IP6_MASK_TABLE_SIZE	(IPV6_ABITS + 1)	/* 129 ptrs */

/*
 * We use the common modulo hash function.  In ip_ire_init(), we make
 * sure that the cache table size is always a power of 2.  That's why
 * we can use & instead of %.  Also note that we try hard to make sure
 * the lower bits of an address capture most info from the whole address.
 * The reason being that since our hash table is probably a lot smaller
 * than 2^32 buckets so the lower bits are the most important.
 */
#define	IRE_ADDR_HASH(addr, table_size) \
	(((addr) ^ ((addr) >> 8) ^ ((addr) >> 16) ^ ((addr) >> 24)) &	\
	((table_size) - 1))

/*
 * Exclusive-or those bytes that are likely to contain the MAC
 * address.  Assumes EUI-64 format for good hashing.
 */
#define	IRE_ADDR_HASH_V6(addr, table_size) 				\
	(((addr).s6_addr32[3] ^						\
	(((addr).s6_addr32[3] ^ (addr).s6_addr32[2]) >> 12)) &		\
	((table_size) - 1))
/* This assumes that the ftable size is a power of 2. */
#define	IRE_ADDR_MASK_HASH_V6(addr, mask, table_size) 			\
	((((addr).s6_addr8[8] & (mask).s6_addr8[8]) ^ 			\
	((addr).s6_addr8[9] & (mask).s6_addr8[9]) ^			\
	((addr).s6_addr8[10] & (mask).s6_addr8[10]) ^ 			\
	((addr).s6_addr8[13] & (mask).s6_addr8[13]) ^ 			\
	((addr).s6_addr8[14] & (mask).s6_addr8[14]) ^ 			\
	((addr).s6_addr8[15] & (mask).s6_addr8[15])) & ((table_size) - 1))

/*
 * match parameter definitions for
 * IRE lookup routines.
 */
#define	MATCH_IRE_DSTONLY	0x0000	/* Match just the address */
#define	MATCH_IRE_TYPE		0x0001	/* Match IRE type */
#define	MATCH_IRE_SRC		0x0002	/* Match IRE source address */
#define	MATCH_IRE_MASK		0x0004	/* Match IRE mask */
#define	MATCH_IRE_WQ		0x0008	/* Match IRE Write Q */
#define	MATCH_IRE_GW		0x0010	/* Match IRE gateway */
#define	MATCH_IRE_IPIF		0x0020	/* Match IRE ipif */
#define	MATCH_IRE_RECURSIVE	0x0040	/* Do recursive lookup if necessary */
#define	MATCH_IRE_DEFAULT	0x0080	/* Return default route if no route */
					/* found. */
#define	MATCH_IRE_RJ_BHOLE	0x0100	/* During lookup if we hit an ire */
					/* with RTF_REJECT or RTF_BLACKHOLE, */
					/* return the ire. No recursive */
					/* lookup should be done. */
#define	MATCH_IRE_IHANDLE	0x0200	/* Match IRE on ihandle */
#define	MATCH_IRE_MARK_HIDDEN	0x0400	/* Match IRE ire_marks with */
					/* IRE_MARK_HIDDEN. */
/*
 * MATCH_IRE_ILL is used whenever we want to specifically match an IRE
 * whose ire_ipif->ipif_ill or (ill_t *)ire_stq->q_ptr matches a given
 * ill. When MATCH_IRE_ILL is used to locate an IRE_CACHE, it implies
 * that the packet will not be load balanced. This is normally used
 * by in.mpathd to send out failure detection probes.
 *
 * MATCH_IRE_ILL_GROUP is used whenever we are not specific about which
 * interface (ill) the packet should be sent out. This implies that the
 * packets will be subjected to load balancing and it might go out on
 * any interface in the group. When there is only interface in the group,
 * MATCH_IRE_ILL_GROUP becomes MATCH_IRE_ILL. Most of the code uses
 * MATCH_IRE_ILL_GROUP and MATCH_IRE_ILL is used in very few cases where
 * we want to disable load balancing.
 *
 * MATCH_IRE_PARENT is used whenever we unconditionally want to get the
 * parent IRE (sire) while recursively searching IREs for an offsubnet
 * destination. With this flag, even if no IRE_CACHETABLE or IRE_INTERFACE
 * is found to help resolving IRE_OFFSUBNET in lookup routines, the
 * IRE_OFFSUBNET sire, if any, is returned to the caller.
 */
#define	MATCH_IRE_ILL_GROUP	0x0800	/* Match IRE on ill or the ill_group. */
#define	MATCH_IRE_ILL		0x1000	/* Match IRE on the ill only */

#define	MATCH_IRE_PARENT	0x2000	/* Match parent ire, if any, */
					/* even if ire is not matched. */
#define	MATCH_IRE_ZONEONLY	0x4000	/* Match IREs in specified zone, ie */
					/* don't match IRE_LOCALs from other */
					/* zones or shared IREs */

/* Structure for ire_cache_count() */
typedef struct {
	int	icc_total;	/* Total number of IRE_CACHE */
	int	icc_unused;	/* # off/no PMTU unused since last reclaim */
	int	icc_offlink;	/* # offlink without PMTU information */
	int	icc_pmtu;	/* # offlink with PMTU information */
	int	icc_onlink;	/* # onlink */
} ire_cache_count_t;

/*
 * Structure for ire_cache_reclaim(). Each field is a fraction i.e. 1 meaning
 * reclaim all, N meaning reclaim 1/Nth of all entries, 0 meaning reclaim none.
 */
typedef struct {
	int	icr_unused;	/* Fraction for unused since last reclaim */
	int	icr_offlink;	/* Fraction for offlink without PMTU info */
	int	icr_pmtu;	/* Fraction for offlink with PMTU info */
	int	icr_onlink;	/* Fraction for onlink */
} ire_cache_reclaim_t;

typedef struct {
	uint64_t ire_stats_alloced;	/* # of ires alloced */
	uint64_t ire_stats_freed;	/* # of ires freed */
	uint64_t ire_stats_inserted;	/* # of ires inserted in the bucket */
	uint64_t ire_stats_deleted;	/* # of ires deleted from the bucket */
} ire_stats_t;

extern ire_stats_t ire_stats_v4;

/*
 * We use atomics so that we get an accurate accounting on the ires.
 * Otherwise we can't determine leaks correctly.
 */
#define	BUMP_IRE_STATS(ire_stats, x) atomic_add_64(&(ire_stats).x, 1)

extern irb_t *ip_forwarding_table_v6[];
extern irb_t *ip_cache_table_v6;
extern irb_t *ip_mrtun_table;
extern irb_t *ip_srcif_table;
extern kmutex_t ire_ft_init_lock;
extern kmutex_t	ire_mrtun_lock;
extern kmutex_t ire_srcif_table_lock;
extern ire_stats_t ire_stats_v6;
extern uint_t	ire_mrtun_count;
extern uint_t ire_srcif_table_count;

#ifdef _KERNEL
extern	ipaddr_t	ip_plen_to_mask(uint_t);
extern	in6_addr_t	*ip_plen_to_mask_v6(uint_t, in6_addr_t *);

extern	int	ip_ire_advise(queue_t *, mblk_t *, cred_t *);
extern	int	ip_ire_delete(queue_t *, mblk_t *, cred_t *);
extern	boolean_t ip_ire_clookup_and_delete(ipaddr_t, ipif_t *);
extern	void	ip_ire_clookup_and_delete_v6(const in6_addr_t *);

extern	int	ip_ire_report(queue_t *, mblk_t *, caddr_t, cred_t *);
extern	int	ip_ire_report_mrtun(queue_t *, mblk_t *, caddr_t, cred_t *);
extern	int	ip_ire_report_srcif(queue_t *, mblk_t *, caddr_t, cred_t *);
extern	int	ip_ire_report_v6(queue_t *, mblk_t *, caddr_t, cred_t *);

extern	void	ip_ire_req(queue_t *, mblk_t *);

extern	int	ip_mask_to_plen(ipaddr_t);
extern	int	ip_mask_to_plen_v6(const in6_addr_t *);

extern	ire_t	*ipif_to_ire(ipif_t *);
extern	ire_t	*ipif_to_ire_v6(ipif_t *);

extern	int	ire_add(ire_t **, queue_t *, mblk_t *, ipsq_func_t);
extern	int	ire_add_mrtun(ire_t **, queue_t *, mblk_t *, ipsq_func_t);
extern	void	ire_add_then_send(queue_t *, ire_t *, mblk_t *);
extern	int	ire_add_v6(ire_t **, queue_t *, mblk_t *, ipsq_func_t);
extern	int	ire_atomic_start(irb_t *irb_ptr, ire_t *ire, queue_t *q,
    mblk_t *mp, ipsq_func_t func);
extern	void	ire_atomic_end(irb_t *irb_ptr, ire_t *ire);

extern	void	ire_cache_count(ire_t *, char *);
extern	ire_t	*ire_cache_lookup(ipaddr_t, zoneid_t);
extern	ire_t	*ire_cache_lookup_v6(const in6_addr_t *, zoneid_t);
extern	void	ire_cache_reclaim(ire_t *, char *);

extern	void	ire_check_bcast_present(ipif_t *, ipaddr_t, int, boolean_t *,
    boolean_t *);
extern	ire_t	*ire_create_mp(uchar_t *, uchar_t *, uchar_t *, uchar_t *,
    uchar_t *, uint_t, mblk_t *, queue_t *, queue_t *, ushort_t, mblk_t *,
    ipif_t *, ill_t *, ipaddr_t, uint32_t, uint32_t, uint32_t, const iulp_t *);

extern	ire_t	*ire_create(uchar_t *, uchar_t *, uchar_t *, uchar_t *,
    uchar_t *, uint_t *, mblk_t *, queue_t *, queue_t *, ushort_t, mblk_t *,
    ipif_t *, ill_t *, ipaddr_t, uint32_t, uint32_t, uint32_t, const iulp_t *);

extern	ire_t	**ire_check_and_create_bcast(ipif_t *, ipaddr_t,
    ire_t **, int);
extern	ire_t	**ire_create_bcast(ipif_t *, ipaddr_t, ire_t **);
extern	ire_t	*ire_init(ire_t *, uchar_t *, uchar_t *, uchar_t *,
    uchar_t *, uchar_t *, uint_t *, mblk_t *, queue_t *, queue_t *, ushort_t,
    mblk_t *, ipif_t *, ill_t *, ipaddr_t, uint32_t, uint32_t, uint32_t,
    const iulp_t *);

extern	void	ire_init_common(ire_t *, uint_t *, mblk_t *, queue_t *,
    queue_t *, ushort_t, mblk_t *, ipif_t *, ill_t *, uint32_t,
    uint32_t, uint32_t, uchar_t, const iulp_t *);

extern	ire_t	*ire_create_v6(const in6_addr_t *, const in6_addr_t *,
    const in6_addr_t *, const in6_addr_t *, uint_t *, mblk_t *, queue_t *,
    queue_t *, ushort_t, mblk_t *, ipif_t *,
    const in6_addr_t *, uint32_t, uint32_t, uint_t, const iulp_t *);

extern	ire_t	*ire_create_mp_v6(const in6_addr_t *, const in6_addr_t *,
    const in6_addr_t *, const in6_addr_t *, mblk_t *, queue_t *,
    queue_t *, ushort_t, mblk_t *, ipif_t *,
    const in6_addr_t *, uint32_t, uint32_t, uint_t, const iulp_t *);

extern	ire_t	*ire_init_v6(ire_t *, const in6_addr_t *, const in6_addr_t *,
    const in6_addr_t *, const in6_addr_t *, uint_t *, mblk_t *, queue_t *,
    queue_t *, ushort_t, mblk_t *, ipif_t *,
    const in6_addr_t *, uint32_t, uint32_t, uint_t, const iulp_t *);

extern	ire_t	*ire_ctable_lookup(ipaddr_t, ipaddr_t, int, ipif_t *,
    zoneid_t, int);

extern	ire_t	*ire_ctable_lookup_v6(const in6_addr_t *, const in6_addr_t *,
    int, ipif_t *, zoneid_t, int);

extern	void	ire_delete(ire_t *);
extern	void	ire_delete_cache_gw(ire_t *, char *);
extern	void	ire_delete_cache_gw_v6(ire_t *, char *);
extern	void	ire_delete_cache_v6(ire_t *, char *);
extern	void	ire_delete_srcif(ire_t *);
extern	void	ire_delete_v6(ire_t *);

extern	void	ire_expire(ire_t *, char *);
extern	void	ire_fastpath_flush(ire_t *, void *);
extern	boolean_t ire_fastpath_update(ire_t *, void *);

extern	void	ire_flush_cache_v4(ire_t *, int);
extern	void	ire_flush_cache_v6(ire_t *, int);

extern	ire_t	*ire_ftable_lookup(ipaddr_t, ipaddr_t, ipaddr_t, int, ipif_t *,
    ire_t **, zoneid_t, uint32_t, int);

extern	ire_t	*ire_ftable_lookup_v6(const in6_addr_t *, const in6_addr_t *,
    const in6_addr_t *, int, ipif_t *, ire_t **, zoneid_t, uint32_t, int);

extern	ire_t	*ire_ihandle_lookup_onlink(ire_t *);
extern	ire_t	*ire_ihandle_lookup_offlink(ire_t *, ire_t *);
extern	ire_t	*ire_ihandle_lookup_offlink_v6(ire_t *, ire_t *);

extern	ire_t 	*ire_lookup_local(zoneid_t);
extern	ire_t 	*ire_lookup_local_v6(zoneid_t);

extern  ire_t	*ire_lookup_multi(ipaddr_t, zoneid_t);
extern  ire_t	*ire_lookup_multi_v6(const in6_addr_t *, zoneid_t);

extern ire_t	*ire_mrtun_lookup(ipaddr_t, ill_t *);

extern	void	ire_refrele(ire_t *);
extern	void	ire_refrele_notr(ire_t *);
extern	ire_t	*ire_route_lookup(ipaddr_t, ipaddr_t, ipaddr_t, int, ipif_t *,
    ire_t **, zoneid_t, int);

extern	ire_t	*ire_route_lookup_v6(const in6_addr_t *, const in6_addr_t *,
    const in6_addr_t *, int, ipif_t *, ire_t **, zoneid_t, int);

extern	ire_t	*ire_srcif_table_lookup(ipaddr_t, int, ipif_t *, ill_t *, int);
extern ill_t	*ire_to_ill(ire_t *);

extern	void	ire_walk(pfv_t, char *);
extern	void	ire_walk_ill(uint_t, uint_t, pfv_t, char *, ill_t *);
extern	void	ire_walk_ill_mrtun(uint_t, uint_t, pfv_t, void *, ill_t *);
extern	void	ire_walk_ill_v4(uint_t, uint_t, pfv_t, char *, ill_t *);
extern	void	ire_walk_ill_v6(uint_t, uint_t, pfv_t, char *, ill_t *);
extern	void	ire_walk_v4(pfv_t, char *, zoneid_t);
extern	void	ire_walk_srcif_table_v4(pfv_t, char *);
extern	void	ire_walk_v6(pfv_t, char *, zoneid_t);

extern boolean_t	ire_multirt_lookup(ire_t **, ire_t **, uint32_t);
extern boolean_t	ire_multirt_need_resolve(ipaddr_t);
extern boolean_t	ire_multirt_lookup_v6(ire_t **, ire_t **, uint32_t);
extern boolean_t	ire_multirt_need_resolve_v6(const in6_addr_t *);

extern ire_t	*ipif_lookup_multi_ire(ipif_t *, ipaddr_t);
extern ire_t	*ipif_lookup_multi_ire_v6(ipif_t *, const in6_addr_t *);

extern void	ire_fastpath_list_dispatch(ill_t *,
    boolean_t (*)(ire_t *, void *), void *);
extern void	ire_fastpath_list_delete(ill_t *, ire_t *);

extern mblk_t *ip_nexthop_route(const struct sockaddr *, char *);
extern mblk_t *ip_nexthop(const struct sockaddr *, const char *);

extern ire_t	*ire_get_next_bcast_ire(ire_t *, ire_t *);
extern ire_t	*ire_get_next_default_ire(ire_t *, ire_t *);

#endif /* _KERNEL */

#ifdef	__cplusplus
}
#endif

#endif	/* _INET_IP_IRE_H */
