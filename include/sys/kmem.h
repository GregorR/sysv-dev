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
 * Copyright 2003 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T */
/*	  All Rights Reserved	*/

#ifndef _SYS_KMEM_H
#define	_SYS_KMEM_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include <sys/types.h>
#include <sys/vmem.h>

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * Kernel memory allocator: DDI interfaces.
 * See kmem_alloc(9F) for details.
 */

#define	KM_SLEEP	0x0000	/* can block for memory; success guaranteed */
#define	KM_NOSLEEP	0x0001	/* cannot block for memory; may fail */
#define	KM_PANIC	0x0002	/* if memory cannot be allocated, panic */
#define	KM_PUSHPAGE	0x0004	/* can block for memory; may use reserve */
#define	KM_VMFLAGS	0x00ff	/* flags that must match VM_* flags */

#define	KM_FLAGS	0xffff	/* all settable kmem flags */

#ifdef _KERNEL

extern void *kmem_alloc(size_t size, int kmflags);
extern void *kmem_zalloc(size_t size, int kmflags);
extern void kmem_free(void *buf, size_t size);
extern void *kmem_alloc_tryhard(size_t size, size_t *alloc_size, int kmflags);

#endif	/* _KERNEL */

/*
 * Kernel memory allocator: private interfaces.
 * These interfaces are still evolving.
 * Do not use them in unbundled drivers.
 */

/*
 * Flags for kmem_cache_create()
 */
#define	KMC_NOTOUCH	0x00010000
#define	KMC_NODEBUG	0x00020000
#define	KMC_NOMAGAZINE	0x00040000
#define	KMC_NOHASH	0x00080000
#define	KMC_QCACHE	0x00100000
#define	KMC_KMEM_ALLOC	0x00200000	/* internal use only */
#define	KMC_IDENTIFIER	0x00400000	/* internal use only */

struct kmem_cache;		/* cache structure is opaque to kmem clients */

typedef struct kmem_cache kmem_cache_t;

#ifdef _KERNEL

extern int kmem_ready;
extern pgcnt_t kmem_reapahead;

extern void kmem_init(void);
extern void kmem_thread_init(void);
extern void kmem_mp_init(void);
extern void kmem_reap(void);
extern void kmem_reap_idspace(void);
extern size_t kmem_avail(void);
extern size_t kmem_maxavail(void);

extern kmem_cache_t *kmem_cache_create(char *, size_t, size_t,
	int (*)(void *, void *, int), void (*)(void *, void *),
	void (*)(void *), void *, vmem_t *, int);
extern void kmem_cache_destroy(kmem_cache_t *);
extern void *kmem_cache_alloc(kmem_cache_t *, int);
extern void kmem_cache_free(kmem_cache_t *, void *);
extern uint64_t kmem_cache_stat(kmem_cache_t *, char *);

#endif	/* _KERNEL */

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_KMEM_H */
