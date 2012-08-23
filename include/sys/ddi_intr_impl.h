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

#ifndef	_SYS_DDI_INTR_IMPL_H
#define	_SYS_DDI_INTR_IMPL_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

/*
 * Sun DDI interrupt implementation specific definitions
 */

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef _KERNEL

/*
 * Typedef for interrupt ops
 */
typedef enum {
	DDI_INTROP_SUPPORTED_TYPES = 1,	/* 1 get supported interrupts types */
	DDI_INTROP_NINTRS,		/* 2 get num of interrupts supported */
	DDI_INTROP_ALLOC,		/* 3 allocate interrupt handle */
	DDI_INTROP_GETPRI,		/* 4 get priority */
	DDI_INTROP_SETPRI,		/* 5 set priority */
	DDI_INTROP_ADDISR,		/* 6 add interrupt handler */
	DDI_INTROP_DUPVEC,		/* 7 duplicate interrupt handler */
	DDI_INTROP_ENABLE,		/* 8 enable interrupt */
	DDI_INTROP_BLOCKENABLE,		/* 9 block enable interrupts */
	DDI_INTROP_BLOCKDISABLE,	/* 10 block disable interrupts */
	DDI_INTROP_DISABLE,		/* 11 disable interrupt */
	DDI_INTROP_REMISR,		/* 12 remove interrupt handler */
	DDI_INTROP_FREE,		/* 13 free interrupt handle */
	DDI_INTROP_GETCAP,		/* 14 get capacity */
	DDI_INTROP_SETCAP,		/* 15 set capacity */
	DDI_INTROP_SETMASK,		/* 16 set mask */
	DDI_INTROP_CLRMASK,		/* 17 clear mask */
	DDI_INTROP_GETPENDING,		/* 18 get pending interrupt */
	DDI_INTROP_NAVAIL		/* 19 get num of available interrupts */
} ddi_intr_op_t;

/* Version number used in the handles */
#define	DDI_INTR_VERSION_1	1
#define	DDI_INTR_VERSION	DDI_INTR_VERSION_1

/*
 * One such data structure is allocated per ddi_intr_handle_t
 * This is the incore copy of the regular interrupt info.
 */
typedef struct ddi_intr_handle_impl {
	dev_info_t		*ih_dip;	/* dip associated with handle */
	uint16_t		ih_type;	/* interrupt type being used */
	ushort_t		ih_inum;	/* interrupt number */
	ushort_t		ih_vector;	/* vector number */
	uint16_t		ih_ver;		/* Version */
	uint_t			ih_state;	/* interrupt handle state */
	uint_t			ih_cap;		/* interrupt capabilities */
	uint_t			ih_pri;		/* priority - bus dependent */
	krwlock_t		ih_rwlock;	/* read/write lock per handle */

	uint_t			(*ih_cb_func)(caddr_t, caddr_t);
	void			*ih_cb_arg1;
	void			*ih_cb_arg2;

	/*
	 * The next set of members are for 'scratch' purpose only.
	 * The DDI interrupt framework uses them internally and their
	 * interpretation is left to the framework. For now,
	 *	scratch1	- used to send NINTRs information
	 *			  to various nexus drivers.
	 *	scratch2	- used to send 'behavior' flag
	 *			  information to the nexus drivers
	 *			  from ddi_intr_alloc()
	 *	private		- used by the DDI framework to
	 *			  pass back and forth 'vector' information
	 *			  It is extensively used on the SPARC side
	 *			  to temporarily hold the 'ddi_ispec_t'
	 */
	void			*ih_private;	/* Platform specific data */
	uint_t			ih_scratch1;	/* Scratch1: #interrupts */
	uint_t			ih_scratch2;	/* Scratch2: flag */
} ddi_intr_handle_impl_t;

/* values for ih_state (strictly for interrupt handle) */
#define	DDI_IHDL_STATE_ALLOC	0x01	/* Allocated. ddi_intr_alloc() called */
#define	DDI_IHDL_STATE_ADDED	0x02	/* Added interrupt handler */
					/* ddi_intr_add_handler() called */
#define	DDI_IHDL_STATE_ENABLE	0x04	/* Enabled. ddi_intr_enable() called */

