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

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

#ifndef	_NFS_EXPORT_H
#define	_NFS_EXPORT_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include <nfs/nfs_sec.h>
#include <rpcsvc/nfsauth_prot.h>
#include <sys/vnode.h>
#include <nfs/nfs4.h>

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * nfs pseudo flavor number is owned by IANA. Need to make sure the
 * Solaris specific NFS_FLAVOR_NOMAP number will not overlap with any
 * new IANA defined pseudo flavor numbers. The chance for the overlap
 * is very small since the growth of new flavor numbers is expected
 * to be limited.
 */
#define	NFS_FLAVOR_NOMAP	999999	/* no nfs flavor mapping */

/*
 * Note: exported_lock is currently used to ensure the integrity of
 * the secinfo fields.
 */
struct secinfo {
	seconfig_t	s_secinfo;	/* /etc/nfssec.conf entry */
	unsigned int	s_flags;	/* flags (see below) */
	uint32_t	s_refcnt;	/* reference count for tracking */
					/* how many children (self included) */
					/* use this flavor. */
	int 		s_window;	/* window */
	int		s_rootcnt;	/* count of root names */
	caddr_t		*s_rootnames;	/* array of root names */
					/* they are strings for AUTH_DES and */
					/* rpc_gss_principal_t for RPCSEC_GSS */
};

#ifdef _SYSCALL32
struct secinfo32 {
	seconfig32_t	s_secinfo;	/* /etc/nfssec.conf entry */
	uint32_t	s_flags;	/* flags (see below) */
	uint32_t	s_refcnt;	/* reference count for tracking */
					/* how many children (self included) */
					/* use this flavor. */
	int32_t 	s_window;	/* window */
	int32_t		s_rootcnt;	/* count of root names */
	caddr32_t	s_rootnames;	/* array of root names */
					/* they are strings for AUTH_DES and */
					/* rpc_gss_principal_t for RPCSEC_GSS */
};
#endif /* _SYSCALL32 */

/*
 * security negotiation related
 */

#define	SEC_QUERY	0x01	/* query sec modes */

struct sec_ol {
	int		sec_flags;	/* security nego flags */
	uint_t		sec_index;	/* index into sec flavor array */
};

/*
 * Per-mode flags (secinfo.s_flags)
 */
#define	M_RO		0x01	/* exported ro to all */
#define	M_ROL		0x02	/* exported ro to all listed */
#define	M_RW		0x04	/* exported rw to all */
#define	M_RWL		0x08	/* exported ro to all listed */
#define	M_ROOT		0x10	/* root list is defined */
#define	M_4SEC_EXPORTED	0x20	/* this is an explicitly shared flavor */

/* invalid secinfo reference count */
#define	SEC_REF_INVALID(p) ((p)->s_refcnt < 1)

/* last secinfo reference */
#define	SEC_REF_LAST(p) ((p)->s_refcnt == 1)

/* sec flavor explicitly shared for the exported node */
#define	SEC_REF_EXPORTED(p) ((p)->s_flags & M_4SEC_EXPORTED)

/* the only reference count left is for referring itself */
#define	SEC_REF_SELF(p) (SEC_REF_LAST(p) && SEC_REF_EXPORTED(p))

/*
 * The export information passed to exportfs() (Version 2)
 */
#define	EX_CURRENT_VERSION 2	/* current version of exportdata struct */

struct exportdata {
	int		ex_version;	/* structure version */
	char		*ex_path;	/* exported path */
	size_t		ex_pathlen;	/* path length */
	int		ex_flags;	/* flags */
	unsigned int	ex_anon;	/* uid for unauthenticated requests */
	int		ex_seccnt;	/* count of security modes */
	struct secinfo	*ex_secinfo;	/* security mode info */
	char		*ex_index;	/* index file for public filesystem */
	char		*ex_log_buffer;	/* path to logging buffer file */
	size_t		ex_log_bufferlen;	/* buffer file path len */
	char		*ex_tag;	/* tag used to identify log config */
	size_t		ex_taglen;	/* tag length */
};

