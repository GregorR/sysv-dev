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

#ifndef _SYS_SOCKFS_NL7C_H
#define	_SYS_SOCKFS_NL7C_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <sys/atomic.h>
#include <sys/cmn_err.h>
#include <sys/stropts.h>
#include <sys/socket.h>
#include <sys/socketvar.h>

/*
 * NL7C (uint64_t)(struct sonode).so_nl7c_flags:
 */

#define	NL7C_ENABLED	0x00000001 /* NL7C enabled socket */
#define	NL7C_SOPERSIST	0x00000002 /* NL7C socket is persistent */
#define	NL7C_WAITWRITE	0x00000004 /* NL7C waiting first write */
#define	NL7C_AF_NCA	0x00000008 /* NL7C enabled socket via AF_NCA */

#define	NL7C_SCHEMEPRIV	0xFFFF0000 /* NL7C scheme private state */

#define	NL7C_UNUSED	0xFFFFFFFF00000000 /* Unused bits */

/*
 * Globals ...
 */

extern boolean_t	nl7c_enabled;

extern clock_t		nl7c_uri_ttl;

/*
 * Function prototypes ...
 */

boolean_t nl7c_process(struct sonode *, boolean_t, int);
void nl7c_data(struct sonode *, uio_t *);
extern void nl7c_urifree(struct sonode *);
void nl7c_close(struct sonode *);
boolean_t nl7c_parse(struct sonode *, boolean_t, boolean_t *, int);

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_SOCKFS_NL7C_H */
