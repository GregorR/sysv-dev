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

#ifndef _SYS_CPUVAR_H
#define	_SYS_CPUVAR_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include <sys/thread.h>
#include <sys/sysinfo.h>	/* has cpu_stat_t definition */
#include <sys/disp.h>
#include <sys/processor.h>

#if (defined(_KERNEL) || defined(_KMEMUSER)) && defined(_MACHDEP)
#include <sys/machcpuvar.h>
#endif

#include <sys/types.h>
#include <sys/file.h>
#include <sys/bitmap.h>
#include <sys/rwlock.h>
#include <sys/msacct.h>
#if defined(__GNUC__) && defined(_ASM_INLINES) && defined(_KERNEL)
#include <asm/cpuvar.h>
#endif

#ifdef	__cplusplus
extern "C" {
#endif

struct squeue_set_s;

#define	CPU_CACHE_COHERENCE_SIZE	64
#define	S_LOADAVG_SZ	11
#define	S_MOVAVG_SZ	10

struct loadavg_s {
	int lg_cur;		/* current loadavg entry */
	unsigned int lg_len;	/* number entries recorded */
	hrtime_t lg_total;	/* used to temporarily hold load totals */
	hrtime_t lg_loads[S_LOADAVG_SZ];	/* table of recorded entries */
};

/*
 * For fast event tracing.
 */
struct ftrace_record;
typedef struct ftrace_data {
	int			ftd_state;	/* ftrace flags */
	kmutex_t		ftd_mutex;	/* ftrace buffer lock */
	struct ftrace_record	*ftd_cur;	/* current record */
	struct ftrace_record	*ftd_first;	/* first record */
	struct ftrace_record	*ftd_last;	/* last record */
} ftrace_data_t;

struct cyc_cpu;
struct nvlist;

/*
 * Per-CPU data.
 */
typedef struct cpu {
	processorid_t	cpu_id;			/* CPU number */
	processorid_t	cpu_seqid;	/* sequential CPU id (0..ncpus-1) */
	volatile cpu_flag_t cpu_flags;		/* flags indicating CPU state */
	struct cpu	*cpu_self;		/* pointer to itself */
	kthread_t	*cpu_thread;		/* current thread */
	kthread_t	*cpu_idle_thread;	/* idle thread for this CPU */
	kthread_t	*cpu_pause_thread;	/* pause thread for this CPU */
	klwp_id_t	cpu_lwp;		/* current lwp (if any) */
	klwp_id_t	cpu_fpowner;		/* currently loaded fpu owner */
	struct cpupart	*cpu_part;		/* partition with this CPU */
	struct lgrp_ld	*cpu_lpl;		/* pointer to this cpu's load */
	struct chip	*cpu_chip;		/* cpu's chip data */
	int		cpu_rechoose;		/* cpu's rechoose_interval */
	int		cpu_cache_offset;	/* see kmem.c for details */

	/*
	 * Links to other CPUs.  It is safe to walk these lists if
	 * one of the following is true:
	 * 	- cpu_lock held
	 * 	- preemption disabled via kpreempt_disable
	 * 	- PIL >= DISP_LEVEL
	 * 	- acting thread is an interrupt thread
	 * 	- all other CPUs are paused
	 */
	struct cpu	*cpu_next;		/* next existing CPU */
	struct cpu	*cpu_prev;		/* prev existing CPU */
	struct cpu	*cpu_next_onln;		/* next online (enabled) CPU */
	struct cpu	*cpu_prev_onln;		/* prev online (enabled) CPU */
	struct cpu	*cpu_next_part;		/* next CPU in partition */
	struct cpu	*cpu_prev_part;		/* prev CPU in partition */
	struct cpu	*cpu_next_lgrp;		/* next CPU in latency group */
	struct cpu	*cpu_prev_lgrp;		/* prev CPU in latency group */
	struct cpu	*cpu_next_chip;		/* next CPU on chip */
	struct cpu	*cpu_prev_chip;		/* prev CPU on chip */
	struct cpu	*cpu_next_lpl;		/* next CPU in lgrp partition */
	struct cpu	*cpu_prev_lpl;
	void		*cpu_reserved[4];	/* reserved for future use */

	/*
	 * Scheduling variables.
	 */
	disp_t		*cpu_disp;		/* dispatch queue data */
	/*
	 * Note that cpu_disp is set before the CPU is added to the system
	 * and is never modified.  Hence, no additional locking is needed
	 * beyond what's necessary to access the cpu_t structure.
	 */
	char		cpu_runrun;	/* scheduling flag - set to preempt */
	char		cpu_kprunrun;		/* force kernel preemption */
	pri_t		cpu_chosen_level; 	/* priority at which cpu */
						/* was chosen for scheduling */
	kthread_t	*cpu_dispthread; /* thread selected for dispatch */
	disp_lock_t	cpu_thread_lock; /* dispatcher lock on current thread */
	uint8_t		cpu_disp_flags;	/* flags used by dispatcher */
	/*
	 * The following field is updated when ever the cpu_dispthread
	 * changes. Also in places, where the current thread(cpu_dispthread)
	 * priority changes. This is used in disp_lowpri_cpu()
	 */
	pri_t		cpu_dispatch_pri; /* priority of cpu_dispthread */
	clock_t		cpu_last_swtch;	/* last time switched to new thread */

	/*
	 * Interrupt data.
	 */
	caddr_t		cpu_intr_stack;	/* interrupt stack */
	kthread_t	*cpu_intr_thread; /* interrupt thread list */
	uint_t		cpu_intr_actv;	/* interrupt levels active (bitmask) */
	int		cpu_base_spl;	/* priority for highest rupt active */

	/*
	 * Statistics.
	 */
	cpu_stats_t	cpu_stats;		/* per-CPU statistics */
	struct kstat	*cpu_info_kstat;	/* kstat for cpu info */

	uintptr_t	cpu_profile_pc;	/* kernel PC in profile interrupt */
	uintptr_t	cpu_profile_upc; /* user PC in profile interrupt */
	uintptr_t	cpu_profile_pil; /* PIL when profile interrupted */

	ftrace_data_t	cpu_ftrace;		/* per cpu ftrace data */

	clock_t		cpu_deadman_lbolt;	/* used by deadman() */
	uint_t		cpu_deadman_countdown;	/* used by deadman() */

	kmutex_t	cpu_cpc_ctxlock; /* protects context for idle thread */
	kcpc_ctx_t	*cpu_cpc_ctx;	/* performance counter context */

	/*
	 * Configuration information for the processor_info system call.
	 */
	processor_info_t cpu_type_info;	/* config info */
	time_t		cpu_state_begin; /* when CPU entered current state */
	char		cpu_cpr_flags;	/* CPR related info */
	struct cyc_cpu	*cpu_cyclic;	/* per cpu cyclic subsystem data */
	struct squeue_set_s *cpu_squeue_set;	/* per cpu squeue set */
	struct nvlist	*cpu_props;	/* pool-related properties */

	krwlock_t	cpu_ft_lock;		/* DTrace: fasttrap lock */
	uintptr_t	cpu_dtrace_caller;	/* DTrace: caller, if any */
	hrtime_t	cpu_dtrace_chillmark;	/* DTrace: chill mark time */
	hrtime_t	cpu_dtrace_chilled;	/* DTrace: total chill time */

	uint16_t	cpu_mstate;		/* cpu microstate */
	hrtime_t	cpu_mstate_start;	/* cpu microstate start time */
	hrtime_t	cpu_acct[NCMSTATES];	/* cpu microstate data */
	hrtime_t	cpu_waitrq;		/* cpu run-queue wait time */
	struct loadavg_s cpu_loadavg;		/* loadavg info for this cpu */

	char		*cpu_idstr;	/* for printing and debugging */
	char		*cpu_brandstr;	/* for printing */

	/*
	 * Sum of all device interrupt weights that are currently directed at
	 * this cpu. Cleared at start of interrupt redistribution.
	 */
	int32_t		cpu_intr_weight;

#if (defined(_KERNEL) || defined(_KMEMUSER)) && defined(_MACHDEP)
	/*
	 * XXX - needs to be fixed. Structure size should not change.
	 *	 probably needs to be a pointer to an opaque structure.
	 * XXX - this is OK as long as cpu structs aren't in an array.
	 *	 A user program will either read the first part,
	 *	 which is machine-independent, or read the whole thing.
	 */
	struct machcpu	cpu_m;		/* per architecture info */
#endif
} cpu_t;

/*
 * The cpu_core structure consists of per-CPU state available in any context.
 * On some architectures, this may mean that the page(s) containing the
 * NCPU-sized array of cpu_core structures must be locked in the TLB -- it
 * is up to the platform to assure that this is performed properly.  Note that
 * the structure is sized to avoid false sharing.
 */
#define	CPUC_SIZE		(sizeof (uint16_t) + sizeof (uintptr_t) + \
				sizeof (kmutex_t))
