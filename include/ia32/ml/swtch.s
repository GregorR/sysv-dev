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

#pragma ident	"%Z%%M%	%I%	%E% SMI"

/*
 * Process switching routines.
 */

#if defined(__lint)
#include <sys/thread.h>
#include <sys/systm.h>
#include <sys/time.h>
#else	/* __lint */
#include "assym.h"
#endif	/* __lint */

#include <sys/asm_linkage.h>
#include <sys/asm_misc.h>
#include <sys/regset.h>
#include <sys/privregs.h>
#include <sys/stack.h>
#include <sys/segments.h>

/*
 * resume(thread_id_t t);
 *
 * a thread can only run on one processor at a time. there
 * exists a window on MPs where the current thread on one
 * processor is capable of being dispatched by another processor.
 * some overlap between outgoing and incoming threads can happen
 * when they are the same thread. in this case where the threads
 * are the same, resume() on one processor will spin on the incoming
 * thread until resume() on the other processor has finished with
 * the outgoing thread.
 *
 * The MMU context changes when the resuming thread resides in a different
 * process.  Kernel threads are known by resume to reside in process 0.
 * The MMU context, therefore, only changes when resuming a thread in
 * a process different from curproc.
 *
 * resume_from_intr() is called when the thread being resumed was not 
 * passivated by resume (e.g. was interrupted).  This means that the
 * resume lock is already held and that a restore context is not needed.
 * Also, the MMU context is not changed on the resume in this case.
 *
 * resume_from_zombie() is the same as resume except the calling thread
 * is a zombie and must be put on the deathrow list after the CPU is
 * off the stack.
 */

#if !defined(__lint)

#if LWP_PCB_FPU != 0
#error LWP_PCB_FPU MUST be defined as 0 for code in swtch.s to work
#endif	/* LWP_PCB_FPU != 0 */

#endif	/* !__lint */

#if defined(__amd64)

/*
 * Save non-volatile regs other than %rsp (%rbx, %rbp, and %r12 - %r15)
 *
 * The stack frame must be created before the save of %rsp so that tracebacks
 * of swtch()ed-out processes show the process as having last called swtch().
 */
#define SAVE_REGS(thread_t, retaddr)			\
	movq	%rbp, T_RBP(thread_t);			\
	movq	%rbx, T_RBX(thread_t);			\
	movq	%r12, T_R12(thread_t);			\
	movq	%r13, T_R13(thread_t);			\
	movq	%r14, T_R14(thread_t);			\
	movq	%r15, T_R15(thread_t);			\
	pushq	%rbp;					\
	movq	%rsp, %rbp;				\
	movq	%rsp, T_SP(thread_t);			\
	movq	retaddr, T_PC(thread_t);		\
	movq	%rdi, %r12;				\
	call	__dtrace_probe___sched_off__cpu

/*
 * Restore non-volatile regs other than %rsp (%rbx, %rbp, and %r12 - %r15)
 *
 * We load up %rsp from the label_t as part of the context switch, so
 * we don't repeat that here.
 *
 * We don't do a 'leave,' because reloading %rsp/%rbp from the label_t
 * already has the effect of putting the stack back the way it was when
 * we came in.
 */
#define RESTORE_REGS(scratch_reg)			\
	movq	%gs:CPU_THREAD, scratch_reg;		\
	movq	T_RBP(scratch_reg), %rbp;		\
	movq	T_RBX(scratch_reg), %rbx;		\
	movq	T_R12(scratch_reg), %r12;		\
	movq	T_R13(scratch_reg), %r13;		\
	movq	T_R14(scratch_reg), %r14;		\
	movq	T_R15(scratch_reg), %r15

/*
 * Get pointer to a thread's hat structure
 */
#define GET_THREAD_HATP(hatp, thread_t, scratch_reg)	\
	movq	T_PROCP(thread_t), hatp;		\
	movq	P_AS(hatp), scratch_reg;		\
	movq	A_HAT(scratch_reg), hatp

#elif defined (__i386)

/*
 * Save non-volatile registers (%ebp, %esi, %edi and %ebx)
 *
 * The stack frame must be created before the save of %esp so that tracebacks
 * of swtch()ed-out processes show the process as having last called swtch().
 */
