/*
 * Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*
 * Copyright (C) 1989-1995 by the Massachusetts Institute of Technology,
 * Cambridge, MA, USA.  All Rights Reserved.
 *
 * This software is being provided to you, the LICENSEE, by the
 * Massachusetts Institute of Technology (M.I.T.) under the following
 * license.  By obtaining, using and/or copying this software, you agree
 * that you have read, understood, and will comply with these terms and
 * conditions:
 *
 * Export of this software from the United States of America may
 * require a specific license from the United States Government.
 * It is the responsibility of any person or organization contemplating
 * export to obtain such a license before exporting.
 *
 * WITHIN THAT CONSTRAINT, permission to use, copy, modify and distribute
 * this software and its documentation for any purpose and without fee or
 * royalty is hereby granted, provided that you agree to comply with the
 * following copyright notice and statements, including the disclaimer, and
 * that the same appear on ALL copies of the software and documentation,
 * including modifications that you make for internal use or for
 * distribution:
 *
 * THIS SOFTWARE IS PROVIDED "AS IS", AND M.I.T. MAKES NO REPRESENTATIONS
 * OR WARRANTIES, EXPRESS OR IMPLIED.  By way of example, but not
 * limitation, M.I.T. MAKES NO REPRESENTATIONS OR WARRANTIES OF
 * MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE OR THAT THE USE OF
 * THE LICENSED SOFTWARE OR DOCUMENTATION WILL NOT INFRINGE ANY THIRD PARTY
 * PATENTS, COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS.
 *
 * The name of the Massachusetts Institute of Technology or M.I.T. may NOT
 * be used in advertising or publicity pertaining to distribution of the
 * software.  Title to copyright in this software and any associated
 * documentation shall at all times remain with M.I.T., and USER agrees to
 * preserve same.
 */
/*
 * Copyright (C) 1998 by the FundsXpress, INC.
 *
 * All rights reserved.
 *
 * Export of this software from the United States of America may require
 * a specific license from the United States Government.  It is the
 * responsibility of any person or organization contemplating export to
 * obtain such a license before exporting.
 *
 * WITHIN THAT CONSTRAINT, permission to use, copy, modify, and
 * distribute this software and its documentation for any purpose and
 * without fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that
 * the name of FundsXpress. not be used in advertising or publicity pertaining
 * to distribution of the software without specific, written prior
 * permission.  FundsXpress makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 * This prototype for k5-int.h (Krb5 internals include file)
 * includes the user-visible definitions from krb5.h and then
 * includes other definitions that are not user-visible but are
 * required for compiling Kerberos internal routines.
 *
 * John Gilmore, Cygnus Support, Sat Jan 21 22:45:52 PST 1995
 */

#ifndef _KRB5_INT_H
#define _KRB5_INT_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifndef	_KERNEL
#include <osconf.h>
#include <security/cryptoki.h>
#else
#include <sys/crypto/common.h>
#include <sys/crypto/api.h>
#endif

#ifdef  DEBUG
#if !defined(KRB5_DEBUG)
#define KRB5_DEBUG
#endif
#ifndef  KRB5_LOG_LVL
#define KRB5_LOG_LVL KRB5_ERR
#endif
#endif  /* DEBUG */

#ifdef  _KERNEL

#ifdef  DEBUG
#include        <sys/types.h>
#include        <sys/cmn_err.h>
 extern  void prom_printf();
#endif  /* DEBUG */

#else   /* !_KERNEL */

#define prom_printf printf

#endif /* !_KERNEL */

#ifdef KRB5_LOG_LVL

/* krb5_log is used to set the logging level to determine what class of messages
 * are output by the mech.  Note, more than one logging level can be used by
 * bit or'ing the log values together.
 *
 * All log messages are captured by syslog.
 */

extern unsigned int krb5_log;

/* Note, these defines should be mutually exclusive bit fields */
#define KRB5_ERR  1   /* Use this debug log level for error path logging. */
#define KRB5_INFO 2   /* Use this debug log level for informational messages. */

#ifdef  _KERNEL

#define KRB5_LOG1(A, B, C, D) \
     ((void)((krb5_log) && (krb5_log & (A)) && (printf((B), (C), (D)), TRUE)))
#define KRB5_LOG(A, B, C) \
     ((void)((krb5_log) && (krb5_log & (A)) && (printf((B), (C)), TRUE)))
#define KRB5_LOG0(A, B)   \
     ((void)((krb5_log) && (krb5_log & (A)) && (printf((B)), TRUE)))

#else	/* !_KERNEL */

#include <syslog.h>

#define KRB5_LOG1(A, B, C, D) \
        ((void)((krb5_log) && (krb5_log & (A)) && \
		(syslog(LOG_DEBUG, (B), (C), (D)), TRUE)))
#define KRB5_LOG(A, B, C) \
        ((void)((krb5_log) && (krb5_log & (A)) && \
		(syslog(LOG_DEBUG, (B), (C)), TRUE)))
#define KRB5_LOG0(A, B)   \
        ((void)((krb5_log) && (krb5_log & (A)) && \
	       	(syslog(LOG_DEBUG, B), TRUE)))

#endif	/* _KERNEL */

#else /* ! KRB5_LOG_LVL */

#define KRB5_LOG1(A, B, C, D)
#define KRB5_LOG(A, B, C)
#define KRB5_LOG0(A, B)

#endif /* KRB5_LOG_LVL */

/* Compatibility switch for SAM preauth */
#define AS_REP_105_SAM_COMPAT

#ifdef POSIX_TYPES
#define timetype time_t
#else
#define timetype long
#endif

/*
 * Begin "k5-config.h"
 */
#ifndef KRB5_CONFIG__
#define KRB5_CONFIG__

/*
 * Machine-type definitions: PC Clone 386 running Microsoft Windows
 */

#if defined(_MSDOS) || defined(_WIN32) || defined(macintosh)
#include "win-mac.h"
#if defined(macintosh) && defined(__CFM68K__) && !defined(__USING_STATIC_LIBS__)
#pragma import on
#endif
#endif

#if defined(_MSDOS) || defined(_WIN32)
/* Kerberos Windows initialization file */
#define KERBEROS_INI    "kerberos.ini"
#define INI_FILES       "Files"
#define INI_KRB_CCACHE  "krb5cc"       /* Location of the ccache */
#define INI_KRB5_CONF   "krb5.ini"		/* Location of krb5.conf file */
#define HAVE_LABS
#define ANSI_STDIO
#endif


#ifndef macintosh
#if defined(__MWERKS__) || defined(applec) || defined(THINK_C)
#define macintosh
#define SIZEOF_INT 4
#define SIZEOF_SHORT 2
#define HAVE_SRAND
#define NO_PASSWORD
#define HAVE_LABS
/*#define ENOMEM -1*/
#define ANSI_STDIO
#ifndef _SIZET
typedef unsigned int size_t;
#define _SIZET
#endif
#include <unix.h>
#include <ctype.h>
#endif
#endif

#ifndef _KERNEL
#ifndef KRB5_AUTOCONF__
#define KRB5_AUTOCONF__
#include <autoconf.h>
#endif
#endif 		/* !_KERNEL  */

#ifndef KRB5_SYSTYPES__
#define KRB5_SYSTYPES__

#ifdef HAVE_SYS_TYPES_H		/* From autoconf.h */
#include <sys/types.h>
#else /* HAVE_SYS_TYPES_H */
#endif /* HAVE_SYS_TYPES_H */
#endif /* KRB5_SYSTYPES__ */

#ifdef SYSV
/* Change srandom and random to use rand and srand */
/* Taken from the Sandia changes.  XXX  We should really just include */
/* srandom and random into Kerberos release, since rand() is a really */
/* bad random number generator.... [tytso:19920616.2231EDT] */
#define random() rand()
#define srandom(a) srand(a)
#endif /* SYSV */

typedef uint64_t krb5_ui_8;
typedef int64_t krb5_int64;

#define DEFAULT_PWD_STRING1 "Enter password:"
#define DEFAULT_PWD_STRING2 "Re-enter password for verification:"

#define	KRB5_KDB_MAX_LIFE	(60*60*24) /* one day */
#define	KRB5_KDB_MAX_RLIFE	(60*60*24*365) /* one year */
#define	KRB5_KDB_EXPIRATION	2145830400 /* Thu Jan  1 00:00:00 2038 UTC */
#define KRB5_DEFAULT_LIFE 60*60*10 /* 10 hours */
#define KRB5_DEFAULT_RENEW_LIFE 7*24*60*60 /* 7 Days */

/*
 * Windows requires a different api interface to each function. Here
 * just define it as NULL.
 */
