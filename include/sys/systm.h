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
/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


/*
 * Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _SYS_SYSTM_H
#define	_SYS_SYSTM_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include <sys/types.h>
#include <sys/t_lock.h>
#include <sys/proc.h>
#include <sys/dditypes.h>

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * The pc_t is the type of the kernel's program counter.  In general, a
 * pc_t is a uintptr_t -- except for a sparcv9 kernel, in which case all
 * instruction text is below 4G, and a pc_t is thus a uint32_t.
 */
#ifdef __sparcv9
typedef uint32_t pc_t;
#else
typedef uintptr_t pc_t;
#endif

/*
 * Random set of variables used by more than one routine.
 */

#ifdef _KERNEL
#include <sys/varargs.h>

extern int hz;			/* system clock rate */
extern struct vnode *rootdir;	/* pointer to vnode of root directory */
extern struct vnode *devicesdir;	/* pointer to /devices vnode */
extern volatile clock_t lbolt;	/* time in HZ since last boot */
extern volatile int64_t lbolt64;	/* lbolt computed as 64-bit value */
extern int interrupts_unleashed;	/* set after the spl0() in main() */

extern char runin;		/* scheduling flag */
extern char runout;		/* scheduling flag */
extern char wake_sched;		/* causes clock to wake swapper on next tick */
extern char wake_sched_sec;	/* causes clock to wake swapper after a sec */

extern pgcnt_t	maxmem;		/* max available memory (pages) */
extern pgcnt_t	physmem;	/* physical memory (pages) on this CPU */
extern pfn_t	physmax;	/* highest numbered physical page present */
extern pgcnt_t	physinstalled;	/* physical pages including PROM/boot use */

extern caddr_t	s_text;		/* start of kernel text segment */
extern caddr_t	e_text;		/* end of kernel text segment */
extern caddr_t	s_data;		/* start of kernel text segment */
extern caddr_t	e_data;		/* end of kernel text segment */
#if defined(__ia64)
extern caddr_t	s_sdata;	/* start of kernel small data segment */
extern caddr_t	e_sdata;	/* end of kernel small data segment */
#endif /* __ia64 */

extern pgcnt_t	availrmem;	/* Available resident (not swapable)	*/
				/* memory in pages.			*/
extern pgcnt_t	availrmem_initial;	/* initial value of availrmem	*/
extern pgcnt_t	segspt_minfree;	/* low water mark for availrmem in seg_spt */
extern pgcnt_t	freemem;	/* Current free memory.			*/

extern dev_t	rootdev;	/* device of the root */
extern struct vnode *rootvp;	/* vnode of root device */
extern int	root_is_svm;	/* root is a mirrored device flag */
extern char *volatile panicstr;	/* panic string pointer */
extern va_list  panicargs;	/* panic arguments */

extern int	rstchown;	/* 1 ==> restrictive chown(2) semantics */
extern int	klustsize;

extern int	abort_enable;	/* Platform input-device abort policy */

#ifdef C2_AUDIT
extern int	audit_active;	/* C2 auditing activate 1, absent 0. */
#endif

extern int	avenrun[];	/* array of load averages */

extern char *isa_list;		/* For sysinfo's isalist option */

extern int noexec_user_stack;		/* patchable via /etc/system */
extern int noexec_user_stack_log;	/* patchable via /etc/system */

extern void report_stack_exec(proc_t *, caddr_t);

extern void startup(void);
extern void clkstart(void);
extern void post_startup(void);
extern void kern_setup1(void);
extern void ka_init(void);
extern void nodename_set(void);

/*
 * for tod fault detection
 */
enum tod_fault_type {
	TOD_REVERSED = 0,
	TOD_STALLED,
	TOD_JUMPED,
	TOD_RATECHANGED,
	TOD_NOFAULT
};

extern time_t tod_validate(time_t);
extern void tod_fault_reset(void);
extern void plat_tod_fault(enum tod_fault_type);
#pragma	weak	plat_tod_fault

#ifndef _LP64
#ifndef min
int min(int, int);
#endif

#ifndef max
int max(int, int);
#endif