#define SAVE_REGS(thread_t, retaddr)			\
	movl	%ebp, T_EBP(thread_t);			\
	movl	%ebx, T_EBX(thread_t);			\
	movl	%esi, T_ESI(thread_t);			\
	movl	%edi, T_EDI(thread_t);			\
	pushl	%ebp;					\
	movl	%esp, %ebp;				\
	movl	%esp, T_SP(thread_t);			\
	movl	retaddr, T_PC(thread_t);		\
	movl	8(%ebp), %edi;				\
	pushl	%edi;					\
	call	__dtrace_probe___sched_off__cpu;	\
	addl	$CLONGSIZE, %esp

/*
 * Restore non-volatile registers (%ebp, %esi, %edi and %ebx)
 *
 * We don't do a 'leave,' because reloading %rsp/%rbp from the label_t
 * already has the effect of putting the stack back the way it was when
 * we came in.
 */
#define RESTORE_REGS(scratch_reg)			\
	movl	%gs:CPU_THREAD, scratch_reg;		\
	movl	T_EBP(scratch_reg), %ebp;		\
	movl	T_EBX(scratch_reg), %ebx;		\
	movl	T_ESI(scratch_reg), %esi;		\
	movl	T_EDI(scratch_reg), %edi

/*
 * Get pointer to a thread's hat structure
 */
#define GET_THREAD_HATP(hatp, thread_t, scratch_reg)	\
	movl	T_PROCP(thread_t), hatp;		\
	movl	P_AS(hatp), scratch_reg;		\
	movl	A_HAT(scratch_reg), hatp

#endif	/* __amd64 */

#if defined(__lint)

/* ARGSUSED */
void
resume(kthread_t *t)
{}

#else	/* __lint */

#if defined(__amd64)

	ENTRY(resume)
	movq	%gs:CPU_THREAD, %rax
	leaq	resume_return(%rip), %r11

	/*
	 * Save non-volatile registers, and set return address for current
	 * thread to resume_return.
	 *
	 * %r12 = t (new thread) when done
	 */
	SAVE_REGS(%rax, %r11)

	LOADCPU(%r15)				/* %r15 = CPU */
	movq	CPU_THREAD(%r15), %r13		/* %r13 = curthread */

	/*
	 * Call savectx if thread has installed context ops.
	 *
	 * Note that if we have floating point context, the save op
	 * (either fpsave_begin or fpxsave_begin) will issue the
	 * async save instruction (fnsave or fxsave respectively)
	 * that we fwait for below.
	 */
	cmpq	$0, T_CTX(%r13)		/* should current thread savectx? */
	je	.nosavectx		/* skip call when zero */

	movq	%r13, %rdi		/* arg = thread pointer */
	call	savectx			/* call ctx ops */

.nosavectx:
	/*
	 * Setup LDT register
	 */
	movq 	T_PROCP(%r12), %rax	/* load new thread proc */

	/* make sure GDT contains the right LDT desc */
	movq	%gs:CPU_GDT, %r11

	movq	P_LDT_DESC(%rax), %r10
	movq	_CONST(P_LDT_DESC+8)(%rax), %rax
	movq	%r10, ULDT_SEL(%r11)
	movq	%rax, _CONST(ULDT_SEL+8)(%r11)
	movl	$ULDT_SEL, %edx
	lldt	%dx

	/* 
	 * Temporarily switch to the idle thread's stack
	 */
	movq	CPU_IDLE_THREAD(%r15), %rax 	/* idle thread pointer */

	/* 
	 * Set the idle thread as the current thread
	 */
	movq	T_SP(%rax), %rsp	/* It is safe to set rsp */
	movq	%rax, CPU_THREAD(%r15)

	/*
	 * Switch in the hat context for the new thread
	 *
	 */
	GET_THREAD_HATP(%rdi, %r12, %r11)
	call	hat_switch

	movq	T_LWP(%r13), %r14
	testq	%r14, %r14
	jz	.disabled_fpu2

	cmpl	$FPU_EN, PCB_FPU_FLAGS(%r14)
	je	.wait_for_fpusave