#ifndef KRB5_CALLCONV
#define KRB5_CALLCONV
#define KRB5_CALLCONV_C
#define KRB5_DLLIMP
#define GSS_DLLIMP
#define KRB5_EXPORTVAR
#define FAR
#define NEAR
#endif
#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef HAVE_LABS
#define labs(x) abs(x)
#endif

#endif /* KRB5_CONFIG__ */

/*
 * End "k5-config.h"
 */

/*
 * After loading the configuration definitions, load the Kerberos definitions.
 */
#include <krb5.h>

#ifndef _KERNEL
#ifdef NEED_SOCKETS
#include <port-sockets.h>
#include <socket-utils.h>
#else
#ifndef SOCK_DGRAM
struct sockaddr;
#endif
#endif
#endif

/* krb5/krb5.h includes many other .h files in the krb5 subdirectory.
   The ones that it doesn't include, we include below.  */

/*
 * Begin "k5-errors.h"
 */
#ifndef KRB5_ERRORS__
#define KRB5_ERRORS__


/* Error codes used in KRB_ERROR protocol messages.
   Return values of library routines are based on a different error table
   (which allows non-ambiguous error codes between subsystems) */

/* KDC errors */
#define	KDC_ERR_NONE			0 /* No error */
#define	KDC_ERR_NAME_EXP		1 /* Client's entry in DB expired */
#define	KDC_ERR_SERVICE_EXP		2 /* Server's entry in DB expired */
#define	KDC_ERR_BAD_PVNO		3 /* Requested pvno not supported */
#define	KDC_ERR_C_OLD_MAST_KVNO		4 /* C's key encrypted in old master */
#define	KDC_ERR_S_OLD_MAST_KVNO		5 /* S's key encrypted in old master */
#define	KDC_ERR_C_PRINCIPAL_UNKNOWN	6 /* Client not found in Kerberos DB */
#define	KDC_ERR_S_PRINCIPAL_UNKNOWN	7 /* Server not found in Kerberos DB */
#define	KDC_ERR_PRINCIPAL_NOT_UNIQUE	8 /* Multiple entries in Kerberos DB */
#define	KDC_ERR_NULL_KEY		9 /* The C or S has a null key */
#define	KDC_ERR_CANNOT_POSTDATE		10 /* Tkt ineligible for postdating */
#define	KDC_ERR_NEVER_VALID		11 /* Requested starttime > endtime */
#define	KDC_ERR_POLICY			12 /* KDC policy rejects request */
#define	KDC_ERR_BADOPTION		13 /* KDC can't do requested opt. */
#define	KDC_ERR_ENCTYPE_NOSUPP		14 /* No support for encryption type */
#define KDC_ERR_SUMTYPE_NOSUPP		15 /* No support for checksum type */
#define KDC_ERR_PADATA_TYPE_NOSUPP	16 /* No support for padata type */
#define KDC_ERR_TRTYPE_NOSUPP		17 /* No support for transited type */
#define KDC_ERR_CLIENT_REVOKED		18 /* C's creds have been revoked */
#define KDC_ERR_SERVICE_REVOKED		19 /* S's creds have been revoked */
#define KDC_ERR_TGT_REVOKED		20 /* TGT has been revoked */
#define KDC_ERR_CLIENT_NOTYET		21 /* C not yet valid */
#define KDC_ERR_SERVICE_NOTYET		22 /* S not yet valid */
#define KDC_ERR_KEY_EXP			23 /* Password has expired */
#define KDC_ERR_PREAUTH_FAILED		24 /* Preauthentication failed */
#define KDC_ERR_PREAUTH_REQUIRED	25 /* Additional preauthentication */
					   /* required */
#define KDC_ERR_SERVER_NOMATCH		26 /* Requested server and */
					   /* ticket don't match*/
/* Application errors */
#define	KRB_AP_ERR_BAD_INTEGRITY 31	/* Decrypt integrity check failed */
#define	KRB_AP_ERR_TKT_EXPIRED	32	/* Ticket expired */
#define	KRB_AP_ERR_TKT_NYV	33	/* Ticket not yet valid */
#define	KRB_AP_ERR_REPEAT	34	/* Request is a replay */
#define	KRB_AP_ERR_NOT_US	35	/* The ticket isn't for us */
#define	KRB_AP_ERR_BADMATCH	36	/* Ticket/authenticator don't match */
#define	KRB_AP_ERR_SKEW		37	/* Clock skew too great */
#define	KRB_AP_ERR_BADADDR	38	/* Incorrect net address */
#define	KRB_AP_ERR_BADVERSION	39	/* Protocol version mismatch */
#define	KRB_AP_ERR_MSG_TYPE	40	/* Invalid message type */
#define	KRB_AP_ERR_MODIFIED	41	/* Message stream modified */
#define	KRB_AP_ERR_BADORDER	42	/* Message out of order */
#define	KRB_AP_ERR_BADKEYVER	44	/* Key version is not available */
#define	KRB_AP_ERR_NOKEY	45	/* Service key not available */
#define	KRB_AP_ERR_MUT_FAIL	46	/* Mutual authentication failed */
#define KRB_AP_ERR_BADDIRECTION	47 	/* Incorrect message direction */
#define KRB_AP_ERR_METHOD	48 	/* Alternative authentication */
					/* method required */
#define KRB_AP_ERR_BADSEQ	49 	/* Incorrect sequence numnber */
					/* in message */
#define KRB_AP_ERR_INAPP_CKSUM	50	/* Inappropriate type of */
					/* checksum in message */
#define	KRB_AP_PATH_NOT_ACCEPTED 51	/* Policy rejects transited path */ 
#define	KRB_ERR_RESPONSE_TOO_BIG 52	/* Response too big for UDP, */ 
					/*   retry with TCP */ 

/* other errors */
#define KRB_ERR_GENERIC		60 	/* Generic error (description */
					/* in e-text) */
#define	KRB_ERR_FIELD_TOOLONG	61	/* Field is too long for impl. */

#endif /* KRB5_ERRORS__ */
/*
 * End "k5-errors.h"
 */

/*
 * This structure is returned in the e-data field of the KRB-ERROR
 * message when the error calling for an alternative form of
 * authentication is returned, KRB_AP_METHOD.
 */
typedef struct _krb5_alt_method {
	krb5_magic	magic;
	krb5_int32	method;
	unsigned int	length;
	krb5_octet	*data;
} krb5_alt_method;

/*
 * A null-terminated array of this structure is returned by the KDC as
 * the data part of the ETYPE_INFO preauth type.  It informs the
 * client which encryption types are supported.
 * The	same data structure is used by both etype-info and etype-info2
 * but s2kparams must be null when encoding etype-info.
 */
typedef struct _krb5_etype_info_entry {
	krb5_magic	magic;
	krb5_enctype	etype;
	unsigned int	length;
	krb5_octet	*salt;
	krb5_data	s2kparams;
} krb5_etype_info_entry;

/*
 *  This is essentially -1 without sign extension which can screw up
 *  comparisons on 64 bit machines. If the length is this value, then
 *  the salt data is not present. This is to distinguish between not
 *  being set and being of 0 length.
 */
#define KRB5_ETYPE_NO_SALT VALID_UINT_BITS

typedef krb5_etype_info_entry ** krb5_etype_info;

/*
 * a sam_challenge is returned for alternate preauth
 */
/*
          SAMFlags ::= BIT STRING {
              use-sad-as-key[0],
              send-encrypted-sad[1],
              must-pk-encrypt-sad[2]
          }
 */
/*
          PA-SAM-CHALLENGE ::= SEQUENCE {
              sam-type[0]                 INTEGER,
              sam-flags[1]                SAMFlags,
              sam-type-name[2]            GeneralString OPTIONAL,
              sam-track-id[3]             GeneralString OPTIONAL,
              sam-challenge-label[4]      GeneralString OPTIONAL,
              sam-challenge[5]            GeneralString OPTIONAL,
              sam-response-prompt[6]      GeneralString OPTIONAL,
              sam-pk-for-sad[7]           EncryptionKey OPTIONAL,
              sam-nonce[8]                INTEGER OPTIONAL,
              sam-cksum[9]                Checksum OPTIONAL
          }
*/
/* sam_type values -- informational only */
#define PA_SAM_TYPE_ENIGMA     1   /*  Enigma Logic */
#define PA_SAM_TYPE_DIGI_PATH  2   /*  Digital Pathways */
#define PA_SAM_TYPE_SKEY_K0    3   /*  S/key where  KDC has key 0 */
#define PA_SAM_TYPE_SKEY       4   /*  Traditional S/Key */
#define PA_SAM_TYPE_SECURID    5   /*  Security Dynamics */
#define PA_SAM_TYPE_CRYPTOCARD 6   /*  CRYPTOCard */
#if 1 /* XXX need to figure out who has which numbers assigned */
#define PA_SAM_TYPE_ACTIVCARD_DEC  6   /*  ActivCard decimal mode */
#define PA_SAM_TYPE_ACTIVCARD_HEX  7   /*  ActivCard hex mode */
#define PA_SAM_TYPE_DIGI_PATH_HEX  8   /*  Digital Pathways hex mode */
#endif
#define PA_SAM_TYPE_EXP_BASE    128 /* experimental */
#define PA_SAM_TYPE_GRAIL		(PA_SAM_TYPE_EXP_BASE+0) /* testing */
#define PA_SAM_TYPE_SECURID_PREDICT	(PA_SAM_TYPE_EXP_BASE+1) /* special */

