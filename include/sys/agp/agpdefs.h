/*
 * Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _SYS_AGPDEFS_H
#define	_SYS_AGPDEFS_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This AGP memory type is required by some hadrware like i810 video
 * card, which need physical contiguous pages to setup hardware cursor.
 * Usually, several tens of kilo bytes are needed in this case.
 * We use DDI DMA interfaces to allocate such memory in agpgart driver,
 * and it can not be exported to user applications directly by calling mmap
 * on agpgart driver. The typical usage scenario is as the following:
 * Firstly, Xserver get the memory physical address by calling AGPIOC_ALLOCATE
 * on agpgart driver. Secondly, Xserver use the physical address to mmap
 * the memory to Xserver space area by xsvc driver.
 *
 */
#define	AGP_PHYSICAL		2	/* Only used for i810, HW curosr */

#ifdef _KERNEL

/* AGP space units */
#define	AGP_PAGE_SHIFT			12
#define	AGP_PAGE_SIZE			(1 << AGP_PAGE_SHIFT)
#define	AGP_PAGE_OFFSET			(AGP_PAGE_SIZE - 1)
#define	AGP_MB2PAGES(x)			((x) << 8)
#define	AGP_PAGES2BYTES(x)		((x) << AGP_PAGE_SHIFT)
#define	AGP_BYTES2PAGES(x)		((x) >> AGP_PAGE_SHIFT)
#define	AGP_PAGES2KB(x)			((x) << 2)
#define	AGP_ALIGNED(offset)		(((offset) & AGP_PAGE_OFFSET) == 0)

/* stand pci register offset */
#define	PCI_CONF_CAP_MASK		0x10
#define	PCI_CONF_CAPID_MASK		0xff
#define	PCI_CONF_NCAPID_MASK		0xff00

#define	INTEL_VENDOR_ID			0x8086
#define	AMD_VENDOR_ID			0x1022
#define	VENDOR_ID_MASK			0xffff

/* macros for device types */
#define	DEVICE_IS_I810		11 /* intel i810 series video card */
#define	DEVICE_IS_I830		12 /* intel i830, i845, i855 series */
#define	DEVICE_IS_AGP		21 /* external AGP video card */
#define	CHIP_IS_INTEL		10 /* intel agp bridge */
#define	CHIP_IS_AMD		20 /* amd agp bridge */

/* AGP bridge device id */
#define	AMD_BR_8151			0x74541022
#define	INTEL_BR_810			0x71208086
#define	INTEL_BR_810DC			0x71228086
#define	INTEL_BR_810E			0x71248086
#define	INTEL_BR_815			0x11308086 /* include 815G/EG/P/EP */
#define	INTEL_BR_830M			0x35758086
#define	INTEL_BR_845			0x25608086 /* include 845G/P */
#define	INTEL_BR_855GM			0x35808086 /* include 852GM/PM */
#define	INTEL_BR_855PM			0x33408086
#define	INTEL_BR_865			0x25708086

/* AGP common register offset in pci configuration space */
#define	AGP_CONF_MISC			0x51 /* one byte */
#define	AGP_CONF_CAPPTR			0x34
#define	AGP_CONF_APERBASE		0x10
#define	AGP_CONF_STATUS			0x04 /* CAP + 0x4 */
#define	AGP_CONF_COMMAND		0x08 /* CAP + 0x8 */

/* AGP target register and mask defines */
#define	AGP_CONF_CONTROL		0x10 /* CAP + 0x10 */
#define	AGP_TARGET_BAR1			1
#define	AGP_32_APERBASE_MASK		0xffc00000 /* 4M aligned */
#define	AGP_64_APERBASE_MASK		0xffffc00000LL /* 4M aligned */
#define	AGP_CONF_APERSIZE		0x14 /* CAP + 0x14 */
#define	AGP_CONF_ATTBASE		0x18 /* CAP + 0x18 */
#define	AGP_ATTBASE_MASK		0xfffff000
#define	AGPCTRL_GTLBEN			(0x1 << 7)
#define	AGP_APER_TYPE_MASK		0x4
#define	AGP_APER_SIZE_MASK		0xf00
#define	AGP_APER_128M_MASK		0x3f
#define	AGP_APER_4G_MASK		0xf00
#define	AGP_APER_4M			0x3f
#define	AGP_APER_8M			0x3e
#define	AGP_APER_16M			0x3c
#define	AGP_APER_32M			0x38
#define	AGP_APER_64M			0x30
#define	AGP_APER_128M			0x20
#define	AGP_APER_256M			0xf00
#define	AGP_APER_512M			0xe00
#define	AGP_APER_1024M			0xc00
#define	AGP_APER_2048M			0x800
#define	AGP_APER_4G			0x000
#define	AGP_MISC_APEN			0x2

/* AGP gart table definition */
#define	AGP_ENTRY_VALID			0x1

/* AGP term definitions */
#define	AGP_CAP_ID			0x2
#define	AGP_CAP_OFF_DEF			0xa0

