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

#ifndef _SYS_ZONE_H
#define	_SYS_ZONE_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include <sys/types.h>
#include <sys/mutex.h>
#include <sys/param.h>
#include <sys/rctl.h>
#include <sys/pset.h>

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * NOTE
 *
 * The contents of this file are private to the implementation of
 * Solaris and are subject to change at any time without notice.
 * Applications and drivers using these interfaces may fail to
 * run on future releases.
 */

/* Available both in kernel and for user space */

/* zone id restrictions and special ids */
#define	MAX_ZONEID	9999
#define	MIN_USERZONEID	1	/* lowest user-creatable zone ID */
#define	MIN_ZONEID	0	/* minimum zone ID on system */
#define	GLOBAL_ZONEID	0
#define	ZONEID_WIDTH	4	/* for printf */

/* system call subcodes */
#define	ZONE_CREATE	0
#define	ZONE_DESTROY	1
#define	ZONE_GETATTR	2
#define	ZONE_ENTER	3
#define	ZONE_LIST	4
#define	ZONE_SHUTDOWN	5
#define	ZONE_LOOKUP	6
#define	ZONE_BOOT	7

/* zone attributes */
#define	ZONE_ATTR_ROOT		1
#define	ZONE_ATTR_NAME		2
#define	ZONE_ATTR_STATUS	3
#define	ZONE_ATTR_PRIVSET	4
#define	ZONE_ATTR_UNIQID	5
#define	ZONE_ATTR_POOLID	6
#define	ZONE_ATTR_INITPID	7

#ifdef _SYSCALL32
typedef struct {
	caddr32_t zone_name;
	caddr32_t zone_root;
	caddr32_t zone_privs;
	caddr32_t rctlbuf;
	size32_t rctlbufsz;
	caddr32_t extended_error;
} zone_def32;
#endif
typedef struct {
	const char *zone_name;
	const char *zone_root;
	const struct priv_set *zone_privs;
	const char *rctlbuf;
	size_t rctlbufsz;
	int *extended_error;
} zone_def;

/* extended error information */
#define	ZE_UNKNOWN	0	/* No extended error info */
#define	ZE_CHROOTED	1	/* tried to zone_create from chroot */
#define	ZE_AREMOUNTS	2	/* there are mounts within the zone */

/* zone_status */
typedef enum {
	ZONE_IS_UNINITIALIZED = 0,
	ZONE_IS_READY,
	ZONE_IS_BOOTING,
	ZONE_IS_RUNNING,
	ZONE_IS_SHUTTING_DOWN,
	ZONE_IS_EMPTY,
	ZONE_IS_DOWN,
	ZONE_IS_DYING,
	ZONE_IS_DEAD
} zone_status_t;
#define	ZONE_MIN_STATE		ZONE_IS_UNINITIALIZED
#define	ZONE_MAX_STATE		ZONE_IS_DEAD

/*
 * Valid commands which may be issued by zoneadm to zoneadmd.  The kernel also
 * communicates with zoneadmd, but only uses Z_REBOOT and Z_HALT.
 */
typedef enum zone_cmd {
	Z_READY, Z_BOOT, Z_REBOOT, Z_HALT, Z_NOTE_UNINSTALLING
} zone_cmd_t;

#define	ZONEBOOTARGS_MAX	257	/* uadmin()'s buffer is 257 bytes. */

/*
 * The structure of a request to zoneadmd.
 */
typedef struct zone_cmd_arg {
	uint64_t	uniqid;		/* unique "generation number" */
	zone_cmd_t	cmd;		/* requested action */
	char locale[MAXPATHLEN];	/* locale in which to render messages */
	char bootbuf[ZONEBOOTARGS_MAX];	/* arguments passed to zone_boot() */
} zone_cmd_arg_t;

/*
 * Structure of zoneadmd's response to a request.  A NULL return value means
 * the caller should attempt to restart zoneadmd and retry.
 */
typedef struct zone_cmd_rval {
	int rval;			/* return value of request */
	char errbuf[1];	/* variable-sized buffer containing error messages */
} zone_cmd_rval_t;

/*
 * The zone support infrastructure uses the zone name as a component
 * of unix domain (AF_UNIX) sockets, which are limited to 108 characters
 * in length, so ZONENAME_MAX is limited by that.
 */
#define	ZONENAME_MAX		64

#define	GLOBAL_ZONENAME		"global"

/*
 * Extended Regular expression (see regex(5)) which matches all valid zone
 * names.
 */
#define	ZONENAME_REGEXP		"[a-zA-Z0-9][-_.a-zA-Z0-9]{0,62}"

/*
 * Where the zones support infrastructure places temporary files.
 */
#define	ZONES_TMPDIR		"/var/run/zones"

/*
 * The path to the door used by clients to communicate with zoneadmd.
 */
#define	ZONE_DOOR_PATH		ZONES_TMPDIR "/%s.zoneadmd_door"


#ifdef _KERNEL
/*
 * We need to protect the definition of 'list_t' from userland applications and
 * libraries which may be defining ther own versions.
 */
