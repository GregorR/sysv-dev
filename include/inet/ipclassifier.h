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

#ifndef	_INET_IPCLASSIFIER_H
#define	_INET_IPCLASSIFIER_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef	__cplusplus
extern "C" {
#endif

#include <inet/common.h>
#include <inet/ip.h>
#include <inet/mi.h>
#include <inet/tcp.h>
#include <inet/ip6.h>
#include <netinet/in.h>		/* for IPPROTO_* constants */
#include <sys/sdt.h>

typedef void (*edesc_spf)(void *, mblk_t *, void *, int);
typedef void (*edesc_rpf)(void *, mblk_t *, void *);

/*
 * ==============================
 * =	The CONNECTION		=
 * ==============================
 */

/*
 * The connection structure contains the common information/flags/ref needed.
 * Implementation will keep the connection struct, the layers (with their
 * respective data for event i.e. tcp_t if event was tcp_input) all in one
 * contiguous memory location.
 */

/* Conn Flags */
#define	IPCL_BOUND		0x80000000	/* Conn in bind table */
#define	IPCL_CONNECTED		0x40000000	/* Conn in connected table */
#define	IPCL_TCP4		0x08000000	/* A TCP connection */
#define	IPCL_TCP6		0x04000000	/* A TCP6 connection */
#define	IPCL_EAGER		0x01000000	/* Incoming connection */
#define	IPCL_CL_LISTENER	0x00800000	/* Cluster listener */
#define	IPCL_ACCEPTOR		0x00400000	/* Sockfs priv acceptor */
#define	IPCL_SOCKET		0x00200000	/* Sockfs connection */
#define	IPCL_CHECK_POLICY	0x00100000	/* Needs policy checking */
#define	IPCL_FULLY_BOUND	0x00080000	/* Bound to correct squeue */
#define	IPCL_TCPMOD		0x00040000	/* Is tcp module instance */

/* Flags identifying the type of conn */
#define	IPCL_TCPCONN		0x00000001	/* Flag to indicate cache */
#define	IPCL_SCTPCONN		0x00000002
#define	IPCL_IPCCONN		0x00000004
#define	IPCL_ISV6		0x00000008	/* Is a V6 connection */

/* Conn Masks */
#define	IPCL_TCP		(IPCL_TCP4|IPCL_TCP6)
#define	IPCL_REMOVED		0x00000020
#define	IPCL_REUSED		0x00000040

#define	IS_TCP_CONN(connp)	(((connp)->conn_flags & IPCL_TCP) != 0)

#define	IPCL_IS_TCP4(connp)						\
	(((connp)->conn_flags & IPCL_TCP4))

#define	IPCL_IS_TCP4_CONNECTED_NO_POLICY(connp)				\
	(((connp)->conn_flags &						\
		(IPCL_TCP4|IPCL_CONNECTED|IPCL_CHECK_POLICY|IPCL_TCP6))	\
		== (IPCL_TCP4|IPCL_CONNECTED))

#define	IPCL_IS_CONNECTED(connp)					\
	((connp)->conn_flags & IPCL_CONNECTED)

#define	IPCL_IS_BOUND(connp)						\
	((connp)->conn_flags & IPCL_BOUND)

#define	IPCL_IS_TCP4_BOUND(connp)					\
	(((connp)->conn_flags &						\
		(IPCL_TCP4|IPCL_BOUND|IPCL_TCP6)) ==			\
		(IPCL_TCP4|IPCL_BOUND))

#define	IPCL_IS_FULLY_BOUND(connp)					\
	((connp)->conn_flags & IPCL_FULLY_BOUND)

#define	IPCL_IS_TCP(connp)						\
	((connp)->conn_flags & (IPCL_TCP4|IPCL_TCP6))

#define	IPCL_IS_IPTUN(connp)						\
	((connp)->conn_ulp == IPPROTO_ENCAP || \
	(connp)->conn_ulp == IPPROTO_IPV6)

typedef struct connf_s connf_t;
typedef struct
{
	int	ctb_depth;
#define	IP_STACK_DEPTH	15
	pc_t	ctb_stack[IP_STACK_DEPTH];
} conn_trace_t;

struct conn_s {
	kmutex_t	conn_lock;
	uint32_t	conn_ref;		/* Reference counter */
	uint_t		conn_state_flags;	/* IP state flags */
	ire_t		*conn_ire_cache; 	/* outbound ire cache */
	uint32_t	conn_flags;		/* Conn Flags */
	unsigned int
		conn_on_sqp : 1,		/* Conn is being processed */
		conn_dontroute : 1,		/* SO_DONTROUTE state */
		conn_loopback : 1,		/* SO_LOOPBACK state */
		conn_broadcast : 1,		/* SO_BROADCAST state */
		conn_reuseaddr : 1,		/* SO_REUSEADDR state */

		conn_multicast_loop : 1,	/* IP_MULTICAST_LOOP */
		conn_multi_router : 1,		/* Wants all multicast pkts */
		conn_draining : 1,		/* ip_wsrv running */
		conn_did_putbq : 1,		/* ip_wput did a putbq */

		conn_unspec_src : 1,		/* IP_UNSPEC_SRC */
		conn_policy_cached : 1,		/* Is policy cached/latched ? */
		conn_in_enforce_policy : 1,	/* Enforce Policy on inbound */
		conn_out_enforce_policy : 1,	/* Enforce Policy on outbound */

		conn_af_isv6 : 1,		/* ip address family ver 6 */
		conn_pkt_isv6 : 1,		/* ip packet format ver 6 */
		conn_ipv6_recvpktinfo : 1,	/* IPV6_RECVPKTINFO option */
		conn_ipv6_recvhoplimit : 1,	/* IPV6_RECVHOPLIMIT option */

		conn_ipv6_recvhopopts : 1,	/* IPV6_RECVHOPOPTS option */
		conn_ipv6_recvdstopts : 1,	/* IPV6_RECVDSTOPTS option */
		conn_ipv6_recvrthdr : 1,	/* IPV6_RECVRTHDR option */
		conn_ipv6_recvrtdstopts : 1,	/* IPV6_RECVRTHDRDSTOPTS */
		conn_ipv6_v6only : 1,		/* IPV6_V6ONLY */
		conn_ipv6_recvtclass : 1,	/* IPV6_RECVTCLASS */
		conn_ipv6_recvpathmtu : 1,	/* IPV6_RECVPATHMTU */
		conn_pathmtu_valid : 1,		/* The cached mtu is valid. */
		conn_ipv6_dontfrag : 1,		/* IPV6_DONTFRAG */
		/*
		 * This option can take on values in [-1, 1] so we store it
		 * +1.  Manifest constants IPV6_USE_MIN_MTU_* describe these
		 * values.
		 */
		conn_fully_bound : 1,		/* Fully bound connection */
		conn_recvif : 1,		/* IP_RECVIF option */
		conn_recvslla : 1,		/* IP_RECVSLLA option */
		conn_mdt_ok : 1,		/* MDT is permitted */
		pad_to_bit_31 : 2;

	tcp_t		*conn_tcp;		/* Pointer to the tcp struct */
	squeue_t	*conn_sqp;		/* Squeue for processing */
	edesc_rpf	conn_recv;		/* Pointer to recv routine */
	void		*conn_pad1;

	ill_t		*conn_xmit_if_ill;	/* Outbound ill */
	ill_t		*conn_nofailover_ill;	/* Failover ill */
	ipsec_latch_t	*conn_latch;		/* latched state */
	ill_t		*conn_outgoing_ill;	/* IP{,V6}_BOUND_IF */
	edesc_spf	conn_send;		/* Pointer to send routine */
	queue_t		*conn_rq;		/* Read queue */
	queue_t		*conn_wq;		/* Write queue */
	dev_t		conn_dev;		/* Minor number */

	cred_t		*conn_cred;		/* Credentials */
	connf_t		*conn_g_fanout;		/* Global Hash bucket head */
	struct conn_s	*conn_g_next;		/* Global Hash chain next */
	struct conn_s	*conn_g_prev;		/* Global Hash chain prev */
	struct ipsec_policy_head_s *conn_policy; /* Configured policy */
	in6_addr_t	conn_bound_source_v6;
#define	conn_bound_source	V4_PART_OF_V6(conn_bound_source_v6)
	void		*conn_void[1];

	connf_t		*conn_fanout;		/* Hash bucket we're part of */
	struct conn_s	*conn_next;		/* Hash chain next */
	struct conn_s	*conn_prev;		/* Hash chain prev */
	struct {
		in6_addr_t connua_laddr;	/* Local address */
		in6_addr_t connua_faddr;	/* Remote address */
	} connua_v6addr;
#define	conn_src	V4_PART_OF_V6(connua_v6addr.connua_laddr)
#define	conn_rem	V4_PART_OF_V6(connua_v6addr.connua_faddr)
#define	conn_srcv6	connua_v6addr.connua_laddr
#define	conn_remv6	connua_v6addr.connua_faddr
	union {
		/* Used for classifier match performance */
		uint32_t		conn_ports2;
		struct {
			in_port_t	tcpu_fport;	/* Remote port */
			in_port_t	tcpu_lport;	/* Local port */
		} tcpu_ports;
	} u_port;
#define	conn_fport	u_port.tcpu_ports.tcpu_fport
#define	conn_lport	u_port.tcpu_ports.tcpu_lport
#define	conn_ports	u_port.conn_ports2
#define	conn_upq	conn_rq
	uint8_t		conn_ulp;		/* protocol type */
	uint8_t		conn_unused_byte;
	kcondvar_t	conn_cv;

	uint_t		conn_proto;		/* SO_PROTOTYPE state */
	ill_t		*conn_incoming_ill;	/* IP{,V6}_BOUND_IF */
	ill_t		*conn_outgoing_pill;	/* IP{,V6}_BOUND_PIF */
	ill_t		*conn_oper_pending_ill; /* pending shared ioctl */
	ill_t		*conn_xioctl_pending_ill; /* pending excl ioctl */

	/* this is used only when an unbind is in progress.. */
	struct sq_s	*conn_pending_sq; /* waiting for ioctl on sq */
	ilg_t	*conn_ilg;		/* Group memberships */
	int	conn_ilg_allocated;	/* Number allocated */
	int	conn_ilg_inuse;		/* Number currently used */
	int	conn_ilg_walker_cnt;	/* No of ilg walkers */
	/* XXXX get rid of this, once ilg_delete_all is fixed */
	kcondvar_t	conn_refcv;

	struct ipif_s	*conn_multicast_ipif;	/* IP_MULTICAST_IF */
	ill_t		*conn_multicast_ill;	/* IPV6_MULTICAST_IF */
	int		conn_orig_bound_ifindex; /* BOUND_IF before MOVE */
	int		conn_orig_multicast_ifindex;
						/* IPv6 MC IF before MOVE */
	int		conn_orig_xmit_ifindex; /* IP_XMIT_IF before move */
	struct conn_s 	*conn_drain_next;	/* Next conn in drain list */
	struct conn_s	*conn_drain_prev;	/* Prev conn in drain list */
	idl_t		*conn_idl;		/* Ptr to the drain list head */
	mblk_t		*conn_ipsec_opt_mp;	/* ipsec option mblk */
	uint32_t	conn_src_preferences;	/* prefs for src addr select */
	/* mtuinfo from IPV6_PACKET_TOO_BIG conditional on conn_pathmtu_valid */
	struct ip6_mtuinfo mtuinfo;
	zoneid_t	conn_zoneid;		/* zone connection is in */
#ifdef CONN_DEBUG
#define	CONN_TRACE_MAX	10
	int		conn_trace_last;	/* ndx of last used tracebuf */
	conn_trace_t	conn_trace_buf[CONN_TRACE_MAX];
#endif
};

/*
 * connf_t - connection fanout data.
 *
 * The hash tables and their linkage (conn_t.{hashnextp, hashprevp} are
 * protected by the per-bucket lock. Each conn_t inserted in the list
 * points back at the connf_t that heads the bucket.
 */

struct connf_s {
	struct conn_s	*connf_head;
	kmutex_t	connf_lock;
};

#define	CONN_INC_REF(connp)	{				\
	DTRACE_PROBE1(conn__inc__ref, conn_t *, connp);		\
	mutex_enter(&(connp)->conn_lock);			\
	ASSERT(conn_trace_ref(connp));				\
	(connp)->conn_ref++;					\
	ASSERT((connp)->conn_ref != 0);				\
	mutex_exit(&(connp)->conn_lock);			\
}

