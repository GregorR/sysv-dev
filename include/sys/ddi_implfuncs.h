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

#ifndef _SYS_DDI_IMPLFUNCS_H
#define	_SYS_DDI_IMPLFUNCS_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/obpdefs.h>
#include <sys/vnode.h>
#include <sys/types.h>
#include <sys/task.h>
#include <sys/project.h>

#ifdef	_KERNEL

/*
 * Declare implementation functions that sunddi functions can
 * call in order to perform their required task. Each kernel
 * architecture must provide them.
 */

int
i_ddi_bus_map(dev_info_t *dip, dev_info_t *rdip, ddi_map_req_t *mp,
	off_t offset, off_t len, caddr_t *vaddrp);

int
i_ddi_apply_range(dev_info_t *dip, dev_info_t *rdip, struct regspec *rp);

struct regspec *
i_ddi_rnumber_to_regspec(dev_info_t *dip, int rnumber);


int
i_ddi_map_fault(dev_info_t *dip, dev_info_t *rdip,
	struct hat *hat, struct seg *seg, caddr_t addr,
	struct devpage *dp, pfn_t pfn, uint_t prot, uint_t lock);

ddi_regspec_t
i_ddi_get_regspec(dev_info_t *dip, dev_info_t *rdip, uint_t rnumber,
	off_t offset, off_t len);

void
i_ddi_set_parent_private(dev_info_t *dip, caddr_t data);

caddr_t
i_ddi_get_parent_private(dev_info_t *dip);

/*
 * Implementation specific memory allocation and de-allocation routines.
 */
int
i_ddi_mem_alloc(dev_info_t *dip, ddi_dma_attr_t *attributes,
	size_t length, int cansleep, int streaming,
	ddi_device_acc_attr_t *accattrp, caddr_t *kaddrp,
	size_t *real_length, ddi_acc_hdl_t *handlep);

int
i_ddi_mem_alloc_lim(dev_info_t *dip, ddi_dma_lim_t *limits,
	size_t length, int cansleep, int streaming,
	ddi_device_acc_attr_t *accattrp, caddr_t *kaddrp,
	uint_t *real_length, ddi_acc_hdl_t *handlep);

void
i_ddi_mem_free(caddr_t kaddr, int streaming);

int
i_ddi_devi_get_ppa(dev_info_t *dip);

void
i_ddi_devi_set_ppa(dev_info_t *dip, int ppa);

/*
 * Access and DMA handle fault set/clear routines
 */
void
i_ddi_acc_set_fault(ddi_acc_handle_t handle);
void
i_ddi_acc_clr_fault(ddi_acc_handle_t handle);
void
i_ddi_dma_set_fault(ddi_dma_handle_t handle);
void
i_ddi_dma_clr_fault(ddi_dma_handle_t handle);

/*
 *      Event-handling functions for rootnex
 *      These provide the standard implementation of fault handling
 */
void
i_ddi_rootnex_init_events(dev_info_t *dip);

int
i_ddi_rootnex_get_eventcookie(dev_info_t *dip, dev_info_t *rdip,
	char *eventname, ddi_eventcookie_t *cookiep);
int
i_ddi_rootnex_add_eventcall(dev_info_t *dip, dev_info_t *rdip,
	ddi_eventcookie_t eventid, void (*handler)(dev_info_t *dip,
	ddi_eventcookie_t event, void *arg, void *impl_data), void *arg,
	ddi_callback_id_t *cb_id);
int
i_ddi_rootnex_remove_eventcall(dev_info_t *devi, ddi_callback_id_t cb_id);

int
i_ddi_rootnex_post_event(dev_info_t *dip, dev_info_t *rdip,
	ddi_eventcookie_t eventid, void *impl_data);

/*
 * Clustering: Return the global devices path base, or
 * the entire global devices path prefix.
 */
const char *
i_ddi_get_dpath_base();

const char *
i_ddi_get_dpath_prefix();

/*
 * Search and return properties from the PROM
 */
int
impl_ddi_bus_prop_op(dev_t dev, dev_info_t *dip,
	dev_info_t *ch_dip, ddi_prop_op_t prop_op, int mod_flags,
	char *name, caddr_t valuep, int *lengthp);

/*
 * Copy an integer from PROM to native machine representation
 */
int
impl_ddi_prop_int_from_prom(uchar_t *intp, int n);


extern int impl_ddi_sunbus_initchild(dev_info_t *);
extern void impl_ddi_sunbus_removechild(dev_info_t *);

/*
 * Implementation specific access handle allocator and init. routines
 */
extern ddi_acc_handle_t impl_acc_hdl_alloc(int (*waitfp)(caddr_t),
	caddr_t arg);
extern void impl_acc_hdl_free(ddi_acc_handle_t handle);