#define	CPUC_PADSIZE		CPU_CACHE_COHERENCE_SIZE - CPUC_SIZE

typedef struct cpu_core {
	uint16_t	cpuc_dtrace_flags;	/* DTrace flags */
	uint8_t		cpuc_pad[CPUC_PADSIZE];	/* padding */
	uintptr_t	cpuc_dtrace_illval;	/* DTrace illegal value */
	kmutex_t	cpuc_pid_lock;		/* DTrace pid provider lock */
} cpu_core_t;

#ifdef _KERNEL
extern cpu_core_t cpu_core[];
#endif /* _KERNEL */

/*
 * CPU_ON_INTR() macro. Returns non-zero if currently on interrupt stack.
 * Note that this isn't a test for a high PIL.  For example, cpu_intr_actv
 * does not get updated when we go through sys_trap from TL>0 at high PIL.
 * getpil() should be used instead to check for PIL levels.
 */
#define	CPU_ON_INTR(cpup) ((cpup)->cpu_intr_actv >> (LOCK_LEVEL + 1))

#if defined(_KERNEL) || defined(_KMEMUSER)

#define	INTR_STACK_SIZE	MAX(DEFAULTSTKSZ, PAGESIZE)

/* MEMBERS PROTECTED BY "atomicity": cpu_flags */

