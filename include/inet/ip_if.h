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
/* Copyright (c) 1990 Mentat Inc. */

#ifndef	_INET_IP_IF_H
#define	_INET_IP_IF_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef	__cplusplus
extern "C" {
#endif

#define	PREFIX_INFINITY	0xffffffffUL
#define	IP_MAX_HW_LEN	40

#define	IP_LOOPBACK_MTU	(8*1024)

/* DLPI SAPs are in host byte order for all systems */
#define	IP_DL_SAP	0x0800
#define	IP6_DL_SAP	0x86dd

#ifdef	_KERNEL
/*
 * Interface flags actually represent the state/properties of 3 different
 * abstractions of interfaces in IP. Interface flags are set using
 * SIOCS[L]IFFLAGS ioctl. The three abstractions are :
 *
 * 1) Physical interface (phyint) : There is one phyint allocated common
 *    to both IPv4 and IPv6 physical interface instance.
 *
 * 2) Physical interface instance (ill) : This encompasses all the state
 *    that is common across all IP addresses assigned to a physical
 *    interface but different between the IPv4 and IPv6 instance.
 *
 * 3) Logical interface (ipif) : This has state about a single IP address.
 *
 * Values for the various states are derived from the same name space
 * as applications querying the state using SIOCGIFFLAGS/SIOCGLIFFLAGS
 * see only one state returned in lifr_flags which is a union of all
 * the above states/properties. Thus deriving the values from the common
 * name space makes implementation easier. All these values are stored in
 * uint64_t and any other structure/code using these flags should use
 * uint64_ts.
 *
 * As we maintain the interface flags in 3 different flags namely
 * phyint_flags, ill_flags, ipif_flags we define the following flag values
 * to be used within the kernel to reduce potential errors. The ones
 * starting with PHYI_ are supposed to be used with phyint_flags, the ones
 * starting with ILLF_ are supposed to be used with ill_flags and the ones
 * starting with IPIF_ are supposed to be used with ipif_flags. If you see
 * any code with a mismatch i.e phyint_flags & IPIF_UP - it is wrong. Only
 * PHYI_XXX can be used with phyint_flags.
 *
 * NOTE : For EVERY FLAG in if.h, there should be a corresponding value
 * defined HERE and this is the one that should be USED within IP. We
 * use IFF_ flags within IP only when we examine lifr_flags.
 */
#define	IFF_PHYINT_FLAGS	(IFF_LOOPBACK|IFF_RUNNING|IFF_PROMISC| \
    IFF_ALLMULTI|IFF_INTELLIGENT|IFF_MULTI_BCAST|IFF_FAILED|IFF_STANDBY| \
    IFF_INACTIVE|IFF_OFFLINE|IFF_VIRTUAL)

#define	IFF_PHYINTINST_FLAGS	(IFF_DEBUG|IFF_NOTRAILERS|IFF_NOARP| \
    IFF_MULTICAST|IFF_ROUTER|IFF_NONUD|IFF_NORTEXCH|IFF_IPV4|IFF_IPV6| \
    IFF_XRESOLV|IFF_COS_ENABLED)

#define	IFF_LOGINT_FLAGS	(IFF_UP|IFF_BROADCAST|IFF_POINTOPOINT| \
    IFF_UNNUMBERED|IFF_DHCPRUNNING|IFF_PRIVATE|IFF_NOXMIT|IFF_NOLOCAL| \
    IFF_DEPRECATED|IFF_ADDRCONF|IFF_ANYCAST|IFF_MIPRUNNING|IFF_NOFAILOVER| \
    IFF_PREFERRED|IFF_TEMPORARY|IFF_FIXEDMTU)

#define	IPIF_REPL_CHECK(to_ipif, failback_cmd)				\
	(((to_ipif)->ipif_replace_zero) || ((failback_cmd) &&		\
	!(to_ipif)->ipif_isv6 && !((to_ipif)->ipif_flags & IPIF_UP) &&	\
	(to_ipif)->ipif_lcl_addr == INADDR_ANY))

