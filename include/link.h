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

#ifndef _LINK_H
#define	_LINK_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include <sys/link.h>

#ifndef _ASM
#include <libelf.h>
#include <sys/types.h>
#include <dlfcn.h>
#endif

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef _ASM
/*
 * ld support library calls
 */
#ifdef __STDC__
extern uint_t	ld_version(uint_t);
extern void	ld_start(const char *, const Elf32_Half, const char *);
extern void	ld_atexit(int);
extern void	ld_file(const char *, const Elf_Kind, int, Elf *);
extern void	ld_input_section(const char *, Elf32_Shdr **, Elf32_Word,
			Elf_Data *, Elf *, uint_t *);
extern void	ld_input_done(uint_t *);
extern void	ld_section(const char *, Elf32_Shdr *, Elf32_Word,
			Elf_Data *, Elf *);

#if defined(_LP64) || defined(_LONGLONG_TYPE)
extern void	ld_start64(const char *, const Elf64_Half, const char *);
extern void	ld_atexit64(int);
extern void	ld_file64(const char *, const Elf_Kind, int, Elf *);
extern void	ld_input_section64(const char *, Elf64_Shdr **, Elf64_Word,
			Elf_Data *, Elf *, uint_t *);
extern void	ld_section64(const char *, Elf64_Shdr *, Elf64_Word,
			Elf_Data *, Elf *);

#endif /* (defined(_LP64) || defined(_LONGLONG_TYPE) */
#else
extern void	ld_version();
extern void	ld_start();
extern void	ld_atexit();
extern void	ld_file();
extern void	ld_input_section();
extern void	ld_input_done();
extern void	ld_section();

#if defined(_LP64) || defined(_LONGLONG_TYPE)
extern void	ld_start64();
extern void	ld_atexit64();
extern void	ld_file64();
extern void	ld_section64();

#endif /* (defined(_LP64) || defined(_LONGLONG_TYPE) */
#endif /* __STDC__ */

/*
 * ld_version() version values
 */
#define	LD_SUP_VNONE	0
#define	LD_SUP_VERSION1	1
#define	LD_SUP_VERSION2	2
#define	LD_SUP_VCURRENT	LD_SUP_VERSION2


/*
 * flags passed to ld support calls
 */
#define	LD_SUP_DERIVED		0x1	/* derived filename */
#define	LD_SUP_INHERITED	0x2	/* file inherited from .so DT_NEEDED */
#define	LD_SUP_EXTRACTED	0x4	/* file extracted from archive */
#endif

#define	LM_ID_BASE		0x00
#define	LM_ID_LDSO		0x01
#define	LM_ID_NUM		2

#define	LM_ID_NEWLM		0xff	/* create a new link-map */


/*
 * Run-Time Link-Edit Auditing
 */
#define	LAV_NONE		0
#define	LAV_VERSION1		1
#define	LAV_VERSION2		2
#define	LAV_VERSION3		3
#define	LAV_CURRENT		LAV_VERSION3
#define	LAV_NUM			4

/*
 * Flags that can be or'd into the la_objopen() return code
 */
#define	LA_FLG_BINDTO		0x0001	/* audit symbinds TO this object */
#define	LA_FLG_BINDFROM		0x0002	/* audit symbinding FROM this object */

/*
 * Flags that can be or'd into the 'flags' argument of la_symbind()
 */
#define	LA_SYMB_NOPLTENTER	0x0001	/* disable pltenter for this symbol */
#define	LA_SYMB_NOPLTEXIT	0x0002	/* disable pltexit for this symbol */
#define	LA_SYMB_STRUCTCALL	0x0004	/* this function call passes a */
					/*	structure as it's return code */
#define	LA_SYMB_DLSYM		0x0008	/* this symbol bindings is due to */
					/*	a call to dlsym() */
#define	LA_SYMB_ALTVALUE	0x0010	/* alternate symbol binding returned */
					/*	by la_symbind() */

/*
 * Flags that describe the object passed to la_objsearch()
 */
#define	LA_SER_ORIG		0x001	/* original (needed) name */
#define	LA_SER_LIBPATH		0x002	/* LD_LIBRARY_PATH entry prepended */
#define	LA_SER_RUNPATH		0x004	/* runpath entry prepended */
#define	LA_SER_CONFIG		0x008	/* configuration entry prepended */
#define	LA_SER_DEFAULT		0x040	/* default path prepended */
#define	LA_SER_SECURE		0x080	/* default (secure) path prepended */

#define	LA_SER_MASK		0xfff	/* mask of known flags */