#include <sys/list.h>

#define	GLOBAL_ZONEUNIQID	0	/* uniqid of the global zone */

/* zone_flags */
#define	ZF_DESTROYED		0x1	/* ZSD destructor callbacks run */

struct pool;

typedef struct zone {
	/*
	 * zone_name is never modified once set.
	 */
	char		*zone_name;	/* zone's configuration name */
	/*
	 * zone_nodename and zone_domain are never freed once allocated.
	 */
	char		*zone_nodename;	/* utsname.nodename equivalent */
	char		*zone_domain;	/* srpc_domain equivalent */
	/*
	 * zone_lock protects the following fields of a zone_t:
	 * 	zone_ref
	 * 	zone_cred_ref
	 * 	zone_ntasks
	 * 	zone_flags
	 * 	zone_zsd
	 */
	kmutex_t	zone_lock;
	/*
	 * zone_linkage is the zone's linkage into the active or
	 * death-row list.  The field is protected by zonehash_lock.
	 */
	list_node_t	zone_linkage;
	zoneid_t	zone_id;	/* ID of zone */
	uint_t		zone_ref;	/* count of zone_hold()s on zone */
	uint_t		zone_cred_ref;	/* count of zone_hold_cred()s on zone */
	/*
	 * zone_rootvp and zone_rootpath can never be modified once set.
	 */
	struct vnode	*zone_rootvp;	/* zone's root vnode */
	char		*zone_rootpath;	/* Path to zone's root + '/' */
	ushort_t	zone_flags;	/* misc flags */
	zone_status_t	zone_status;	/* protected by zone_status_lock */
	uint_t		zone_ntasks;	/* number of tasks executing in zone */
	kmutex_t	zone_nlwps_lock; /* protects zone_nlwps, and *_nlwps */
					/* counters in projects and tasks */
					/* that are within the zone */
	rctl_qty_t	zone_nlwps;	/* number of lwps in zone */
	rctl_qty_t	zone_nlwps_ctl; /* protected by zone_rctls->rcs_lock */

	uint_t		zone_rootpathlen; /* strlen(zone_rootpath) + 1 */
	uint32_t	zone_shares;	/* FSS shares allocated to zone */
	rctl_set_t	*zone_rctls;	/* zone-wide (zone.*) rctls */
	list_t		zone_zsd;	/* list of Zone-Specific Data values */
	kcondvar_t	zone_cv;	/* used to signal state changes */
	struct proc	*zone_zsched;	/* Dummy kernel "zsched" process */
	pid_t		zone_proc_initpid; /* pid of "init" for this zone */
	int		zone_boot_err;  /* for zone_boot() if boot fails */
	char		*zone_bootargs;	/* arguments passed via zone_boot() */
	/*
	 * zone_kthreads is protected by zone_status_lock.
	 */
	kthread_t	*zone_kthreads;	/* kernel threads in zone */
	struct priv_set	*zone_privset;	/* limit set for zone */
	/*
	 * zone_vfslist is protected by vfs_list_lock().
	 */
	struct vfs	*zone_vfslist;	/* list of FS's mounted in zone */
	uint64_t	zone_uniqid;	/* unique zone generation number */
	struct cred	*zone_kcred;	/* kcred-like, zone-limited cred */
	/*
	 * zone_pool is protected by pool_lock().
	 */
	struct pool	*zone_pool;	/* pool the zone is bound to */
	hrtime_t	zone_pool_mod;	/* last pool bind modification time */
	/* zone_psetid is protected by cpu_lock */
	psetid_t	zone_psetid;	/* pset the zone is bound to */
	/*
	 * The following two can be read without holding any locks.  They are
	 * updated under cpu_lock.
	 */
	int		zone_ncpus;  /* zone's idea of ncpus */
	int		zone_ncpus_online; /* zone's idea of ncpus_online */
} zone_t;

/*
 * Special value of zone_psetid to indicate that pools are disabled.
 */
#define	ZONE_PS_INVAL	PS_MYID

extern zone_t zone0;
extern zone_t *global_zone;
extern uint_t maxzones;
extern rctl_hndl_t rc_zone_nlwps;

extern const char * const zone_initname;

extern long zone(int, void *, void *, void *, void *, void *);
extern void zone_zsd_init(void);
extern void zone_init(void);
extern void zone_hold(zone_t *);
extern void zone_rele(zone_t *);
extern void zone_cred_hold(zone_t *);
extern void zone_cred_rele(zone_t *);
extern void zone_task_hold(zone_t *);
extern void zone_task_rele(zone_t *);
extern zone_t *zone_find_by_id(zoneid_t);
extern zone_t *zone_find_by_name(char *);
extern zone_t *zone_find_by_path(const char *);
extern zoneid_t getzoneid(void);

/*
 * Zone-specific data (ZSD) APIs
 */
/*
 * The following is what code should be initializing its zone_key_t to if it
 * calls zone_getspecific() without necessarily knowing that zone_key_create()
 * has been called on the key.
 */