typedef struct _krb5_predicted_sam_response {
	krb5_magic	magic;
	krb5_keyblock	sam_key;
        krb5_flags      sam_flags; /* Makes key munging easier */
        krb5_timestamp  stime;  /* time on server, for replay detection */
        krb5_int32      susec;
        krb5_principal  client;
        krb5_data       msd;    /* mechanism specific data */
} krb5_predicted_sam_response;

typedef struct _krb5_sam_challenge {
	krb5_magic	magic;
	krb5_int32	sam_type; /* information */
	krb5_flags	sam_flags; /* KRB5_SAM_* values */
	krb5_data	sam_type_name;
	krb5_data	sam_track_id;
	krb5_data	sam_challenge_label;
	krb5_data	sam_challenge;
	krb5_data	sam_response_prompt;
	krb5_data	sam_pk_for_sad;
	krb5_int32	sam_nonce;
	krb5_checksum	sam_cksum;
} krb5_sam_challenge;

typedef struct _krb5_sam_key {	/* reserved for future use */
	krb5_magic	magic;
	krb5_keyblock	sam_key;
} krb5_sam_key;

typedef struct _krb5_enc_sam_response_enc {
	krb5_magic	magic;
	krb5_int32	sam_nonce;
	krb5_timestamp	sam_timestamp;
	krb5_int32	sam_usec;
	krb5_data	sam_sad;
} krb5_enc_sam_response_enc;

typedef struct _krb5_sam_response {
	krb5_magic	magic;
	krb5_int32	sam_type; /* informational */
	krb5_flags	sam_flags; /* KRB5_SAM_* values */
	krb5_data	sam_track_id; /* copied */
	krb5_enc_data	sam_enc_key; /* krb5_sam_key - future use */
	krb5_enc_data	sam_enc_nonce_or_ts; /* krb5_enc_sam_response_enc */
	krb5_int32	sam_nonce;
	krb5_timestamp	sam_patimestamp;
} krb5_sam_response;

typedef struct _krb5_sam_challenge_2 {
	krb5_data	sam_challenge_2_body;
	krb5_checksum	**sam_cksum;		/* Array of checksums */
} krb5_sam_challenge_2;

typedef struct _krb5_sam_challenge_2_body {
	krb5_magic	magic;
	krb5_int32	sam_type; /* information */
	krb5_flags	sam_flags; /* KRB5_SAM_* values */
	krb5_data	sam_type_name;
	krb5_data	sam_track_id;
	krb5_data	sam_challenge_label;
	krb5_data	sam_challenge;
	krb5_data	sam_response_prompt;
	krb5_data	sam_pk_for_sad;
	krb5_int32	sam_nonce;
	krb5_enctype	sam_etype;
} krb5_sam_challenge_2_body;

typedef struct _krb5_sam_response_2 {
	krb5_magic	magic;
	krb5_int32	sam_type; /* informational */
	krb5_flags	sam_flags; /* KRB5_SAM_* values */
	krb5_data	sam_track_id; /* copied */
	krb5_enc_data	sam_enc_nonce_or_sad; /* krb5_enc_sam_response_enc */
	krb5_int32	sam_nonce;
} krb5_sam_response_2;

typedef struct _krb5_enc_sam_response_enc_2 {
	krb5_magic	magic;
	krb5_int32	sam_nonce;
	krb5_data	sam_sad;
} krb5_enc_sam_response_enc_2;

/*
 * Begin "dbm.h"
 */
#ifndef _KERNEL

/*
 * Since we are always using db, use the db-ndbm include header file.
 */	

#include "db-ndbm.h"
	
#endif /* !KERNEL */
/*
 * End "dbm.h"
 */

/*
 * Begin "ext-proto.h"
 */
#ifndef KRB5_EXT_PROTO__
#define KRB5_EXT_PROTO__

#ifndef _KERNEL
#include <stdlib.h>

#ifdef HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

#endif /* !_KERNEL */

#ifndef HAVE_STRDUP
extern char *strdup (const char *);
#endif

#ifndef _KERNEL
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#endif /* !_KERNEL */

#endif /* KRB5_EXT_PROTO__ */
/*
 * End "ext-proto.h"
 */

/*
 * Begin "sysincl.h"
 */
#ifndef KRB5_SYSINCL__
#define KRB5_SYSINCL__

#ifndef KRB5_SYSTYPES__
#define KRB5_SYSTYPES__
/* needed for much of the rest -- but already handled in krb5.h? */
/* #include <sys/types.h> */
#endif /* KRB5_SYSTYPES__ */

#ifdef	_KERNEL
#include <sys/time.h>
#else
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#ifdef TIME_WITH_SYS_TIME
#include <time.h>
#endif
#else
#include <time.h>
#endif
#endif /* _KERNEL */

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>			/* struct stat, stat() */
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>			/* MAXPATHLEN */
#endif

#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>			/* prototypes for file-related
					   syscalls; flags for open &
					   friends */
#endif

#ifdef _KERNEL
#include <sys/fcntl.h>
#else
#include <fcntl.h>
#endif

#endif /* KRB5_SYSINCL__ */
/*
 * End "sysincl.h"
 */

/*
 * Begin "los-proto.h"
 */
#ifndef KRB5_LIBOS_PROTO__
#define KRB5_LIBOS_PROTO__

#ifndef	_KERNEL
#include <stdio.h>

struct addrlist;
#endif

/* libos.spec */
krb5_error_code krb5_lock_file
	(krb5_context, int, int);

krb5_error_code krb5_unlock_file
	(krb5_context, int);

int krb5_net_read
	(krb5_context, int , char *, int);

int krb5_net_write
	(krb5_context, int , const char *, int);

krb5_error_code krb5_sendto_kdc
	(krb5_context, const krb5_data *, const krb5_data *,
	   krb5_data *, int, int);

krb5_error_code krb5_get_krbhst
	(krb5_context, const krb5_data *, char ***);

krb5_error_code krb5_free_krbhst
	(krb5_context, char * const *);

krb5_error_code krb5_gen_replay_name
    (krb5_context, const krb5_address *, const char *, char **);

krb5_error_code krb5_create_secure_file
	(krb5_context, const char * pathname);

#ifndef	_KERNEL
krb5_error_code krb5_sync_disk_file
	(krb5_context, FILE *fp);

krb5_error_code
krb5_open_pkcs11_session(CK_SESSION_HANDLE *);
#endif


krb5_error_code krb5_read_message
	(krb5_context, krb5_pointer, krb5_data *);

krb5_error_code krb5_write_message
	(krb5_context, krb5_pointer, krb5_data *);

krb5_error_code krb5int_get_fq_local_hostname (char *, size_t);

krb5_error_code krb5_os_init_context
        (krb5_context);

void krb5_os_free_context (krb5_context);

krb5_error_code krb5_find_config_files(void);

#ifndef _KERNEL
/* N.B.: You need to include fake-addrinfo.h *before* k5-int.h if you're
 * going to use this structure.  */
struct addrlist {
	struct addrinfo **addrs;
	int naddrs;
	int space;
};

#define	ADDRLIST_INIT { 0, 0, 0 }
extern void krb5int_free_addrlist (struct addrlist *);
extern int krb5int_grow_addrlist (struct addrlist *, int);
extern int krb5int_add_host_to_list (struct addrlist *, const char *,
			int, int, int, int);

krb5_error_code krb5_locate_srv_conf
	(krb5_context, const krb5_data *, const char *,
	struct sockaddr **, int*, int);

#ifdef KRB5_DNS_LOOKUP
/* no context? */
krb5_error_code krb5_locate_srv_dns
	(const krb5_data *, const char *,
	const char *, struct sockaddr **, int *,
	char *, unsigned short *, boolean_t);

int _krb5_conf_boolean(char *);
int _krb5_use_dns_kdc(krb5_context);
int _krb5_use_dns_realm(krb5_context);

