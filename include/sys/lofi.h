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


#ifndef	_SYS_LOFI_H
#define	_SYS_LOFI_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include <sys/types.h>
#include <sys/time.h>
#include <sys/taskq.h>
#include <sys/vtoc.h>
#include <sys/dkio.h>
#include <sys/vnode.h>

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * /dev names:
 *	/dev/lofictl	- master control device
 *	/dev/lofi	- block devices, named by minor number
 *	/dev/rlofi	- character devices, named by minor number
 */
#define	LOFI_DRIVER_NAME	"lofi"
#define	LOFI_CTL_NODE		"ctl"
#define	LOFI_CTL_NAME		LOFI_DRIVER_NAME LOFI_CTL_NODE
#define	LOFI_BLOCK_NAME		LOFI_DRIVER_NAME
#define	LOFI_CHAR_NAME		"r" LOFI_DRIVER_NAME

/*
 *
 * Use is:
 *	ld = open("/dev/lofictl", O_RDWR | O_EXCL);
 *
 * lofi must be opened exclusively. Access is controlled by permissions on
 * the device, which is 644 by default. Write-access is required for ioctls
 * that change state, but only read-access is required for the ioctls that
 * return information. Basically, only root can add and remove files, but
 * non-root can look at the current lists.
 *
 * ioctl usage:
 *
 * kernel ioctls
 *
 *	strcpy(li.li_filename, "somefilename");
 *	ioctl(ld, LOFI_MAP_FILE, &li);
 *	newminor = li.li_minor;
 *
 *	strcpy(li.li_filename, "somefilename");
 *	ioctl(ld, LOFI_UNMAP_FILE, &li);
 *
 *	strcpy(li.li_filename, "somefilename");
 *	li.li_minor = minor_number;
 *	ioctl(ld, LOFI_MAP_FILE_MINOR, &li);
 *
 *	li.li_minor = minor_number;
 *	ioctl(ld, LOFI_UNMAP_FILE_MINOR, &li);
 *
 *	li.li_minor = minor_number;
 *	ioctl(ld, LOFI_GET_FILENAME, &li);
 *
 *	strcpy(li.li_filename, "somefilename");
 *	ioctl(ld, LOFI_GET_MINOR, &li);
 *
 *	li.li_minor = 0;
 *	ioctl(ld, LOFI_GET_MAXMINOR, &li);
 *	maxminor = li.li_minor;
 *
 * Oh, and last but not least: these ioctls are totally private and only
 * for use by lofiadm(1M).
 *
 */

struct lofi_ioctl {
	uint32_t li_minor;
	char	li_filename[MAXPATHLEN + 1];
};

#define	LOFI_IOC_BASE		(('L' << 16) | ('F' << 8))

#define	LOFI_MAP_FILE		(LOFI_IOC_BASE | 0x01)
#define	LOFI_MAP_FILE_MINOR	(LOFI_IOC_BASE | 0x02)
#define	LOFI_UNMAP_FILE		(LOFI_IOC_BASE | 0x03)
#define	LOFI_UNMAP_FILE_MINOR	(LOFI_IOC_BASE | 0x04)
#define	LOFI_GET_FILENAME	(LOFI_IOC_BASE | 0x05)
#define	LOFI_GET_MINOR		(LOFI_IOC_BASE | 0x06)
#define	LOFI_GET_MAXMINOR	(LOFI_IOC_BASE | 0x07)

/*
 * file types that might be usable with lofi, maybe. Only regular
 * files are documented though.
 */
#define	S_ISLOFIABLE(mode) \
	(S_ISREG(mode) || S_ISBLK(mode) || S_ISCHR(mode))

#if defined(_KERNEL)

/*
 * We limit the maximum number of active lofi devices to 128, which seems very
 * large. You can tune this by changing lofi_max_files in /etc/system.
 * If you change it dynamically, which you probably shouldn't do, make sure
 * to only _increase_ it.
 */
#define	LOFI_MAX_FILES	128
extern uint32_t lofi_max_files;

#define	V_ISLOFIABLE(vtype) \
	((vtype == VREG) || (vtype == VBLK) || (vtype == VCHR))

struct lofi_state {
	char	*ls_filename;	/* filename to open */
	size_t	ls_filename_sz;
	struct vnode	*ls_vp; /* open vnode */
	u_offset_t	ls_vp_size;
	uint32_t	ls_blk_open;
	uint32_t	ls_chr_open;
	uint32_t	ls_lyr_open_count;
	int		ls_openflag;
	taskq_t		*ls_taskq;
	kstat_t		*ls_kstat;
	kmutex_t	ls_kstat_lock;
	struct dk_geom	ls_dkg;
	struct vtoc	ls_vtoc;
	struct dk_cinfo	ls_ci;
};

#endif

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_LOFI_H */
