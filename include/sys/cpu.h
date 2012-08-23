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

#ifndef _SYS_CPU_H
#define	_SYS_CPU_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

/*
 * WARNING:
 *	This header file is Obsolete and may be deleted in a
 *	future release of Solaris.
 */

/*
 * Include generic bustype cookies.
 */
#include <sys/bustypes.h>
#if defined(__GNUC__) && defined(_ASM_INLINES) && defined(_KERNEL)
#include <asm/cpu.h>
#endif

#ifdef	__cplusplus
extern "C" {
#endif

#if defined(_KERNEL)
extern void ht_pause(void);
extern void cli(void);
extern void sti(void);
extern void i86_halt(void);

/*
 * Used to insert cpu-dependent instructions into spin loops
 */
#define	SMT_PAUSE()		ht_pause()

#endif	/* _KERNEL */

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_CPU_H */