#endif /* KRB5_DNS_LOOKUP */
#endif /* _KERNEL */

#endif /* KRB5_LIBOS_PROTO__ */

/* new encryption provider api */

struct krb5_enc_provider {
    void (*block_size) (size_t *output);

    /* keybytes is the input size to make_key;
       keylength is the output size */
    void (*keysize) (size_t *keybytes, size_t *keylength);

    /* ivec == 0 is an all-zeros ivec */
    krb5_error_code (*encrypt) (
	krb5_context context,
	krb5_const krb5_keyblock *key, krb5_const krb5_data *ivec,
	krb5_const krb5_data *input, krb5_data *output);

    krb5_error_code (*decrypt) (
	krb5_context context,
	krb5_const krb5_keyblock *key, krb5_const krb5_data *ivec,
	krb5_const krb5_data *input, krb5_data *output);

    krb5_error_code (*make_key)
    (krb5_context, krb5_const krb5_data *, krb5_keyblock *);

    krb5_error_code (*init_state) (krb5_context,
			const krb5_keyblock *,
			krb5_keyusage, krb5_data *);
    krb5_error_code (*free_state) (krb5_context, krb5_data *);

};

struct krb5_hash_provider {
    void (*hash_size) (size_t *output);

    void (*block_size) (size_t *output);

    /* this takes multiple inputs to avoid lots of copying. */
    krb5_error_code (*hash) (krb5_context context,
	unsigned int icount, krb5_const krb5_data *input,
	krb5_data *output);
};

struct krb5_keyhash_provider {
    void (*hash_size) (size_t *output);

    krb5_error_code (*hash) (
	krb5_context context,
	krb5_const krb5_keyblock *key,
	krb5_keyusage keyusage,
	krb5_const krb5_data *ivec,
	krb5_const krb5_data *input, krb5_data *output);

    krb5_error_code (*verify) (
	krb5_context context,
	krb5_const krb5_keyblock *key,
	krb5_keyusage keyusage,
	krb5_const krb5_data *ivec,
	krb5_const krb5_data *input,
	krb5_const krb5_data *hash,
	krb5_boolean *valid);

};

typedef void (*krb5_encrypt_length_func) (
  krb5_const struct krb5_enc_provider *enc,
  krb5_const struct krb5_hash_provider *hash,
  size_t inputlen, size_t *length);

typedef krb5_error_code (*krb5_crypt_func) (
  krb5_context context,
  krb5_const struct krb5_enc_provider *enc,
  krb5_const struct krb5_hash_provider *hash,
  krb5_const krb5_keyblock *key, krb5_keyusage usage,
  krb5_const krb5_data *ivec,
  krb5_const krb5_data *input, krb5_data *output);

#ifndef	_KERNEL
typedef krb5_error_code (*krb5_str2key_func) (
  krb5_context context,
  krb5_const struct krb5_enc_provider *enc, krb5_const krb5_data *string,
  krb5_const krb5_data *salt, krb5_const krb5_data *params,
  krb5_keyblock *key);
#endif	/* _KERNEL */

struct krb5_keytypes {
    krb5_enctype etype;
    char *in_string;
    char *out_string;
    const struct krb5_enc_provider *enc;
    const struct krb5_hash_provider *hash;
    krb5_encrypt_length_func encrypt_len;
    krb5_crypt_func encrypt;
    krb5_crypt_func decrypt;
    krb5_cksumtype required_ctype;
#ifndef	_KERNEL
    /* Solaris Kerberos:  strings to key conversion not done in the kernel */
    krb5_str2key_func str2key;
#else	/* _KERNEL */
    char *mt_e_name;
    char *mt_h_name;
    crypto_mech_type_t kef_cipher_mt;
    crypto_mech_type_t kef_hash_mt;
#endif	/* _KERNEL */
};


struct krb5_cksumtypes {
    krb5_cksumtype ctype;
    unsigned int flags;
    char *in_string;
    char *out_string;
    /* if the hash is keyed, this is the etype it is keyed with.
       Actually, it can be keyed by any etype which has the same
       enc_provider as the specified etype.  DERIVE checksums can
       be keyed with any valid etype. */
    krb5_enctype keyed_etype;
    /* I can't statically initialize a union, so I'm just going to use
       two pointers here.  The keyhash is used if non-NULL.  If NULL,
       then HMAC/hash with derived keys is used if the relevant flag
       is set.  Otherwise, a non-keyed hash is computed.  This is all
       kind of messy, but so is the krb5 api. */
    const struct krb5_keyhash_provider *keyhash;
    const struct krb5_hash_provider *hash;
    /* This just gets uglier and uglier.  In the key derivation case,
	we produce an hmac.  To make the hmac code work, we can't hack
	the output size indicated by the hash provider, but we may want
	a truncated hmac.  If we want truncation, this is the number of
	bytes we truncate to; it should be 0 otherwise.	*/
    unsigned int trunc_size;
#ifdef _KERNEL
    char *mt_c_name;
    crypto_mech_type_t kef_cksum_mt;
#endif /* _KERNEL */
};

#define KRB5_CKSUMFLAG_DERIVE		0x0001
#define KRB5_CKSUMFLAG_NOT_COLL_PROOF	0x0002

krb5_error_code krb5int_des_init_state(
	krb5_context,
	const krb5_keyblock *,
	krb5_keyusage, krb5_data *);

krb5_error_code krb5int_c_mandatory_cksumtype(
	krb5_context,
	krb5_enctype,
	krb5_cksumtype *);

/*
 * normally to free a cipher_state you can just memset the length to zero and
 * free it.
 */
krb5_error_code krb5int_default_free_state(krb5_context, krb5_data *);

/*
 * Combine two keys (normally used by the hardware preauth mechanism)
 */
krb5_error_code krb5int_c_combine_keys
(krb5_context context, krb5_keyblock *key1, krb5_keyblock *key2,
		krb5_keyblock *outkey);
/*
 * in here to deal with stuff from lib/crypto
 */

void krb5_nfold (int inbits, krb5_const unsigned char *in,
		int outbits, unsigned char *out);

#ifdef _KERNEL

int k5_ef_crypto(
	const char *, char *,
	long, krb5_keyblock *,
	krb5_data *, int);

krb5_error_code
krb5_hmac(krb5_context, const krb5_keyblock *,
	krb5_const krb5_data *, krb5_data *);

#else
krb5_error_code krb5_hmac
	(krb5_context,
	krb5_const struct krb5_hash_provider *,
	krb5_const krb5_keyblock *, krb5_const unsigned int,
	krb5_const krb5_data *, krb5_data *);

#endif /* _KERNEL */

krb5_error_code krb5int_pbkdf2_hmac_sha1 (krb5_context,
		const krb5_data *,
		unsigned long,
		krb5_enctype,
		const krb5_data *,
		const krb5_data *);

krb5_error_code krb5_crypto_us_timeofday
    (krb5_int32  *, krb5_int32  *);

/* this helper fct is in libkrb5, but it makes sense declared here. */

krb5_error_code krb5_encrypt_helper
	(krb5_context context, krb5_const krb5_keyblock *key,
	krb5_keyusage usage, krb5_const krb5_data *plain,
	krb5_enc_data *cipher);

/*
 * End "los-proto.h"
 */

/*
 * Include the KDB definitions.
 */
#ifndef _KERNEL
#include <krb5/kdb.h>
#endif /* !_KERNEL */
/*
 * Begin "libos.h"
 */
#ifndef KRB5_LIBOS__
#define KRB5_LIBOS__

typedef struct _krb5_os_context {
	krb5_magic	magic;
	krb5_int32	time_offset;
	krb5_int32	usec_offset;
	krb5_int32	os_flags;
	char *		default_ccname;
	krb5_principal	default_ccprincipal;
} *krb5_os_context;

/*
 * Flags for the os_flags field
 *
 * KRB5_OS_TOFFSET_VALID means that the time offset fields are valid.
 * The intention is that this facility to correct the system clocks so
 * that they reflect the "real" time, for systems where for some
 * reason we can't set the system clock.  Instead we calculate the
 * offset between the system time and real time, and store the offset
 * in the os context so that we can correct the system clock as necessary.
 *
 * KRB5_OS_TOFFSET_TIME means that the time offset fields should be
 * returned as the time by the krb5 time routines.  This should only
 * be used for testing purposes (obviously!)
 */
#define KRB5_OS_TOFFSET_VALID	1
#define KRB5_OS_TOFFSET_TIME	2

/* lock mode flags */
#define	KRB5_LOCKMODE_SHARED	0x0001
#define	KRB5_LOCKMODE_EXCLUSIVE	0x0002
#define	KRB5_LOCKMODE_DONTBLOCK	0x0004
#define	KRB5_LOCKMODE_UNLOCK	0x0008

