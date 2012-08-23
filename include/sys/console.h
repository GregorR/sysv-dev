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

#ifndef	_SYS_CONSOLE_H
#define	_SYS_CONSOLE_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef _KERNEL

#include <sys/vnode.h>
#include <sys/taskq.h>
#include <sys/varargs.h>

extern void console_get_size(ushort_t *r, ushort_t *c,
	ushort_t *x, ushort_t *y);
/*PRINTFLIKE1*/
extern void console_printf(const char *, ...) __KPRINTFLIKE(1);
extern void console_vprintf(const char *, va_list) __KVPRINTFLIKE(1);

extern void console_puts(const char *, size_t);
extern void console_putc(int);

extern void console_gets(char *, size_t);
extern int console_getc(void);

extern int console_enter(int);
extern void console_exit(int, int);

extern vnode_t *console_vnode;
extern taskq_t *console_taskq;

#endif	/* _KERNEL */

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_CONSOLE_H */