#define	PHYI_LOOPBACK		IFF_LOOPBACK	/* is a loopback net */
#define	PHYI_RUNNING		IFF_RUNNING	/* resources allocated */
#define	PHYI_PROMISC		IFF_PROMISC	/* receive all packets */
#define	PHYI_ALLMULTI		IFF_ALLMULTI	/* receive all multi packets */
#define	PHYI_INTELLIGENT	IFF_INTELLIGENT	/* protocol code on board */
#define	PHYI_MULTI_BCAST	IFF_MULTI_BCAST	/* multicast using broadcast */
#define	PHYI_FAILED		IFF_FAILED	/* NIC has failed */
#define	PHYI_STANDBY		IFF_STANDBY	/* Standby NIC  */
#define	PHYI_INACTIVE		IFF_INACTIVE	/* Standby active or not ? */
#define	PHYI_OFFLINE		IFF_OFFLINE	/* NIC has been offlined */
#define	PHYI_VIRTUAL		IFF_VIRTUAL	/* Will not send or recv pkts */

#define	ILLF_DEBUG		IFF_DEBUG	/* turn on debugging */
#define	ILLF_NOTRAILERS		IFF_NOTRAILERS	/* avoid use of trailers */
#define	ILLF_NOARP		IFF_NOARP	/* no ARP for this interface */
#define	ILLF_MULTICAST		IFF_MULTICAST	/* supports multicast */
#define	ILLF_ROUTER		IFF_ROUTER	/* router on this interface */
#define	ILLF_NONUD		IFF_NONUD	/* No NUD on this interface */
#define	ILLF_NORTEXCH		IFF_NORTEXCH	/* No routing info exchange */
#define	ILLF_IPV4		IFF_IPV4	/* IPv4 interface */
#define	ILLF_IPV6		IFF_IPV6	/* IPv6 interface */
#define	ILLF_XRESOLV		IFF_XRESOLV	/* IPv6 external resolver */
#define	ILLF_COS_ENABLED	IFF_COS_ENABLED	/* Is CoS marking supported */

#define	IPIF_UP			IFF_UP		/* interface is up */
#define	IPIF_BROADCAST		IFF_BROADCAST	/* broadcast address valid */
#define	IPIF_POINTOPOINT	IFF_POINTOPOINT	/* point-to-point link */
#define	IPIF_UNNUMBERED		IFF_UNNUMBERED	/* non-unique address */
#define	IPIF_DHCPRUNNING	IFF_DHCPRUNNING	/* DHCP controlled interface */
#define	IPIF_PRIVATE		IFF_PRIVATE	/* do not advertise */
#define	IPIF_NOXMIT		IFF_NOXMIT	/* Do not transmit packets */
#define	IPIF_NOLOCAL		IFF_NOLOCAL	/* Just on-link subnet */
#define	IPIF_DEPRECATED		IFF_DEPRECATED	/* address deprecated */
#define	IPIF_ADDRCONF		IFF_ADDRCONF	/* stateless addrconf */
#define	IPIF_ANYCAST		IFF_ANYCAST	/* Anycast address */
#define	IPIF_MIPRUNNING		IFF_MIPRUNNING	/* Mobip Controlled interface */
#define	IPIF_NOFAILOVER		IFF_NOFAILOVER	/* No failover on NIC failure */
#define	IPIF_PREFERRED		IFF_PREFERRED	/* Prefer as source address */
#define	IPIF_TEMPORARY		IFF_TEMPORARY	/* RFC3041 */
#define	IPIF_FIXEDMTU		IFF_FIXEDMTU	/* set with SIOCSLIFMTU */

extern	ill_t	*illgrp_scheduler(ill_t *);
extern	mblk_t	*ill_arp_alloc(ill_t *, uchar_t *, caddr_t);
extern	void	ill_dlpi_done(ill_t *, t_uscalar_t);
extern	void	ill_dlpi_send(ill_t *, mblk_t *);
extern	mblk_t	*ill_dlur_gen(uchar_t *, uint_t, t_uscalar_t, t_scalar_t);
extern  ill_t	*ill_lookup_on_ifindex(uint_t, boolean_t, queue_t *, mblk_t *,
    ipsq_func_t, int *);
