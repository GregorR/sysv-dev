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

#ifndef	_SYS_PANIC_H
#define	_SYS_PANIC_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#if !defined(_ASM)
#include <sys/types.h>
#include <sys/thread.h>
#include <sys/cpuvar.h>
#endif	/* !_ASM */

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef _LP64
#define	PANICSTKSIZE	16384
#else
#define	PANICSTKSIZE	8192
#endif

#define	PANICBUFSIZE	8192
#define	PANICBUFVERS	1

#define	PANICNVNAMELEN	16

/*
 * Panicbuf Format:
 *
 * The kernel records the formatted panic message and an optional array of
 * name/value pairs into panicbuf[], a fixed-size buffer which is saved in
 * the crash dump and, on some platforms, is persistent across reboots.
 * The initial part of the buffer is a struct of type panic_data_t, which
 * includes a version number for identifying the format of subsequent data.
 *
 * The pd_msgoff word identifies the byte offset into panicbuf[] at which the
 * null-terminated panic message is located.  This is followed by an optional
 * variable-sized array of panic_nv_t items, which are used to record CPU
 * register values.  The number of items in pd_nvdata is computed as follows:
 *
 * (pd_msgoff - (sizeof (panic_data_t) - sizeof (panic_nv_t))) /
 * 	sizeof (panic_nv_t);
 *
 * In addition to panicbuf, debuggers can access the panic_* variables shown
 * below to determine more information about the initiator of the panic.
 */

#if !defined(_ASM)

typedef struct panic_nv {
	char pnv_name[PANICNVNAMELEN];	/* String name */
	uint64_t pnv_value;		/* Value */
} panic_nv_t;

typedef struct panic_data {
	uint32_t pd_version;		/* Version number of panic_data_t */
	uint32_t pd_msgoff;		/* Message byte offset in panicbuf */
	panic_nv_t pd_nvdata[1];	/* Array of named data */
} panic_data_t;

#if defined(_KERNEL)

/*
 * Kernel macros for adding information to pd_nvdata[].  PANICNVGET() returns
 * a panic_nv_t pointer (pnv) after the end of the existing data, PANICNVADD()
 * modifies the current item and increments pnv, and PANICNVSET() rewrites
 * pd_msgoff to indicate the end of pd_nvdata[].
 */
#define	PANICNVGET(pdp)							\
	((pdp)->pd_nvdata + (((pdp)->pd_msgoff -			\
	(sizeof (panic_data_t) - sizeof (panic_nv_t))) / sizeof (panic_nv_t)))

#define	PANICNVADD(pnv, n, v)						\
	{								\
		(void) strncpy((pnv)->pnv_name, (n), PANICNVNAMELEN);	\
		(pnv)->pnv_value = (uint64_t)(v); (pnv)++;		\
	}

#define	PANICNVSET(pdp, pnv) \
	(pdp)->pd_msgoff = (uint32_t)((char *)(pnv) - (char *)(pdp));

/*
 * Kernel panic data; preserved in crash dump for debuggers.
 */
#pragma align 8(panicbuf)
extern char panicbuf[PANICBUFSIZE];
extern kthread_t *panic_thread;
extern cpu_t panic_cpu;
extern hrtime_t panic_hrtime;
extern timespec_t panic_hrestime;

/*
 * Miscellaneous state variables defined in or used by the panic code:
 */
extern char *panic_bootstr;
extern int panic_bootfcn;
extern int panic_forced;
extern int halt_on_panic;
extern int nopanicdebug;
extern int do_polled_io;
extern int obpdebug;
extern int in_sync;
extern int panic_quiesce;
extern int panic_sync;
extern int panic_dump;

/*
 * Forward declarations for types:
 */
struct trap_info;
struct regs;

/*
 * Panic functions called from the common panic code which must be
 * implemented by architecture or platform-specific code:
 */
extern void panic_saveregs(panic_data_t *, struct regs *);
extern void panic_savetrap(panic_data_t *, struct trap_info *);
extern void panic_showtrap(struct trap_info *);
extern void panic_stopcpus(cpu_t *, kthread_t *, int);
extern void panic_enter_hw(int);
extern void panic_quiesce_hw(panic_data_t *);
extern void panic_dump_hw(int);
extern int panic_trigger(int *);

#endif /* _KERNEL */
#endif /* !_ASM */

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_PANIC_H */
