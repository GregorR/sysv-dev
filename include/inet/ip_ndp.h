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
 * Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef	_INET_IP_NDP_H
#define	_INET_IP_NDP_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

/*
 * Internal definitions for the kernel implementation of the IPv6
 * Neighbor Discovery Protocol (NDP).
 */

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef _KERNEL
/*
 * ndp_g_lock protects neighbor cache tables access and
 * insertion/removal of cache entries into/from these tables.
 * nce_lock protects nce_pcnt, nce_rcnt, nce_qd_mp nce_state,
 * nce_res_mp, nce_refcnt and nce_last.
 * nce_refcnt is incremented for every ire pointing to this nce and
 * every time ndp_lookup() finds an nce.
 * Should there be a need to obtain nce_lock and ndp_g_lock, ndp_g_lock is
 * acquired first.
 * To avoid becoming exclusive when deleting NCEs, ndp_walk() routine holds
 * the ndp_g_lock (i.e global lock) and marks NCEs to be deleted with
 * NCE_F_CONDEMNED.  When all active users of such NCEs are gone the walk
 * routine passes a list for deletion to nce_ire_delete_list().
 */
kmutex_t	ndp_g_lock; /* Lock protecting nighbor cache hash table */
/* NDP Cache Entry */
typedef struct nce_s {
	struct	nce_s	*nce_next;	/* Hash chain next pointer */
	struct	nce_s	**nce_ptpn;	/* Pointer to previous next */
	struct 	ill_s	*nce_ill;	/* Associated ill */
	uint16_t	nce_flags;	/* See below */
	uint16_t	nce_state;	/* See reachability states in if.h */
	int16_t		nce_pcnt;	/* Probe counter */
	uint16_t	nce_rcnt;	/* Retransmit counter */
	in6_addr_t	nce_addr;	/* address of the nighbor */
	in6_addr_t	nce_mask;	/* If not all ones, mask allows an */
	    /* entry  to respond to requests for a group of addresses, for */
	    /* instantance multicast addresses				   */
	in6_addr_t	nce_extract_mask; /* For mappings */
	uint32_t	nce_ll_extract_start;	/* For mappings */
#define	nce_first_mp_to_free	nce_fp_mp
	mblk_t		*nce_fp_mp;	/* link layer fast path mp */
	mblk_t		*nce_res_mp;	/* DL_UNITDATA_REQ or link layer mp */
	mblk_t		*nce_qd_mp;	/* Head outgoing queued packets */
#define	nce_last_mp_to_free	nce_qd_mp
	mblk_t		*nce_timer_mp;	/* NDP timer mblk */
	mblk_t		*nce_mp;	/* mblk we are in, last to be freed */
	uint64_t	nce_last;	/* Time last reachable in msec */
	uint32_t	nce_refcnt;	/* nce active usage count */
	kmutex_t	nce_lock;	/* See comments on top for what */
					/* this field protects */
	int		nce_unsolicit_count; /* Unsolicited Adv count */
	struct nce_s	*nce_fastpath;	/* for fastpath list */
	timeout_id_t	nce_timeout_id;
#ifdef NCE_DEBUG
	th_trace_t	*nce_trace[IP_TR_HASH_MAX];
	boolean_t	nce_trace_disable;	/* True when alloc fails */
#endif
} nce_t;

/* nce_flags  */
#define	NCE_F_PERMANENT		0x1
#define	NCE_F_MAPPING		0x2
#define	NCE_F_ISROUTER		0x4
#define	NCE_F_PROXY		0x8
#define	NCE_F_NONUD		0x10
#define	NCE_F_ANYCAST		0x20
#define	NCE_F_CONDEMNED		0x40
#define	NCE_F_UNSOL_ADV		0x80

#define	NCE_EXTERNAL_FLAGS_MASK \
	(NCE_F_PERMANENT | NCE_F_MAPPING | NCE_F_ISROUTER | NCE_F_NONUD | \
	NCE_F_ANYCAST | NCE_F_UNSOL_ADV)

/* State REACHABLE, STALE, DELAY or PROBE */
#define	NCE_ISREACHABLE(nce)			\
	(((((nce)->nce_state) >= ND_REACHABLE) &&	\
	((nce)->nce_state) <= ND_PROBE))

/* NDP flags set in SOL/ADV requests */
#define	NDP_UNICAST		0x1
#define	NDP_ISROUTER		0x2
#define	NDP_SOLICITED		0x4
#define	NDP_ORIDE		0x8

