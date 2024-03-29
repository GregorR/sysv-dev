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

#ifndef	_INET_SCTP_ITF_H
#define	_INET_SCTP_ITF_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Kernel SCTP programming interface.  Note that this interface
 * is private to Sun and can be changed without notice.
 */

#ifdef _KERNEL

/*
 * The version number of the SCTP kernel interface.  Use it with
 * sctp_itf_ver() to verify if the kernel supports the correct
 * version of the interface.
 *
 * NOTE: do not assume backward compatibility of the interface.
 * If the return value of sctp_itf_ver() is different from what
 * is expected, do not call any of the routines.
 */
#define	SCTP_ITF_VER	1

/*
 * This struct holds all the upcalls the SCTP kernel module will
 * invoke for different events.  When calling sctp_create() to create
 * a SCTP handle, the caller must provide this information.
 */
typedef struct sctp_upcalls_s {
	void *	(*su_newconn)(void *parenthandle, void *connind);
	void	(*su_connected)(void *handle);
	int	(*su_disconnected)(void *handle, int error);
	void	(*su_disconnecting)(void *handle);
	int	(*su_recv)(void *handle, mblk_t *mp, int flags);
	void	(*su_xmitted)(void *handle, int txqueued);
	void	(*su_properties)(void *handle, int wroff, size_t maxblk);
} sctp_upcalls_t;

/*
 * This struct holds various flow control limits the caller of
 * sctp_create() should observe when interacting with SCTP.
 */
typedef struct sctp_sockbuf_limits_s {
	int sbl_rxbuf;
	int sbl_rxlowat;
	int sbl_txbuf;
	int sbl_txlowat;
} sctp_sockbuf_limits_t;

/*
 * Parameter to SCTP_UC_SWAP setsockopt
 */
struct sctp_uc_swap {
	void		*sus_handle;
	sctp_upcalls_t	*sus_upcalls;
};

struct sctp_s;

/*
 * The list of routines the SCTP kernel module provides.
 */
extern mblk_t *sctp_alloc_hdr(const char *name, int namelen,
    const char *control, int controllen, int flags);
extern int sctp_bind(struct sctp_s *conn, struct sockaddr *addr,
    socklen_t addrlen);
extern int sctp_bindx(struct sctp_s *conn, const void *addrs, int addrcnt,
    int flags);
extern void sctp_close(struct sctp_s *conn);
extern int sctp_connect(struct sctp_s *conn, const struct sockaddr *dst,
    socklen_t addrlen);
extern struct sctp_s *sctp_create(void *newhandle, struct sctp_s *parent,
    int family, int flags, const sctp_upcalls_t *su,
    sctp_sockbuf_limits_t *sbl, cred_t *cr);
extern int sctp_disconnect(struct sctp_s *conn);
extern int sctp_get_opt(struct sctp_s *conn, int level, int opt, void *opts,
    socklen_t *optlen);
extern int sctp_getpeername(struct sctp_s *conn, struct sockaddr *addr,
    socklen_t *addrlen);
extern int sctp_getsockname(struct sctp_s *conn, struct sockaddr *addr,
    socklen_t *addrlen);
extern int sctp_itf_ver(int);
extern int sctp_listen(struct sctp_s *conn);
extern void sctp_recvd(struct sctp_s *conn, int len);
extern int sctp_sendmsg(struct sctp_s *conn, mblk_t *mp, int flags);
extern int sctp_set_opt(struct sctp_s *conn, int level, int opt,
    const void *opts, socklen_t optlen);

/* Flags for sctp_create(), sctp_alloc_hdr() */
#define	SCTP_CAN_BLOCK			0x01

/* Flags for upcall su_recv() */
#define	SCTP_NOTIFICATION		0x01	/* message is a notification */
#define	SCTP_PARTIAL_DATA		0x02	/* not a full message */

/* Use by sockfs to do sctp_peeloff(). */
#define	SCTP_UC_SWAP			255

#endif /* _KERNEL */

#ifdef __cplusplus
}
#endif

#endif /* _INET_SCTP_ITF_H */