#endif /* KRB5_LIBOS__ */
/*
 * End "libos.h"
 */

/*
 * Define our view of the size of a DES key.
 */
#define	KRB5_MIT_DES_KEYSIZE		8

/*
 * Define a couple of SHA1 constants
 */
#define	SHS_DATASIZE	64
#define	SHS_DIGESTSIZE	20

/*
 * Check if des_int.h has been included before us.  If so, then check to see
 * that our view of the DES key size is the same as des_int.h's.
 */
#ifdef	MIT_DES_KEYSIZE
#if	MIT_DES_KEYSIZE != KRB5_MIT_DES_KEYSIZE
error(MIT_DES_KEYSIZE does not equal KRB5_MIT_DES_KEYSIZE)
#endif	/* MIT_DES_KEYSIZE != KRB5_MIT_DES_KEYSIZE */
#endif	/* MIT_DES_KEYSIZE */

#ifndef _KERNEL
/* Solaris Kerberos: only define PROVIDE_DES3_CBC_SHA if the following are
 * defined.
 */
#define PROVIDE_DES3_CBC_SHA 1
#define PROVIDE_NIST_SHA 1

#endif /* !_KERNEL */

/*
 * Begin "preauth.h"
 *
 * (Originally written by Glen Machin at Sandia Labs.)
 */
/*
 * Sandia National Laboratories also makes no representations about the
 * suitability of the modifications, or additions to this software for
 * any purpose.  It is provided "as is" without express or implied warranty.
 *
 */
#ifndef KRB5_PREAUTH__
#define KRB5_PREAUTH__

typedef struct _krb5_pa_enc_ts {
    krb5_timestamp	patimestamp;
    krb5_int32		pausec;
} krb5_pa_enc_ts;

typedef krb5_error_code (*krb5_preauth_obtain_proc)
    (krb5_context,
	    krb5_pa_data *,
	    krb5_etype_info,
	    krb5_keyblock *,
	    krb5_error_code ( * )(krb5_context,
				  krb5_const krb5_enctype,
				  krb5_data *,
				  krb5_const_pointer,
				  krb5_keyblock **),
	    krb5_const_pointer,
	    krb5_creds *,
	    krb5_kdc_req *,
	    krb5_pa_data **);

typedef krb5_error_code (*krb5_preauth_process_proc)
    (krb5_context,
	    krb5_pa_data *,
	    krb5_kdc_req *,
	    krb5_kdc_rep *,
	    krb5_error_code ( * )(krb5_context,
				  krb5_const krb5_enctype,
				  krb5_data *,
				  krb5_const_pointer,
				  krb5_keyblock **),
	    krb5_const_pointer,
	    krb5_error_code ( * )(krb5_context,
				  krb5_const krb5_keyblock *,
				  krb5_const_pointer,
				  krb5_kdc_rep * ),
	    krb5_keyblock **,
	    krb5_creds *,
	    krb5_int32 *,
	    krb5_int32 *);

typedef struct _krb5_preauth_ops {
    krb5_magic magic;
    int     type;
    int	flags;
    krb5_preauth_obtain_proc	obtain;
    krb5_preauth_process_proc	process;
} krb5_preauth_ops;

void krb5_free_etype_info (krb5_context, krb5_etype_info);

/*
 * Preauthentication property flags
 */
#define KRB5_PREAUTH_FLAGS_ENCRYPT	0x00000001
#define KRB5_PREAUTH_FLAGS_HARDWARE	0x00000002

#endif /* KRB5_PREAUTH__ */
/*
 * End "preauth.h"
 */

krb5_error_code
krb5int_copy_data_contents (krb5_context, const krb5_data *, krb5_data *);

#ifndef _KERNEL /* needed for lib/krb5/krb/ */
typedef krb5_error_code (*krb5_gic_get_as_key_fct)
    (krb5_context,
     krb5_principal,
     krb5_enctype,
     krb5_prompter_fct,
     void *prompter_data,
     krb5_data *salt,
     krb5_data *s2kparams,
     krb5_keyblock *as_key,
     void *gak_data);

krb5_error_code KRB5_CALLCONV
krb5_get_init_creds
(krb5_context context,
	krb5_creds *creds,
	krb5_principal client,
	krb5_prompter_fct prompter,
	void *prompter_data,
	krb5_deltat start_time,
	char *in_tkt_service,
	krb5_get_init_creds_opt *options,
	krb5_gic_get_as_key_fct gak,
	void *gak_data,
	int master,
	krb5_kdc_rep **as_reply);

krb5_error_code krb5_do_preauth
	(krb5_context, krb5_kdc_req *,
	krb5_pa_data **, krb5_pa_data ***,
	krb5_data *, krb5_data *, krb5_enctype *,
	krb5_keyblock *,
	krb5_prompter_fct, void *,
	krb5_gic_get_as_key_fct, void *);
#endif /* _KERNEL */

void KRB5_CALLCONV krb5_free_sam_challenge
	(krb5_context, krb5_sam_challenge * );
void KRB5_CALLCONV krb5_free_sam_challenge_2
	(krb5_context, krb5_sam_challenge_2 * );
void KRB5_CALLCONV krb5_free_sam_challenge_2_body
	(krb5_context, krb5_sam_challenge_2_body *);
void KRB5_CALLCONV krb5_free_sam_response
	(krb5_context, krb5_sam_response * );
void KRB5_CALLCONV krb5_free_sam_response_2
	(krb5_context, krb5_sam_response_2 * );
void KRB5_CALLCONV krb5_free_predicted_sam_response
	(krb5_context, krb5_predicted_sam_response * );
void KRB5_CALLCONV krb5_free_enc_sam_response_enc
	(krb5_context, krb5_enc_sam_response_enc * );
void KRB5_CALLCONV krb5_free_enc_sam_response_enc_2
	(krb5_context, krb5_enc_sam_response_enc_2 * );
void KRB5_CALLCONV krb5_free_sam_challenge_contents
	(krb5_context, krb5_sam_challenge * );
void KRB5_CALLCONV krb5_free_sam_challenge_2_contents
	(krb5_context, krb5_sam_challenge_2 * );
void KRB5_CALLCONV krb5_free_sam_challenge_2_body_contents
	(krb5_context, krb5_sam_challenge_2_body * );
void KRB5_CALLCONV krb5_free_sam_response_contents
	(krb5_context, krb5_sam_response * );
void KRB5_CALLCONV krb5_free_sam_response_2_contents
	(krb5_context, krb5_sam_response_2 *);
void KRB5_CALLCONV krb5_free_predicted_sam_response_contents
	(krb5_context, krb5_predicted_sam_response * );
void KRB5_CALLCONV krb5_free_enc_sam_response_enc_contents
	(krb5_context, krb5_enc_sam_response_enc * );
void KRB5_CALLCONV krb5_free_enc_sam_response_enc_2_contents
	(krb5_context, krb5_enc_sam_response_enc_2 * );

void KRB5_CALLCONV krb5_free_pa_enc_ts
	(krb5_context, krb5_pa_enc_ts *);

/* #include "krb5/wordsize.h" -- comes in through base-defs.h. */
/*
 * Solaris Kerberos: moved from sendto_kdc.c so other code can reference
 */
#define	DEFAULT_UDP_PREF_LIMIT   1465

#ifndef	_KERNEL
#include "profile.h"
#include <strings.h>
#endif /* _KERNEL */

#define KEY_CHANGED(k1, k2) \
(k1 == NULL || \
 k1 != k2 || \
 k1->enctype != k2->enctype || \
 k1->length != k2->length || \
 bcmp(k1->contents, k2->contents, k1->length))

#ifndef _KERNEL
typedef struct _arcfour_ctx {
	CK_SESSION_HANDLE eSession; /* encrypt session handle */
	CK_SESSION_HANDLE dSession; /* decrypt session handle */
	CK_OBJECT_HANDLE  eKey; /* encrypt key object */
	CK_OBJECT_HANDLE  dKey; /* decrype key object */
	uchar_t           initialized;
}arcfour_ctx_rec;

#endif /* !_KERNEL */

