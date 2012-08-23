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
 * Copyright 1998-2003 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _SYS_SHA1_H
#define	_SYS_SHA1_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include <sys/types.h>		/* for uint_* */

#ifdef	__cplusplus
extern "C" {
#endif

/* SHA-1 context. */
typedef struct 	{
	uint32_t state[5];	/* state (ABCDE) */
	uint32_t count[2];	/* number of bits, modulo 2^64 (msb first) */
	union 	{
		uint8_t		buf8[64];	/* undigested input */
		uint32_t	buf32[16];	/* realigned input */
	} buf_un;
} SHA1_CTX;

void SHA1Init(SHA1_CTX *);
void SHA1Update(SHA1_CTX *, const uint8_t *, uint32_t);
void SHA1Final(uint8_t *, SHA1_CTX *);

#ifdef	__cplusplus
}
#endif

#endif /* _SYS_SHA1_H */
