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
/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/


/*
 * Copyright 2004 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _STRING_H
#define	_STRING_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"	/* SVr4.0 1.7.1.12 */

#include <iso/string_iso.h>

/*
 * Allow global visibility for symbols defined in
 * C++ "std" namespace in <iso/string_iso.h>.
 */
#if __cplusplus >= 199711L
using std::size_t;
using std::memchr;
using std::memcmp;
using std::memcpy;
using std::memmove;
using std::memset;
using std::strcat;
using std::strchr;
using std::strcmp;
using std::strcoll;
using std::strcpy;
using std::strcspn;
using std::strerror;
using std::strlen;
using std::strncat;
using std::strncmp;
using std::strncpy;
using std::strpbrk;
using std::strrchr;
using std::strspn;
using std::strstr;
using std::strtok;
using std::strxfrm;
#endif

#ifdef	__cplusplus
extern "C" {
#endif

#if defined(__STDC__)

#if defined(__EXTENSIONS__) || \
	(!defined(_STRICT_STDC) && !defined(__XOPEN_OR_POSIX)) || \
	defined(_XPG6) || defined(_REENTRANT)
extern int strerror_r(int, char *, size_t);
#endif

#if defined(__EXTENSIONS__) || \
	(!defined(_STRICT_STDC) && !defined(__XOPEN_OR_POSIX)) || \
	(_POSIX_C_SOURCE - 0 >= 199506L) || defined(_REENTRANT)
extern char *strtok_r(char *_RESTRICT_KYWD, const char *_RESTRICT_KYWD,
	char **_RESTRICT_KYWD);
#endif

#if defined(__EXTENSIONS__) || !defined(_STRICT_STDC) || \
	defined(__XOPEN_OR_POSIX)
extern void *memccpy(void *_RESTRICT_KYWD, const void *_RESTRICT_KYWD,
		int, size_t);
#endif

#if defined(__EXTENSIONS__) || \
	(!defined(_STRICT_STDC) && !defined(__XOPEN_OR_POSIX))
extern int ffs(int);
extern size_t strlcpy(char *, const char *, size_t);
extern size_t strlcat(char *, const char *, size_t);
#endif /* defined(__EXTENSIONS__)... */

#if defined(__EXTENSIONS__) || \
	(!defined(_STRICT_STDC) && !defined(__XOPEN_OR_POSIX)) || \
	defined(_XPG4_2)
extern char *strdup(const char *);
#endif

#else	/* __STDC__ */

#if defined(__EXTENSIONS__) || !defined(__XOPEN_OR_POSIX) || \
	defined(_XPG6) || defined(_REENTRANT)
extern int strerror_r();
#endif

#if defined(__EXTENSIONS__) || !defined(__XOPEN_OR_POSIX) || \
	(_POSIX_C_SOURCE - 0 >= 199506L) || defined(_REENTRANT)
extern char *strtok_r();
#endif

#if defined(__EXTENSIONS__) || !defined(_STRICT_STDC) || \
	defined(__XOPEN_OR_POSIX)
extern void *memccpy();
#endif

#if defined(__EXTENSIONS__) || !defined(__XOPEN_OR_POSIX)
extern int ffs();
extern size_t strlcpy();
extern size_t strlcat();
#endif /* defined(__EXTENSIONS__) ... */

#if defined(__EXTENSIONS__) || !defined(__XOPEN_OR_POSIX) || defined(_XPG4_2)
extern char *strdup();
#endif

#endif	/* __STDC__ */

#ifdef	__cplusplus
}
#endif

#endif	/* _STRING_H */
