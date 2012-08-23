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

#ifndef _LIBZONECFG_H
#define	_LIBZONECFG_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

/*
 * Zone configuration header file.
 */

#ifdef __cplusplus
extern "C" {
#endif

/* sys/socket.h is required by net/if.h, which has a constant needed here */
#include <sys/param.h>
#include <sys/fstyp.h>
#include <sys/mount.h>
#include <priv.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <net/if.h>
#include <stdio.h>
#include <rctl.h>
#include <zone.h>

#define	ZONE_ID_UNDEFINED	-1

#define	Z_OK			0
#define	Z_EMPTY_DOCUMENT	1	/* XML doc root element is null */
#define	Z_WRONG_DOC_TYPE	2	/* top-level XML doc element != zone */
#define	Z_BAD_PROPERTY		3	/* libxml-level property problem */
#define	Z_TEMP_FILE		4	/* problem creating temporary file */
#define	Z_SAVING_FILE		5	/* libxml error saving or validating */
#define	Z_NO_ENTRY		6	/* no such entry */
#define	Z_BOGUS_ZONE_NAME	7	/* illegal zone name */
#define	Z_REQD_RESOURCE_MISSING	8	/* required resource missing */
#define	Z_REQD_PROPERTY_MISSING	9	/* required property missing */
#define	Z_BAD_HANDLE		10	/* bad document handle */
#define	Z_NOMEM			11	/* out of memory (like ENOMEM) */
#define	Z_INVAL			12	/* invalid argument (like EINVAL) */
#define	Z_ACCES			13	/* permission denied (like EACCES) */
#define	Z_TOO_BIG		14	/* string won't fit in char array */
#define	Z_MISC_FS		15	/* miscellaneous file-system error */
#define	Z_NO_ZONE		16	/* no such zone */
#define	Z_NO_RESOURCE_TYPE	17	/* no/wrong resource type */
#define	Z_NO_RESOURCE_ID	18	/* no/wrong resource id */
#define	Z_NO_PROPERTY_TYPE	19	/* no/wrong property type */
#define	Z_NO_PROPERTY_ID	20	/* no/wrong property id */
#define	Z_RESOURCE_EXISTS	21	/* resource already exists */
#define	Z_INVALID_DOCUMENT	22	/* libxml can't validate against DTD */
#define	Z_ID_IN_USE		23	/* add_index conflict */
#define	Z_NO_SUCH_ID		24	/* delete_index: no old ID */
#define	Z_UPDATING_INDEX	25	/* add/modify/delete_index problem */
#define	Z_LOCKING_FILE		26	/* problem locking index file */
#define	Z_UNLOCKING_FILE	27	/* problem unlocking index file */
#define	Z_SYSTEM		28	/* consult errno instead */
#define	Z_INSUFFICIENT_SPEC	29	/* resource insufficiently specified */
#define	Z_RESOLVED_PATH		34	/* resolved path mismatch */
#define	Z_IPV6_ADDR_PREFIX_LEN	35	/* IPv6 address prefix length needed */
#define	Z_BOGUS_ADDRESS		36	/* not IPv[4|6] address or host name */

#define	ZONE_STATE_CONFIGURED		0
#define	ZONE_STATE_INCOMPLETE		1
#define	ZONE_STATE_INSTALLED		2
#define	ZONE_STATE_READY		3
#define	ZONE_STATE_RUNNING		4
#define	ZONE_STATE_SHUTTING_DOWN	5
#define	ZONE_STATE_DOWN			6

#define	ZONE_STATE_MAXSTRLEN	14

#define	LIBZONECFG_PATH		"libzonecfg.so.1"

#define	ZONE_CONFIG_ROOT	"/etc/zones"
#define	ZONE_INDEX_FILE		ZONE_CONFIG_ROOT "/index"

/*
 * The integer field expresses the current values on a get.
 * On a put, it represents the new values if >= 0 or "don't change" if < 0.
 */
struct zoneent {
	char	zone_name[ZONENAME_MAX];	/* name of the zone */
	int	zone_state;	/* configured | incomplete | installed */
	char	zone_path[MAXPATHLEN];
};

typedef struct zone_dochandle *zone_dochandle_t;	/* opaque handle */

typedef uint_t zone_state_t;

typedef struct zone_fsopt {
	struct zone_fsopt *zone_fsopt_next;
	char		   zone_fsopt_opt[MAX_MNTOPT_STR];
} zone_fsopt_t;

struct zone_fstab {
	char		zone_fs_special[MAXPATHLEN]; 	/* special file */
	char		zone_fs_dir[MAXPATHLEN];	/* mount point */
	char		zone_fs_type[FSTYPSZ];		/* e.g. ufs */
	zone_fsopt_t   *zone_fs_options;		/* mount options */
	char		zone_fs_raw[MAXPATHLEN];	/* device to fsck */
};

struct zone_nwiftab {
	char	zone_nwif_address[INET6_ADDRSTRLEN];
	char	zone_nwif_physical[LIFNAMSIZ];
};

struct zone_devtab {
	char	zone_dev_match[MAXPATHLEN];
};

struct zone_rctlvaltab {
	char	zone_rctlval_priv[MAXNAMELEN];
	char	zone_rctlval_limit[MAXNAMELEN];
	char	zone_rctlval_action[MAXNAMELEN];
	struct zone_rctlvaltab *zone_rctlval_next;
};

struct zone_rctltab {
	char	zone_rctl_name[MAXNAMELEN];
	struct zone_rctlvaltab *zone_rctl_valptr;
};

struct zone_attrtab {
	char	zone_attr_name[MAXNAMELEN];
	char	zone_attr_type[MAXNAMELEN];
	char	zone_attr_value[2 * BUFSIZ];
};

/*
 * Basic configuration management routines.
 */
extern	zone_dochandle_t	zonecfg_init_handle(void);
extern	int	zonecfg_get_handle(char *, zone_dochandle_t);
extern	int	zonecfg_get_snapshot_handle(char *, zone_dochandle_t);
extern	int	zonecfg_check_handle(zone_dochandle_t);
extern	void	zonecfg_fini_handle(zone_dochandle_t);
extern	int	zonecfg_destroy(const char *);
extern	int	zonecfg_destroy_snapshot(char *);
extern	int	zonecfg_save(zone_dochandle_t);
extern	int	zonecfg_create_snapshot(char *);
extern	char	*zonecfg_strerror(int);
extern	int	zonecfg_access(const char *, int);

/*
 * Zone name, path to zone directory, autoboot setting and pool.
 */
extern	int	zonecfg_get_name(zone_dochandle_t, char *, size_t);
extern	int	zonecfg_set_name(zone_dochandle_t, char *);
extern	int	zonecfg_get_zonepath(zone_dochandle_t, char *, size_t);
extern	int	zonecfg_set_zonepath(zone_dochandle_t, char *);
extern	int	zonecfg_get_autoboot(zone_dochandle_t, boolean_t *);
extern	int	zonecfg_set_autoboot(zone_dochandle_t, boolean_t);
extern	int	zonecfg_get_pool(zone_dochandle_t, char *, size_t);
extern	int	zonecfg_set_pool(zone_dochandle_t, char *);

/*
 * Filesystem configuration.
 */
extern	int	zonecfg_add_filesystem(zone_dochandle_t, struct zone_fstab *);
extern	int	zonecfg_delete_filesystem(zone_dochandle_t,
    struct zone_fstab *);
extern	int	zonecfg_modify_filesystem(zone_dochandle_t,
    struct zone_fstab *, struct zone_fstab *);
extern	int	zonecfg_lookup_filesystem(zone_dochandle_t,
    struct zone_fstab *);
extern	int	zonecfg_add_ipd(zone_dochandle_t, struct zone_fstab *);
extern	int	zonecfg_delete_ipd(zone_dochandle_t, struct zone_fstab *);
extern	int	zonecfg_modify_ipd(zone_dochandle_t,
    struct zone_fstab *, struct zone_fstab *);
extern	int	zonecfg_lookup_ipd(zone_dochandle_t, struct zone_fstab *);
extern	int	zonecfg_add_fs_option(struct zone_fstab *, char *);
extern	int	zonecfg_remove_fs_option(struct zone_fstab *, char *);
extern	void	zonecfg_free_fs_option_list(zone_fsopt_t *);
extern	int 	zonecfg_find_mounts(char *, int(*)(const char *, void *),
    void *);

/*
 * Network interface configuration.
 */
extern	int	zonecfg_add_nwif(zone_dochandle_t, struct zone_nwiftab *);
extern	int	zonecfg_delete_nwif(zone_dochandle_t, struct zone_nwiftab *);
extern	int	zonecfg_modify_nwif(zone_dochandle_t, struct zone_nwiftab *,
    struct zone_nwiftab *);
extern	int	zonecfg_lookup_nwif(zone_dochandle_t, struct zone_nwiftab *);

/*
 * Device configuration and rule matching.
 */
extern	int	zonecfg_add_dev(zone_dochandle_t, struct zone_devtab *);
extern	int	zonecfg_delete_dev(zone_dochandle_t, struct zone_devtab *);
extern	int	zonecfg_modify_dev(zone_dochandle_t, struct zone_devtab *,
    struct zone_devtab *);
extern	int	zonecfg_lookup_dev(zone_dochandle_t, struct zone_devtab *);
extern	int	zonecfg_match_dev(zone_dochandle_t, char *,
    struct zone_devtab *);

/*
 * Resource control configuration.
 */
extern	int	zonecfg_add_rctl(zone_dochandle_t, struct zone_rctltab *);
extern	int	zonecfg_delete_rctl(zone_dochandle_t, struct zone_rctltab *);
extern	int	zonecfg_modify_rctl(zone_dochandle_t, struct zone_rctltab *,
    struct zone_rctltab *);
extern	int	zonecfg_lookup_rctl(zone_dochandle_t, struct zone_rctltab *);
extern	int	zonecfg_add_rctl_value(struct zone_rctltab *,
    struct zone_rctlvaltab *);
extern	int	zonecfg_remove_rctl_value(struct zone_rctltab *,
    struct zone_rctlvaltab *);
extern	void	zonecfg_free_rctl_value_list(struct zone_rctlvaltab *);

/*
 * Generic attribute configuration and type/value extraction.
 */
extern	int	zonecfg_add_attr(zone_dochandle_t, struct zone_attrtab *);
extern	int	zonecfg_delete_attr(zone_dochandle_t, struct zone_attrtab *);
extern	int	zonecfg_modify_attr(zone_dochandle_t, struct zone_attrtab *,
    struct zone_attrtab *);
extern	int	zonecfg_lookup_attr(zone_dochandle_t, struct zone_attrtab *);
extern	int	zonecfg_get_attr_boolean(const struct zone_attrtab *,
    boolean_t *);
extern	int	zonecfg_get_attr_int(const struct zone_attrtab *, int64_t *);
extern	int	zonecfg_get_attr_string(const struct zone_attrtab *, char *,
    size_t);
extern	int	zonecfg_get_attr_uint(const struct zone_attrtab *, uint64_t *);

/*
 * '*ent' iterator routines.
 */
extern	int	zonecfg_setfsent(zone_dochandle_t);
extern	int	zonecfg_getfsent(zone_dochandle_t, struct zone_fstab *);
extern	int	zonecfg_endfsent(zone_dochandle_t);
extern	int	zonecfg_setipdent(zone_dochandle_t);
extern	int	zonecfg_getipdent(zone_dochandle_t, struct zone_fstab *);
extern	int	zonecfg_endipdent(zone_dochandle_t);
extern	int	zonecfg_setnwifent(zone_dochandle_t);
extern	int	zonecfg_getnwifent(zone_dochandle_t, struct zone_nwiftab *);
extern	int	zonecfg_endnwifent(zone_dochandle_t);
extern	int	zonecfg_setdevent(zone_dochandle_t);
extern	int	zonecfg_getdevent(zone_dochandle_t, struct zone_devtab *);
extern	int	zonecfg_enddevent(zone_dochandle_t);
extern	int	zonecfg_setattrent(zone_dochandle_t);
extern	int	zonecfg_getattrent(zone_dochandle_t, struct zone_attrtab *);
extern	int	zonecfg_endattrent(zone_dochandle_t);
extern	int	zonecfg_setrctlent(zone_dochandle_t);
extern	int	zonecfg_getrctlent(zone_dochandle_t, struct zone_rctltab *);
extern	int	zonecfg_endrctlent(zone_dochandle_t);

/*
 * Privilege-related functions.
 */
extern	int	zonecfg_get_privset(priv_set_t *);

/*
 * Index update routines.
 */
extern	int	zonecfg_add_index(char *, char *);
extern	int	zonecfg_delete_index(char *);

/*
 * Higher-level routines.
 */
extern	int	zone_get_rootpath(char *, char *, size_t);
extern	int	zone_get_zonepath(char *, char *, size_t);
extern	int	zone_get_state(char *, zone_state_t *);
extern	int	zone_set_state(char *, zone_state_t);
extern	char	*zone_state_str(zone_state_t);

/*
 * Iterator for configured zones.
 */
extern FILE		*setzoneent(void);
extern char		*getzoneent(FILE *);
extern struct zoneent	*getzoneent_private(FILE *);
extern void		endzoneent(FILE *);

/*
 * File-system-related convenience functions.
 */
extern boolean_t zonecfg_valid_fs_type(const char *);

/*
 * Network-related convenience functions.
 */
extern boolean_t zonecfg_same_net_address(char *, char *);
extern int zonecfg_valid_net_address(char *, struct lifreq *);

/*
 * Rctl-related common functions.
 */
extern boolean_t zonecfg_is_rctl(const char *);
extern boolean_t zonecfg_valid_rctlname(const char *);
extern boolean_t zonecfg_valid_rctlblk(const rctlblk_t *);
extern boolean_t zonecfg_valid_rctl(const char *, const rctlblk_t *);
extern int zonecfg_construct_rctlblk(const struct zone_rctlvaltab *,
    rctlblk_t *);

#ifdef __cplusplus
}
#endif

#endif	/* _LIBZONECFG_H */