#define	CONN_INC_REF_LOCKED(connp)	{			\
	DTRACE_PROBE1(conn__inc__ref, conn_t *, connp);		\
	ASSERT(MUTEX_HELD(&(connp)->conn_lock));	 	\
	ASSERT(conn_trace_ref(connp));				\
	(connp)->conn_ref++;					\
	ASSERT((connp)->conn_ref != 0);				\
}

#define	CONN_DEC_REF(connp)	{					\
	DTRACE_PROBE1(conn__dec__ref, conn_t *, connp);			\
	mutex_enter(&(connp)->conn_lock);				\
	if ((connp)->conn_ref <= 0)					\
		cmn_err(CE_PANIC, "CONN_DEC_REF: connp(%p) has ref "	\
			"= %d\n", (void *)(connp), (connp)->conn_ref);	\
	ASSERT(conn_untrace_ref(connp));				\
	(connp)->conn_ref--;						\
	if ((connp)->conn_ref == 0) {					\
		/* Refcnt can't increase again, safe to drop lock */	\
		mutex_exit(&(connp)->conn_lock);			\
		ipcl_conn_destroy(connp);				\
	} else {							\
		cv_broadcast(&(connp)->conn_cv);			\
		mutex_exit(&(connp)->conn_lock);			\
	}								\
}