extern	ill_t	*ill_lookup_on_name(char *, boolean_t,
    boolean_t, queue_t *, mblk_t *, ipsq_func_t, int *, boolean_t *);
extern	void	ill_ipif_cache_delete(ire_t *, char *);
extern	void	ill_delete(ill_t *);
extern	void	ill_delete_tail(ill_t *);
extern	int	ill_dl_phys(ill_t *, ipif_t *, mblk_t *, queue_t *);
extern	int	ill_dls_info(struct sockaddr_dl *, ipif_t *);
extern	void	ill_fastpath_ack(ill_t *, mblk_t *);
extern	void	ill_fastpath_nack(ill_t *);
extern	int	ill_fastpath_probe(ill_t *, mblk_t *);
extern	void	ill_frag_prune(ill_t *, uint_t);
extern	void	ill_frag_free_pkts(ill_t *, ipfb_t *, ipf_t *, int);
extern	time_t	ill_frag_timeout(ill_t *, time_t);
extern	int	ill_init(queue_t *, ill_t *);
extern	int	ill_nominate_mcast_rcv(ill_group_t *);
extern	boolean_t	ill_setdefaulttoken(ill_t *);

extern void	ill_lock_ills(ill_t **, int);
extern mblk_t	*ill_pending_mp_get(ill_t *, conn_t **, uint_t);
extern boolean_t ill_pending_mp_add(ill_t *, conn_t *, mblk_t *);
extern	boolean_t ill_is_quiescent(ill_t *ill);
extern	void	ill_unlock_ills(ill_t **, int);
extern	void	ill_lock_ills(ill_t **, int);
extern	void	ill_refhold(ill_t *);
extern	void	ill_refhold_locked(ill_t *);
extern	int	ill_check_and_refhold(ill_t *);
extern	void	ill_refrele(ill_t *);
extern	boolean_t ill_waiter_inc(ill_t *);
extern	void	ill_waiter_dcr(ill_t *);
extern	void	ill_trace_ref(ill_t *);
extern	void	ill_untrace_ref(ill_t *);
extern	boolean_t ill_down_start(queue_t *, mblk_t *);
extern	ill_t	*ill_lookup_group_v6(const in6_addr_t *, zoneid_t);
extern	void	ill_capability_ack(ill_t *, mblk_t *);
extern	void	ill_capability_probe(ill_t *);
extern	void	ill_capability_reset(ill_t *);
extern	void	ill_mtu_change(ire_t *, char *);
extern void	ill_group_cleanup(ill_t *);
extern int	ill_up_ipifs(ill_t *, queue_t *, mblk_t *);
extern	boolean_t ill_is_probeonly(ill_t *);

extern	char	*ipif_get_name(ipif_t *, char *, int);
extern	void	ipif_init(void);
extern	ipif_t	*ipif_lookup_addr(ipaddr_t, ill_t *, zoneid_t, queue_t *,
    mblk_t *, ipsq_func_t, int *);
extern	ipif_t	*ipif_lookup_addr_v6(const in6_addr_t *, ill_t *, zoneid_t,
    queue_t *, mblk_t *, ipsq_func_t, int *);
extern	ipif_t	*ipif_lookup_group(ipaddr_t, zoneid_t);
extern	ipif_t	*ipif_lookup_group_v6(const in6_addr_t *, zoneid_t);
extern  ipif_t	*ipif_lookup_interface(ipaddr_t, ipaddr_t,
    queue_t *, mblk_t *, ipsq_func_t, int *);
extern	ipif_t	*ipif_lookup_remote(ill_t *, ipaddr_t, zoneid_t);
extern	ipif_t	*ipif_lookup_onlink_addr(ipaddr_t, zoneid_t);
extern	ipif_t	*ipif_lookup_seqid(ill_t *, uint_t);
extern	boolean_t	ipif_lookup_zoneid(ill_t *, zoneid_t, int,
    ipif_t **);
