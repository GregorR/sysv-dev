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

#ifndef	_MDESC_H_
#define	_MDESC_H_

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Each logical domain is detailed via a (Virtual) Machine Description
 * available to each guest Operating System courtesy of a
 * Hypervisor service.
 */



#ifdef	_ASM
#define	U8(_s)	_s
#define	U16(_s)	_s
#define	U32(_s)	_s
#define	U64(_s)	_s
#else
#define	U8(_s)	((uint8_t)(_s))
#define	U16(_s)	((uint16_t)(_s))
#define	U32(_s)	((uint32_t)(_s))
#define	U64(_s)	((uint64_t)(_s))
#endif





	/* the version this library understands */

#define	MD_HEADER_VERS_OFF	0x0
#define	MD_HEADER_NODE_OFF	0x4
#define	MD_HEADER_NAME_OFF	0x8
#define	MD_HEADER_DATA_OFF	0xc

#define	MD_HEADER_SIZE	0x10

#define	MD_TRANSPORT_VERSION	U32(0x10000)

#define	MD_ELEMENT_SIZE	0x10

#define	MDE_ILLEGAL_IDX		U64(-1)

#define	MDET_LIST_END	U8(0x0)
#define	MDET_NULL	U8(' ')
#define	MDET_NODE	U8('N')
#define	MDET_NODE_END	U8('E')
#define	MDET_PROP_ARC	U8('a')
#define	MDET_PROP_VAL	U8('v')
#define	MDET_PROP_STR	U8('s')
#define	MDET_PROP_DAT	U8('d')


#ifndef _ASM	/* { */

typedef uint64_t mde_cookie_t;
#define	MDE_INVAL_ELEM_COOKIE	((mde_cookie_t)-1)

typedef	uint32_t		mde_str_cookie_t;
#define	MDE_INVAL_STR_COOKIE	((mde_str_cookie_t)-1)


	/* Opaque structure for handling in functions */
typedef void * md_t;




extern md_t		*md_init(void *);
extern md_t		*md_init_intern(uint64_t *, void*(*)(size_t),
				void (*)(void*));

extern int		md_fini(md_t *);

extern int		md_node_count(md_t *);

extern mde_str_cookie_t md_find_name(md_t *, char *namep);

extern mde_cookie_t	md_root_node(md_t *);

extern int		md_scan_dag(md_t *,
				mde_cookie_t,
				mde_str_cookie_t,
				mde_str_cookie_t,
				mde_cookie_t *);

extern int		md_get_prop_val(md_t *,
				mde_cookie_t,
				char *,
				uint64_t *);

extern int		md_get_prop_str(md_t *,
				mde_cookie_t,
				char *,
				char **);

extern int		md_get_prop_data(md_t *,
				mde_cookie_t,
				char *,
				uint8_t **,
				int *);



#endif	/* } _ASM */



/*
 * ioctl info for mdesc device
 */

#define	MDESCIOC	('m' << 24 | 'd' << 16 | 'd' << 8)

#define	MDESCIOCGSZ	(MDESCIOC | 1)   /* Get quote buffer size */
#define	MDESCIOCSSZ	(MDESCIOC | 2)   /* Set new quote buffer size */
#define	MDESCIOCDISCARD	(MDESCIOC | 3)   /* Discard quotes and reset */


#ifdef __cplusplus
}
#endif

#endif	/* _MDESC_H_ */