extern ddi_acc_hdl_t *impl_acc_hdl_get(ddi_acc_handle_t handle);
extern void impl_acc_hdl_init(ddi_acc_hdl_t *hp);

/* access error handling support */
extern void impl_acc_err_init(ddi_acc_hdl_t *);
extern int i_ddi_ontrap(ddi_acc_handle_t);
extern void i_ddi_notrap(ddi_acc_handle_t);
extern int i_ddi_prot_trampoline();
extern int i_ddi_caut_trampoline();

/*
 * misc/bootdev entry points - these are private routines and subject
 * to change.
 */
extern int
i_devname_to_promname(char *dev_name, char *ret_buf, size_t);

extern int
i_promname_to_devname(char *prom_name, char *ret_buf);

extern char *
i_convert_boot_device_name(char *, char *, size_t *);

/*
 * Nodeid management ...
 */
void impl_ddi_init_nodeid(void);
int impl_ddi_alloc_nodeid(int *nodeid);
int impl_ddi_take_nodeid(int nodeid, int kmflag);
void impl_ddi_free_nodeid(int nodeid);

/*
 * minorname/devtspectype conversions
 */
extern char *
i_ddi_devtspectype_to_minorname(dev_info_t *dip, dev_t dev, int spec_type);

extern int
i_ddi_minorname_to_devtspectype(dev_info_t *dip, char *minor_name,
	dev_t *devp, int *spectypep);


/*
 * Helper functions
 */
char *i_ddi_strdup(char *, uint_t);
void i_ddi_prop_list_delete(ddi_prop_t *);
ddi_prop_t *i_ddi_prop_list_dup(ddi_prop_t *, uint_t);
int i_ddi_load_drvconf(major_t);
int i_ddi_unload_drvconf(major_t);
ddi_node_state_t i_ddi_node_state(dev_info_t *);

/*
 * Routines in ddi_v9_asm.s
 */
extern int do_peek(size_t, void *, void *);
extern int do_poke(size_t, void *, void *);
extern void peek_fault(void);
extern void poke_fault(void);

extern int peekpoke_mem(ddi_ctl_enum_t, peekpoke_ctlops_t *);

char *i_ddi_strdup(char *, uint_t);
void i_ddi_parse_name(char *, char **, char **, char **);
ddi_node_state_t i_ddi_node_state(dev_info_t *);
void i_ddi_set_node_state(dev_info_t *, ddi_node_state_t);
int i_ddi_detach_installed_driver(major_t, int);
int i_ddi_load_drvconf(major_t);
int i_ddi_unload_drvconf(major_t);
void i_ddi_set_binding_name(dev_info_t *, char *);
void i_ddi_bind_devs();
void i_ddi_unbind_devs(major_t);
ddi_prop_t *i_ddi_prop_list_dup(ddi_prop_t *, uint_t);
void i_ddi_prop_list_delete(ddi_prop_t *);
ddi_prop_list_t *i_ddi_prop_list_create(ddi_prop_t *);
struct devnames;
void i_ddi_prop_list_hold(ddi_prop_list_t *, struct devnames *);
void i_ddi_prop_list_rele(ddi_prop_list_t *, struct devnames *);
ddi_prop_t *i_ddi_prop_search(dev_t, char *, uint_t, ddi_prop_t **);
int resolve_pathname(char *, dev_info_t **, dev_t *, int *);
int i_ddi_prompath_to_devfspath(char *, char *);
int i_ddi_attach_node_hierarchy(dev_info_t *);
dev_info_t *i_ddi_attach_pseudo_node(char *);
int i_ddi_attach_hw_nodes(char *);
int i_ddi_devs_attached(major_t);

/* non-DDI functions: wrapper around mod_hold/rele_dev_by_major() */
struct dev_ops *ddi_hold_driver(major_t);
void ddi_rele_driver(major_t);

/*
 * devid cache
 */
void i_ddi_devices_init(void);
void i_ddi_read_devices_files(void);

int i_ddi_devi_get_devid(dev_t, dev_info_t *, ddi_devid_t *);

int e_ddi_devid_discovery(ddi_devid_t);

int e_devid_cache_register(dev_info_t *, ddi_devid_t);
void e_devid_cache_unregister(dev_info_t *);
void e_devid_cache_cleanup(void);

int e_devid_cache_to_devt_list(ddi_devid_t, char *, int *, dev_t **);
void e_devid_cache_free_devt_list(int, dev_t *);

/*
 * Resource control functions to lock down device memory.
 */
extern int i_ddi_incr_locked_memory(proc_t *, task_t *, kproject_t *,
	zone_t *, rctl_qty_t);
extern void i_ddi_decr_locked_memory(proc_t *, task_t *, kproject_t *,
	zone_t *, rctl_qty_t);

#endif	/* _KERNEL */

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_DDI_IMPLFUNCS_H */