/* Number of packets queued in NDP for a neighbor */
#define	ND_MAX_Q		4


#ifdef NCE_DEBUG
#define	NCE_TRACE_REF(nce)		nce_trace_ref(nce)
#define	NCE_UNTRACE_REF(nce)		nce_untrace_ref(nce)
#else
#define	NCE_TRACE_REF(nce)
#define	NCE_UNTRACE_REF(nce)
#endif

#define	NCE_REFHOLD(nce) {		\
	mutex_enter(&(nce)->nce_lock);	\
	(nce)->nce_refcnt++;		\
	ASSERT((nce)->nce_refcnt != 0);	\
	NCE_TRACE_REF(nce);		\
	mutex_exit(&(nce)->nce_lock);	\
}

#define	NCE_REFHOLD_NOTR(nce) {		\
	mutex_enter(&(nce)->nce_lock);	\
	(nce)->nce_refcnt++;		\
	ASSERT((nce)->nce_refcnt != 0);	\
	mutex_exit(&(nce)->nce_lock);	\
}

#define	NCE_REFHOLD_LOCKED(nce) {		\
	ASSERT(MUTEX_HELD(&(nce)->nce_lock));	\
	(nce)->nce_refcnt++;			\
	NCE_TRACE_REF(nce);			\
}

/* nce_inactive destroys the mutex thus no mutex_exit is needed */
#define	NCE_REFRELE(nce) {		\
	mutex_enter(&(nce)->nce_lock);	\
	NCE_UNTRACE_REF(nce);		\
	ASSERT((nce)->nce_refcnt != 0);	\
	if (--(nce)->nce_refcnt == 0)	\
		ndp_inactive(nce);	\
	else {				\
		mutex_exit(&(nce)->nce_lock);\
	}				\
}

#define	NCE_REFRELE_NOTR(nce) {		\
	mutex_enter(&(nce)->nce_lock);	\
	ASSERT((nce)->nce_refcnt != 0);	\
	if (--(nce)->nce_refcnt == 0)	\
		ndp_inactive(nce);	\
	else {				\
		mutex_exit(&(nce)->nce_lock);\
	}				\
}

#define	NDP_RESTART_TIMER(nce, ms) {	\
	ASSERT(!MUTEX_HELD(&(nce)->nce_lock));				\
	if ((nce)->nce_timeout_id != 0) {				\
		/* Ok to untimeout bad id. we don't hold a lock. */	\
		(void) untimeout((nce)->nce_timeout_id);		\
	}								\
	mutex_enter(&(nce)->nce_lock);					\
	/* Don't start the timer if the nce has been deleted */		\
	if (!((nce)->nce_flags & NCE_F_CONDEMNED)) 			\
		nce->nce_timeout_id = timeout(ndp_timer, nce, 		\
		    MSEC_TO_TICK(ms) == 0 ? 1 : MSEC_TO_TICK(ms));	\
	mutex_exit(&(nce)->nce_lock);					\
}

/* Structure for ndp_cache_count() */
typedef struct {
	int	ncc_total;	/* Total number of NCEs */
	int	ncc_host;	/* NCE entries without R bit set */
} ncc_cache_count_t;

/*
 * Structure of ndp_cache_reclaim().  Each field is a fraction i.e. 1 means
 * reclaim all, N means reclaim 1/Nth of all entries, 0 means reclaim none.
 */
typedef struct {
	int	ncr_host;	/* Fraction for host entries */
} nce_cache_reclaim_t;

/* When SAP is greater than zero address appears before SAP */
#define	NCE_LL_ADDR_OFFSET(ill)	(((ill)->ill_sap_length) < 0 ? \
	(sizeof (dl_unitdata_req_t)) : \
	((sizeof (dl_unitdata_req_t)) + (ABS((ill)->ill_sap_length))))

#define	NCE_LL_SAP_OFFSET(ill) (((ill)->ill_sap_length) < 0 ? \
	((sizeof (dl_unitdata_req_t)) + ((ill)->ill_phys_addr_length)) : \
	(sizeof (dl_unitdata_req_t)))

#ifdef _BIG_ENDIAN
#define	NCE_LL_SAP_COPY(ill, mp) \
	{ \
	size_t abs_sap_len = ABS((ill)->ill_sap_length); \
	if (abs_sap_len > 0) { \
		ASSERT(abs_sap_len <= sizeof (uint32_t)); \
		ASSERT((mp)->b_rptr + NCE_LL_SAP_OFFSET(ill) + \
		    abs_sap_len <= ((mp)->b_wptr)); \
		bcopy((uint8_t *)&(ill)->ill_sap + sizeof (ill->ill_sap) - \
		    abs_sap_len, \
		    ((mp)->b_rptr + NCE_LL_SAP_OFFSET(ill)), \
		    abs_sap_len); \
	} \
	}