struct _krb5_context {
	krb5_magic	magic;
	krb5_enctype	*in_tkt_ktypes;
	int		in_tkt_ktype_count;
	krb5_enctype	*tgs_ktypes;
	int		tgs_ktype_count;
	void		*os_context;
	char		*default_realm;
	int		ser_ctx_count;
	krb5_boolean	profile_secure;
	void	      	*ser_ctx;
#ifndef _KERNEL
	profile_t	profile;
	void		*db_context;
	void		*kdblog_context;
	/* allowable clock skew */
	krb5_deltat 	clockskew;
	krb5_cksumtype	kdc_req_sumtype;
	krb5_cksumtype	default_ap_req_sumtype;
	krb5_cksumtype	default_safe_sumtype;
	krb5_flags 	kdc_default_options;
	krb5_flags	library_options;
	int		fcc_default_format;
	int		scc_default_format;
	krb5_prompt_type *prompt_types;
	/* Message size above which we'll try TCP first in send-to-kdc
	   type code.  Aside from the 2**16 size limit, we put no
	   absolute limit on the UDP packet size.  */
	int		udp_pref_limit;

	/* This is the tgs_ktypes list as read from the profile, or
	   set to compiled-in defaults.	 The application code cannot
	   override it.	 This is used for session keys for
	   intermediate ticket-granting tickets used to acquire the
	   requested ticket (the session key of which may be
	   constrained by tgs_ktypes above).  */
	krb5_enctype	*conf_tgs_ktypes;
	int		conf_tgs_ktypes_count;

	/* Use the _configured version?	 */
	krb5_boolean	use_conf_ktypes;
#ifdef KRB5_DNS_LOOKUP
	krb5_boolean	profile_in_memory;
#endif /* KRB5_DNS_LOOKUP */

	pid_t pid;  /* fork safety: PID of process that did last PKCS11 init */

	/* Solaris Kerberos: handles for PKCS#11 crypto */
	/* 
	 * Warning, do not access hSession directly as this is not fork() safe.
	 * Instead use the krb_ctx_hSession() macro below. 
	 */
	CK_SESSION_HANDLE hSession;	
	int		cryptoki_initialized;

	/* arcfour_ctx: used only for rcmd stuff so no fork safety issues apply */
	arcfour_ctx_rec arcfour_ctx;
#else /* ! KERNEL */
	crypto_mech_type_t kef_cipher_mt;
	crypto_mech_type_t kef_hash_mt;
	crypto_mech_type_t kef_cksum_mt;
#endif /* ! KERNEL */
};

#ifndef  _KERNEL
extern pid_t __krb5_current_pid;

CK_SESSION_HANDLE krb5_reinit_ef_handle(krb5_context);

/* 
 * fork safety: barring the ef_init code, every other function must use the
 * krb_ctx_hSession() macro to access the hSession field in a krb context.
 * Note, if the pid of the krb ctx == the current global pid then it is safe to
 * use the ctx hSession otherwise it needs to be re-inited before it is returned
 * to the caller.
 */
#define krb_ctx_hSession(ctx) \
    ((ctx)->pid == __krb5_current_pid) ? (ctx)->hSession : krb5_reinit_ef_handle((ctx))
#endif

#define MD5_CKSUM_LENGTH 16
#define RSA_MD5_CKSUM_LENGTH 16
#define MD5_BLOCKSIZE 64


/*
 * Solaris Kerberos:
 * This next section of prototypes and constants
 * are all unique to the Solaris Kerberos implementation.
 * Because Solaris uses the native encryption framework
 * to provide crypto support, the following routines
 * are needed to support this system.
 */

/*
 * Begin Solaris Crypto Prototypes
 */

/*
 * define constants that are used for creating the constant
 * which is used to make derived keys.
 */
#define DK_ENCR_KEY_BYTE 0xAA
#define DK_HASH_KEY_BYTE 0x55
#define DK_CKSUM_KEY_BYTE 0x99

int init_derived_keydata(krb5_context, const struct krb5_enc_provider *,
			krb5_keyblock *, krb5_keyusage,
			krb5_keyblock **, krb5_keyblock **);

krb5_error_code add_derived_key(krb5_keyblock *, krb5_keyusage, uchar_t,
				krb5_keyblock *);

krb5_keyblock *find_derived_key(krb5_keyusage, uchar_t, krb5_keyblock *);
krb5_keyblock *krb5_create_derived_keyblock(int);

#ifdef _KERNEL
int k5_ef_hash(krb5_context, int, const krb5_data *, krb5_data *);

int k5_ef_mac(krb5_context, krb5_keyblock *, krb5_data *,
        const krb5_data *, krb5_data *);

void make_kef_key(krb5_keyblock *);
int init_key_kef(crypto_mech_type_t, krb5_keyblock *);
int update_key_template(krb5_keyblock *);
void setup_kef_keytypes();
void setup_kef_cksumtypes();
crypto_mech_type_t get_cipher_mech_type(krb5_context, krb5_keyblock *);
crypto_mech_type_t get_hash_mech_type(krb5_context, krb5_keyblock *);

#else
/* 
 * This structure is used to map Kerberos supported OID's,
 * to PKCS11 mechanisms
 */
#define USE_ENCR	0x01
#define	USE_HASH	0x02

typedef struct krb5_mech_2_pkcs {
	uchar_t		flags;
	CK_MECHANISM_TYPE enc_algo;
	CK_MECHANISM_TYPE hash_algo;
	CK_MECHANISM_TYPE str2key_algo;
} KRB5_MECH_TO_PKCS; 

#define ENC_DEFINED(x)	(((x).flags & USE_ENCR))
#define HASH_DEFINED(x)	(((x).flags & USE_HASH))

extern CK_RV get_algo(krb5_enctype etype, KRB5_MECH_TO_PKCS * algos);
extern CK_RV get_key_type (krb5_enctype etype, CK_KEY_TYPE * keyType);
extern krb5_error_code slot_supports_krb5 (CK_SLOT_ID_PTR slotid);

krb5_error_code init_key_uef(CK_SESSION_HANDLE, krb5_keyblock *);

krb5_error_code k5_ef_hash(krb5_context, CK_MECHANISM *,
	unsigned int, const krb5_data *, krb5_data *);

krb5_error_code k5_ef_mac(krb5_context context,
	krb5_keyblock *key, krb5_data *ivec,
	krb5_const krb5_data *input, krb5_data *output);

#endif	/* !_KERNEL */

krb5_error_code
derive_3des_keys(krb5_context, struct krb5_enc_provider *,
                krb5_keyblock *, krb5_keyusage,
                krb5_keyblock *, krb5_keyblock *);

/*
 * End Solaris Crypto Prototypes
 */

#define KRB5_LIBOPT_SYNC_KDCTIME	0x0001

/*
 * Begin "asn1.h"
 */
#ifndef KRB5_ASN1__
#define KRB5_ASN1__

/* ASN.1 encoding knowledge; KEEP IN SYNC WITH ASN.1 defs! */
/* here we use some knowledge of ASN.1 encodings */
/*
  Ticket is APPLICATION 1.
  Authenticator is APPLICATION 2.
  AS_REQ is APPLICATION 10.
  AS_REP is APPLICATION 11.
  TGS_REQ is APPLICATION 12.
  TGS_REP is APPLICATION 13.
  AP_REQ is APPLICATION 14.
  AP_REP is APPLICATION 15.
  KRB_SAFE is APPLICATION 20.
  KRB_PRIV is APPLICATION 21.
  KRB_CRED is APPLICATION 22.
  EncASRepPart is APPLICATION 25.
  EncTGSRepPart is APPLICATION 26.
  EncAPRepPart is APPLICATION 27.
  EncKrbPrivPart is APPLICATION 28.
  EncKrbCredPart is APPLICATION 29.
  KRB_ERROR is APPLICATION 30.
 */
/* allow either constructed or primitive encoding, so check for bit 6
   set or reset */
#define krb5_is_krb_ticket(dat)\
	((dat) && (dat)->length && ((dat)->data[0] == 0x61 ||\
				    (dat)->data[0] == 0x41))
#define krb5_is_krb_authenticator(dat)\
	((dat) && (dat)->length && ((dat)->data[0] == 0x62 ||\
				    (dat)->data[0] == 0x42))
#define krb5_is_as_req(dat)\
	((dat) && (dat)->length && ((dat)->data[0] == 0x6a ||\
				    (dat)->data[0] == 0x4a))
#define krb5_is_as_rep(dat)\
	((dat) && (dat)->length && ((dat)->data[0] == 0x6b ||\
				    (dat)->data[0] == 0x4b))
#define krb5_is_tgs_req(dat)\
	((dat) && (dat)->length && ((dat)->data[0] == 0x6c ||\
				    (dat)->data[0] == 0x4c))
#define krb5_is_tgs_rep(dat)\
	((dat) && (dat)->length && ((dat)->data[0] == 0x6d ||\
				    (dat)->data[0] == 0x4d))
#define krb5_is_ap_req(dat)\
	((dat) && (dat)->length && ((dat)->data[0] == 0x6e ||\
				    (dat)->data[0] == 0x4e))