/*
 * Flags in the CPU structure.
 *
 * These are protected by cpu_lock (except during creation).
 *
 * Offlined-CPUs have three stages of being offline:
 *
 * CPU_ENABLE indicates that the CPU is participating in I/O interrupts
 * that can be directed at a number of different CPUs.  If CPU_ENABLE
 * is off, the CPU will not be given interrupts that can be sent elsewhere,
 * but will still get interrupts from devices associated with that CPU only,
 * and from other CPUs.
 *
 * CPU_OFFLINE indicates that the dispatcher should not allow any threads
 * other than interrupt threads to run on that CPU.  A CPU will not have
 * CPU_OFFLINE set if there are any bound threads (besides interrupts).
 *
 * CPU_QUIESCED is set if p_offline was able to completely turn idle the
 * CPU and it will not have to run interrupt threads.  In this case it'll
 * stay in the idle loop until CPU_QUIESCED is turned off.
 *
 * CPU_FROZEN is used only by CPR to mark CPUs that have been successfully
 * suspended (in the suspend path), or have yet to be resumed (in the resume
 * case).
 *
 * On some platforms CPUs can be individually powered off.
 * The following flags are set for powered off CPUs: CPU_QUIESCED,
 * CPU_OFFLINE, and CPU_POWEROFF.  The following flags are cleared:
 * CPU_RUNNING, CPU_READY, CPU_EXISTS, and CPU_ENABLE.
 */
#define	CPU_RUNNING	0x001		/* CPU running */
#define	CPU_READY	0x002		/* CPU ready for cross-calls */
#define	CPU_QUIESCED	0x004		/* CPU will stay in idle */
#define	CPU_EXISTS	0x008		/* CPU is configured */
#define	CPU_ENABLE	0x010		/* CPU enabled for interrupts */
#define	CPU_OFFLINE	0x020		/* CPU offline via p_online */
#define	CPU_POWEROFF	0x040		/* CPU is powered off */
#define	CPU_FROZEN	0x080		/* CPU is frozen via CPR suspend */
#define	CPU_SPARE	0x100		/* CPU offline available for use */
#define	CPU_FAULTED	0x200		/* CPU offline diagnosed faulty */

#define	CPU_ACTIVE(cpu)	(((cpu)->cpu_flags & CPU_OFFLINE) == 0)

/*
 * Flags for cpu_offline(), cpu_faulted(), and cpu_spare().
 */
#define	CPU_FORCED	0x0001		/* Force CPU offline */

/*
 * DTrace flags.
 */