extern	boolean_t	ipif_lookup_zoneid_group(ill_t *, zoneid_t, int,
    ipif_t **);
extern	ipif_t	*ipif_select_source(ill_t *, ipaddr_t, zoneid_t);
extern	boolean_t	ipif_usesrc_avail(ill_t *, zoneid_t);
extern	void	ipif_refhold(ipif_t *);
extern	void	ipif_refhold_locked(ipif_t *);
extern	void		ipif_refrele(ipif_t *);
extern	boolean_t	ipif_ire_active(ipif_t *);
extern	void	ipif_all_down_tail(ipsq_t *, queue_t *, mblk_t *, void *);
extern	int	ipif_resolver_up(ipif_t *, boolean_t);
extern	int	ipif_arp_setup_multicast(ipif_t *, mblk_t **);
extern	int	ipif_down(ipif_t *, queue_t *, mblk_t *);
extern	void	ipif_multicast_up(ipif_t *);
extern	void	ipif_ndp_down(ipif_t *);
extern	int	ipif_ndp_up(ipif_t *, const in6_addr_t *, boolean_t);
extern	int	ipif_ndp_setup_multicast(ipif_t *, struct nce_s **);
extern	int	ipif_up_done(ipif_t *);
extern	int	ipif_up_done_v6(ipif_t *);
extern	void	ipif_update_other_ipifs_v6(ipif_t *, ill_group_t *);
extern	void	ipif_recreate_interface_routes_v6(ipif_t *, ipif_t *);
extern	void	ill_update_source_selection(ill_t *);
extern	ipif_t	*ipif_select_source_v6(ill_t *, const in6_addr_t *, boolean_t,
    uint32_t, zoneid_t);
extern	boolean_t	ipif_cant_setlinklocal(ipif_t *);
extern	int	ipif_setlinklocal(ipif_t *);
extern	void	ipif_set_tun_llink(ill_t *, struct iftun_req *);
extern	ipif_t	*ipif_lookup_on_ifindex(uint_t, boolean_t, zoneid_t, queue_t *,
    mblk_t *, ipsq_func_t, int *);
extern	ipif_t	*ipif_get_next_ipif(ipif_t *curr, ill_t *ill);
extern	void	ipif_ill_refrele_tail(ill_t *ill);
extern	void	ipif_arp_down(ipif_t *ipif);

extern	int	illgrp_insert(ill_group_t **, ill_t *, char *, ill_group_t *,
    boolean_t);

extern	void	ipsq_enq(ipsq_t *, queue_t *, mblk_t *, ipsq_func_t, int,
    ill_t *);
extern	boolean_t ipsq_enter(ill_t *, boolean_t);
extern	ipsq_t	*ipsq_try_enter(ipif_t *, ill_t *, queue_t *, mblk_t *,
    ipsq_func_t, int, boolean_t);
extern	void	ipsq_exit(ipsq_t *, boolean_t, boolean_t);
extern mblk_t	*ipsq_pending_mp_get(ipsq_t *, conn_t **);
extern boolean_t ipsq_pending_mp_add(conn_t *, ipif_t *, queue_t *,
    mblk_t *, int);
extern	void qwriter_ip(ipif_t *, ill_t *, queue_t *, mblk_t *, ipsq_func_t,
    int, boolean_t);

extern	int	ip_extract_lifreq_cmn(queue_t *, mblk_t *, int, int,
    cmd_info_t *, ipsq_func_t);
extern  int	ip_extract_tunreq(queue_t *, mblk_t *, ipif_t **, ipsq_func_t);
extern	int	ip_addr_availability_check(ipif_t *new_ipif);
extern	int	ip_ill_report(queue_t *, mblk_t *, caddr_t, cred_t *);
extern	int	ip_ipif_report(queue_t *, mblk_t *, caddr_t, cred_t *);
extern	void	ip_ll_subnet_defaults(ill_t *, mblk_t *);
extern	int	ip_rt_add(ipaddr_t, ipaddr_t, ipaddr_t, ipaddr_t, int,
    ipif_t *, ipif_t *, ire_t **, boolean_t, queue_t *, mblk_t *, ipsq_func_t);