.disabled_fpu2:
	/* 
	 * Clear and unlock previous thread's t_lock
	 * to allow it to be dispatched by another processor.
	 */
	movb	$0, T_LOCK(%r13)

	/*
	 * IMPORTANT: Registers at this point must be:
	 *       %r12 = new thread
	 *
	 * Here we are in the idle thread, have dropped the old thread.
	 */
	ALTENTRY(_resume_from_idle)
	/*
	 * spin until dispatched thread's mutex has
	 * been unlocked. this mutex is unlocked when
	 * it becomes safe for the thread to run.
	 */
.lock_thread_mutex:
	lock
	btsl	$0, T_LOCK(%r12) 	/* attempt to lock new thread's mutex */
	jnc	.thread_mutex_locked	/* got it */

.spin_thread_mutex:
	pause
	cmpb	$0, T_LOCK(%r12)	/* check mutex status */
	jz	.lock_thread_mutex	/* clear, retry lock */
	jmp	.spin_thread_mutex	/* still locked, spin... */

.thread_mutex_locked:
	/*
	 * Fix CPU structure to indicate new running thread.
	 * Set pointer in new thread to the CPU structure.
	 */
	LOADCPU(%r13)			/* load current CPU pointer */
	cmpq	%r13, T_CPU(%r12)
	je	.setup_cpu

	/* cp->cpu_stats.sys.cpumigrate++ */
	incq    CPU_STATS_SYS_CPUMIGRATE(%r13)
	movq	%r13, T_CPU(%r12)	/* set new thread's CPU pointer */

.setup_cpu:
	/*
	 * Setup rsp0 (kernel stack) in TSS to curthread's stack.
	 * (Note: Since we don't have saved 'regs' structure for all
	 *	  the threads we can't easily determine if we need to
	 *	  change rsp0. So, we simply change the rsp0 to bottom 
	 *	  of the thread stack and it will work for all cases.)
	 *
	 * XX64 - Is this correct?
	 */
	movq	CPU_TSS(%r13), %r14
	movq	T_STACK(%r12), %rax
	addq	$REGSIZE+MINFRAME, %rax	/* to the bottom of thread stack */
	movq	%rax, TSS_RSP0(%r14)

	movq	%r12, CPU_THREAD(%r13)	/* set CPU's thread pointer */
	xorl	%ebp, %ebp		/* make $<threadlist behave better */
	movq	T_LWP(%r12), %rax 	/* set associated lwp to  */
	movq	%rax, CPU_LWP(%r13) 	/* CPU's lwp ptr */

	movq	T_SP(%r12), %rsp	/* switch to outgoing thread's stack */
	movq	T_PC(%r12), %r13	/* saved return addr */

	/*
	 * Call restorectx if context ops have been installed.
	 */
	cmpq	$0, T_CTX(%r12)		/* should resumed thread restorectx? */
	jz	.norestorectx		/* skip call when zero */
	movq	%r12, %rdi		/* arg = thread pointer */
	call	restorectx		/* call ctx ops */

.norestorectx:

	/*
	 * If we are resuming an interrupt thread, store a timestamp 
	 * in the thread structure.
	 */
	testw	$T_INTR_THREAD, T_FLAGS(%r12)
	jz	1f

0:
	/*
	 * If an interrupt occurs between the rdtsc instruction and its
	 * subsequent store, the timestamp will be stale by the time it is
	 * stored. We can detect this by doing a compare-and-swap on the
	 * thread's timestamp, since any interrupt occurring in this window
	 * will put a new timestamp in the thread's t_intr_start field.
	 */
	movq	T_INTR_START(%r12), %rcx
	rdtsc

	/*
	 * After rdtsc:
	 *     High 32 bits of TC are in %edx
	 *     Low 32 bits of TC are in %eax
	 */
	shlq	$32, %rdx
	movl	%eax, %r14d
	orq	%rdx, %r14
	movq	%rcx, %rax
	cmpxchgq %r14, T_INTR_START(%r12)
	jnz	0b
1:
	/*
	 * Restore non-volatile registers, then have spl0 return to the
	 * resuming thread's PC after first setting the priority as low as
	 * possible and blocking all interrupt threads that may be active.
	 */
	movq	%r13, %rax	/* save return address */	
	RESTORE_REGS(%r11)
	pushq	%rax		/* push return address for spl0() */
	call	__dtrace_probe___sched_on__cpu
	jmp	spl0