#define	CPU_DTRACE_NOFAULT	0x0001	/* Don't fault */
#define	CPU_DTRACE_DROP		0x0002	/* Drop this ECB */
#define	CPU_DTRACE_BADADDR	0x0004	/* DTrace fault: bad address */
#define	CPU_DTRACE_BADALIGN	0x0008	/* DTrace fault: bad alignment */
#define	CPU_DTRACE_DIVZERO	0x0010	/* DTrace fault: divide by zero */
#define	CPU_DTRACE_ILLOP	0x0020	/* DTrace fault: illegal operation */
#define	CPU_DTRACE_NOSCRATCH	0x0040	/* DTrace fault: out of scratch */
#define	CPU_DTRACE_KPRIV	0x0080	/* DTrace fault: bad kernel access */
#define	CPU_DTRACE_UPRIV	0x0100	/* DTrace fault: bad user access */
#define	CPU_DTRACE_TUPOFLOW	0x0200	/* DTrace fault: tuple stack overflow */
#if defined(__sparc)
#define	CPU_DTRACE_FAKERESTORE	0x0400	/* pid provider hint to getreg */
#endif
#define	CPU_DTRACE_ENTRY	0x0800	/* pid provider hint to ustack() */

#define	CPU_DTRACE_FAULT	(CPU_DTRACE_BADADDR | CPU_DTRACE_BADALIGN | \
				CPU_DTRACE_DIVZERO | CPU_DTRACE_ILLOP | \
				CPU_DTRACE_NOSCRATCH | CPU_DTRACE_KPRIV | \
				CPU_DTRACE_UPRIV | CPU_DTRACE_TUPOFLOW)
#define	CPU_DTRACE_ERROR	(CPU_DTRACE_FAULT | CPU_DTRACE_DROP)

/*
 * Dispatcher flags
 * These flags must be changed only by the current CPU.
 */
#define	CPU_DISP_DONTSTEAL	0x01	/* CPU undergoing context swtch */
#define	CPU_DISP_HALTED		0x02	/* CPU halted waiting for interrupt */


#endif /* _KERNEL || _KMEMUSER */

#if (defined(_KERNEL) || defined(_KMEMUSER)) && defined(_MACHDEP)

/*
 * Macros for manipulating sets of CPUs as a bitmap.  Note that this
 * bitmap may vary in size depending on the maximum CPU id a specific
 * platform supports.  This may be different than the number of CPUs
 * the platform supports, since CPU ids can be sparse.  We define two
 * sets of macros; one for platforms where the maximum CPU id is less
 * than the number of bits in a single word (32 in a 32-bit kernel,
 * 64 in a 64-bit kernel), and one for platforms that require bitmaps
 * of more than one word.
 */

#define	CPUSET_WORDS	BT_BITOUL(NCPU)
#define	CPUSET_NOTINSET	((uint_t)-1)

#if	CPUSET_WORDS > 1

typedef struct cpuset {
	ulong_t	cpub[CPUSET_WORDS];
} cpuset_t;

/*
 * Private functions for manipulating cpusets that do not fit in a
 * single word.  These should not be used directly; instead the
 * CPUSET_* macros should be used so the code will be portable
 * across different definitions of NCPU.
 */
extern	void	cpuset_all(cpuset_t *);
extern	void	cpuset_all_but(cpuset_t *, uint_t);
extern	int	cpuset_isnull(cpuset_t *);
extern	int	cpuset_cmp(cpuset_t *, cpuset_t *);
extern	void	cpuset_only(cpuset_t *, uint_t);
extern	uint_t	cpuset_find(cpuset_t *);

#define	CPUSET_ALL(set)			cpuset_all(&(set))
#define	CPUSET_ALL_BUT(set, cpu)	cpuset_all_but(&(set), cpu)
#define	CPUSET_ONLY(set, cpu)		cpuset_only(&(set), cpu)
#define	CPU_IN_SET(set, cpu)		BT_TEST((set).cpub, cpu)
#define	CPUSET_ADD(set, cpu)		BT_SET((set).cpub, cpu)
#define	CPUSET_DEL(set, cpu)		BT_CLEAR((set).cpub, cpu)
#define	CPUSET_ISNULL(set)		cpuset_isnull(&(set))
#define	CPUSET_ISEQUAL(set1, set2)	cpuset_cmp(&(set1), &(set2))

/*
 * Find one CPU in the cpuset.
 * Sets "cpu" to the id of the found CPU, or CPUSET_NOTINSET if no cpu
 * could be found. (i.e. empty set)
 */