/* Intel integrated video card, chipset id */
#define	INTEL_IGD_810			0x71218086
#define	INTEL_IGD_810DC			0x71238086
#define	INTEL_IGD_810E			0x71258086
#define	INTEL_IGD_815			0x11328086
#define	INTEL_IGD_830M			0x35778086
#define	INTEL_IGD_845G			0x25628086
#define	INTEL_IGD_855GM			0x35828086
#define	INTEL_IGD_865G			0x25728086
#define	I8XX_CONF_GMADR			0x10 /* offset in PCI config space */
#define	I8XX_CONF_GC			0x52 /* offset in PCI config space */

/* Intel integrated video card graphics mode mask */
#define	I8XX_GC_MODE_MASK		0x70
#define	I8XX_GC_MODE0			0x00
#define	I8XX_GC_MODE1			0x10
#define	I8XX_GC_MODE2			0x20
#define	I8XX_GC_MODE3			0x30
#define	I8XX_GC_MODE4			0x40
#define	I8XX_GC_MODE5			0x50

/* Intel integrated video card GTT definition */
#define	GTT_PAGE_SHIFT			12
#define	GTT_PAGE_SIZE			(1 << GTT_PAGE_SHIFT)
#define	GTT_PAGE_OFFSET			(GTT_PAGE_SIZE - 1)
#define	GTT_PTE_MASK			(~GTT_PAGE_OFFSET)
#define	GTT_PTE_VALID			0x1
#define	GTT_TABLE_VALID			0x1
#define	GTT_BASE_MASK			0xfffff000
#define	GTT_MB_TO_PAGES(m)		((m) << 8)
#define	GTT_POINTER_MASK		0xffffffff

/* Intel i810 register offset */
#define	I810_POINTER_MASK		0x3fffffff
#define	I810_CONF_SMRAM			0x70 /* offset in PCI config space */
#define	I810_GMS_MASK			0xc0 /* smram register mask */
/*
 *	GART and GTT entry format table
 *
 * 		AMD64 GART entry
 * 	from bios and kernel develop guide for amd64
 *	 -----------------------------
 * 	Bits		Description	|
 * 	0		valid		|
 * 	1		coherent	|
 * 	3:2		reserved	|
 * 	11:4		physaddr[39:32]	|
 * 	31:12	physaddr[31:12]	|
 * 	-----------------------------
 *		Intel GTT entry
 * 	Intel video programming manual
 * 	-----------------------------
 * 	Bits		descrition	|
 * 	0		valid		|
 * 	2:1		memory type	|
 * 	29:12		PhysAddr[29:12]	|
 * 	31:30		reserved	|
 * 	-----------------------------
 *		AGP entry
 * 	from AGP protocol 3.0
 * 	-----------------------------
 * 	Bits		descrition	|
 * 	0		valid		|
 * 	1		coherent	|
 * 	3:2		reserved	|
 * 	11:4		PhysAddr[39:32]	|
 * 	31:12	PhysAddr[31:12]		|
 * 	63:32	PhysAddr[71:40]		|
 *	 -----------------------------
 */

/*
 *	gart and gtt table base register format
 *
 *  		AMD64 register format
 * 	from bios and kernel develop guide for AMD64
 * 	---------------------------------------------
 * 	Bits			Description		|
 * 	3:0			reserved		|
 * 	31:4			physical addr 39:12	|
 * 	----------------------------------------------
 * 		INTEL AGPGART table base register format
 * 	from AGP protocol 3.0 p142, only support 32 bits
 * 	---------------------------------------------
 * 	Bits			Description		|
 * 	11:0			reserved		|
 * 	31:12		physical addr 31:12		|
 * 	63:32		physical addr 63:32		|
 * 	---------------------------------------------
 * 		INTEL i810 GTT table base register format
 * 	_____________________________________________
 * 	Bits			Description		|
 * 	0			GTT table enable bit	|
 * 	11:1			reserved		|
 * 	31:12			physical addr 31:12	|
 * 	---------------------------------------------
 */

/* Intel agp bridge specific */
#define	AGP_INTEL_POINTER_MASK		0xffffffff

/* Amd64 cpu gart device reigster offset */
#define	AMD64_APERTURE_CONTROL		0x90
#define	AMD64_APERTURE_BASE		0x94
#define	AMD64_GART_CACHE_CTL		0x9c
#define	AMD64_GART_BASE			0x98

/* Amd64 cpu gart bits */
#define	AMD64_APERBASE_SHIFT		25
#define	AMD64_APERBASE_MASK		0x00007fff
#define	AMD64_GARTBASE_SHIFT		8
#define	AMD64_GARTBASE_MASK		0xfffffff0
#define	AMD64_POINTER_MASK		0xffffffffff
#define	AMD64_INVALID_CACHE		0x1
#define	AMD64_GART_SHIFT		12
#define	AMD64_RESERVE_SHIFT		4
#define	AMD64_APERSIZE_MASK		0xe
#define	AMD64_GARTEN			0x1
#define	AMD64_DISGARTCPU		0x10
#define	AMD64_DISGARTIO			0x20
#define	AMD64_ENTRY_VALID		0x1

/* Other common routines */
#define	MB2BYTES(m)		((m) << 20)
#define	BYTES2MB(m)		((m) >> 20)
#define	GIGA_MASK		0xC0000000
#define	UI32_MASK		0xffffffffU
#define	MAXPHYSMEM		0x400000000LL
#define	MAXAPERMEGAS		0x100000L

#endif /* _KERNEL */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_AGPDEFS_H */
