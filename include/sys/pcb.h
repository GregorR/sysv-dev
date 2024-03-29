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

#ifndef _SYS_PCB_H
#define	_SYS_PCB_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include <sys/regset.h>
#include <sys/segments.h>

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef _ASM
typedef struct fpu_ctx {
	kfpu_t		fpu_regs;	/* kernel save area for FPU */
	uint_t		fpu_flags;	/* FPU state flags */
} fpu_ctx_t;

typedef struct pcb {
	fpu_ctx_t	pcb_fpu;	/* fpu state */
	uint_t		pcb_flags;	/* state flags; cleared on fork */
	greg_t		pcb_drstat;	/* status debug register (%dr6) */
	unsigned char	pcb_instr;	/* /proc: instruction at stop */
#if defined(__amd64)
	uintptr_t	pcb_fsbase;
	uintptr_t	pcb_gsbase;
	selector_t	pcb_ds;
	selector_t	pcb_es;
	selector_t	pcb_fs;
	selector_t	pcb_gs;
#endif /* __amd64 */
	user_desc_t	pcb_fsdesc;	/* private per-lwp %fs descriptors */
	user_desc_t	pcb_gsdesc;	/* private per-lwp %gs descriptors */
} pcb_t;

#endif /* ! _ASM */

/* pcb_flags */
#define	DEBUG_PENDING	0x02	/* single-step of lcall for a sys call */
#define	INSTR_VALID	0x08	/* value in pcb_instr is valid (/proc) */
#define	NORMAL_STEP	0x10	/* normal debugger-requested single-step */
#define	WATCH_STEP	0x20	/* single-stepping in watchpoint emulation */
#define	CPC_OVERFLOW	0x40	/* performance counters overflowed */
#define	RUPDATE_PENDING	0x80	/* new register values in the pcb -> regs */

/* fpu_flags */
#define	FPU_EN		0x1	/* flag signifying fpu in use */
#define	FPU_VALID	0x2	/* fpu_regs has valid fpu state */
#define	FPU_MODIFIED	0x4	/* fpu_regs is modified (/proc) */

#define	FPU_INVALID	0x0	/* fpu context is not in use */

/* fpu_flags */

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_PCB_H */