#define	CPUSET_FIND(set, cpu)		{		\
	cpu = cpuset_find(&(set));			\
}

/*
 * Atomic cpuset operations
 * These are safe to use for concurrent cpuset manipulations.
 * "xdel" and "xadd" are exclusive operations, that set "result" to "0"
 * if the add or del was successful, or "-1" if not successful.
 * (e.g. attempting to add a cpu to a cpuset that's already there, or
 * deleting a cpu that's not in the cpuset)
 */

#define	CPUSET_ATOMIC_DEL(set, cpu)	BT_ATOMIC_CLEAR((set).cpub, (cpu))
#define	CPUSET_ATOMIC_ADD(set, cpu)	BT_ATOMIC_SET((set).cpub, (cpu))

#define	CPUSET_ATOMIC_XADD(set, cpu, result) \
	BT_ATOMIC_SET_EXCL((set).cpub, cpu, result)

#define	CPUSET_ATOMIC_XDEL(set, cpu, result) \
	BT_ATOMIC_CLEAR_EXCL((set).cpub, cpu, result)


#define	CPUSET_OR(set1, set2)		{		\
	int _i;						\
	for (_i = 0; _i < CPUSET_WORDS; _i++)		\
		(set1).cpub[_i] |= (set2).cpub[_i];	\
}

#define	CPUSET_AND(set1, set2)		{		\
	int _i;						\
	for (_i = 0; _i < CPUSET_WORDS; _i++)		\
		(set1).cpub[_i] &= (set2).cpub[_i];	\
}

#define	CPUSET_ZERO(set)		{		\
	int _i;						\
	for (_i = 0; _i < CPUSET_WORDS; _i++)		\
		(set).cpub[_i] = 0;			\
}

#elif	CPUSET_WORDS == 1

typedef	ulong_t	cpuset_t;	/* a set of CPUs */

#define	CPUSET(cpu)			(1UL << (cpu))

#define	CPUSET_ALL(set)			((void)((set) = ~0UL))
#define	CPUSET_ALL_BUT(set, cpu)	((void)((set) = ~CPUSET(cpu)))
#define	CPUSET_ONLY(set, cpu)		((void)((set) = CPUSET(cpu)))
#define	CPU_IN_SET(set, cpu)		((set) & CPUSET(cpu))
#define	CPUSET_ADD(set, cpu)		((void)((set) |= CPUSET(cpu)))
#define	CPUSET_DEL(set, cpu)		((void)((set) &= ~CPUSET(cpu)))
#define	CPUSET_ISNULL(set)		((set) == 0)
#define	CPUSET_ISEQUAL(set1, set2)	((set1) == (set2))
#define	CPUSET_OR(set1, set2)		((void)((set1) |= (set2)))
#define	CPUSET_AND(set1, set2)		((void)((set1) &= (set2)))
#define	CPUSET_ZERO(set)		((void)((set) = 0))

#define	CPUSET_FIND(set, cpu)		{		\
	cpu = (uint_t)(lowbit(set) - 1);				\
}

#define	CPUSET_ATOMIC_DEL(set, cpu)	atomic_and_long(&(set), ~CPUSET(cpu))
#define	CPUSET_ATOMIC_ADD(set, cpu)	atomic_or_long(&(set), CPUSET(cpu))

#define	CPUSET_ATOMIC_XADD(set, cpu, result) \
	{ result = atomic_set_long_excl(&(set), (cpu)); }

#define	CPUSET_ATOMIC_XDEL(set, cpu, result) \
	{ result = atomic_clear_long_excl(&(set), (cpu)); }

#else	/* CPUSET_WORDS <= 0 */

#error NCPU is undefined or invalid

#endif	/* CPUSET_WORDS	*/

extern cpuset_t cpu_seqid_inuse;

#endif	/* (_KERNEL || _KMEMUSER) && _MACHDEP */

#define	CPU_CPR_OFFLINE		0x0
#define	CPU_CPR_ONLINE		0x1
#define	CPU_CPR_IS_OFFLINE(cpu)	(((cpu)->cpu_cpr_flags & CPU_CPR_ONLINE) == 0)
#define	CPU_SET_CPR_FLAGS(cpu, flag)	((cpu)->cpu_cpr_flags |= flag)

#if defined(_KERNEL) || defined(_KMEMUSER)

