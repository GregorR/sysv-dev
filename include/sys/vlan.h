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

/*
 * vlan.h header for common (non-media specific) VLAN declarations.
 */

#ifndef	_SYS_VLAN_H
#define	_SYS_VLAN_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef	__cplusplus
extern "C" {
#endif

#define	VLAN_TAGSZ	4

#define	VLAN_TPID	0x8100u

#define	VLAN_ID_MASK	0x0fffu
#define	VLAN_ID_SIZE	12
#define	VLAN_ID_SHIFT	0

#define	VLAN_CFI_MASK	0x0001u
#define	VLAN_CFI_SIZE	1
#define	VLAN_CFI_SHIFT	(VLAN_ID_SHIFT + VLAN_ID_SIZE)

#define	VLAN_PRI_MASK	0x0007u
#define	VLAN_PRI_SIZE	3
#define	VLAN_PRI_SHIFT	(VLAN_CFI_SHIFT + VLAN_CFI_SIZE)

#define	VLAN_ID_NONE	0
#define	VLAN_ID_MIN	1
#define	VLAN_ID_MAX	4094

#define	VLAN_TCI(pri, cfi, vid) \
	(((pri) << VLAN_PRI_SHIFT) | \
	((cfi) << VLAN_CFI_SHIFT) | \
	((vid) << VLAN_ID_SHIFT))

/*
 * Deconstruct a VTAG ...
 */
#define	VLAN_PRI(tci)	(((tci) >> VLAN_PRI_SHIFT) & VLAN_PRI_MASK)
#define	VLAN_CFI(tci)	(((tci) >> VLAN_CFI_SHIFT) & VLAN_CFI_MASK)
#define	VLAN_ID(tci)	(((tci) >> VLAN_ID_SHIFT) & VLAN_ID_MASK)

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_VLAN_H */
