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

#ifndef	_SYS_AUXV_SPARC_H
#define	_SYS_AUXV_SPARC_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Flags used to describe various instruction set extensions available
 * on different SPARC processors.
 *
 * [The first three are legacy descriptions.]
 */
#define	AV_SPARC_MUL32	0x0001	/* 32x32-bit smul/umul is efficient */
#define	AV_SPARC_DIV32	0x0002	/* 32x32-bit sdiv/udiv is efficient */
#define	AV_SPARC_FSMULD	0x0004	/* fsmuld is efficient */
#define	AV_SPARC_V8PLUS	0x0008	/* V9 instructions available to 32-bit apps */
#define	AV_SPARC_POPC	0x0010	/* popc is efficient */
#define	AV_SPARC_VIS	0x0020	/* VIS instruction set supported */
#define	AV_SPARC_VIS2	0x0040	/* VIS2 instruction set supported */
#define	AV_SPARC_ASI_BLK_INIT	0x0080	/* ASI_BLK_INIT_xxx ASI */

#define	FMT_AV_SPARC	\
	"\20\10ASIBlkInit\7vis2\6vis\5popc\4v8plus\3fsmuld\2div32\1mul32"

/*
 * compatibility defines: Obsolete
 */
#define	AV_SPARC_HWMUL_32x32	AV_SPARC_MUL32
#define	AV_SPARC_HWDIV_32x32	AV_SPARC_DIV32
#define	AV_SPARC_HWFSMULD	AV_SPARC_FSMULD

#ifdef __cplusplus
}
#endif

#endif	/* !_SYS_AUXV_SPARC_H */
