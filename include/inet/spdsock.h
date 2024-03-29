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
 * Copyright 2001-2003 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef	_INET_SPDSOCK_H
#define	_INET_SPDSOCK_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * spdsock (PF_POLICY) session state; one per open PF_POLICY socket.
 *
 * These are kept on a linked list by the spdsock module.
 */

typedef struct spdsock_s
{
	uint_t	spdsock_state;	/* TLI gorp */

	minor_t spdsock_minor;

	/*
	 * In-progress SPD_DUMP state, valid if spdsock_dump_req is non-NULL.
	 *
	 * spdsock_dump_req is the request which got us started.
	 * spdsock_dump_head is a reference to a policy head.
	 * spdsock_dump_cur_* tell us where we are in the policy walk,
	 * validated by looking at spdsock_dump_gen vs
	 * dump_head->iph_gen after taking a read lock on the policy
	 * head.
	 */
	mblk_t			*spdsock_dump_req;
	ipsec_policy_head_t 	*spdsock_dump_head;
	uint64_t 		spdsock_dump_gen;
	timeout_id_t		spdsock_timeout;
	mblk_t			*spdsock_timeout_arg;
	int			spdsock_dump_cur_type;
	int			spdsock_dump_cur_af;
	ipsec_policy_t 		*spdsock_dump_cur_rule;
	uint32_t		spdsock_dump_count;
} spdsock_t;

#define	LOADCHECK_INTERVAL	(drv_usectohz(30000))

/*
 * Socket option boilerplate code.
 */

extern optdb_obj_t	spdsock_opt_obj;
extern uint_t		spdsock_max_optsize;

extern int spdsock_opt_get(queue_t *, int, int, uchar_t *);
extern int spdsock_opt_set(queue_t *, uint_t, int, int, uint_t, uchar_t *,
    uint_t *, uchar_t *, void *, cred_t *,
    mblk_t *);

#ifdef	__cplusplus
}
#endif

#endif /* _INET_SPDSOCK_H */