#define	_IPCL_V4_MATCH(v6addr, v4addr)	\
	(V4_PART_OF_V6((v6addr)) == (v4addr) && IN6_IS_ADDR_V4MAPPED(&(v6addr)))

#define	_IPCL_V4_MATCH_ANY(addr)	\
	(IN6_IS_ADDR_V4MAPPED_ANY(&(addr)) || IN6_IS_ADDR_UNSPECIFIED(&(addr)))

/*
 * IPCL_PROTO_MATCH() only matches conns with the specified zoneid, while
 * IPCL_PROTO_MATCH_V6() can match other conns in the multicast case, see
 * ip_fanout_proto().
 */
#define	IPCL_PROTO_MATCH(connp, protocol, ipha, ill,			\
    fanout_flags, zoneid)						\
	((((connp)->conn_src == INADDR_ANY) ||				\
	(((connp)->conn_src == ((ipha)->ipha_dst)) &&			\
	(((connp)->conn_rem == INADDR_ANY) ||				\
	((connp)->conn_rem == ((ipha)->ipha_src))))) &&			\
	((connp)->conn_zoneid == (zoneid)) &&				\
	(conn_wantpacket((connp), (ill), (ipha),			\
	(fanout_flags), (zoneid)) || ((protocol) == IPPROTO_PIM) ||	\
	((protocol) == IPPROTO_RSVP)))

