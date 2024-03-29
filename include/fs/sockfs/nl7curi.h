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

#ifndef _SYS_SOCKFS_NL7CURI_H
#define	_SYS_SOCKFS_NL7CURI_H

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
 * Some usefull chararcter macros:
 */

#ifndef	tolower
#define	tolower(c) ((c) >= 'A' && (c) <= 'Z' ? (c) | 0x20 : (c))
#endif

#ifndef	isdigit
#define	isdigit(c) ((c) >= '0' && (c) <= '9')
#endif

#ifndef isalpha
#define	isalpha(c) (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))
#endif

#ifndef isspace
#define	isspace(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || \
		(c) == '\r' || (c) == '\f' || (c) == '\013')
#endif

/*
 * ref_t - reference type, ...
 *
 * Note, all struct's must contain a single ref_t, all must use
 * kmem_cache, all must use the REF_* macros for free.
 */

typedef struct ref_s {
	uint32_t	cnt;		/* Reference count */
	void		(*last)(void *); /* Call-back for last ref */
	kmem_cache_t	*kmc;		/* Container allocator cache */
} ref_t;

#define	REF_INIT(container, count, inactive, kmem) {			\
	(container)->ref.cnt = (count);					\
	(container)->ref.last = (void (*)(void *))((inactive));		\
	(container)->ref.kmc = (kmem);					\
}

#define	REF_HOLD(container) {						\
	atomic_add_32(&(container)->ref.cnt, 1);			\
	ASSERT((container)->ref.cnt != 0);				\
}

#define	REF_RELE(container) {						\
	if (atomic_add_32_nv(&(container)->ref.cnt, -1) == 0) {		\
		(container)->ref.last((container));			\
		kmem_cache_free((container)->ref.kmc, (container));	\
	}								\
}

#define	REF_ASSERT(container, count)					\
	ASSERT((container)->ref.cnt == (count));

/*
 * str_t - string type, used to access a an arbitrary span of a char[].
 */

typedef struct str_s {
	char	*cp;			/* Char pointer current char */
	char	*ep;			/* Char pointer past end of string */
} str_t;

/*
 * uri_*_t - URI descriptor, used to describe a cached URI object.
 */

typedef struct uri_rd_s {
	size_t		sz;		/* Size of data */
	offset_t	off;		/* Offset into file or -1 for kmem */
	union {				/* Response data */
		char	*kmem;		/* Data in kmem */
		vnode_t	*vnode;		/* Data in vnode */
	} data;
	struct uri_rd_s *next;		/* Next response descriptor */
} uri_rd_t;

typedef struct uri_desc_s {
	struct uri_desc_s *hash;	/* Hash *next */
	uint64_t	hit;		/* Hit counter */
	clock_t		expire;		/* URI lbolt expires on (-1 = NEVER) */
	boolean_t	nocache;	/* URI no cache */

	mblk_t		*reqmp;		/* Request mblk_t */
	str_t		path;		/* Path name of response  */
	str_t		auth;		/* Authority for response */
	ssize_t		resplen;	/* Response length */
	char		*eoh;		/* End of header pointer */
	void		*scheme;	/* Scheme private state */

	ref_t		ref;		/* Reference stuff */

	size_t		count;		/* rd_t chain byte cound */
	uri_rd_t	*tail;		/* Last response descriptor */
	uri_rd_t	response;	/* First response descriptor */

	struct sonode	*proc;		/* Socket processing this uri */
	kcondvar_t	waiting;	/* Socket(s) waiting for processing */
	kmutex_t	proclock;	/* Lock for proc and waiting */
} uri_desc_t;

#define	URI_TEMP (uri_desc_t *)-1	/* Temp (nocache) uri_t.hash pointer */
#define	URI_TEMP_PARSE_SZ 512		/* Enough bytes to parse for headers */

typedef struct uri_segmap_s {
	ref_t		ref;		/* Reference, one per uri_desb_t */
	caddr_t		base;		/* Base addr of segmap mapping */
	size_t		len;		/* Length of segmap mapping */
	vnode_t		*vp;		/* Vnode mapped */
} uri_segmap_t;

typedef struct uri_desb_s {
	frtn_t		frtn;		/* For use by esballoc() and freinds */
	uri_desc_t	*uri;		/* Containing URI of REF_HOLD() */
	uri_segmap_t	*segmap;	/* If segmap mapped else NULL */
} uri_desb_t;

/*
 * Function prototypes:
 */

boolean_t nl7c_http_request(char **, char *, uri_desc_t *, struct sonode *);
boolean_t nl7c_http_response(char **, char *, uri_desc_t *, struct sonode *);
boolean_t nl7c_http_cmp(void *, void *);
mblk_t *nl7c_http_persist(struct sonode *);
void nl7c_http_free(void *arg);
void nl7c_http_init(void);

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_SOCKFS_NL7CURI_H */