resume_return:
	/*
	 * Remove stack frame created in SAVE_REGS()
	 */
	addq	$CLONGSIZE, %rsp
	ret

.wait_for_fpusave:
	/* mark copy in pcb as valid */
	movq	%cr0, %rax
	movl	$_CONST(FPU_VALID|FPU_EN), PCB_FPU_FLAGS(%r14)
	orl	$CR0_TS, %eax	/* set to trap on next switch */
	fwait			/* ensure save is done before we unlock */
	finit			/* (ensure x87 tags cleared for fxsave case) */
	movq	%rax, %cr0
	jmp	.disabled_fpu2
	SET_SIZE(_resume_from_idle)
	SET_SIZE(resume)

#elif defined (__i386)

	ENTRY(resume)
	movl	%gs:CPU_THREAD, %eax
	movl	$resume_return, %ecx

	/*
	 * Save non-volatile registers, and set return address for current
	 * thread to resume_return.
	 *
	 * %edi = t (new thread) when done.
	 */
	SAVE_REGS(%eax,  %ecx)

	LOADCPU(%ebx)			/* %ebx = CPU */
	movl	CPU_THREAD(%ebx), %esi	/* %esi = curthread */

	/*
	 * Call savectx if thread has installed context ops.
	 *
	 * Note that if we have floating point context, the save op
	 * (either fpsave_begin or fpxsave_begin) will issue the
	 * async save instruction (fnsave or fxsave respectively)
	 * that we fwait for below.
	 */
	movl	T_CTX(%esi), %eax	/* should current thread savectx? */
	testl	%eax, %eax
	jz	.nosavectx		/* skip call when zero */
	pushl	%esi			/* arg = thread pointer */
	call	savectx			/* call ctx ops */
	addl	$4, %esp		/* restore stack pointer */

.nosavectx:
	movl	T_LWP(%esi), %ecx
	pushl	%ecx			/* save fp address for later check */

	/*
	 * Setup LDT register
	 */
	movl 	T_PROCP(%edi), %eax	/* load new proc */

	/* make sure GDT contains the right LDT desc */
	movl	%gs:CPU_GDT, %ecx

	movl	P_LDT_DESC(%eax), %edx
	movl	_CONST(P_LDT_DESC+4)(%eax), %eax
	movl	%edx, ULDT_SEL(%ecx)
	movl	%eax, _CONST(ULDT_SEL+4)(%ecx)
	movl	$ULDT_SEL, %edx
	lldt	%dx

	/* 
	 * Temporarily switch to the idle thread's stack
	 */
	movl	CPU_IDLE_THREAD(%ebx), %eax 	/* idle thread pointer */
	popl	%ecx			/* restore pointer to fp structure. */

	/* 
	 * Set the idle thread as the current thread
	 */
	movl	T_SP(%eax), %esp	/* It is safe to set esp */
	movl	%eax, CPU_THREAD(%ebx)
	movl	%ecx, %ebx		/* save pcb_fpu pointer in %ebx */

	/* switch in the hat context for the new thread */
	GET_THREAD_HATP(%ecx, %edi, %ecx)
	pushl	%ecx
	call	hat_switch
	addl	$4, %esp
	
	xorl	%ecx, %ecx
	testl	%ebx, %ebx			/* check pcb_fpu pointer */
	jz	.disabled_fpu2
	cmpl	$FPU_EN, PCB_FPU_FLAGS(%ebx)	/* is PCB_FPU_FLAGS FPU_EN? */
	je	.wait_for_fpusave
.disabled_fpu2:
	/* 
	 * Clear and unlock previous thread's t_lock
	 * to allow it to be dispatched by another processor.
	 */
	movb	%cl, T_LOCK(%esi)

	/*
	 * IMPORTANT: Registers at this point must be:
	 *       %edi = new thread
	 *
	 * Here we are in the idle thread, have dropped the old thread.
	 */
	ALTENTRY(_resume_from_idle)
	/*
	 * spin until dispatched thread's mutex has
	 * been unlocked. this mutex is unlocked when
	 * it becomes safe for the thread to run.
	 */