extern struct cpu	*cpu[];		/* indexed by CPU number */
extern cpu_t		*cpu_list;	/* list of CPUs */
extern int		ncpus;		/* number of CPUs present */
extern int		ncpus_online;	/* number of CPUs not quiesced */
extern int		max_ncpus;	/* max present before ncpus is known */
extern int		boot_max_ncpus;	/* like max_ncpus but for real */
extern processorid_t	max_cpuid;	/* maximum CPU number */
extern struct cpu	*cpu_inmotion;	/* offline or partition move target */

#if defined(__i386) || defined(__amd64)
extern struct cpu *curcpup(void);
#define	CPU		(curcpup())	/* Pointer to current CPU */
#else
#define	CPU		(curthread->t_cpu)	/* Pointer to current CPU */
#endif

/*
 * CPU_CURRENT indicates to thread_affinity_set to use CPU->cpu_id
 * as the target and to grab cpu_lock instead of requiring the caller
 * to grab it.
 */
#define	CPU_CURRENT	-3

/*
 * Per-CPU statistics
 *
 * cpu_stats_t contains numerous system and VM-related statistics, in the form
 * of gauges or monotonically-increasing event occurrence counts.
 */

#define	CPU_STATS_ENTER_K()	kpreempt_disable()
#define	CPU_STATS_EXIT_K()	kpreempt_enable()

#define	CPU_STATS_ADD_K(class, stat, amount) \
	{	kpreempt_disable(); /* keep from switching CPUs */\
		CPU_STATS_ADDQ(CPU, class, stat, amount); \
		kpreempt_enable(); \
	}

#define	CPU_STATS_ADDQ(cp, class, stat, amount)	{			\
	extern void __dtrace_probe___cpu_##class##info_##stat(uint_t,	\
	    uint64_t *, cpu_t *);					\
	uint64_t *stataddr = &((cp)->cpu_stats.class.stat);		\
	__dtrace_probe___cpu_##class##info_##stat((amount),		\
	    stataddr, cp);						\
	*(stataddr) += (amount);					\
}

#define	CPU_STATS(cp, stat)                                       \
	((cp)->cpu_stats.stat)

#endif /* _KERNEL || _KMEMUSER */

/*
 * CPU support routines.
 */
#if	defined(_KERNEL) && defined(__STDC__)	/* not for genassym.c */

struct zone;

void	cpu_list_init(cpu_t *);
void	cpu_add_unit(cpu_t *);
void	cpu_del_unit(int cpuid);
void	cpu_add_active(cpu_t *);
void	cpu_kstat_init(cpu_t *);
void	cpu_visibility_add(cpu_t *, struct zone *);
void	cpu_visibility_remove(cpu_t *, struct zone *);
void	cpu_visibility_configure(cpu_t *, struct zone *);
void	cpu_visibility_unconfigure(cpu_t *, struct zone *);
void	cpu_visibility_online(cpu_t *, struct zone *);
void	cpu_visibility_offline(cpu_t *, struct zone *);
void	cpu_create_intrstat(cpu_t *);
void	cpu_delete_intrstat(cpu_t *);
int	cpu_kstat_intrstat_update(kstat_t *, int);
void	cpu_intr_swtch_enter(kthread_t *);
void	cpu_intr_swtch_exit(kthread_t *);

void	mbox_lock_init(void);	 /* initialize cross-call locks */
void	mbox_init(int cpun);	 /* initialize cross-calls */
void	poke_cpu(int cpun);	 /* interrupt another CPU (to preempt) */

void	pause_cpus(cpu_t *off_cp);
void	start_cpus(void);
int	cpus_paused(void);

void	cpu_pause_init(void);
cpu_t	*cpu_get(processorid_t cpun);	/* get the CPU struct associated */

int	cpu_online(cpu_t *cp);			/* take cpu online */
int	cpu_offline(cpu_t *cp, int flags);	/* take cpu offline */
int	cpu_spare(cpu_t *cp, int flags);	/* take cpu to spare */
int	cpu_faulted(cpu_t *cp, int flags);	/* take cpu to faulted */
int	cpu_poweron(cpu_t *cp);		/* take powered-off cpu to offline */
int	cpu_poweroff(cpu_t *cp);	/* take offline cpu to powered-off */

