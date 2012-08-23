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
 * High Sierra filesystem structure definitions
 * Copyright 2004 Sun Microsystems, Inc.
 * All rights reserved.
 * Use is subject to license terms.
 */

#ifndef	_SYS_FS_HSFS_NODE_H
#define	_SYS_FS_HSFS_NODE_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef	__cplusplus
extern "C" {
#endif

struct	hs_direntry {
	uint_t		ext_lbn;	/* LBN of start of extent */
	uint_t		ext_size;    	/* no. of data bytes in extent */
	struct timeval	cdate;		/* creation date */
	struct timeval	mdate;		/* last modification date */
	struct timeval	adate;		/* last access date */
	enum vtype	type;		/* file type */
	mode_t		mode;		/* mode and type of file (UNIX) */
	uint_t		nlink;		/* no. of links to file */
	uid_t		uid;		/* owner's user id */
	gid_t		gid;		/* owner's group id */
	dev_t		r_dev;		/* major/minor device numbers */
	uint_t		xar_prot :1;	/* 1 if protection in XAR */
	uchar_t		xar_len;	/* no. of Logical blocks in XAR */
	uchar_t		intlf_sz;	/* intleaving size */
	uchar_t		intlf_sk;	/* intleaving skip factor */
	ushort_t	sym_link_flag;	/* flags for sym link */
	char		*sym_link; 	/* path of sym link for readlink() */
};

struct	ptable {
	uchar_t	filler[7];		/* filler */
	uchar_t	dname_len;		/* length of directory name */
	uchar_t	dname[HS_DIR_NAMELEN+1];	/* directory name */
};

struct ptable_idx {
	struct ptable_idx *idx_pptbl_idx; /* parent's path table index entry */
	struct ptable	*idx_mptbl;	/* path table entry for myself */
	ushort_t idx_nochild;		/* no. of children */
	ushort_t idx_childid;		/* directory no of first child */
};

/*
 * hsnode structure:
 *
 * hs_offset, hs_ptbl_idx, base  apply to VDIR type only
 *
 * nodeid uniquely identifies an hsnode, ISO9660 means
 * nodeid can be very big.
 * For directories it is the disk address of
 * the data extent of the dir (the directory itself,
 * ".", and ".." all point to same data extent).
 * For non-directories, it is the disk address of the
 * directory entry for the file; note that this does
 * not permit hard links, as it assumes a single dir
 * entry per file.
 */

struct  hsnode {
	struct hsnode	*hs_hash;	/* next hsnode in hash list */
	struct hsnode	*hs_freef;	/* next hsnode in free list */
	struct hsnode	*hs_freeb;	/* previous hsnode in free list */
	struct vnode	*hs_vnode;	/* the real vnode for the file */
	struct hs_direntry hs_dirent;	/* the directory entry for this file */
	ino64_t		hs_nodeid;	/* "inode" number for hsnode */
	uint_t		hs_dir_lbn;	/* LBN of directory entry */
	uint_t		hs_dir_off;	/* offset in LBN of directory entry */
	struct ptable_idx	*hs_ptbl_idx;	/* path table index */
	uint_t		hs_offset;	/* start offset in dir for searching */
	long		hs_mapcnt;	/* mappings to file pages */
	uint_t		hs_seq;		/* sequence number */
	uint_t		hs_flags;	/* (see below) */
	kmutex_t	hs_contents_lock;	/* protects hsnode contents */
						/* 	except hs_offset */
};

/* hs_flags */
#define	HREF	1			/* hsnode is referenced */

/* hs_modes */

#define	HFDIR	0040000			/* directory */
#define	HFREG	0100000			/* regular file */

struct  hsfid {
	ushort_t	hf_len;		/* length of fid */
	ushort_t	hf_dir_off;	/* offset in LBN of directory entry */
	uint_t		hf_dir_lbn;	/* LBN of directory */
};


/*
 * All of the fields in the hs_volume are read-only once they have been
 * initialized.
 */
struct	hs_volume {
	ulong_t		vol_size; 	/* no. of Logical blocks in Volume */
	uint_t		lbn_size;	/* no. of bytes in a block */
	uint_t		lbn_shift;	/* shift to convert lbn to bytes */
	uint_t		lbn_secshift;	/* shift to convert lbn to sec */
	uint_t		lbn_maxoffset;	/* max lbn-relative offset and mask */
	uchar_t		file_struct_ver; /* version of directory structure */
	uid_t		vol_uid;	/* uid of volume */
	gid_t		vol_gid;	/* gid of volume */
	uint_t		vol_prot;	/* protection (mode) of volume */
	struct timeval	cre_date;	/* volume creation time */
	struct timeval	mod_date;	/* volume modification time */
	struct	hs_direntry root_dir;	/* dir entry for Root Directory */
	ushort_t	ptbl_len;	/* number of bytes in Path Table */
	uint_t		ptbl_lbn;	/* logical block no of Path Table */
	ushort_t	vol_set_size;	/* number of CD in this vol set */
	ushort_t	vol_set_seq;	/* the sequence number of this CD */
	char		vol_id[32];		/* volume id in PVD */
};

/*
 * The hsnode table is no longer fixed in size but grows
 * and shrinks dynamically. However a cache of nodes is still maintained
 * for efficiency. This cache size (nhsnode) is a tunable which
 * is either specified in /etc/system or calculated as the number
 * that will fit into the number of bytes defined by HS_HSNODESPACE (below).
 */
#define	HS_HASHSIZE	32		/* hsnode hash table size */
#define	HS_HSNODESPACE	16384		/* approx. space used for hsnodes */

/*
 * High Sierra filesystem structure.
 * There is one of these for each mounted High Sierra filesystem.
 */
enum hs_vol_type {
	HS_VOL_TYPE_HS = 0, HS_VOL_TYPE_ISO = 1
};
#define	HSFS_MAGIC 0x03095500
struct hsfs {
	struct hsfs	*hsfs_next;	/* ptr to next entry in linked list */
	long		hsfs_magic;	/* should be HSFS_MAGIC */
	struct vfs	*hsfs_vfs;	/* vfs for this fs */
	struct vnode	*hsfs_rootvp;	/* vnode for root of filesystem */
	struct vnode	*hsfs_devvp;	/* device mounted on */
	enum hs_vol_type hsfs_vol_type; /* 0 hsfs 1 iso 2 hsfs+sun 3 iso+sun */
	struct hs_volume hsfs_vol;	/* File Structure Volume Descriptor */
	struct ptable	*hsfs_ptbl;	/* pointer to incore Path Table */
	int		hsfs_ptbl_size;	/* size of incore path table */
	struct ptable_idx *hsfs_ptbl_idx; /* pointer to path table index */
	int		hsfs_ptbl_idx_size;	/* no. of path table index */
	ulong_t		hsfs_ext_impl;	/* ext. information bits */
	ushort_t	hsfs_sua_off;	/* the SUA offset */
	ushort_t	hsfs_namemax;	/* maximum file name length */
	ulong_t		hsfs_err_flags;	/* ways in which fs is non-conformant */
	char		*hsfs_fsmnt;	/* name mounted on */
	ulong_t		hsfs_flags;	/* hsfs-specific mount flags */
	krwlock_t	hsfs_hash_lock;	/* protect hash table & hst_nohsnode */
	struct hsnode	*hsfs_hash[HS_HASHSIZE]; /* head of hash lists */
	uint32_t	hsfs_nohsnode;	/* no. of allocated hsnodes */
	kmutex_t	hsfs_free_lock;	/* protects free list */
	struct hsnode	*hsfs_free_f;	/* first entry of free list */
	struct hsnode	*hsfs_free_b;	/* last entry of free list */
};

/*
 * Error types: bit offsets into hsfs_err_flags.
 * Also serves as index into hsfs_error[], so must be
 * kept in sync with that data structure.
 */
#define	HSFS_ERR_TRAILING_JUNK	0
#define	HSFS_ERR_LOWER_CASE_NM	1
#define	HSFS_ERR_BAD_ROOT_DIR	2
#define	HSFS_ERR_UNSUP_TYPE	3
#define	HSFS_ERR_BAD_FILE_LEN	4

#define	HSFS_HAVE_LOWER_CASE(fsp) \
	((fsp)->hsfs_err_flags & (1 << HSFS_ERR_LOWER_CASE_NM))


/*
 * File system parameter macros
 */
#define	hs_blksize(HSFS, HSP, OFF)	/* file system block size */ \
	((HSP)->hs_vn.v_flag & VROOT ? \
	    ((OFF) >= \
		((HSFS)->hsfs_rdirsec & ~((HSFS)->hsfs_spcl - 1))*HS_SECSIZE ?\
		((HSFS)->hsfs_rdirsec & ((HSFS)->hsfs_spcl - 1))*HS_SECSIZE :\
		(HSFS)->hsfs_clsize): \
	    (HSFS)->hsfs_clsize)
#define	hs_blkoff(OFF)		/* offset within block */ \
	((OFF) & (HS_SECSIZE - 1))

/*
 * Conversion macros
 */
#define	VFS_TO_HSFS(VFSP)	((struct hsfs *)(VFSP)->vfs_data)
#define	HSFS_TO_VFS(FSP)	((FSP)->hsfs_vfs)

#define	VTOH(VP)		((struct hsnode *)(VP)->v_data)
#define	HTOV(HP)		(((HP)->hs_vnode))

/*
 * Convert between Logical Block Number and Sector Number.
 */
#define	LBN_TO_SEC(lbn, vfsp)	((lbn)>>((struct hsfs *)((vfsp)->vfs_data))->  \
				hsfs_vol.lbn_secshift)

#define	SEC_TO_LBN(sec, vfsp)	((sec)<<((struct hsfs *)((vfsp)->vfs_data))->  \
				hsfs_vol.lbn_secshift)

#define	LBN_TO_BYTE(lbn, vfsp)	((lbn)<<((struct hsfs *)((vfsp)->vfs_data))->  \
				hsfs_vol.lbn_shift)
#define	BYTE_TO_LBN(boff, vfsp)	((boff)>>((struct hsfs *)((vfsp)->vfs_data))-> \
				hsfs_vol.lbn_shift)

/*
 * Create a nodeid.
 * We construct the nodeid from the location of the directory
 * entry which points to the file.  We divide by 32 to
 * compress the range of nodeids; we know that the minimum size
 * for an ISO9660 dirent is 34, so we will never have adjacent
 * dirents with the same nodeid.
 */
#define	HSFS_MIN_DL_SHFT	5
#define	MAKE_NODEID(lbn, off, vfsp) \
		((LBN_TO_BYTE((lbn), (vfsp)) + (off)) >> HSFS_MIN_DL_SHFT)

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_FS_HSFS_NODE_H */
