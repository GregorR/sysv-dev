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
 * Copyright 2004 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1983, 1984, 1985, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*
 * Portions of this source code were derived from Berkeley 4.3 BSD
 * under license from the Regents of the University of California.
 */

#ifndef _SYS_CRED_H
#define	_SYS_CRED_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include <sys/types.h>

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * The credential is an opaque kernel private data structure defined in
 * <sys/cred_impl.h>.
 */

typedef struct cred cred_t;

#ifdef _KERNEL

#define	CRED()		curthread->t_cred

struct proc;				/* cred.h is included in proc.h */
struct prcred;

struct auditinfo_addr;			/* cred.h is included in audit.h */

extern int ngroups_max;
/*
 * kcred is used when you need all privileges.
 */
extern struct cred *kcred;

extern void cred_init(void);
extern void crhold(cred_t *);
extern void crfree(cred_t *);
extern cred_t *cralloc(void);		/* all but ref uninitialized */
extern cred_t *crget(void);		/* initialized */
extern cred_t *crcopy(cred_t *);
extern void crcopy_to(cred_t *, cred_t *);
extern cred_t *crdup(cred_t *);
extern void crdup_to(cred_t *, cred_t *);
extern cred_t *crgetcred(void);
extern void crset(struct proc *, cred_t *);
extern int groupmember(gid_t, const cred_t *);
extern int supgroupmember(gid_t, const cred_t *);
extern int hasprocperm(const cred_t *, const cred_t *);
extern int prochasprocperm(struct proc *, struct proc *, const cred_t *);
extern int crcmp(const cred_t *, const cred_t *);

extern uid_t crgetuid(const cred_t *);
extern uid_t crgetruid(const cred_t *);
extern uid_t crgetsuid(const cred_t *);
extern gid_t crgetgid(const cred_t *);
extern gid_t crgetrgid(const cred_t *);
extern gid_t crgetsgid(const cred_t *);
extern zoneid_t crgetzoneid(const cred_t *);
extern projid_t crgetprojid(const cred_t *);

extern const struct auditinfo_addr *crgetauinfo(const cred_t *);
extern struct auditinfo_addr *crgetauinfo_modifiable(cred_t *);

extern uint_t crgetref(const cred_t *);

extern const gid_t *crgetgroups(const cred_t *);

extern int crgetngroups(const cred_t *);

/*
 * Sets real, effective and/or saved uid/gid;
 * -1 argument accepted as "no change".
 */
extern int crsetresuid(cred_t *, uid_t, uid_t, uid_t);
extern int crsetresgid(cred_t *, gid_t, gid_t, gid_t);

/*
 * Sets real, effective and saved uids/gids all to the same
 * values.  Both values must be non-negative and <= MAXUID
 */
extern int crsetugid(cred_t *, uid_t, gid_t);

extern int crsetgroups(cred_t *, int, gid_t *);

/*
 * Private interface for setting zone association of credential.
 */
struct zone;
extern void crsetzone(cred_t *, struct zone *);

/*
 * Private interface for setting project id in credential.
 */
extern void crsetprojid(cred_t *, projid_t);

/*
 * Private interface for nfs.
 */
extern cred_t *crnetadjust(cred_t *);

/*
 * Private interface for procfs.
 */
extern void cred2prcred(const cred_t *, struct prcred *);

#endif	/* _KERNEL */

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_CRED_H */
