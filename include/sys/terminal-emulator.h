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
 * Copyright (c) 1998,2001 by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ifndef	_SYS_TERMINAL_EMULATOR_H
#define	_SYS_TERMINAL_EMULATOR_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _KERNEL
struct terminal_emulator;
int tem_init(struct terminal_emulator **, char *, cred_t *, int, int);
int tem_write(struct terminal_emulator *, unsigned char *, int, cred_t *);
int tem_polled_write(struct terminal_emulator *, unsigned char *, int);
int tem_fini(struct terminal_emulator *);
void tem_get_size(struct terminal_emulator *, int *, int *, int *, int *);
#endif /* _KERNEL */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_TERMINAL_EMULATOR_H */