.L4:
	lock
	btsl	$0, T_LOCK(%edi) /* lock new thread's mutex */
	jc	.L4_2			/* lock did not succeed */

	/*
	 * Fix CPU structure to indicate new running thread.
	 * Set pointer in new thread to the CPU structure.
	 */
	LOADCPU(%esi)			/* load current CPU pointer */
	movl	T_STACK(%edi), %eax	/* here to use v pipeline of */
					/* Pentium. Used few lines below */
	cmpl	%esi, T_CPU(%edi)
	jne	.L5_2
.L5_1:
	/*
	 * Setup esp0 (kernel stack) in TSS to curthread's stack.
	 * (Note: Since we don't have saved 'regs' structure for all
	 *	  the threads we can't easily determine if we need to
	 *	  change esp0. So, we simply change the esp0 to bottom 
	 *	  of the thread stack and it will work for all cases.)
	 */
	movl	CPU_TSS(%esi), %ecx
	addl	$REGSIZE+MINFRAME, %eax	/* to the bottom of thread stack */
	movl	%eax, TSS_ESP0(%ecx)

	movl	%edi, CPU_THREAD(%esi)	/* set CPU's thread pointer */
	xorl	%ebp, %ebp		/* make $<threadlist behave better */
	movl	T_LWP(%edi), %eax 	/* set associated lwp to  */
	movl	%eax, CPU_LWP(%esi) 	/* CPU's lwp ptr */

	movl	T_SP(%edi), %esp	/* switch to outgoing thread's stack */
	movl	T_PC(%edi), %esi	/* saved return addr */

	/*
	 * Call restorectx if context ops have been installed.
	 */
	movl	T_CTX(%edi), %eax	/* should resumed thread restorectx? */
	testl	%eax, %eax
	jz	.norestorectx		/* skip call when zero */
	pushl	%edi			/* arg = thread pointer */
	call	restorectx		/* call ctx ops */
	addl	$4, %esp		/* restore stack pointer */
.norestorectx:

	/*
	 * If we are resuming an interrupt thread, store a timestamp 
	 * in the thread structure.
	 */
	testw	$T_INTR_THREAD, T_FLAGS(%edi)
	jz	1f
	pushl	%ecx
0:
	/*
	 * If an interrupt occurs between the rdtsc instruction and its
	 * subsequent store, the timestamp will be stale by the time it is
	 * stored. We can detect this by doing a compare-and-swap on the
	 * thread's timestamp, since any interrupt occurring in this window
	 * will put a new timestamp in the thread's t_intr_start field.
	 */
	pushl	T_INTR_START(%edi)
	pushl	T_INTR_START+4(%edi)
	.globl	_tsc_patch15
_tsc_patch15:
	nop; nop			/* patched to rdtsc if available */
	movl	%eax, %ebx
	movl	%edx, %ecx
	popl	%edx
	popl	%eax
	cmpxchg8b T_INTR_START(%edi)
	jnz	0b
	popl	%ecx
1:
	/*
	 * Restore non-volatile registers, then have spl0 return to the
	 * resuming thread's PC after first setting the priority as low as
	 * possible and blocking all interrupt threads that may be active.
	 */
	movl	%esi, %eax		/* save return address */
	RESTORE_REGS(%ecx)
	pushl	%eax			/* push return address for spl0() */
	call	__dtrace_probe___sched_on__cpu
	jmp	spl0

resume_return:
	/*
	 * Remove stack frame created in SAVE_REGS()
	 */
	addl	$CLONGSIZE, %esp
	ret

.wait_for_fpusave:
	mov	%cr0, %eax

	/* mark copy in pcb as valid */
	movl	$_CONST(FPU_VALID|FPU_EN), PCB_FPU_FLAGS(%ebx)

	orl	$CR0_TS, %eax			/* set to trap on next switch */
	fwait			/* ensure save is done before we unlock */
	finit			/* (ensure x87 tags cleared for fxsave case) */
	movl	%eax, %cr0
	jmp	.disabled_fpu2

.L4_2:
	pause
	cmpb	$0, T_LOCK(%edi)
	je	.L4
	jmp	.L4_2