#ifdef _SYSCALL32
struct exportdata32 {
	int32_t		ex_version;	/* structure version */
	caddr32_t	ex_path;	/* exported path */
	int32_t		ex_pathlen;	/* path length */
	int32_t		ex_flags;	/* flags */
	uint32_t	ex_anon;	/* uid for unauthenticated requests */
	int32_t		ex_seccnt;	/* count of security modes */
	caddr32_t	ex_secinfo;	/* security mode info */
	caddr32_t	ex_index;	/* index file for public filesystem */
	caddr32_t	ex_log_buffer;	/* path to logging buffer file */
	int32_t		ex_log_bufferlen;	/* buffer file path len */
	caddr32_t	ex_tag;		/* tag used to identify log config */
	int32_t		ex_taglen;	/* tag length */
};
#endif /* _SYSCALL32 */

/*
 * exported vfs flags.
 */

#define	EX_NOSUID	0x01	/* exported with unsetable set[ug]ids */
#define	EX_ACLOK	0x02	/* exported with maximal access if acl exists */
#define	EX_PUBLIC	0x04	/* exported with public filehandle */
#define	EX_NOSUB	0x08	/* no nfs_getfh or MCL below export point */
#define	EX_INDEX	0x10	/* exported with index file specified */
#define	EX_LOG		0x20	/* logging enabled */
#define	EX_LOG_ALLOPS	0x40	/* logging of all RPC operations enabled */
				/* by default only operations which affect */
				/* transaction logging are enabled */
#define	EX_PSEUDO	0x80	/* pseudo filesystem export */
#ifdef VOLATILE_FH_TEST
#define	EX_VOLFH	0x100	/* XXX nfsv4 fh may expire anytime */
#define	EX_VOLRNM	0x200	/* XXX nfsv4 fh expire at rename */
#define	EX_VOLMIG	0x400	/* XXX nfsv4 fh expire at migration */
#define	EX_NOEXPOPEN	0x800	/* XXX nfsv4 fh no expire with open */
#endif /* VOLATILE_FH_TEST */

#ifdef	_KERNEL

#define	RPC_IDEMPOTENT	0x1	/* idempotent or not */
/*
 * Be very careful about which NFS procedures get the RPC_ALLOWANON bit.
 * Right now, it this bit is on, we ignore the results of per NFS request
 * access control.
 */
#define	RPC_ALLOWANON	0x2	/* allow anonymous access */
#define	RPC_MAPRESP	0x4	/* use mapped response buffer */
#define	RPC_AVOIDWORK	0x8	/* do work avoidance for dups */
#define	RPC_PUBLICFH_OK	0x10	/* allow use of public filehandle */

/*
 * RPC_ALL is an or of all above bits to be used with "don't care"
 * nfsv4 ops. The flags of an nfsv4 request is the bit-AND of the
 * per-op flags.
 */
#define	RPC_ALL	(RPC_IDEMPOTENT|RPC_ALLOWANON|RPC_AVOIDWORK|RPC_PUBLICFH_OK)


#ifdef VOLATILE_FH_TEST
struct ex_vol_rename {
	nfs_fh4_fmt_t vrn_fh_fmt;
	struct ex_vol_rename *vrn_next;
};
#endif /* VOLATILE_FH_TEST */

/*
 * An authorization cache entry
 */
struct auth_cache {
	struct netbuf		auth_addr;
	int			auth_flavor;
	int			auth_access;
	time_t			auth_time;
	struct auth_cache	*auth_next;
};

#define	AUTH_TABLESIZE	32

/*
 * Structure containing log file meta-data.
 */
struct log_file {
	unsigned int	lf_flags;	/* flags (see below) */
	int		lf_writers;	/* outstanding writers */
	int		lf_refcnt;	/* references to this struct */
	caddr_t		lf_path;	/* buffer file location */
	vnode_t		*lf_vp;		/* vnode for the buffer file */
	kmutex_t	lf_lock;
	kcondvar_t	lf_cv_waiters;
};

/*
 * log_file and log_buffer flags.
 */
#define	L_WAITING	0x01		/* flush of in-core data to stable */
					/* storage in progress */
#define	L_PRINTED	0x02		/* error message printed to console */
#define	L_ERROR		0x04		/* error condition detected */

/*
 * The logging buffer information.
 * This structure may be shared by multiple exportinfo structures,
 * if they share the same buffer file.
 * This structure contains the basic information about the buffer, such
 * as it's location in the filesystem.
 *
 * 'lb_lock' protects all the fields in this structure except for 'lb_path',
 * and 'lb_next'.
 * 'lb_path' is a write-once/read-many field which needs no locking, it is
 * set before the structure is linked to any exportinfo structure.
 * 'lb_next' is protected by the log_buffer_list_lock.
 */
