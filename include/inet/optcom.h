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
/* Copyright (c) 1990 Mentat Inc. */

#ifndef	_INET_OPTCOM_H
#define	_INET_OPTCOM_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef	__cplusplus
extern "C" {
#endif

#if defined(_KERNEL) && defined(__STDC__)

/* Options Description Structure */
typedef struct opdes_s {
	t_uscalar_t	opdes_name;	/* option name */
	t_uscalar_t	opdes_level;	/* option "level" */
	int	opdes_access_nopriv;	/* permissions for non-privileged */
	int	opdes_access_priv;	/* permissions for privileged */
	int	opdes_access_req_priv;	/* required privilege, OP_NP if none */
	int	opdes_props;	/* properties of associated with option */
	t_uscalar_t	opdes_size;	/* length of option */
					/* [ or maxlen if variable */
			/* length(OP_VARLEN) property set for option] */
	union {
		/*
		 *
		 * Note: C semantics:
		 * static initializer of "union" type assume
		 * the constant on RHS is of the type of the
		 * first member of the union. So what comes first
		 * is important.
		 */
#define	OPDES_DEFSZ_MAX		64
		int64_t  opdes_def_int64;
		char	opdes_def_charbuf[OPDES_DEFSZ_MAX];
	} opdes_def;
} opdes_t;

#define	opdes_default	opdes_def.opdes_def_int64
#define	opdes_defbuf	opdes_def.opdes_def_charbuf
/*
 * Flags to set in opdes_acces_{all,priv} fields in opdes_t
 *
 *	OA_R	read access
 *	OA_W	write access
 *	OA_RW	read-write access
 *	OA_X	execute access
 *
 * Note: - semantics "execute" access used for operations excuted using
 *		option management interface
 *	- no bits set means this option is not visible. Some options may not
 *	  even be visible to all but priviliged users.
 */
#define	OA_R	0x1
#define	OA_W	0x2
#define	OA_X	0x4

/*
 * Utility macros to test permissions needed to compose more
 * complex ones. (Only a few really used directly in code).
 */
#define	OA_RW	(OA_R|OA_W)
#define	OA_WX	(OA_W|OA_X)
#define	OA_RX	(OA_R|OA_X)
#define	OA_RWX	(OA_R|OA_W|OA_X)

#define	OA_ANY_ACCESS(x) ((x)->opdes_access_nopriv|(x)->opdes_access_priv)
#define	OA_R_NOPRIV(x)	((x)->opdes_access_nopriv & OA_R)
#define	OA_R_ANYPRIV(x)	(OA_ANY_ACCESS(x) & OA_R)
#define	OA_W_NOPRIV(x)	((x)->opdes_access_nopriv & OA_W)
#define	OA_X_ANYPRIV(x)	(OA_ANY_ACCESS(x) & OA_X)
#define	OA_X_NOPRIV(x)	((x)->opdes_access_nopriv & OA_X)
#define	OA_W_ANYPRIV(x)	(OA_ANY_ACCESS(x) & OA_W)
#define	OA_WX_NOPRIV(x)	((x)->opdes_access_nopriv & OA_WX)
#define	OA_WX_ANYPRIV(x)	(OA_ANY_ACCESS(x) & OA_WX)
#define	OA_RWX_ANYPRIV(x)	(OA_ANY_ACCESS(x) & OA_RWX)
#define	OA_RONLY_NOPRIV(x)	(((x)->opdes_access_nopriv & OA_RWX) == OA_R)
#define	OA_RONLY_ANYPRIV(x)	((OA_ANY_ACCESS(x) & OA_RWX) == OA_R)

#define	OP_NP		(-1)			/* No privilege required */
#define	OP_CONFIG	(0)			/* Network configuration */
#define	OP_RAW		(1)			/* Raw packets */
#define	OP_PRIVPORT	(2)			/* Privileged ports */


/*
 * Following macros supply the option and their privilege and
 * are used to determine permissions.
 */
#define	OA_POLICY_OK(x, c) \
		(secpolicy_net((c), (x)->opdes_access_req_priv, B_FALSE) == 0)

#define	OA_POLICY_ONLY_OK(x, c) \
		(secpolicy_net((c), (x)->opdes_access_req_priv, B_TRUE) == 0)

#define	OA_MATCHED_PRIV(x, c)	((x)->opdes_access_req_priv != OP_NP && \
		OA_POLICY_ONLY_OK((x), (c)))

#define	OA_READ_PERMISSION(x, c)	(OA_R_NOPRIV(x) || \
		(OA_R_ANYPRIV(x) && OA_POLICY_OK((x), (c))))

#define	OA_WRITE_OR_EXECUTE(x, c)	(OA_WX_NOPRIV(x) || \
		(OA_WX_ANYPRIV(x) && OA_POLICY_OK((x), (c))))