#define	IPCL_PROTO_MATCH_V6(connp, protocol, ip6h, ill,			   \
    fanout_flags, zoneid)						   \
	((IN6_IS_ADDR_UNSPECIFIED(&(connp)->conn_srcv6) ||		   \
	(IN6_ARE_ADDR_EQUAL(&(connp)->conn_srcv6, &((ip6h)->ip6_dst)) &&   \
	(IN6_IS_ADDR_UNSPECIFIED(&(connp)->conn_remv6) ||		   \
	IN6_ARE_ADDR_EQUAL(&(connp)->conn_remv6, &((ip6h)->ip6_src))))) && \
	(conn_wantpacket_v6((connp), (ill), (ip6h),			   \
	(fanout_flags), (zoneid)) || ((protocol) == IPPROTO_RSVP)))

#define	IPCL_CONN_HASH(src, ports)					\
	((unsigned)(ntohl((src)) ^ ((ports) >> 24) ^ ((ports) >> 16) ^	\
	((ports) >> 8) ^ (ports)) % ipcl_conn_fanout_size)

#define	IPCL_CONN_HASH_V6(src, ports)			\
	IPCL_CONN_HASH(V4_PART_OF_V6((src)), (ports))

#define	IPCL_CONN_MATCH(connp, proto, src, dst, ports)			\
	((connp)->conn_ulp == (proto) &&				\
		(connp)->conn_ports == (ports) &&      			\
		_IPCL_V4_MATCH((connp)->conn_remv6, (src)) &&		\
		_IPCL_V4_MATCH((connp)->conn_srcv6, (dst)) &&		\
		!(connp)->conn_ipv6_v6only)