#define krb5_is_ap_rep(dat)\
	((dat) && (dat)->length && ((dat)->data[0] == 0x6f ||\
				    (dat)->data[0] == 0x4f))
#define krb5_is_krb_safe(dat)\
	((dat) && (dat)->length && ((dat)->data[0] == 0x74 ||\
				    (dat)->data[0] == 0x54))
#define krb5_is_krb_priv(dat)\
	((dat) && (dat)->length && ((dat)->data[0] == 0x75 ||\
				    (dat)->data[0] == 0x55))
#define krb5_is_krb_cred(dat)\
	((dat) && (dat)->length && ((dat)->data[0] == 0x76 ||\
				    (dat)->data[0] == 0x56))
#define krb5_is_krb_enc_as_rep_part(dat)\
	((dat) && (dat)->length && ((dat)->data[0] == 0x79 ||\
				    (dat)->data[0] == 0x59))
#define krb5_is_krb_enc_tgs_rep_part(dat)\
	((dat) && (dat)->length && ((dat)->data[0] == 0x7a ||\
				    (dat)->data[0] == 0x5a))
#define krb5_is_krb_enc_ap_rep_part(dat)\
	((dat) && (dat)->length && ((dat)->data[0] == 0x7b ||\
				    (dat)->data[0] == 0x5b))
#define krb5_is_krb_enc_krb_priv_part(dat)\
	((dat) && (dat)->length && ((dat)->data[0] == 0x7c ||\
				    (dat)->data[0] == 0x5c))
#define krb5_is_krb_enc_krb_cred_part(dat)\
	((dat) && (dat)->length && ((dat)->data[0] == 0x7d ||\
				    (dat)->data[0] == 0x5d))
#define krb5_is_krb_error(dat)\
	((dat) && (dat)->length && ((dat)->data[0] == 0x7e ||\
				    (dat)->data[0] == 0x5e))

/*************************************************************************
 * Prototypes for krb5_encode.c
 *************************************************************************/

/*
   krb5_error_code encode_krb5_structure(const krb5_structure *rep,
					 krb5_data **code);
   modifies  *code
   effects   Returns the ASN.1 encoding of *rep in **code.
             Returns ASN1_MISSING_FIELD if a required field is emtpy in *rep.
             Returns ENOMEM if memory runs out.
*/

krb5_error_code encode_krb5_authenticator
	(const krb5_authenticator *rep, krb5_data **code);

krb5_error_code encode_krb5_ticket
	(const krb5_ticket *rep, krb5_data **code);

krb5_error_code encode_krb5_encryption_key
	(const krb5_keyblock *rep, krb5_data **code);

krb5_error_code encode_krb5_enc_tkt_part
	(const krb5_enc_tkt_part *rep, krb5_data **code);

krb5_error_code encode_krb5_enc_kdc_rep_part
	(const krb5_enc_kdc_rep_part *rep, krb5_data **code);

/* yes, the translation is identical to that used for KDC__REP */
krb5_error_code encode_krb5_as_rep
	(const krb5_kdc_rep *rep, krb5_data **code);

/* yes, the translation is identical to that used for KDC__REP */
krb5_error_code encode_krb5_tgs_rep
	(const krb5_kdc_rep *rep, krb5_data **code);

krb5_error_code encode_krb5_ap_req
	(const krb5_ap_req *rep, krb5_data **code);

krb5_error_code encode_krb5_ap_rep
	(const krb5_ap_rep *rep, krb5_data **code);

krb5_error_code encode_krb5_ap_rep_enc_part
	(const krb5_ap_rep_enc_part *rep, krb5_data **code);

krb5_error_code encode_krb5_as_req
	(const krb5_kdc_req *rep, krb5_data **code);

krb5_error_code encode_krb5_tgs_req
	(const krb5_kdc_req *rep, krb5_data **code);

krb5_error_code encode_krb5_kdc_req_body
	(const krb5_kdc_req *rep, krb5_data **code);

krb5_error_code encode_krb5_safe
	(const krb5_safe *rep, krb5_data **code);

krb5_error_code encode_krb5_safe_with_body
	(const krb5_safe *rep, const krb5_data *body, krb5_data **code);

krb5_error_code encode_krb5_priv
	(const krb5_priv *rep, krb5_data **code);

krb5_error_code encode_krb5_enc_priv_part
	(const krb5_priv_enc_part *rep, krb5_data **code);

krb5_error_code encode_krb5_cred
	(const krb5_cred *rep, krb5_data **code);

krb5_error_code encode_krb5_enc_cred_part
	(const krb5_cred_enc_part *rep, krb5_data **code);

krb5_error_code encode_krb5_error
	(const krb5_error *rep, krb5_data **code);

krb5_error_code encode_krb5_authdata
	(const krb5_authdata **rep, krb5_data **code);

krb5_error_code encode_krb5_pwd_sequence
	(const passwd_phrase_element *rep, krb5_data **code);

krb5_error_code encode_krb5_pwd_data
	(const krb5_pwd_data *rep, krb5_data **code);

krb5_error_code encode_krb5_padata_sequence
	(const krb5_pa_data ** rep, krb5_data **code);

krb5_error_code encode_krb5_alt_method
	(const krb5_alt_method *, krb5_data **code);

krb5_error_code encode_krb5_etype_info
	(const krb5_etype_info_entry **, krb5_data **code);

krb5_error_code encode_krb5_etype_info2
	(const krb5_etype_info_entry **, krb5_data **code);

krb5_error_code encode_krb5_enc_data
    	(const krb5_enc_data *, krb5_data **);

krb5_error_code encode_krb5_pa_enc_ts
    	(const krb5_pa_enc_ts *, krb5_data **);

krb5_error_code encode_krb5_sam_challenge
	(const krb5_sam_challenge * , krb5_data **);

krb5_error_code encode_krb5_sam_key
	(const krb5_sam_key * , krb5_data **);

krb5_error_code encode_krb5_enc_sam_response_enc
	(const krb5_enc_sam_response_enc * , krb5_data **);

krb5_error_code encode_krb5_sam_response
	(const krb5_sam_response * , krb5_data **);

krb5_error_code encode_krb5_predicted_sam_response
	(const krb5_predicted_sam_response * , krb5_data **);

krb5_error_code encode_krb5_sam_challenge_2
	(const krb5_sam_challenge_2 * , krb5_data **);

krb5_error_code encode_krb5_sam_challenge_2_body
	(const krb5_sam_challenge_2_body * , krb5_data **);

krb5_error_code encode_krb5_enc_sam_response_enc_2
	(const krb5_enc_sam_response_enc_2 * , krb5_data **);

krb5_error_code encode_krb5_sam_response_2
	(const krb5_sam_response_2 * , krb5_data **);

/*************************************************************************
 * End of prototypes for krb5_encode.c
 *************************************************************************/

/*************************************************************************
 * Prototypes for krb5_decode.c
 *************************************************************************/

/*
   krb5_error_code decode_krb5_structure(const krb5_data *code,
                                         krb5_structure **rep);

   requires  Expects **rep to not have been allocated;
              a new *rep is allocated regardless of the old value.
   effects   Decodes *code into **rep.
	     Returns ENOMEM if memory is exhausted.
             Returns asn1 and krb5 errors.
*/

krb5_error_code decode_krb5_authenticator
	(const krb5_data *code, krb5_authenticator **rep);

krb5_error_code decode_krb5_ticket
	(const krb5_data *code, krb5_ticket **rep);

krb5_error_code decode_krb5_encryption_key
	(const krb5_data *output, krb5_keyblock **rep);

krb5_error_code decode_krb5_enc_tkt_part
	(const krb5_data *output, krb5_enc_tkt_part **rep);

krb5_error_code decode_krb5_enc_kdc_rep_part
	(const krb5_data *output, krb5_enc_kdc_rep_part **rep);

krb5_error_code decode_krb5_as_rep
	(const krb5_data *output, krb5_kdc_rep **rep);

krb5_error_code decode_krb5_tgs_rep
	(const krb5_data *output, krb5_kdc_rep **rep);

krb5_error_code decode_krb5_ap_req
	(const krb5_data *output, krb5_ap_req **rep);

krb5_error_code decode_krb5_ap_rep
	(const krb5_data *output, krb5_ap_rep **rep);

krb5_error_code decode_krb5_ap_rep_enc_part
	(const krb5_data *output, krb5_ap_rep_enc_part **rep);

krb5_error_code decode_krb5_as_req
	(const krb5_data *output, krb5_kdc_req **rep);

krb5_error_code decode_krb5_tgs_req
	(const krb5_data *output, krb5_kdc_req **rep);

krb5_error_code decode_krb5_kdc_req_body
	(const krb5_data *output, krb5_kdc_req **rep);