struct log_buffer {
	unsigned int	lb_flags;	/* L_ONLIST set? */
	int		lb_refcnt;	/* references to this struct */
	unsigned int	lb_rec_id;	/* used to generate unique id */
	caddr_t		lb_path;	/* buffer file pathname */
	struct log_file	*lb_logfile;	/* points to log_file structure */
	kmutex_t	lb_lock;
	struct log_buffer	*lb_next;
	kcondvar_t	lb_cv_waiters;
	caddr_t		lb_records;	/* linked list of records to write */
	int		lb_num_recs;	/* # of records to write */
	ssize_t		lb_size_queued; /* number of bytes queued for write */
};

#define	LOG_BUFFER_HOLD(lbp)	{ \
	mutex_enter(&(lbp)->lb_lock); \
	(lbp)->lb_refcnt++; \
	mutex_exit(&(lbp)->lb_lock); \
}

#define	LOG_BUFFER_RELE(lbp)	{ \
	log_buffer_rele(lbp); \
}

#define	EXPTABLESIZE	16

/*
 * A node associated with an export entry on the
 * list of exported filesystems.
 *
 * exi_count+exi_lock protects an individual exportinfo from being freed
 * when in use.
 * You must have the writer lock on exported_lock to add/delete an exportinfo
 * structure to/from the list.
 *
 * exi_volatile_dev maps to VSW_VOLATILEDEV.  It means that the
 * underlying fs devno can change on each mount.  When set, the server
 * should not use va_fsid for a GETATTR(FATTR4_FSID) reply.  It must
 * use exi_fsid because it is guaranteed to be persistent.  This isn't
 * in any way related to NFS4 volatile filehandles.
 */
struct exportinfo {
	struct exportdata	exi_export;
	fsid_t			exi_fsid;
	struct fid		exi_fid;
	struct exportinfo	*exi_hash;
	fhandle_t		exi_fh;
	krwlock_t		exi_cache_lock;
	kmutex_t		exi_lock;
	uint_t			exi_count;
	vnode_t			*exi_vp;
	vnode_t			*exi_dvp;
	struct auth_cache	*exi_cache[AUTH_TABLESIZE];
	struct log_buffer	*exi_logbuffer;
	struct exp_visible	*exi_visible;
	unsigned		exi_volatile_dev:1;
#ifdef VOLATILE_FH_TEST
	uint32_t		exi_volatile_id;
	struct ex_vol_rename	*exi_vol_rename;
	kmutex_t		exi_vol_rename_lock;
#endif /* VOLATILE_FH_TEST */
};

/*
 * exp_visible is a visible list per filesystem. It is for filesystems
 * that may need a limited view of its contents. A pseudo export and
 * a real export at the mount point (VROOT) which has a subtree shared
 * has a visible list.
 *
 * The exi_visible field is NULL for normal, non=pseudo filesystems
 * which do not have any subtree exported. If the field is non-null,
 * it points to a list of visible entries, identified by vis_fid and/or
 * vis_ino. The presence of a "visible" list means that if this export
 * can only have a limited view, it can only view the entries in the
 * exp_visible list. The directories in the fid list comprise paths that
 * lead to exported directories.
 *
 * The vis_count field records the number of paths in this filesystem
 * that use this directory. The vis_exported field is non-zero if the
 * entry is an exported directory (leaf node).
 */

struct exp_visible {
	vnode_t			*vis_vp;
	fid_t			vis_fid;
	u_longlong_t		vis_ino;
	int			vis_count;
	int			vis_exported;
	struct exp_visible	*vis_next;
};

#define	PSEUDO(exi)	((exi)->exi_export.ex_flags & EX_PSEUDO)

#define	EQFSID(fsidp1, fsidp2)	\
	(((fsidp1)->val[0] == (fsidp2)->val[0]) && \
	    ((fsidp1)->val[1] == (fsidp2)->val[1]))

#define	EQFID(fidp1, fidp2)	\
	((fidp1)->fid_len == (fidp2)->fid_len && \
	    nfs_fhbcmp((char *)(fidp1)->fid_data, (char *)(fidp2)->fid_data, \
	    (uint_t)(fidp1)->fid_len) == 0)

#define	exportmatch(exi, fsid, fid)	\
	(EQFSID(&(exi)->exi_fsid, (fsid)) && EQFID(&(exi)->exi_fid, (fid)))