.L5_2:
	/* cp->cpu_stats.sys.cpumigrate++ */
	addl    $1, CPU_STATS_SYS_CPUMIGRATE(%esi)
	adcl    $0, CPU_STATS_SYS_CPUMIGRATE+4(%esi)
	movl	%esi, T_CPU(%edi)	/* set new thread's CPU pointer */
	jmp	.L5_1

	SET_SIZE(_resume_from_idle)
	SET_SIZE(resume)

#endif	/* __amd64 */
#endif	/* __lint */

#if defined(__lint)

/* ARGSUSED */
void
resume_from_zombie(kthread_t *t)
{}

#else	/* __lint */

#if defined(__amd64)

	ENTRY(resume_from_zombie)
	movq	%gs:CPU_THREAD, %rax
	leaq	resume_from_zombie_return(%rip), %r11

	/*
	 * Save non-volatile registers, and set return address for current
	 * thread to resume_from_zombie_return.
	 *
	 * %r12 = t (new thread) when done
	 */
	SAVE_REGS(%rax, %r11)

	movq	%gs:CPU_THREAD, %r13	/* %r13 = curthread */

	/*
	 * Setup LDT register
	 */
	movq 	T_PROCP(%r12), %rax	/* load new thread proc */

	/* make sure GDT contains the right LDT desc */
	movq	%gs:CPU_GDT, %r11

	movq	P_LDT_DESC(%rax), %r10
	movq	_CONST(P_LDT_DESC+8)(%rax), %rax
	movq	%r10, ULDT_SEL(%r11)
	movq	%rax, _CONST(ULDT_SEL+8)(%r11)
	movl	$ULDT_SEL, %edx
	lldt	%dx

	/* clean up the fp unit. It might be left enabled */
	movq	%cr0, %rax
	testq	$CR0_TS, %rax
	jnz	.zfpu_disabled		/* if TS already set, nothing to do */
	fninit				/* init fpu & discard pending error */

	/*
	 * Store a zero word into the mxcsr register to disable any sse
	 * floating point exceptions
	 */
	pushq	$0
	movq	%rsp, %rdi
	ldmxcsr	(%rdi)
	addq	$CLONGSIZE, %rsp
	orq	$CR0_TS, %rax
	movq	%rax, %cr0
.zfpu_disabled:

	/* 
	 * Temporarily switch to the idle thread's stack so that the zombie
	 * thread's stack can be reclaimed by the reaper.
	 */
	movq	%gs:CPU_IDLE_THREAD, %rax /* idle thread pointer */
	movq	T_SP(%rax), %rsp	/* get onto idle thread stack */

	/*
	 * Sigh. If the idle thread has never run thread_start()
	 * then t_sp is mis-aligned by thread_load().
	 */
	andq	$_BITNOT(STACK_ALIGN-1), %rsp

	/* 
	 * Set the idle thread as the current thread.
	 */
	movq	%rax, %gs:CPU_THREAD

	/* switch in the hat context for the new thread */
	GET_THREAD_HATP(%rdi, %r12, %r11)
	call	hat_switch

	/* 
	 * Put the zombie on death-row.
	 */
	movq	%r13, %rdi
	call	reapq_add

	jmp	_resume_from_idle	/* finish job of resume */

resume_from_zombie_return:
	RESTORE_REGS(%r11)		/* restore non-volatile registers */
	call	__dtrace_probe___sched_on__cpu

	/*
	 * Remove stack frame created in SAVE_REGS()
	 */
	addq	$CLONGSIZE, %rsp
	ret
	SET_SIZE(resume_from_zombie)