#define	IPCL_CONN_MATCH_V6(connp, proto, src, dst, ports)		\
	((connp)->conn_ulp == (proto) &&				\
		(connp)->conn_ports == (ports) &&      			\
		IN6_ARE_ADDR_EQUAL(&(connp)->conn_remv6, &(src)) &&	\
		IN6_ARE_ADDR_EQUAL(&(connp)->conn_srcv6, &(dst)))

#define	IPCL_CONN_INIT(connp, protocol, src, rem, ports) {		\
	(connp)->conn_ulp = protocol;					\
	IN6_IPADDR_TO_V4MAPPED(src, &(connp)->conn_srcv6);		\
	IN6_IPADDR_TO_V4MAPPED(rem, &(connp)->conn_remv6);		\
	(connp)->conn_ports = ports;					\
}

#define	IPCL_CONN_INIT_V6(connp, protocol, src, rem, ports) {		\
	(connp)->conn_ulp = protocol;					\
	(connp)->conn_srcv6 = src;					\
	(connp)->conn_remv6 = rem;					\
	(connp)->conn_ports = ports;					\
}

#define	IPCL_BIND_HASH(lport)						\
	((unsigned)(((lport) >> 8) ^ (lport)) % ipcl_bind_fanout_size)

#define	IPCL_BIND_MATCH(connp, proto, laddr, lport)			\
	((connp)->conn_ulp == (proto) &&				\
		(connp)->conn_lport == (lport) &&			\
		(_IPCL_V4_MATCH_ANY((connp)->conn_srcv6) ||		\
		_IPCL_V4_MATCH((connp)->conn_srcv6, (laddr))) &&	\
		!(connp)->conn_ipv6_v6only)

#define	IPCL_BIND_MATCH_V6(connp, proto, laddr, lport)			\
	((connp)->conn_ulp == (proto) &&				\
		(connp)->conn_lport == (lport) &&			\
		(IN6_ARE_ADDR_EQUAL(&(connp)->conn_srcv6, &(laddr)) ||	\
		IN6_IS_ADDR_UNSPECIFIED(&(connp)->conn_srcv6)))

#define	IPCL_UDP_MATCH(connp, lport, laddr, fport, faddr)		\
	(((connp)->conn_lport == (lport)) &&				\
	((_IPCL_V4_MATCH_ANY((connp)->conn_srcv6) ||			\
	(_IPCL_V4_MATCH((connp)->conn_srcv6, (laddr)) &&		\
	(_IPCL_V4_MATCH_ANY((connp)->conn_remv6) ||			\
	(_IPCL_V4_MATCH((connp)->conn_remv6, (faddr)) &&		\
	(connp)->conn_fport == (fport)))))) &&				\
	!(connp)->conn_ipv6_v6only)

#define	IPCL_UDP_MATCH_V6(connp, lport, laddr, fport, faddr)	\
	(((connp)->conn_lport == (lport)) &&			\
	(IN6_IS_ADDR_UNSPECIFIED(&(connp)->conn_srcv6) ||	\
	(IN6_ARE_ADDR_EQUAL(&(connp)->conn_srcv6, &(laddr)) &&	\
	(IN6_IS_ADDR_UNSPECIFIED(&(connp)->conn_remv6) ||	\
	(IN6_ARE_ADDR_EQUAL(&(connp)->conn_remv6, &(faddr)) &&	\
	(connp)->conn_fport == (fport))))))

#define	IPCL_TCP_EAGER_INIT(connp, protocol, src, rem, ports) {		\
	(connp)->conn_flags |= (IPCL_TCP4|IPCL_EAGER);			\
	(connp)->conn_ulp = protocol;					\
	IN6_IPADDR_TO_V4MAPPED(src, &(connp)->conn_srcv6);		\
	IN6_IPADDR_TO_V4MAPPED(rem, &(connp)->conn_remv6);		\
	(connp)->conn_ports = ports;					\
	(connp)->conn_send = ip_output;					\
	(connp)->conn_sqp = IP_SQUEUE_GET(lbolt);			\
}

#define	IPCL_TCP_EAGER_INIT_V6(connp, protocol, src, rem, ports) {	\
	(connp)->conn_flags |= (IPCL_TCP6|IPCL_EAGER);			\
	(connp)->conn_ulp = protocol;					\
	(connp)->conn_srcv6 = src;					\
	(connp)->conn_remv6 = rem;					\
	(connp)->conn_ports = ports;					\
	(connp)->conn_send = ip_output_v6;				\
	(connp)->conn_sqp = IP_SQUEUE_GET(lbolt);			\
}