/*
 * Returns true iff exported filesystem is read-only to the given host.
 *
 * Note:  this macro should be as fast as possible since it's called
 * on each NFS modification request.
 */
#define	rdonly(exi, req)  (nfsauth_access(exi, req) & NFSAUTH_RO)
#define	rdonly4(exi, vp, req)  \
	(vn_is_readonly(vp) || \
	    (nfsauth4_access(exi, vp, req) & (NFSAUTH_RO | NFSAUTH_LIMITED)))

extern int	nfsauth4_access(struct exportinfo *, vnode_t *,
				struct svc_req *);
extern int	nfsauth4_secinfo_access(struct exportinfo *,
				struct svc_req *, int, int);
extern int	nfs_fhhash(fsid_t *, fid_t *);
extern int	nfs_fhbcmp(char *, char *, int);
extern int	nfs_exportinit(void);
extern void	nfs_exportfini(void);
extern int	chk_clnt_sec(struct exportinfo *, struct svc_req *req);
extern int	makefh(fhandle_t *, struct vnode *, struct exportinfo *);
extern int	makefh_ol(fhandle_t *, struct exportinfo *, uint_t);
extern int	makefh3(nfs_fh3 *, struct vnode *, struct exportinfo *);
extern int	makefh3_ol(nfs_fh3 *, struct exportinfo *, uint_t);
extern vnode_t *nfs_fhtovp(fhandle_t *, struct exportinfo *);
extern vnode_t *nfs3_fhtovp(nfs_fh3 *, struct exportinfo *);
extern vnode_t *lm_fhtovp(fhandle_t *fh);
extern vnode_t *lm_nfs3_fhtovp(nfs_fh3 *fh);
extern struct	exportinfo *checkexport(fsid_t *, struct fid *);
extern struct	exportinfo *checkexport4(fsid_t *, struct fid *, vnode_t *vp);
extern void	exi_rele(struct exportinfo *);
extern struct exportinfo *nfs_vptoexi(vnode_t *, vnode_t *, cred_t *, int *,
    int *, bool_t);
extern int	nfs_check_vpexi(vnode_t *, vnode_t *, cred_t *,
			struct exportinfo **);
extern void	export_link(struct exportinfo *);
extern int	export_unlink(fsid_t *, fid_t *, vnode_t *,
			struct exportinfo **);
extern vnode_t *untraverse(vnode_t *);

/*
 * Functions that handle the NFSv4 server namespace
 */
extern int	treeclimb_export(struct exportinfo *);
extern int	treeclimb_unexport(struct exportinfo *);
extern int	nfs_visible(struct exportinfo *, vnode_t *, int *);
extern int	nfs_visible_inode(struct exportinfo *, ino64_t, int *);
extern int	has_visible(struct exportinfo *, vnode_t *);
extern void	free_visible(struct exp_visible *);
extern int	nfs_exported(struct exportinfo *, vnode_t *);
extern int	pseudo_exportfs(vnode_t *, struct exp_visible *,
					struct exportdata *);
extern int	vop_fid_pseudo(vnode_t *, fid_t *fidp);
extern int	nfs4_vget_pseudo(struct exportinfo *, vnode_t **, fid_t *);
/*
 * Functions that handle the NFSv4 server namespace security flavors
 * information.
 */
extern void	srv_secinfo_exp2pseu(struct exportdata *, struct exportdata *);

/*
 * "public" and default (root) location for public filehandle
 */
extern struct exportinfo *exi_public, *exi_root;
extern fhandle_t nullfh2;	/* for comparing V2 filehandles */
extern krwlock_t exported_lock;
extern struct exportinfo *exptable[];

/*
 * Two macros for identifying public filehandles.
 * A v2 public filehandle is 32 zero bytes.
 * A v3 public filehandle is zero length.
 */
#define	PUBLIC_FH2(fh) \
	((fh)->fh_fsid.val[1] == 0 && \
	bcmp((fh), &nullfh2, sizeof (fhandle_t)) == 0)

#define	PUBLIC_FH3(fh) \
	((fh)->fh3_length == 0)

extern int	makefh4(nfs_fh4 *, struct vnode *, struct exportinfo *);
extern vnode_t *nfs4_fhtovp(nfs_fh4 *, struct exportinfo *, nfsstat4 *);

#endif /* _KERNEL */

#ifdef	__cplusplus
}
#endif

#endif	/* _NFS_EXPORT_H */