#define	DDI_INTR_IS_MSI_OR_MSIX(type) \
	((type) == DDI_INTR_TYPE_MSI || (type) == DDI_INTR_TYPE_MSIX)

/*
 * One such data structure is allocated per ddi_soft_intr_handle
 * This is the incore copy of the softint info.
 */
typedef struct ddi_softint_hdl_impl {
	dev_info_t	*ih_dip;		/* dip associated with handle */
	uint_t		ih_pri;			/* priority - bus dependent */
	krwlock_t	ih_rwlock;		/* read/write lock per handle */
	uint_t		ih_pending;		/* whether softint is pending */

	uint_t		(*ih_cb_func)(caddr_t, caddr_t);
						/* cb function for soft ints */
	void		*ih_cb_arg1;		/* arg1 of callback function */
	void		*ih_cb_arg2;		/* arg2 passed to "trigger" */

	/*
	 * The next member is for 'scratch' purpose only.
	 * The DDI interrupt framework uses it internally and its
	 * interpretation is left to the framework.
	 *	private		- used by the DDI framework to pass back
	 *			  and forth 'softid' information on SPARC
	 *			  side only. Not used on X86 platform.
	 */
	void		*ih_private;		/* Platform specific data */
} ddi_softint_hdl_impl_t;

/* Softint internal implementation defines */
#define	DDI_SOFT_INTR_PRI_M	4
#define	DDI_SOFT_INTR_PRI_H	6

/*
 * One such data structure is allocated for MSI-X enabled
 * device. If no MSI-X is enabled then it is NULL
 */
typedef struct ddi_intr_msix {
	uint_t			msix_intrs_in_use;	/* MSI-X intrs in use */

	/* MSI-X Table related information */
	ddi_acc_handle_t	msix_tbl_hdl;		/* MSI-X table handle */
	caddr_t			msix_tbl_addr;		/* MSI-X table addr */
	offset_t		msix_tbl_offset;	/* MSI-X table offset */

	/* MSI-X PBA Table related information */
	ddi_acc_handle_t	msix_pba_hdl;		/* MSI-X PBA handle */
	caddr_t			msix_pba_addr;		/* MSI-X PBA addr */
	offset_t		msix_pba_offset;	/* MSI-X PBA offset */

	ddi_device_acc_attr_t	msix_dev_attr;		/* MSI-X device attr */
} ddi_intr_msix_t;


/*
 * One such data structure is allocated for each dip.
 * It has interrupt related information that can be
 * stored/retrieved for convenience.
 */
typedef struct devinfo_intr {
	/* These three fields show what the device is capable of */
	uint_t		devi_intr_sup_types;	/* Intrs supported by device */

	ddi_intr_msix_t	*devi_msix_p;		/* MSI-X info, if supported */

	/* Next three fields show current status for the device */
	uint_t		devi_intr_curr_type;	/* Interrupt type being used */
	uint_t		devi_intr_sup_nintrs;	/* #intr supported */
	uint_t		devi_intr_curr_nintrs;	/* #intr currently being used */

	ddi_intr_handle_t **devi_intr_handle_p;	/* Hdl for legacy intr APIs */
} devinfo_intr_t;

#define	NEXUS_HAS_INTR_OP(dip)	\
	((DEVI(dip)->devi_ops->devo_bus_ops) && \
	(DEVI(dip)->devi_ops->devo_bus_ops->busops_rev >= BUSO_REV_9) && \
	(DEVI(dip)->devi_ops->devo_bus_ops->bus_intr_op))

int	i_ddi_handle_intr_ops(dev_info_t *dip, dev_info_t *rdip,
	    ddi_intr_op_t op, ddi_intr_handle_impl_t *hdlp, void *result);
int	i_ddi_intr_ops(dev_info_t *dip, dev_info_t *rdip, ddi_intr_op_t op,
	    ddi_intr_handle_impl_t *hdlp, void *result);

int	i_ddi_add_softint(ddi_softint_hdl_impl_t *);
void	i_ddi_remove_softint(ddi_softint_hdl_impl_t *);
int	i_ddi_trigger_softint(ddi_softint_hdl_impl_t *);
int	i_ddi_set_softint_pri(ddi_softint_hdl_impl_t *, uint_t);