#else
#define	NCE_LL_SAP_COPY(ill, mp) \
	{ \
	size_t abs_sap_len = ABS((ill)->ill_sap_length); \
	if (abs_sap_len > 0) { \
		uint32_t abs_sap_len = ABS((ill)->ill_sap_length); \
		ASSERT(abs_sap_len <= sizeof (uint32_t)); \
		ASSERT((mp)->b_rptr + NCE_LL_SAP_OFFSET(ill) + \
		    abs_sap_len <= ((mp)->b_wptr)); \
		bcopy(&((ill)->ill_sap), \
		((mp)->b_rptr + NCE_LL_SAP_OFFSET(ill)), \
		abs_sap_len); \
	} \
	}
#endif

/*
 * Exclusive-or the 6 bytes that are likely to contain the MAC
 * address. Assumes table_size does not exceed 256.
 * Assumes EUI-64 format for good hashing.
 */
#define	NCE_ADDR_HASH_V6(addr, table_size)				\
	(((addr).s6_addr8[8] ^ (addr).s6_addr8[9] ^			\
	(addr).s6_addr8[10] ^ (addr).s6_addr8[13] ^			\
	(addr).s6_addr8[14] ^ (addr).s6_addr8[15]) % (table_size))

extern	void	ndp_cache_count(nce_t *, char *);
extern	void	ndp_cache_reclaim(nce_t *, char *);
extern	void	ndp_delete(nce_t *);
extern	void	ndp_delete_per_ill(nce_t *, uchar_t *);
extern	void	ndp_fastpath_flush(nce_t *, char  *);
extern	boolean_t ndp_fastpath_update(nce_t *, void  *);
extern	nd_opt_hdr_t *ndp_get_option(nd_opt_hdr_t *, int, int);
extern	void	ndp_inactive(nce_t *);
extern	void	ndp_input(ill_t *, mblk_t *);
extern	nce_t	*ndp_lookup(ill_t *, const in6_addr_t *, boolean_t);
extern	int	ndp_lookup_then_add(ill_t *, uchar_t *, const in6_addr_t *,
    const in6_addr_t *, const in6_addr_t *, uint32_t, uint16_t,
    uint16_t, nce_t **);
extern	int	ndp_mcastreq(ill_t *, const in6_addr_t *, uint32_t, uint32_t,
    mblk_t *);
extern	int	ndp_noresolver(ill_t *, const in6_addr_t *);
extern	void	ndp_process(nce_t *, uchar_t *, uint32_t, boolean_t);
extern	int	ndp_query(ill_t *, lif_nd_req_t *);
extern	int	ndp_report(queue_t *, mblk_t *, caddr_t, cred_t *);
extern	int	ndp_resolver(ill_t *, const in6_addr_t *, mblk_t *, zoneid_t);
extern	int	ndp_sioc_update(ill_t *, lif_nd_req_t *);
extern	boolean_t	ndp_verify_optlen(nd_opt_hdr_t *, int);
extern	void	ndp_timer(void *);
extern	void	ndp_walk(ill_t *, pfi_t, uchar_t *);
extern	void	ndp_walk_impl(ill_t *, pfi_t, uchar_t *, boolean_t);
extern	int	ndp_add(ill_t *, uchar_t *, const in6_addr_t *,
		    const in6_addr_t *, const in6_addr_t *,
		    uint32_t, uint16_t, uint16_t, nce_t **);
extern	void	nce_resolv_failed(nce_t *);
extern	void	nce_fastpath_list_add(nce_t *);
extern	void	nce_fastpath_list_delete(nce_t *);
extern	void	nce_fastpath_list_dispatch(ill_t *,
    boolean_t (*)(nce_t *, void  *), void *);
#ifdef NCE_DEBUG
extern	void	nce_trace_inactive(nce_t *);
extern	void	nce_trace_ref(nce_t *);
extern	void	nce_untrace_ref(nce_t *);
extern	int	nce_thread_exit(nce_t *, caddr_t);
#endif

#endif	/* _KERNEL */

#ifdef	__cplusplus
}
#endif

#endif	/* _INET_IP_NDP_H */
