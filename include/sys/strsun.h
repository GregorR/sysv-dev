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

#ifndef	_SYS_STRSUN_H
#define	_SYS_STRSUN_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include <sys/stream.h>
#include <sys/types.h>

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * Solaris DDI STREAMS utility routines.
 *
 * See the appropriate section 9F manpage for documentation.
 */

#define	DB_BASE(mp)	((mp)->b_datap->db_base)
#define	DB_LIM(mp)	((mp)->b_datap->db_lim)
#define	DB_REF(mp)	((mp)->b_datap->db_ref)
#define	DB_TYPE(mp)	((mp)->b_datap->db_type)

#define	MBLKL(mp)	((mp)->b_wptr - (mp)->b_rptr)
#define	MBLKSIZE(mp)	((mp)->b_datap->db_lim - (mp)->b_datap->db_base)
#define	MBLKHEAD(mp)	((mp)->b_rptr - (mp)->b_datap->db_base)
#define	MBLKTAIL(mp)	((mp)->b_datap->db_lim - (mp)->b_wptr)
#define	MBLKIN(mp, off, len) (((off) <= MBLKL(mp)) && \
			(((mp)->b_rptr + (off) + (len)) <= (mp)->b_wptr))

#ifdef	_KERNEL
extern void	mcopyin(mblk_t *, void *, size_t, void *);
extern void	mcopyout(mblk_t *, void *, size_t, void *, mblk_t *);
extern void	merror(queue_t *, mblk_t *, int);
extern void	mioc2ack(mblk_t *, mblk_t *, size_t, int);
extern void	miocack(queue_t *, mblk_t *, int, int);
extern void	miocnak(queue_t *, mblk_t *, int, int);
extern int	miocpullup(mblk_t *, size_t);
extern mblk_t	*mexchange(queue_t *, mblk_t *, size_t, uchar_t, int32_t);
extern size_t	msgsize(mblk_t *);
extern void	mcopymsg(mblk_t *, void *);

#endif	/* _KERNEL */

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_STRSUN_H */
