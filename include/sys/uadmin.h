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

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


#ifndef _SYS_UADMIN_H
#define	_SYS_UADMIN_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#if !defined(_ASM)
#include <sys/types.h>
#include <sys/cred.h>
#endif

#ifdef	__cplusplus
extern "C" {
#endif

#define	A_REBOOT	1
#define	A_SHUTDOWN	2
#define	A_FREEZE	3	/* For freeze and thaw */
#define	A_REMOUNT	4
#define	A_DUMP		5
#define	A_FTRACE	15
#define	A_SWAPCTL	16
/*			17-21	   reserved for obsolete interface */

#define	AD_HALT		0	/* halt the processor */
#define	AD_BOOT		1	/* multi-user reboot */
#define	AD_IBOOT	2	/* multi-user reboot, ask for name of file */
#define	AD_SBOOT	3	/* single-user reboot */
#define	AD_SIBOOT	4	/* single-user reboot, ask for name of file */
#define	AD_POWEROFF	6	/* software poweroff */
#define	AD_NOSYNC	7	/* do not sync filesystems on next A_DUMP */

/*
 * Functions reserved for A_FREEZE (may not be available on all platforms)
 */
#define	AD_COMPRESS	0	/* store state file compressed during CPR */
#define	AD_FORCE	1	/* force to do AD_COMPRESS */
#define	AD_CHECK	2	/* test if CPR module is available */
/*
 * NOTE: the following defines comprise an Unstable interface.  Their semantics
 * may change or they may be removed completely in a later release
 */
#define	AD_REUSEINIT	3	/* prepare for AD_REUSABLE */
#define	AD_REUSABLE	4	/* create reusable statefile */
#define	AD_REUSEFINI	5	/* revert to normal CPR mode (not reusable) */

#define	AD_FTRACE_START	1
#define	AD_FTRACE_STOP	2

#if !defined(_ASM)

#if defined(_KERNEL)
extern kmutex_t ualock;
extern void mdboot(int, int, char *);
extern void mdpreboot(int, int, char *);
extern int kadmin(int, int, void *, cred_t *);
extern void killall(zoneid_t);
#endif

#if defined(__STDC__)
extern int uadmin(int, int, uintptr_t);
#else
extern int uadmin();
#endif

#endif	/* _ASM */

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_UADMIN_H */