uint_t umin(uint_t, uint_t);
uint_t umax(uint_t, uint_t);
#endif /* !_LP64 */
int grow(caddr_t);
int grow_internal(caddr_t, uint_t);
int brk_internal(caddr_t, uint_t);
timeout_id_t timeout(void (*)(void *), void *, clock_t);
timeout_id_t realtime_timeout(void (*)(void *), void *, clock_t);
clock_t untimeout(timeout_id_t);
void delay(clock_t);
int delay_sig(clock_t);
int nodev();
int nulldev();
major_t getudev(void);
int cmpldev(dev32_t *, dev_t);
dev_t expldev(dev32_t);
int bcmp(const void *, const void *, size_t) __PURE;
int stoi(char **);
void numtos(ulong_t, char *);
size_t strlen(const char *) __PURE;
char *strcat(char *, const char *);
char *strncat(char *, const char *, size_t);
char *strcpy(char *, const char *);
char *strncpy(char *, const char *, size_t);
/* Need to be consistent with <string.h> C++ definitions */
#if __cplusplus >= 199711L
extern const char *strchr(const char *, int);
#ifndef _STRCHR_INLINE
#define	_STRCHR_INLINE
extern "C++" {
	inline char *strchr(char *__s, int __c) {
		return (char *)strchr((const char *)__s, __c);
	}
}
#endif /* _STRCHR_INLINE */
extern const char *strrchr(const char *, int);
#ifndef	_STRRCHR_INLINE
#define	_STRRCHR_INLINE
extern "C++" {
	inline char *strrchr(char *__s, int __c) {
		return (char *)strrchr((const char *)__s, __c);
	}
}
#endif	/* _STRRCHR_INLINE */
extern const char *strstr(const char *, const char *);
#ifndef	_STRSTR_INLINE
#define	_STRSTR_INLINE
extern "C++" {
	inline char *strstr(char *__s1, const char *__s2) {
		return (char *)strstr((const char *)__s1, __s2);
	}
}
#endif  /* _STRSTR_INLINE */
#else	/* __cplusplus >= 199711L */
char *strchr(const char *, int);
char *strrchr(const char *, int);
char *strstr(const char *, const char *);
#endif	/* __cplusplus >= 199711L */
char *strnrchr(const char *, int, size_t);
int strcmp(const char *, const char *) __PURE;
int strncmp(const char *, const char *, size_t) __PURE;
int strcasecmp(const char *, const char *) __PURE;
int strncasecmp(const char *, const char *, size_t) __PURE;
/* Need to be consistent with <string.h> C++ definitions */
#if __cplusplus >= 199711L
extern const char *strpbrk(const char *, const char *);
#ifndef _STRPBRK_INLINE
#define	_STRPBRK_INLINE
extern "C++" {
	inline char *strpbrk(char *__s1, const char *__s2) {
		return (char *)strpbrk((const char *)__s1, __s2);
	}
}
#endif /* _STRPBRK_INLINE */
#else /* __cplusplus >= 199711L */
char *strpbrk(const char *, const char *);
#endif /* __cplusplus >= 199711L */
int strident_valid(const char *);
void strident_canon(char *, size_t);
int getsubopt(char **optionsp, char * const *tokens, char **valuep);
char *append_subopt(const char *, size_t, char *, const char *);
int ffs(long);
int copyin(const void *, void *, size_t);
void copyin_noerr(const void *, void *, size_t);
int xcopyin(const void *, void *, size_t);
int xcopyin_nta(const void *, void *, size_t, int);
int copyout(const void *, void *, size_t);
void copyout_noerr(const void *, void *, size_t);
int xcopyout(const void *, void *, size_t);
int xcopyout_nta(const void *, void *, size_t, int);
int copyinstr(const char *, char *, size_t, size_t *);
int copyinstr_noerr(const char *, char *, size_t, size_t *);
int copyoutstr(const char *, char *, size_t, size_t *);
int copyoutstr_noerr(const char *, char *, size_t, size_t *);
int copystr(const char *, char *, size_t, size_t *);
void bcopy(const void *, void *, size_t);
void ucopy(const void *, void *, size_t);
void pgcopy(const void *, void *, size_t);
void ovbcopy(const void *, void *, size_t);
void bzero(void *, size_t);
void uzero(void *, size_t);
int kcopy(const void *, void *, size_t);
int kcopy_nta(const void *, void *, size_t, int);
int kzero(void *, size_t);

extern void *memset(void *, int, size_t);
extern void *memcpy(void *, const void *, size_t);
extern void *memmove(void *, const void *, size_t);
extern int memcmp(const void *, const void *, size_t);

int fuword8(const void *, uint8_t *);
int fuword16(const void *, uint16_t *);
int fuword32(const void *, uint32_t *);
int fulword(const void *, ulong_t *);
void fuword8_noerr(const void *, uint8_t *);
void fuword16_noerr(const void *, uint16_t *);
void fuword32_noerr(const void *, uint32_t *);
void fulword_noerr(const void *, ulong_t *);

#ifdef _LP64
int fuword64(const void *, uint64_t *);
void fuword64_noerr(const void *, uint64_t *);
#endif

int subyte(void *, uint8_t);
int suword8(void *, uint8_t);
int suword16(void *, uint16_t);
int suword32(void *, uint32_t);
int sulword(void *, ulong_t);
void subyte_noerr(void *, uint8_t);
void suword8_noerr(void *, uint8_t);
void suword16_noerr(void *, uint16_t);
void suword32_noerr(void *, uint32_t);
void sulword_noerr(void *, ulong_t);

#ifdef _LP64
int suword64(void *, uint64_t);
void suword64_noerr(void *, uint64_t);
#endif

#if !defined(_BOOT)
int setjmp(label_t *);
extern void longjmp(label_t *)
	__NORETURN;
#pragma unknown_control_flow(setjmp)
#endif

