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
 * Copyright (c) 1991-1998 by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ifndef	_SYS_MEMLIST_H
#define	_SYS_MEMLIST_H

#pragma ident	"%Z%%M%	%I%	%E% SMI" /* SunOS-4.0 1.7 */

/*
 * Common memlist format, exported by boot.
 */

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Boot programs of version 4 and greater pass a linked list
 * of struct memlist to the kernel.
 */
struct memlist {
	uint64_t	address;	/* starting address of memory segment */
	uint64_t	size;		/* size of same */
	struct memlist	*next;		/* link to next list element */
	struct memlist	*prev;		/* link to previous list element */
};

extern int address_in_memlist(struct memlist *, uint64_t, size_t);

/*
 * phys_install is the pointer to the physical installed memory list
 * which may change when memory is added or deleted. The functions
 * memlist_read_lock() and memlist_read_unlock() should be used
 * to protect reading this list.
 */
extern struct memlist *phys_install;
extern void memlist_read_lock(void);
extern void memlist_read_unlock(void);

#ifdef __cplusplus
}
#endif

#endif	/* _SYS_MEMLIST_H */