#define	OA_READONLY_PERMISSION(x, c)	(OA_RONLY_NOPRIV(x) || \
		(OA_RONLY_ANYPRIV(x) && OA_POLICY_OK((x), (c))))

#define	OA_WRITE_PERMISSION(x, c)	(OA_W_NOPRIV(x) || \
		(OA_W_ANYPRIV(x) && OA_POLICY_ONLY_OK((x), (c))))

#define	OA_EXECUTE_PERMISSION(x, c)	(OA_X_NOPRIV(x) || \
		(OA_X_ANYPRIV(x) && OA_POLICY_ONLY_OK((x), (c))))

#define	OA_NO_PERMISSION(x, c)		(OA_MATCHED_PRIV((x), (c)) ? \
		((x)->opdes_access_priv == 0) : ((x)->opdes_access_nopriv == 0))

/*
 * Other properties set in opdes_props field.
 */
#define	OP_PASSNEXT	0x1	/* to pass option to next module or not */
#define	OP_VARLEN	0x2	/* option is varible length  */
#define	OP_NOT_ABSREQ	0x4	/* option is not a "absolute requirement" */
				/* i.e. failure to negotiate does not */
				/* abort primitive ("ignore" semantics ok) */
#define	OP_NODEFAULT	0x8	/* no concept of "default value"  */
#define	OP_DEF_FN	0x10	/* call a "default function" to get default */
				/* value, not from static table  */


/*
 * Structure to represent attributed of option management specific
 * to one particular layer of "transport".
 */

typedef	t_uscalar_t optlevel_t;

typedef int (*opt_def_fn)(queue_t *, int, int, uchar_t *);
typedef int (*opt_get_fn)(queue_t *, int, int, uchar_t *);
typedef int (*opt_set_fn)(queue_t *, uint_t, int, int, uint_t, uchar_t *,
    uint_t *, uchar_t *, void *, cred_t *, mblk_t *);

typedef struct optdb_obj {
	opt_def_fn	odb_deffn;	/* default value function */
	opt_get_fn	odb_getfn;	/* get function */
	opt_set_fn	odb_setfn;	/* set function */
	boolean_t	odb_topmost_tpiprovider; /* whether topmost tpi */
					/* provider or downstream */
	uint_t		odb_opt_arr_cnt; /* count of number of options in db */
	opdes_t		*odb_opt_des_arr; /* option descriptors in db */
	uint_t		odb_valid_levels_arr_cnt;
					/* count of option levels supported */
	optlevel_t	*odb_valid_levels_arr;
					/* array of option levels supported */
} optdb_obj_t;

/*
 * This is used to restart option processing. This goes inside an M_CTL
 * which is prepended to the packet. IP may need to become exclusive on
 * an ill for setting some options. For dg. IP_ADD_MEMBERSHIP. Since
 * there can be more than 1 option packed in an option buffer, we need to
 * remember where to restart option processing after resuming from a wait
 * for exclusive condition in IP.
 */
typedef struct opt_restart_s {
	struct	opthdr	*or_start;		/* start of option buffer */
	struct	opthdr	*or_end;		/* end of option buffer */
	struct	opthdr	*or_ropt;		/* restart option here */
	t_uscalar_t	or_worst_status;	/* Used by tpi_optcom_req */
	t_uscalar_t	or_type;		/* svr4 or tpi optcom variant */
	int		or_private;		/* currently used by CGTP */
} opt_restart_t;
/*
 * Values for "optset_context" parameter passed to
 * transport specific "setfn()" routines
 */
#define	SETFN_OPTCOM_CHECKONLY		1 /* "checkonly" semantics T_CHECK */
#define	SETFN_OPTCOM_NEGOTIATE		2 /* semantics for T_*_OPTCOM_REQ */
#define	SETFN_UD_NEGOTIATE		3 /* semantics for T_UNITDATA_REQ */
#define	SETFN_CONN_NEGOTIATE		4 /* semantics for T_CONN_*_REQ */

/*
 * Function prototypes
 */
extern void optcom_err_ack(queue_t *, mblk_t *, t_scalar_t, int);
extern int svr4_optcom_req(queue_t *, mblk_t *, cred_t *, optdb_obj_t *);
extern int tpi_optcom_req(queue_t *, mblk_t *, cred_t *, optdb_obj_t *);
extern int  tpi_optcom_buf(queue_t *, mblk_t *, t_scalar_t *, t_scalar_t,
    cred_t *, optdb_obj_t *, void *, int *);
extern t_uscalar_t optcom_max_optsize(opdes_t *, uint_t);

#endif	/* defined(_KERNEL) && defined(__STDC__) */

#ifdef	__cplusplus
}
#endif

#endif	/* _INET_OPTCOM_H */