caddr_t caller(void);
caddr_t callee(void);
int getpcstack(pc_t *, int);
int on_fault(label_t *);
void no_fault(void);
void halt(char *);
int scanc(size_t, uchar_t *, uchar_t *, uchar_t);
int movtuc(size_t, uchar_t *, uchar_t *, uchar_t *);
int splr(int);
int splhigh(void);
int splhi(void);
int splzs(void);
int spl0(void);
int spl6(void);
int spl7(void);
int spl8(void);
void splx(int);
void set_base_spl(void);
int __ipltospl(int);

void softcall_init(void);
void softcall(void (*)(void *), void *);
void softint(void);

extern void sync_icache(caddr_t, uint_t);
extern void sync_data_memory(caddr_t, size_t);
extern void hot_patch_kernel_text(caddr_t, uint32_t, uint_t);

void _insque(caddr_t, caddr_t);
void _remque(caddr_t);

/* casts to keep lint happy */
#define	insque(q, p)	_insque((caddr_t)q, (caddr_t)p)
#define	remque(q)	_remque((caddr_t)q)

#pragma unknown_control_flow(on_fault)

struct timeval;
extern void	uniqtime(struct timeval *);
struct timeval32;
extern void	uniqtime32(struct timeval32 *);

uint_t page_num_pagesizes(void);
size_t page_get_pagesize(uint_t n);

extern int maxusers;
extern int pidmax;

extern void param_preset(void);
extern void param_calc(int);
extern void param_init(void);
extern void param_check(void);

#endif /* _KERNEL */

/*
 * Structure of the system-entry table.
 *
 * 	Changes to struct sysent should maintain binary compatibility with
 *	loadable system calls, although the interface is currently private.
 *
 *	This means it should only be expanded on the end, and flag values
 * 	should not be reused.
 *
 *	It is desirable to keep the size of this struct a power of 2 for quick
 *	indexing.
 */
struct sysent {
	char		sy_narg;	/* total number of arguments */
#ifdef _LP64
	unsigned short	sy_flags;	/* various flags as defined below */
#else
	unsigned char	sy_flags;	/* various flags as defined below */
#endif
	int		(*sy_call)();	/* argp, rvalp-style handler */
	krwlock_t	*sy_lock;	/* lock for loadable system calls */
	int64_t		(*sy_callc)();	/* C-style call hander or wrapper */
};

extern struct sysent	sysent[];
#ifdef _SYSCALL32_IMPL
extern struct sysent	sysent32[];
#endif

extern struct sysent	nosys_ent;	/* entry for invalid system call */

#define	NSYSCALL 	256		/* number of system calls */

#define	LOADABLE_SYSCALL(s)	(s->sy_flags & SE_LOADABLE)
#define	LOADED_SYSCALL(s)	(s->sy_flags & SE_LOADED)

/*
 * sy_flags values
 * 	Values 1, 2, and 4 were used previously for SETJUMP, ASYNC, and IOSYS.
 */
#define	SE_32RVAL1	0x0		/* handler returns int32_t in rval1 */
#define	SE_32RVAL2	0x1		/* handler returns int32_t in rval2 */
#define	SE_64RVAL	0x2		/* handler returns int64_t in rvals */
#define	SE_RVAL_MASK	0x3		/* mask of rval_t bits */

#define	SE_LOADABLE	0x08		/* syscall is loadable */
#define	SE_LOADED	0x10		/* syscall is completely loaded */
#define	SE_NOUNLOAD	0x20		/* syscall never needs unload */
#define	SE_ARGC		0x40		/* syscall takes C-style args */

/*
 * Structure of the return-value parameter passed by reference to
 * system entries.
 */
union rval {
	struct	{
		int	r_v1;
		int	r_v2;
	} r_v;
	off_t	r_off;
	offset_t r_offset;
	time_t	r_time;
	int64_t	r_vals;
};
#define	r_val1	r_v.r_v1
#define	r_val2	r_v.r_v2

typedef union rval rval_t;

#ifdef	_KERNEL

extern void reset_syscall_args(void);
extern int save_syscall_args(void);
extern uint_t get_syscall_args(klwp_t *lwp, long *argp, int *nargsp);
#ifdef _SYSCALL32_IMPL
extern uint_t get_syscall32_args(klwp_t *lwp, int *argp, int *nargp);
#endif

extern uint_t set_errno(uint_t error);
#pragma rarely_called(set_errno)

extern int64_t syscall_ap(void);
extern int64_t loadable_syscall(long, long, long, long, long, long, long, long);
extern int64_t nosys(void);

extern void swtch(void);

extern uint_t	kcpc_key;	/* TSD key for performance counter context */

#ifdef __lint
extern	int	__lintzero;	/* for spoofing lint */
#endif


/*
 * initname holds the path to init.  It is defined by main.c, may be set by
 * bootflags.c, and is used by main.c:exec_init().
 */
#define	INITNAME_SZ	32
#define	INITARGS_SZ	80

extern char initname[INITNAME_SZ];
extern char initargs[INITARGS_SZ];

extern int exec_init(const char *, int, const char *);

#endif	/* _KERNEL */

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_SYSTM_H */