#elif defined (__i386)

	ENTRY(resume_from_zombie)
	movl	%gs:CPU_THREAD, %eax
	movl	$resume_from_zombie_return, %ecx

	/*
	 * Save non-volatile registers, and set return address for current
	 * thread to resume_from_zombie_return.
	 *
	 * %edi = t (new thread) when done.
	 */
	SAVE_REGS(%eax, %ecx)

	movl	%gs:CPU_THREAD, %esi	/* %esi = curthread */

	/*
	 * Setup LDT register
	 */
	movl 	T_PROCP(%edi), %ecx	/* load new proc  */

	/* make sure GDT contains the right LDT desc */
	movl	%gs:CPU_GDT, %eax

	movl	P_LDT_DESC(%ecx), %edx
	movl	_CONST(P_LDT_DESC+4)(%ecx), %ecx
	movl	%edx, ULDT_SEL(%eax)
	movl	%ecx, _CONST(ULDT_SEL+4)(%eax)
	movl	$ULDT_SEL, %edx
	lldt	%dx

	/* clean up the fp unit. It might be left enabled */
	movl	%cr0, %eax
	testl	$CR0_TS, %eax
	jnz	.zfpu_disabled		/* if TS already set, nothing to do */
	fninit				/* init fpu & discard pending error */

	/*
	 * If this machine supports fxsave/fxrstor, the next string of
	 * nops may be patched to store a zero word off the stack into
	 * the mxcsr register to disable any sse floating point exceptions
	 */
	pushl	$0
	mov	%esp, %ebx
	.globl	_patch_ldmxcsr_ebx
_patch_ldmxcsr_ebx:
	nop; nop; nop			/* ldmxcsr (%ebx) */
	addl	$4, %esp
	orl	$CR0_TS, %eax
	movl	%eax, %cr0
.zfpu_disabled:
	/* 
	 * Temporarily switch to the idle thread's stack so that the zombie
	 * thread's stack can be reclaimed by the reaper.
	 */
	movl	%gs:CPU_IDLE_THREAD, %eax /* idle thread pointer */
	movl	T_SP(%eax), %esp	/* get onto idle thread stack */

	/* 
	 * Set the idle thread as the current thread.
	 */
	movl	%eax, %gs:CPU_THREAD

	/* switch in the hat context for the new thread */
	GET_THREAD_HATP(%ecx, %edi, %ecx)
	pushl	%ecx
	call	hat_switch
	addl	$4, %esp
	/* 
	 * Put the zombie on death-row.
	 */
	pushl	%esi
	call	reapq_add
	addl	$4, %esp
	jmp	_resume_from_idle	/* finish job of resume */

resume_from_zombie_return:
	RESTORE_REGS(%ecx)		/* restore non-volatile registers */
	call	__dtrace_probe___sched_on__cpu

	/*
	 * Remove stack frame created in SAVE_REGS()
	 */
	addl	$CLONGSIZE, %esp
	ret
	SET_SIZE(resume_from_zombie)

#endif	/* __amd64 */
#endif	/* __lint */

#if defined(__lint)

/* ARGSUSED */
void
resume_from_intr(kthread_t *t)
{}

#else	/* __lint */

#if defined(__amd64)

	ENTRY(resume_from_intr)
	movq	%gs:CPU_THREAD, %rax
	leaq	resume_from_intr_return(%rip), %r11

	/*
	 * Save non-volatile registers, and set return address for current
	 * thread to resume_from_intr_return.
	 *
	 * %r12 = t (new thread) when done
	 */
	SAVE_REGS(%rax, %r11)

	movq	%gs:CPU_THREAD, %r13	/* %r13 = curthread */
	movq	%r12, %gs:CPU_THREAD	/* set CPU's thread pointer */
	movq	T_SP(%r12), %rsp	/* restore resuming thread's sp */
	xorl	%ebp, %ebp		/* make $<threadlist behave better */

	/* 
	 * Unlock outgoing thread's mutex dispatched by another processor.
	 */
	xorl	%eax, %eax
	xchgb	%al, T_LOCK(%r13)

	/*
	 * If we are resuming an interrupt thread, store a timestamp in
	 * the thread structure.
	 */
	testw	$T_INTR_THREAD, T_FLAGS(%r12)
	jz	1f
0:
	/*
	 * If an interrupt occurs between the rdtsc instruction and its
	 * subsequent store, the timestamp will be stale by the time it is
	 * stored. We can detect this by doing a compare-and-swap on the
	 * thread's timestamp, since any interrupt occurring in this window
	 * will put a new timestamp in the thread's t_intr_start field.
	 */
	movq	T_INTR_START(%r12), %rcx
	rdtsc

	/*
	 * After rdtsc:
	 *     High 32 bits of TC are in %edx
	 *     Low 32 bits of TC are in %eax
	 */
	shlq	$32, %rdx
	movl	%eax, %r14d
	orq	%rdx, %r14
	movq	%rcx, %rax
	cmpxchgq %r14, T_INTR_START(%r12)
	jnz	0b