/*
 * Flags that describe the la_activity()
 */
#define	LA_ACT_CONSISTENT	0x00	/* add/deletion of objects complete */
#define	LA_ACT_ADD		0x01	/* objects being added */
#define	LA_ACT_DELETE		0x02	/* objects being deleted */


#ifndef	_KERNEL
#ifndef	_ASM

#if defined(_LP64)
typedef long	lagreg_t;
#else
typedef int	lagreg_t;
#endif

struct _la_sparc_regs {
	lagreg_t	lr_rego0;
	lagreg_t	lr_rego1;
	lagreg_t	lr_rego2;
	lagreg_t	lr_rego3;
	lagreg_t	lr_rego4;
	lagreg_t	lr_rego5;
	lagreg_t	lr_rego6;
	lagreg_t	lr_rego7;
};

#if defined(_LP64)
typedef struct _la_sparc_regs	La_sparcv9_regs;
typedef struct {
	lagreg_t	lr_rsp;
	lagreg_t	lr_rbp;
	lagreg_t	lr_rdi;	    /* arg1 */
	lagreg_t	lr_rsi;	    /* arg2 */
	lagreg_t	lr_rdx;	    /* arg3 */
	lagreg_t	lr_rcx;	    /* arg4 */
	lagreg_t	lr_r8;	    /* arg5 */
	lagreg_t	lr_r9;	    /* arg6 */
} La_amd64_regs;
#else
typedef struct _la_sparc_regs	La_sparcv8_regs;
typedef struct {
	lagreg_t	lr_esp;
	lagreg_t	lr_ebp;
} La_i86_regs;
#endif

#if	!defined(_SYS_INT_TYPES_H)
#if	defined(_LP64) || defined(_I32LPx)
typedef unsigned long		uintptr_t;
#else
typedef	unsigned int		uintptr_t;
#endif
#endif


#ifdef	__STDC__
extern uint_t		la_version(uint_t);
extern void		la_activity(uintptr_t *, uint_t);
extern void		la_preinit(uintptr_t *);
extern char		*la_objsearch(const char *, uintptr_t *, uint_t);
extern uint_t		la_objopen(Link_map *, Lmid_t, uintptr_t *);
extern uint_t		la_objclose(uintptr_t *);
extern int		la_objfilter(uintptr_t *, const char *, uintptr_t *,
				uint_t);
#if	defined(_LP64)
extern uintptr_t	la_amd64_pltenter(Elf64_Sym *, uint_t, uintptr_t *,
				uintptr_t *, La_amd64_regs *,	uint_t *,
				const char *);
extern uintptr_t	la_symbind64(Elf64_Sym *, uint_t, uintptr_t *,
				uintptr_t *, uint_t *, const char *);
extern uintptr_t	la_sparcv9_pltenter(Elf64_Sym *, uint_t, uintptr_t *,
				uintptr_t *, La_sparcv9_regs *,	uint_t *,
				const char *);
extern uintptr_t	la_pltexit64(Elf64_Sym *, uint_t, uintptr_t *,
				uintptr_t *, uintptr_t, const char *);
#else  /* !defined(_LP64) */
extern uintptr_t	la_symbind32(Elf32_Sym *, uint_t, uintptr_t *,
				uintptr_t *, uint_t *);
extern uintptr_t	la_sparcv8_pltenter(Elf32_Sym *, uint_t, uintptr_t *,
				uintptr_t *, La_sparcv8_regs *, uint_t *);
extern uintptr_t	la_i86_pltenter(Elf32_Sym *, uint_t, uintptr_t *,
				uintptr_t *, La_i86_regs *, uint_t *);
extern uintptr_t	la_pltexit(Elf32_Sym *, uint_t, uintptr_t *,
				uintptr_t *, uintptr_t);
#endif /* _LP64 */
#else  /* __STDC__ */
extern uint_t		la_version();
extern void		la_preinit();
extern uint_t		la_objopen();
extern uint_t		la_objclose();
extern int		la_objfilter();
#if	defined(_LP64)
extern uintptr_t	la_sparcv9_pltenter();
extern uintptr_t	la_pltexit64();
extern uintptr_t	la_symbind64();
#else  /* _ILP32 */
extern uintptr_t	la_sparcv8_pltenter();
extern uintptr_t	la_i86_pltenter();
extern uintptr_t	la_pltexit();
extern uintptr_t	la_symbind32();
#endif /* _LP64 */
#endif /* __STDC__ */


#endif /* _ASM */
#endif /* _KERNEL */

#ifdef __cplusplus
}
#endif

#endif	/* _LINK_H */