void	i_ddi_intr_devi_init(dev_info_t *dip);
void	i_ddi_intr_devi_fini(dev_info_t *dip);

uint_t	i_ddi_intr_get_supported_types(dev_info_t *dip);
void	i_ddi_intr_set_supported_types(dev_info_t *dip, int sup_type);
uint_t	i_ddi_intr_get_current_type(dev_info_t *dip);
void	i_ddi_intr_set_current_type(dev_info_t *dip, int intr_type);
uint_t	i_ddi_intr_get_supported_nintrs(dev_info_t *dip, int intr_type);
void	i_ddi_intr_set_supported_nintrs(dev_info_t *dip, int nintrs);
uint_t	i_ddi_intr_get_current_nintrs(dev_info_t *dip);
void	i_ddi_intr_set_current_nintrs(dev_info_t *dip, int nintrs);

void	i_ddi_set_intr_handle(dev_info_t *dip, int inum,
	    ddi_intr_handle_t *hdlp);
ddi_intr_handle_t *i_ddi_get_intr_handle(dev_info_t *dip, int inum);

ddi_intr_msix_t	*i_ddi_get_msix(dev_info_t *dip);
void	i_ddi_set_msix(dev_info_t *dip, ddi_intr_msix_t *msix_p);

int32_t i_ddi_get_intr_weight(dev_info_t *);
int32_t i_ddi_set_intr_weight(dev_info_t *, int32_t);

int	i_ddi_get_nintrs(dev_info_t *dip);

#define	DDI_INTR_ASSIGN_HDLR_N_ARGS(hdlp, func, arg1, arg2) \
	hdlp->ih_cb_func = func; \
	hdlp->ih_cb_arg1 = arg1; \
	hdlp->ih_cb_arg2 = arg2;

#else	/* _KERNEL */

typedef struct devinfo_intr devinfo_intr_t;

#endif	/* _KERNEL */

/*
 * Used only by old DDI interrupt interfaces.
 */

/*
 * This structure represents one interrupt possible from the given
 * device. It is used in an array for devices with multiple interrupts.
 */
struct intrspec {
	uint_t intrspec_pri;		/* interrupt priority */
	uint_t intrspec_vec;		/* vector # (0 if none) */
	uint_t (*intrspec_func)();	/* function to call for interrupt, */
					/* If (uint_t (*)()) 0, none. */
					/* If (uint_t (*)()) 1, then */
};

#ifdef _KERNEL
/*
 * This structure is allocated by i_ddi_add_softintr and its address is used
 * as a cookie passed back to the caller to be used later by
 * i_ddi_remove_softintr
 */
struct soft_intrspec {
	struct dev_info *si_devi;	/* records dev_info of caller */
	struct intrspec si_intrspec;	/* and the intrspec */
};

/*
 * NOTE:
 *	The following 4 busops entry points are obsoleted with version
 *	9 or greater. Use i_ddi_intr_op interface in place of these
 *	obsolete interfaces.
 *
 *	Remove these busops entry points and all related data structures
 *	in future minor/major solaris release.
 */
typedef enum {DDI_INTR_CTLOPS_NONE} ddi_intr_ctlop_t;

/* The following are the obsolete interfaces */
ddi_intrspec_t	i_ddi_get_intrspec(dev_info_t *dip, dev_info_t *rdip,
	    uint_t inumber);

int	i_ddi_add_intrspec(dev_info_t *dip, dev_info_t *rdip,
	    ddi_intrspec_t intrspec, ddi_iblock_cookie_t *iblock_cookiep,
	    ddi_idevice_cookie_t *idevice_cookiep,
	    uint_t (*int_handler)(caddr_t int_handler_arg),
	    caddr_t int_handler_arg, int kind);

void	i_ddi_remove_intrspec(dev_info_t *dip, dev_info_t *rdip,
	    ddi_intrspec_t intrspec, ddi_iblock_cookie_t iblock_cookie);

int	i_ddi_intr_ctlops(dev_info_t *dip, dev_info_t *rdip,
	    ddi_intr_ctlop_t op, void *arg, void *val);

#endif	/* _KERNEL */

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_DDI_INTR_IMPL_H */