#define	ipcl_proto_search(protocol)					\
	(ipcl_proto_fanout[(protocol)].connf_head)

#ifdef _BIG_ENDIAN
#define	IPCL_UDP_HASH(port)		((port) & 0xFF)
#else   /* _BIG_ENDIAN */
#define	IPCL_UDP_HASH(port)		(((uint16_t)(port)) >> 8)
#endif  /* _BIG_ENDIAN */

#define	CONN_G_HASH_SIZE	1024

/* Raw socket hash function. */
#define	IPCL_RAW_HASH(lport) (((lport) * 31) & (ipcl_raw_fanout_size - 1))

/* hash tables */
extern connf_t	rts_clients;
extern connf_t	*ipcl_conn_fanout;
extern connf_t	*ipcl_bind_fanout;
extern connf_t	ipcl_proto_fanout[IPPROTO_MAX + 1];
extern connf_t	ipcl_proto_fanout_v6[IPPROTO_MAX + 1];
extern connf_t	*ipcl_udp_fanout;
extern connf_t	*ipcl_globalhash_fanout;
extern connf_t	*ipcl_raw_fanout;
extern uint_t	ipcl_conn_fanout_size;
extern uint_t	ipcl_bind_fanout_size;
extern uint_t	ipcl_udp_fanout_size;
extern uint_t	ipcl_raw_fanout_size;

/* Function prototypes */
extern void ipcl_init(void);
extern void ipcl_destroy(void);
extern conn_t *ipcl_conn_create(uint32_t, int);
extern void ipcl_conn_destroy(conn_t *);

void ipcl_hash_insert_connected(connf_t *, conn_t *);
void ipcl_hash_insert_bound(connf_t *, conn_t *);
void ipcl_hash_insert_wildcard(connf_t *, conn_t *);
void ipcl_hash_remove(conn_t *);
void ipcl_hash_remove_locked(conn_t *connp, connf_t *connfp);

extern int	ipcl_bind_insert(conn_t *, uint8_t, ipaddr_t, uint16_t);
extern int	ipcl_bind_insert_v6(conn_t *, uint8_t, const in6_addr_t *,
		    uint16_t);
extern int	ipcl_conn_insert(conn_t *, uint8_t, ipaddr_t, ipaddr_t,
		    uint32_t);
extern int	ipcl_conn_insert_v6(conn_t *, uint8_t, const in6_addr_t *,
		    const in6_addr_t *, uint32_t, uint_t);

void ipcl_proto_insert(conn_t *, uint8_t);
void ipcl_proto_insert_v6(conn_t *, uint8_t);
conn_t *ipcl_classify_v4(mblk_t *, uint8_t, uint_t, zoneid_t);
conn_t *ipcl_classify_v6(mblk_t *, uint8_t, uint_t, zoneid_t);
conn_t *ipcl_classify(mblk_t *, zoneid_t);
conn_t *ipcl_classify_raw(uint8_t, zoneid_t, uint32_t, ipha_t *);
void	ipcl_globalhash_insert(conn_t *);
void	ipcl_globalhash_remove(conn_t *);
void	ipcl_walk(pfv_t, void *);
conn_t	*ipcl_tcp_lookup_reversed_ipv4(ipha_t *, tcph_t *, int);
conn_t	*ipcl_tcp_lookup_reversed_ipv6(ip6_t *, tcpha_t *, int, uint_t);
conn_t	*ipcl_lookup_listener_v4(uint16_t, ipaddr_t, zoneid_t);
conn_t	*ipcl_lookup_listener_v6(uint16_t, in6_addr_t *, uint_t, zoneid_t);
int	conn_trace_ref(conn_t *);
int	conn_untrace_ref(conn_t *);
conn_t *ipcl_conn_tcp_lookup_reversed_ipv4(conn_t *, ipha_t *, tcph_t *);
conn_t *ipcl_conn_tcp_lookup_reversed_ipv6(conn_t *, ip6_t *, tcph_t *);
#ifdef	__cplusplus
}
#endif

#endif	/* _INET_IPCLASSIFIER_H */