1:
	/*
	 * Restore non-volatile registers, then have spl0 return to the
	 * resuming thread's PC after first setting the priority as low as
	 * possible and blocking all interrupt threads that may be active.
	 */
	movq	T_PC(%r12), %rax	/* saved return addr */
	RESTORE_REGS(%r11);
	pushq	%rax			/* push return address for spl0() */
	call	__dtrace_probe___sched_on__cpu
	jmp	spl0

resume_from_intr_return:
	/*
	 * Remove stack frame created in SAVE_REGS()
	 */
	addq 	$CLONGSIZE, %rsp
	ret
	SET_SIZE(resume_from_intr)

#elif defined (__i386)

	ENTRY(resume_from_intr)
	movl	%gs:CPU_THREAD, %eax
	movl	$resume_from_intr_return, %ecx

	/*
	 * Save non-volatile registers, and set return address for current
	 * thread to resume_return.
	 *
	 * %edi = t (new thread) when done.
	 */
	SAVE_REGS(%eax, %ecx)

	movl	%gs:CPU_THREAD, %esi	/* %esi = curthread */
	movl	%edi, %gs:CPU_THREAD	/* set CPU's thread pointer */
	movl	T_SP(%edi), %esp	/* restore resuming thread's sp */
	xorl	%ebp, %ebp		/* make $<threadlist behave better */

	/* 
	 * Unlock outgoing thread's mutex dispatched by another processor.
	 */
	xorl	%eax,%eax
	xchgb	%al, T_LOCK(%esi)

	/*
	 * If we are resuming an interrupt thread, store a timestamp in
	 * the thread structure.
	 */
	testw	$T_INTR_THREAD, T_FLAGS(%edi)
	jz	1f
0:
	/*
	 * If an interrupt occurs between the rdtsc instruction and its
	 * subsequent store, the timestamp will be stale by the time it is
	 * stored. We can detect this by doing a compare-and-swap on the
	 * thread's timestamp, since any interrupt occurring in this window
	 * will put a new timestamp in the thread's t_intr_start field.
	 */
	pushl	T_INTR_START(%edi)
	pushl	T_INTR_START+4(%edi)
	.globl	_tsc_patch16
_tsc_patch16:
	nop; nop			/* patched to rdtsc if available */
	movl	%eax, %ebx
	movl	%edx, %ecx
	popl	%edx
	popl	%eax
	cmpxchg8b T_INTR_START(%edi)
	jnz	0b
1:
	/*
	 * Restore non-volatile registers, then have spl0 return to the
	 * resuming thread's PC after first setting the priority as low as
	 * possible and blocking all interrupt threads that may be active.
	 */
	movl	T_PC(%edi), %eax	/* saved return addr */
	RESTORE_REGS(%ecx)
	pushl	%eax			/* push return address for spl0() */
	call	__dtrace_probe___sched_on__cpu
	jmp	spl0

resume_from_intr_return:
	/*
	 * Remove stack frame created in SAVE_REGS()
	 */
	addl	$CLONGSIZE, %esp
	ret
	SET_SIZE(resume_from_intr)

#endif	/* __amd64 */
#endif /* __lint */

#if defined(__lint)

void
thread_start(void)
{}

#else   /* __lint */

#if defined(__amd64)

	ENTRY(thread_start)
	popq	%rax		/* start() */
	popq	%rdi		/* arg */
	popq	%rsi		/* len */
	movq	%rsp, %rbp
	call	*%rax
	call	thread_exit	/* destroy thread if it returns. */
	/*NOTREACHED*/
	SET_SIZE(thread_start)

#elif defined(__i386)

	ENTRY(thread_start)
	popl	%eax
	movl	%esp, %ebp
	addl	$8, %ebp
	call	*%eax
	addl	$8, %esp
	call	thread_exit	/* destroy thread if it returns. */
	/*NOTREACHED*/
	SET_SIZE(thread_start)

#endif	/* __i386 */

#endif  /* __lint */