#define	ZONE_KEY_UNINITIALIZED	0

typedef uint_t zone_key_t;

extern void	zone_key_create(zone_key_t *, void *(*)(zoneid_t),
    void (*)(zoneid_t, void *), void (*)(zoneid_t, void *));
extern int 	zone_key_delete(zone_key_t);
extern void	*zone_getspecific(zone_key_t, zone_t *);
extern int	zone_setspecific(zone_key_t, zone_t *, const void *);

/*
 * The definition of a zsd_entry is truly private to zone.c and is only
 * placed here so it can be shared with mdb.
 */
struct zsd_entry {
	zone_key_t		zsd_key;	/* Key used to lookup value */
	void			*zsd_data;	/* Caller-managed value */
	/*
	 * Callbacks to be executed when a zone is created, shutdown, and
	 * destroyed, respectively.
	 */
	void			*(*zsd_create)(zoneid_t);
	void			(*zsd_shutdown)(zoneid_t, void *);
	void			(*zsd_destroy)(zoneid_t, void *);
	list_node_t		zsd_linkage;
};

/*
 * Macros to help with zone visibility restrictions.
 */

/*
 * Is process in the global zone?
 */
#define	INGLOBALZONE(p) \
	((p)->p_zone == global_zone)

/*
 * Can process view objects in given zone?
 */
#define	HASZONEACCESS(p, zoneid) \
	((p)->p_zone->zone_id == (zoneid) || INGLOBALZONE(p))

/*
 * Convenience macro to see if a resolved path is visible from within a
 * given zone.
 *
 * The basic idea is that the first (zone_rootpathlen - 1) bytes of the
 * two strings must be equal.  Since the rootpathlen has a trailing '/',
 * we want to skip everything in the path up to (but not including) the
 * trailing '/'.
 */
#define	ZONE_PATH_VISIBLE(path, zone) \
	(strncmp((path), (zone)->zone_rootpath,		\
	    (zone)->zone_rootpathlen - 1) == 0)

/*
 * Convenience macro to go from the global view of a path to that seen
 * from within said zone.  It is the responsibility of the caller to
 * ensure that the path is a resolved one (ie, no '..'s or '.'s), and is
 * in fact visible from within the zone.
 */
#define	ZONE_PATH_TRANSLATE(path, zone)	\
	(ASSERT(ZONE_PATH_VISIBLE(path, zone)),	\
	(path) + (zone)->zone_rootpathlen - 2)

/*
 * Special processes visible in all zones.
 */
#define	ZONE_SPECIALPID(x)	 ((x) == 0 || (x) == 1)

/*
 * Special zoneid_t token to refer to all zones.
 */
#define	ALL_ZONES	(-1)

/*
 * Zone-safe version of thread_create() to be used when the caller wants to
 * create a kernel thread to run within the current zone's context.
 */
extern kthread_t *zthread_create(caddr_t, size_t, void (*)(), void *, size_t,
    pri_t);
extern void zthread_exit(void);

/*
 * Functions for an external observer to register interest in a zone's status
 * change.  Observers will be woken up when the zone status equals the status
 * argument passed in (in the case of zone_status_timedwait, the function may
 * also return because of a timeout; zone_status_wait_sig may return early due
 * to a signal being delivered; zone_status_timedwait_sig may return for any of
 * the above reasons).
 *
 * Otherwise these behave identically to cv_timedwait(), cv_wait(), and
 * cv_wait_sig() respectively.
 */
extern clock_t zone_status_timedwait(zone_t *, clock_t, zone_status_t);
extern clock_t zone_status_timedwait_sig(zone_t *, clock_t, zone_status_t);
extern void zone_status_wait(zone_t *, zone_status_t);
extern int zone_status_wait_sig(zone_t *, zone_status_t);

/*
 * Get the status  of the zone (at the time it was called).  The state may
 * have progressed by the time it is returned.
 */
extern zone_status_t zone_status_get(zone_t *);

/*
 * Get the "kcred" credentials corresponding to the given zone.
 */
extern struct cred *zone_get_kcred(zoneid_t);

/*
 * Get/set the pool the zone is currently bound to.
 */
extern struct pool *zone_pool_get(zone_t *);
extern void zone_pool_set(zone_t *, struct pool *);

/*
 * Get/set the pset the zone is currently using.
 */
extern psetid_t zone_pset_get(zone_t *);
extern void zone_pset_set(zone_t *, psetid_t);

/*
 * Get the number of cpus/online-cpus visible from the given zone.
 */
extern int zone_ncpus_get(zone_t *);
extern int zone_ncpus_online_get(zone_t *);

/*
 * zone version of uadmin()
 */
extern int zone_uadmin(int, int, struct cred *);
extern void zone_shutdown_global(void);

extern void mount_in_progress(void);
extern void mount_completed(void);

extern int zone_walk(int (*)(zone_t *, void *), void *);

#endif	/* _KERNEL */

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_ZONE_H */
