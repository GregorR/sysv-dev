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

#ifndef	_SYS_FILEP_H
#define	_SYS_FILEP_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/fs/ufs_fsdir.h>
#include <sys/fs/ufs_fs.h>
#include <sys/fs/ufs_inode.h>

/*
 *  These structs should be invisible to the caller of
 *  the user-level routines
 */
typedef struct dev_ident {	/* device identifier block */
	char		*di_desc;
	int		di_dcookie;
	char		di_taken;
	union {
		struct	fs	di_fs;
		char	dummy[SBSIZE];
	}		un_fs;
} devid_t;

typedef struct file_ident {	/* file identifier block */
	uint_t		fi_filedes;
	char		*fi_path;
	daddr_t		fi_blocknum;
	uint_t		fi_count;
	off_t		fi_offset;
	caddr_t		fi_memp;
	char		fi_taken;
	devid_t		*fi_devp;
	char		fi_buf[MAXBSIZE];
	struct	inode	*fi_inode;
	struct file_ident *fi_forw;
	struct file_ident *fi_back;
}  fileid_t;

extern int diskread(fileid_t *);

#ifdef __cplusplus
}
#endif

#endif	/* _SYS_FILEP_H */