extern	int	ip_mrtun_rt_add(ipaddr_t, int, ipif_t *, ipif_t *, ire_t **,
    queue_t *, mblk_t *, ipsq_func_t);
extern	int	ip_rt_add_v6(const in6_addr_t *, const in6_addr_t *,
    const in6_addr_t *, const in6_addr_t *, int, ipif_t *, ire_t **,
    queue_t *, mblk_t *, ipsq_func_t);
extern	int	ip_rt_delete(ipaddr_t, ipaddr_t, ipaddr_t, uint_t, int,
    ipif_t *, ipif_t *, boolean_t, queue_t *, mblk_t *, ipsq_func_t);
extern	int	ip_mrtun_rt_delete(ipaddr_t, ipif_t *);

extern	int	ip_rt_delete_v6(const in6_addr_t *, const in6_addr_t *,
    const in6_addr_t *, uint_t, int, ipif_t *, queue_t *, mblk_t *,
    ipsq_func_t);
extern int ip_siocdelndp_v6(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_siocqueryndp_v6(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_siocsetndp_v6(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_siocaddrt(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_siocdelrt(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_addr(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_addr_restart(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_get_addr(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_dstaddr(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_dstaddr_restart(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_get_dstaddr(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_flags(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_flags_restart(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_get_flags(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_mtu(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_get_mtu(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_get_ifconf(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_get_lifconf(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_get_ifnum(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_get_lifnum(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_token(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_get_token(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int if_unitsel(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int if_unitsel_restart(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_sifname(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_slifname(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_slifname_restart(ipif_t *, sin_t *, queue_t *,
    mblk_t *, ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_slifindex(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_get_lifindex(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_brdaddr(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_get_brdaddr(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_get_muxid(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_muxid(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_netmask(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_get_netmask(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_netmask_restart(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_subnet(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_subnet_restart(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_get_subnet(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_lnkinfo(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_get_lnkinfo(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_metric(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_get_metric(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_arp(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_xarp(ipif_t *, sin_t *, queue_t *,
    mblk_t *, ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_addif(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_removeif(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_removeif_restart(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_tonlink(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_tmysite(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_tmyaddr(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_tunparam(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_groupname(ipif_t *, sin_t *, queue_t *,
    mblk_t *, ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_get_groupname(ipif_t *, sin_t *, queue_t *,
    mblk_t *, ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_slifoindex(ipif_t *, sin_t *, queue_t *,
    mblk_t *, ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_get_oindex(ipif_t *, sin_t *, queue_t *,
    mblk_t *, ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_get_lifzone(ipif_t *, sin_t *, queue_t *,
    mblk_t *, ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_slifzone(ipif_t *, sin_t *, queue_t *,
    mblk_t *, ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_slifzone_restart(ipif_t *, sin_t *, queue_t *,
    mblk_t *, ip_ioctl_cmd_t *, void *);

extern int ip_sioctl_get_lifusesrc(ipif_t *, sin_t *, queue_t *,
    mblk_t *, ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_slifusesrc(ipif_t *, sin_t *, queue_t *,
    mblk_t *, ip_ioctl_cmd_t *, void *);
extern int ip_sioctl_get_lifsrcof(ipif_t *, sin_t *, queue_t *,
    mblk_t *, ip_ioctl_cmd_t *, void *);

extern	void	ip_sioctl_copyin_resume(ipsq_t *, queue_t *, mblk_t *, void *);
extern	void	ip_sioctl_copyin_setup(queue_t *, mblk_t *);
extern	void	ip_sioctl_iocack(queue_t *, mblk_t *);
extern	ip_ioctl_cmd_t *ip_sioctl_lookup(int);
extern int ip_sioctl_move(ipif_t *, sin_t *, queue_t *, mblk_t *,
    ip_ioctl_cmd_t *, void *);

extern	void	conn_delete_ire(conn_t *, caddr_t);

#endif /* _KERNEL */

#ifdef	__cplusplus
}
#endif

#endif	/* _INET_IP_IF_H */
