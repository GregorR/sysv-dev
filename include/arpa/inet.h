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

/*	Copyright (c) 1983, 1984, 1985, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*
 * Portions of this source code were derived from Berkeley 4.3 BSD
 * under license from the Regents of the University of California.
 */

#ifndef _ARPA_INET_H
#define	_ARPA_INET_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include <sys/feature_tests.h>

#if !defined(_XPG4_2) || defined(_XPG6) || defined(__EXTENSIONS__)
#include <sys/socket.h>
#endif /* !defined(_XPG4_2) || defined(_XPG6) || defined(__EXTENSIONS__) */

#include <netinet/in.h>
#if defined(_XPG4_2) && !defined(__EXTENSIONS__)
#include <sys/byteorder.h>
#endif /* defined(_XPG4_2) && !defined(__EXTENSIONS__) */

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * External definitions for
 * functions in inet(3N)
 */
#ifdef __STDC__
#if !defined(_XPG4_2) || defined(__EXTENSIONS__)
extern int inet_net_pton(int, const char *, void *, size_t);
#endif /* !defined(_XPG4_2) || defined(__EXTENSIONS__) */

extern in_addr_t inet_addr(const char *);

extern in_addr_t inet_lnaof(struct in_addr);
extern struct in_addr inet_makeaddr(in_addr_t, in_addr_t);
extern in_addr_t inet_netof(struct in_addr);
extern in_addr_t inet_network(const char *);
extern char *inet_ntoa(struct in_addr);
extern int inet_aton(const char *, struct in_addr *);
#else
unsigned long inet_addr();
char	*inet_ntoa();
struct	in_addr inet_makeaddr();
unsigned long inet_network();
extern unsigned long inet_lnaof();
extern unsigned long inet_netof();
extern int inet_pton();
extern const char *inet_ntop();
extern int inet_aton();
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* _ARPA_INET_H */