krb5_error_code decode_krb5_safe
	(const krb5_data *output, krb5_safe **rep);

krb5_error_code decode_krb5_safe_with_body
	(const krb5_data *output, krb5_safe **rep, krb5_data *body);

krb5_error_code decode_krb5_priv
	(const krb5_data *output, krb5_priv **rep);

krb5_error_code decode_krb5_enc_priv_part
	(const krb5_data *output, krb5_priv_enc_part **rep);

krb5_error_code decode_krb5_cred
	(const krb5_data *output, krb5_cred **rep);

krb5_error_code decode_krb5_enc_cred_part
	(const krb5_data *output, krb5_cred_enc_part **rep);

krb5_error_code decode_krb5_error
	(const krb5_data *output, krb5_error **rep);

krb5_error_code decode_krb5_authdata
	(const krb5_data *output, krb5_authdata ***rep);

krb5_error_code decode_krb5_pwd_sequence
	(const krb5_data *output, passwd_phrase_element **rep);

krb5_error_code decode_krb5_pwd_data
	(const krb5_data *output, krb5_pwd_data **rep);

krb5_error_code decode_krb5_padata_sequence
	(const krb5_data *output, krb5_pa_data ***rep);

krb5_error_code decode_krb5_alt_method
	(const krb5_data *output, krb5_alt_method **rep);

krb5_error_code decode_krb5_etype_info
	(const krb5_data *output, krb5_etype_info_entry ***rep);

krb5_error_code decode_krb5_etype_info2
	(const krb5_data *output, krb5_etype_info_entry ***rep);

krb5_error_code decode_krb5_enc_data
	(const krb5_data *output, krb5_enc_data **rep);

krb5_error_code decode_krb5_pa_enc_ts
	(const krb5_data *output, krb5_pa_enc_ts **rep);

krb5_error_code decode_krb5_sam_challenge
	(const krb5_data *, krb5_sam_challenge **);

krb5_error_code decode_krb5_sam_key
	(const krb5_data *, krb5_sam_key **);

krb5_error_code decode_krb5_enc_sam_response_enc
	(const krb5_data *, krb5_enc_sam_response_enc **);

krb5_error_code decode_krb5_sam_response
	(const krb5_data *, krb5_sam_response **);

krb5_error_code decode_krb5_predicted_sam_response
	(const krb5_data *, krb5_predicted_sam_response **);

krb5_error_code decode_krb5_sam_challenge_2
	(const krb5_data *, krb5_sam_challenge_2 **);

krb5_error_code decode_krb5_sam_challenge_2_body
	(const krb5_data *, krb5_sam_challenge_2_body **);

krb5_error_code decode_krb5_enc_sam_response_enc_2
	(const krb5_data *, krb5_enc_sam_response_enc_2 **);

krb5_error_code decode_krb5_sam_response_2
	(const krb5_data *, krb5_sam_response_2 **);


/*************************************************************************
 * End of prototypes for krb5_decode.c
 *************************************************************************/

#endif /* KRB5_ASN1__ */
/*
 * End "asn1.h"
 */


/*
 * Internal krb5 library routines
 */
krb5_error_code krb5_encrypt_tkt_part
	(krb5_context,
	   krb5_const krb5_keyblock *,
	   krb5_ticket *);


krb5_error_code krb5_encode_kdc_rep
	(krb5_context,
	   krb5_const krb5_msgtype,
	   krb5_const krb5_enc_kdc_rep_part *,
	   int using_subkey,
	   krb5_const krb5_keyblock *,
	   krb5_kdc_rep *,
	   krb5_data ** );

krb5_error_code krb5_validate_times
	(krb5_context, krb5_ticket_times *);

/*
 * [De]Serialization Handle and operations.
 */
struct __krb5_serializer {
    krb5_magic		odtype;
    krb5_error_code	(*sizer) (krb5_context,
				  krb5_pointer,
				  size_t *);
    krb5_error_code	(*externalizer) (krb5_context,
					 krb5_pointer,
					 krb5_octet **,
					 size_t *);
    krb5_error_code	(*internalizer) (krb5_context,
					 krb5_pointer *,
					 krb5_octet **,
					 size_t *);
};
typedef struct __krb5_serializer * krb5_ser_handle;
typedef struct __krb5_serializer krb5_ser_entry;

krb5_ser_handle krb5_find_serializer
	(krb5_context, krb5_magic);

krb5_error_code krb5_register_serializer
	(krb5_context, const krb5_ser_entry *);

/* Determine the external size of a particular opaque structure */
KRB5_DLLIMP krb5_error_code KRB5_CALLCONV krb5_size_opaque
	(krb5_context, krb5_magic, krb5_pointer, size_t *);

/* Serialize the structure into a buffer */
KRB5_DLLIMP krb5_error_code KRB5_CALLCONV krb5_externalize_opaque
	(krb5_context,
	krb5_magic,
	krb5_pointer,
	krb5_octet * *,
	size_t *);

/* Deserialize the structure from a buffer */
KRB5_DLLIMP krb5_error_code KRB5_CALLCONV krb5_internalize_opaque
	(krb5_context, krb5_magic, krb5_pointer *,
		krb5_octet * *, size_t *);

/* Serialize data into a buffer */
krb5_error_code krb5_externalize_data
	(krb5_context, krb5_pointer, krb5_octet **, size_t *);
/*
 * Initialization routines.
 */

/* Initialize serialization for krb5_[os_]context */
KRB5_DLLIMP krb5_error_code KRB5_CALLCONV krb5_ser_context_init
	(krb5_context);

/* Initialize serialization for krb5_auth_context */
KRB5_DLLIMP krb5_error_code KRB5_CALLCONV krb5_ser_auth_context_init
	(krb5_context);

/* Initialize serialization for krb5_keytab */
KRB5_DLLIMP krb5_error_code KRB5_CALLCONV krb5_ser_keytab_init
	(krb5_context);

/* Initialize serialization for krb5_ccache */
KRB5_DLLIMP krb5_error_code KRB5_CALLCONV krb5_ser_ccache_init
	(krb5_context);

/* Initialize serialization for krb5_rcache */
KRB5_DLLIMP krb5_error_code KRB5_CALLCONV krb5_ser_rcache_init
	(krb5_context);

/* [De]serialize 4-byte integer */
krb5_error_code KRB5_CALLCONV krb5_ser_pack_int32
	(krb5_int32, krb5_octet * *, size_t *);

krb5_error_code KRB5_CALLCONV krb5_ser_pack_int64
	(krb5_int64, krb5_octet * *, size_t *);

krb5_error_code KRB5_CALLCONV krb5_ser_unpack_int32
	(krb5_int32 *, krb5_octet **, size_t *);

krb5_error_code KRB5_CALLCONV krb5_ser_unpack_int64
	(krb5_int64 *, krb5_octet **, size_t *);

/* [De]serialize byte string */
KRB5_DLLIMP krb5_error_code KRB5_CALLCONV krb5_ser_pack_bytes
	(krb5_octet *,
	size_t, krb5_octet * *, size_t *);

KRB5_DLLIMP krb5_error_code KRB5_CALLCONV krb5_ser_unpack_bytes
	(krb5_octet *,
	size_t, krb5_octet * *, size_t *);

KRB5_DLLIMP krb5_error_code KRB5_CALLCONV krb5int_cc_default
	(krb5_context, krb5_ccache *);

krb5_error_code KRB5_CALLCONV krb5_cc_retrieve_cred_default
	(krb5_context, krb5_ccache, krb5_flags, krb5_creds *, krb5_creds *);

void krb5int_set_prompt_types
	(krb5_context, krb5_prompt_type *);

krb5_error_code
krb5int_generate_and_save_subkey (krb5_context, krb5_auth_context,
		krb5_keyblock * /* Old keyblock, not new!  */);



extern const struct krb5_hash_provider krb5int_hash_md5;
extern const struct krb5_enc_provider krb5int_enc_arcfour;

struct srv_dns_entry {
	struct srv_dns_entry *next;
	int priority;
	int weight;
	unsigned short port;
	char *host;
};

krb5_error_code
krb5int_make_srv_query_realm(const krb5_data *realm,
	const char *service,
	const char *protocol,
	struct srv_dns_entry **answers);
void krb5int_free_srv_dns_data(struct srv_dns_entry *);

/*
 * Convenience function for structure magic number
 */
#define KRB5_VERIFY_MAGIC(structure,magic_number) \
    if ((structure)->magic != (magic_number)) return (magic_number);

int krb5_seteuid  (int);

char * krb5_getenv(const char *);

int krb5_setenv  (const char *, const char *, int);

void krb5_unsetenv  (const char *);

#endif /* _KRB5_INT_H */