cpu_t	*cpu_intr_next(cpu_t *cp);	/* get next online CPU taking intrs */
int	cpu_intr_count(cpu_t *cp);	/* count # of CPUs handling intrs */
int	cpu_intr_on(cpu_t *cp);		/* CPU taking I/O interrupts? */
void	cpu_intr_enable(cpu_t *cp);	/* enable I/O interrupts */
int	cpu_intr_disable(cpu_t *cp);	/* disable I/O interrupts */

/*
 * Routines for checking CPU states.
 */
int	cpu_is_online(cpu_t *);		/* check if CPU is online */
int	cpu_is_nointr(cpu_t *);		/* check if CPU can service intrs */
int	cpu_is_active(cpu_t *);		/* check if CPU can run threads */
int	cpu_is_offline(cpu_t *);	/* check if CPU is offline */
int	cpu_is_poweredoff(cpu_t *);	/* check if CPU is powered off */

int	cpu_flagged_online(cpu_flag_t);	/* flags show CPU is online */
int	cpu_flagged_nointr(cpu_flag_t);	/* flags show CPU not handling intrs */
int	cpu_flagged_active(cpu_flag_t); /* flags show CPU scheduling threads */
int	cpu_flagged_offline(cpu_flag_t); /* flags show CPU is offline */
int	cpu_flagged_poweredoff(cpu_flag_t); /* flags show CPU is powered off */

/*
 * The processor_info(2) state of a CPU is a simplified representation suitable
 * for use by an application program.  Kernel subsystems should utilize the
 * internal per-CPU state as given by the cpu_flags member of the cpu structure,
 * as this information may include platform- or architecture-specific state
 * critical to a subsystem's disposition of a particular CPU.
 */
void	cpu_set_state(cpu_t *);		/* record/timestamp current state */
int	cpu_get_state(cpu_t *);		/* get current cpu state */
const char *cpu_get_state_str(cpu_t *);	/* get current cpu state as string */

int	cpu_configure(int);
int	cpu_unconfigure(int);
void	cpu_destroy_bound_threads(cpu_t *cp);

extern int cpu_bind_thread(kthread_t *tp, processorid_t bind,
    processorid_t *obind, int *error);
extern int cpu_unbind(processorid_t cpu_id);
extern void thread_affinity_set(kthread_t *t, int cpu_id);
extern void thread_affinity_clear(kthread_t *t);
extern void affinity_set(int cpu_id);
extern void affinity_clear(void);
extern void init_cpu_mstate(struct cpu *, int);
extern void term_cpu_mstate(struct cpu *);
extern void new_cpu_mstate(struct cpu *, int);
extern void thread_nomigrate(void);
extern void thread_allowmigrate(void);
extern void weakbinding_stop(void);
extern void weakbinding_start(void);

/*
 * The following routines affect the CPUs participation in interrupt processing,
 * if that is applicable on the architecture.  This only affects interrupts
 * which aren't directed at the processor (not cross calls).
 *
 * cpu_disable_intr returns non-zero if interrupts were previously enabled.
 */
int	cpu_disable_intr(struct cpu *cp); /* stop issuing interrupts to cpu */
void	cpu_enable_intr(struct cpu *cp); /* start issuing interrupts to cpu */

/*
 * The mutex cpu_lock protects cpu_flags for all CPUs, as well as the ncpus
 * and ncpus_online counts.
 */
extern kmutex_t	cpu_lock;	/* lock protecting CPU data */

typedef enum {
	CPU_INIT,
	CPU_CONFIG,
	CPU_UNCONFIG,
	CPU_ON,
	CPU_OFF,
	CPU_CPUPART_IN,
	CPU_CPUPART_OUT
} cpu_setup_t;

typedef int cpu_setup_func_t(cpu_setup_t, int, void *);

/*
 * Routines used to register interest in cpu's being added to or removed
 * from the system.
 */
extern void register_cpu_setup_func(cpu_setup_func_t *, void *);
extern void unregister_cpu_setup_func(cpu_setup_func_t *, void *);
extern void cpu_state_change_notify(int, cpu_setup_t);

/*
 * Create various strings that describe the given CPU for the
 * processor_info system call and configuration-related kstats.
 */
#define	CPU_IDSTRLEN	100

extern void init_cpu_info(struct cpu *);

#endif	/* _KERNEL */

#ifdef	__cplusplus
}
#endif

#endif /* _SYS_CPUVAR_H */
