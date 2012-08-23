/*
 * Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef	_SYS_AGPMASTER_IO_H
#define	_SYS_AGPMASTER_IO_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _KERNEL

#define	AGPMASTER_NAME		"agpmaster"
#define	AGPMASTER_DEVLINK	"/dev/agp/agpmaster"

/* macros for layered ioctls */
#define	AGPMASTERIOC_BASE		'M'
#define	DEVICE_DETECT		_IOR(AGPMASTERIOC_BASE, 10, int)
#define	I8XX_GET_INFO		_IOR(AGPMASTERIOC_BASE, 11, igd_info_t)
#define	I810_SET_GTT_BASE	_IOW(AGPMASTERIOC_BASE, 12, uint32_t)
#define	I8XX_ADD2GTT		_IOW(AGPMASTERIOC_BASE, 13, igd_gtt_seg_t)
#define	I8XX_REM_GTT		_IOW(AGPMASTERIOC_BASE, 14, igd_gtt_seg_t)
#define	I8XX_UNCONFIG		_IO(AGPMASTERIOC_BASE, 16)
#define	AGP_MASTER_GETINFO	_IOR(AGPMASTERIOC_BASE, 20, agp_info_t)
#define	AGP_MASTER_SETCMD	_IOW(AGPMASTERIOC_BASE, 21, uint32_t)

/* used for IGD to bind/unbind gtt entries */
typedef struct igd_gtt_seg {
	uint32_t	igs_pgstart;
	uint32_t	igs_npage;
	uint32_t	*igs_phyaddr; /* pointer to address array */
	uint32_t	igs_type; /* reserved for other memory type */
} igd_gtt_seg_t;

/* used for IGD to get info */
typedef struct igd_info {
	uint32_t	igd_devid;
	uint32_t	igd_aperbase;
	size_t		igd_apersize; /* in MB */
} igd_info_t;

#endif /* _KERNEL */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_AGPMASTER_IO_H */
