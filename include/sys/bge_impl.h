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

#ifndef _SYS_BGE_IMPL_H
#define	_SYS_BGE_IMPL_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <sys/stream.h>
#include <sys/strsun.h>
#include <sys/strsubr.h>
#include <sys/stat.h>
#include <sys/pci.h>
#include <sys/note.h>
#include <sys/modctl.h>
#include <sys/crc32.h>
#ifdef	__sparcv9
#include <v9/sys/membar.h>
#endif	/* __sparcv9 */
#include <sys/kstat.h>
#include <sys/ethernet.h>
#include <sys/vlan.h>
#include <sys/errno.h>
#include <sys/dlpi.h>
#include <sys/devops.h>
#include <sys/debug.h>
#include <sys/cyclic.h>
#include <sys/conf.h>

#include <netinet/ip6.h>

#include <inet/common.h>
#include <inet/ip.h>
#include <inet/mi.h>
#include <inet/nd.h>
#include <sys/pattr.h>

#include <sys/dditypes.h>
#include <sys/ddi.h>
#include <sys/sunddi.h>

#include <sys/mac.h>

/*
 * <sys/ethernet.h> *may* already have provided the typedef ether_addr_t;
 * but of course C doesn't provide a way to check this directly.  So here
 * we rely on the fact that the symbol ETHERTYPE_AT was added to the
 * header file (as a #define, which we *can* test for) at the same time
 * as the typedef for ether_addr_t ;-!
 */
#ifndef	ETHERTYPE_AT
typedef uchar_t ether_addr_t[ETHERADDRL];
#endif	/* ETHERTYPE_AT */

/*
 * Reconfiguring the network devices requires the net_config privilege
 * in Solaris 10+.  Prior to this, root privilege is required.  In order
 * that the driver binary can run on both S10+ and earlier versions, we
 * make the decisiion as to which to use at runtime.  These declarations
 * allow for either (or both) to exist ...
 */
extern int secpolicy_net_config(const cred_t *, boolean_t);
extern int drv_priv(cred_t *);
#pragma weak    secpolicy_net_config
#pragma weak    drv_priv

#include <sys/netlb.h>			/* originally from cassini	*/
#include <sys/miiregs.h>		/* by fjlite out of intel 	*/

#include <sys/bge.h>
#include <sys/bge_hw.h>

/*
 * Compile-time feature switches ...
 */
#define	BGE_DO_PPIO		0	/* peek/poke ioctls		*/
#define	BGE_RX_SOFTINT		0	/* softint per receive ring	*/
#define	BGE_CHOOSE_SEND_METHOD	0	/* send by copying only		*/

/*
 * NOTES:
 *
 * #defines:
 *
 *	BGE_PCI_CONFIG_RNUMBER and BGE_PCI_OPREGS_RNUMBER are the
 *	register-set numbers to use for the config space registers
 *	and the operating registers respectively.  On an OBP-based
 *	machine, regset 0 refers to CONFIG space, and regset 1 will
 *	be the operating registers in MEMORY space.  If an expansion
 *	ROM is fitted, it may appear as a further register set.
 *
 *	BGE_DMA_MODE defines the mode (STREAMING/CONSISTENT) used
 *	for the data buffers.  The descriptors are always set up
 *	in CONSISTENT mode.
 *
 *	BGE_HEADROOM defines how much space we'll leave in allocated
 *	mblks before the first valid data byte.  This should be chosen
 *	to be 2 modulo 4, so that once the ethernet header (14 bytes)
 *	has been stripped off, the packet data will be 4-byte aligned.
 *	The remaining space can be used by upstream modules to prepend
 *	any headers required.
 */

#define	BGE_PCI_CONFIG_RNUMBER	0
#define	BGE_PCI_OPREGS_RNUMBER	1
#define	BGE_DMA_MODE		DDI_DMA_STREAMING
#define	BGE_HEADROOM		34

/*
 *	BGE_HALFTICK is half the period of the cyclic callback (in
 *	nanoseconds), chosen so that 0.5s <= cyclic period <= 1s.
 *	Other time values are derived as odd multiples of this value
 *	so that there's little chance of ambiguity w.r.t. which tick
 *	a timeout expires on.
 *
 *	BGE_PHY_STABLE_TIME is the period for which the contents of the
 *	PHY's status register must remain unchanging before we accept
 *	that the link has come up.  [Sometimes the link comes up, only
 *	to go down again within a short time as the autonegotiation
 *	process cycles through various options before finding the best
 *	compatible mode.  We don't want to report repeated link up/down
 *	cycles, so we wait until we think it's stable.]
 *
 *	BGE_SERDES_STABLE_TIME is the analogous value for the SerDes
 *	interface.  It's much shorter, 'cos the SerDes doesn't show
 *	these effects as much as the copper PHY.
 *
 *	BGE_LINK_SETTLE_TIME is the period during which we regard link
 *	up/down cycles as an normal event after resetting/reprogramming
 *	the PHY.  During this time, link up/down messages are sent to
 *	the log only, not the console.  At any other time, link change
 *	events are regarded as unexpected and sent to both console & log.
 *
 *	These latter two values have no theoretical justification, but
 *	are derived from observations and heuristics - the values below
 *	just seem to work quite well.
 */

#define	BGE_HALFTICK		268435456LL		/* 2**28 ns!	*/
#define	BGE_CYCLIC_PERIOD	(2*BGE_HALFTICK)	/*    ~0.5s	*/
#define	BGE_SERDES_STABLE_TIME	(3*BGE_HALFTICK)	/*    ~0.8s	*/
#define	BGE_PHY_STABLE_TIME	(11*BGE_HALFTICK)	/*    ~3.0s	*/
#define	BGE_LINK_SETTLE_TIME	(111*BGE_HALFTICK)	/*   ~30.0s	*/

/*
 * Indices used to identify the different buffer rings internally
 */
#define	BGE_STD_BUFF_RING	0
#define	BGE_JUMBO_BUFF_RING	1
#define	BGE_MINI_BUFF_RING	2

/*
 * Current implementation limits
 */
#define	BGE_BUFF_RINGS_USED	2		/* std & jumbo ring	*/
						/* for now		*/
#define	BGE_RECV_RINGS_USED	16		/* up to 16 rtn rings	*/
						/* for now		*/
#define	BGE_SEND_RINGS_USED	4		/* up to 4 tx rings	*/
						/* for now		*/
#define	BGE_HASH_TABLE_SIZE	128		/* may be 256 later	*/

/*
 * Ring/buffer size parameters
 *
 * All of the (up to) 16 TX rings & and the corresponding buffers are the
 * same size.
 *
 * Each of the (up to) 3 receive producer (aka buffer) rings is a different
 * size and has different sized buffers associated with it too.
 *
 * The (up to) 16 receive return rings have no buffers associated with them.
 * The number of slots per receive return ring must be 2048 if the mini
 * ring is enabled, otherwise it may be 1024.  See Broadcom document
 * 570X-PG102-R page 56.
 *
 * Note: only the 5700 supported external memory (and therefore the mini
 * ring); the 5702/3/4 don't.  This driver doesn't support the original
 * 5700, so we won't ever use the mini ring capability.
 */

#define	BGE_SEND_RINGS_DEFAULT		1
#define	BGE_RECV_RINGS_DEFAULT		1

#define	BGE_SEND_BUFF_SIZE_DEFAULT	1536
#define	BGE_SEND_BUFF_SIZE_JUMBO	9022
#define	BGE_SEND_SLOTS_USED	512

#define	BGE_STD_BUFF_SIZE	1536		/* 0x600		*/
#define	BGE_STD_SLOTS_USED	512

#define	BGE_JUMBO_BUFF_SIZE	9022		/* 9k			*/
#define	BGE_JUMBO_SLOTS_USED	256

#define	BGE_MINI_BUFF_SIZE	128		/* 64? 256?		*/
#define	BGE_MINI_SLOTS_USED	0		/* must be 0; see above	*/

#define	BGE_RECV_BUFF_SIZE	0
#if	BGE_MINI_SLOTS_USED > 0
#define	BGE_RECV_SLOTS_USED	2048		/* required		*/
#else
#define	BGE_RECV_SLOTS_USED	1024		/* could be 2048 anyway	*/
#endif

/*
 * PCI type. PCI-Express or PCI/PCIX
 */
#define	BGE_PCI_E	1
#define	BGE_PCI_X	2

/*
 * Pad the h/w defined status block (which can be up to 80 bytes long)
 * to a power-of-two boundary
 */
#define	BGE_STATUS_PADDING	(128 - sizeof (bge_status_t))

/*
 * On platforms which support DVMA, we can simply allocate one big piece
 * of memory for all the Tx buffers and another for the Rx buffers, and
 * then carve them up as required.  It doesn't matter if they aren't just
 * one physically contiguous piece each, because both the CPU *and* the
 * I/O device can see them *as though they were*.
 *
 * However, if only physically-addressed DMA is possible, this doesn't
 * work; we can't expect to get enough contiguously-addressed memory for
 * all the buffers of each type, so in this case we request a number of
 * smaller pieces, each still large enough for several buffers but small
 * enough to fit within "an I/O page" (e.g. 64K).
 *
 * The #define below specifies how many pieces of memory are to be used;
 * 16 has been shown to work on an i86pc architecture but this could be
 * different on other non-DVMA platforms ...
 */
#ifdef	_DMA_USES_VIRTADDR
#define	BGE_SPLIT		1		/* no split required	*/
#else
#if ((BGE_BUFF_RINGS_USED > 1) || (BGE_SEND_RINGS_USED > 1) || \
	(BGE_RECV_RINGS_USED > 1))
#define	BGE_SPLIT		128		/* split 128 ways	*/
#else
#define	BGE_SPLIT		16		/* split 16 ways	*/
#endif
#endif	/* _DMA_USES_VIRTADDR */

#define	BGE_RECV_RINGS_SPLIT	(BGE_RECV_RINGS_MAX + 1)

/*
 * MONOLITHIC allocation is a hardware debugging aid, so that a logic
 * analyser can more easily be programmed with the (single) range of
 * memory addresses that the chip will then use for DMA.
 *
 * It's incompatible with non-DVMA architectures that require BGE_SPLIT
 * to be set greater than 1.  Here, it overrides BGE_SPLIT, so the code
 * will compile correctly but will *probably* fail at runtime because it
 * simply won't be able to allocate a big enough piece of memory ...
 */
#define	BGE_MONOLITHIC	0
#if	BGE_MONOLITHIC
#undef	BGE_SPLIT
#define	BGE_SPLIT	1		/* must be 1 if MONOLITHIC	*/
#endif	/* BGE_MONOLITHIC */

/*
 * STREAMS parameters
 */
#define	BGE_IDNUM		0		/* zero seems to work	*/
#define	BGE_LOWAT		(256)
#define	BGE_HIWAT		(256*1024)


/*
 * Basic data types, for clarity in distinguishing 'numbers'
 * used for different purposes ...
 *
 * A <bge_regno_t> is a register 'address' (offset) in any one of
 * various address spaces (PCI config space, PCI memory-mapped I/O
 * register space, MII registers, etc).  None of these exceeds 64K,
 * so we could use a 16-bit representation but pointer-sized objects
 * are more "natural" in most architectures; they seem to be handled
 * more efficiently on SPARC and no worse on x86.
 *
 * BGE_REGNO_NONE represents the non-existent value in this space.
 */
typedef uintptr_t bge_regno_t;			/* register # (offset)	*/
#define	BGE_REGNO_NONE		(~(uintptr_t)0u)

/*
 * Describes one chunk of allocated DMA-able memory
 *
 * In some cases, this is a single chunk as allocated from the system;
 * but we also use this structure to represent slices carved off such
 * a chunk.  Even when we don't really need all the information, we
 * use this structure as a convenient way of correlating the various
 * ways of looking at a piece of memory (kernel VA, IO space DVMA,
 * handle+offset, etc).
 */
typedef struct {
	ddi_acc_handle_t	acc_hdl;	/* handle for memory	*/
	void			*mem_va;	/* CPU VA of memory	*/
	uint32_t		nslots;		/* number of slots	*/
	uint32_t		size;		/* size per slot	*/
	size_t			alength;	/* allocated size	*/
						/* >= product of above	*/

	ddi_dma_handle_t	dma_hdl;	/* DMA handle		*/
	offset_t		offset;		/* relative to handle	*/
	ddi_dma_cookie_t	cookie;		/* associated cookie	*/
	uint32_t		ncookies;	/* must be 1		*/
	uint32_t		token;		/* arbitrary identifier	*/
} dma_area_t;					/* 0x50 (80) bytes	*/

/*
 * Software version of the Receive Buffer Descriptor
 * There's one of these for each receive buffer (up to 256/512/1024 per ring).
 */
typedef struct sw_rbd {
	dma_area_t		pbuf;		/* (const) related	*/
						/* buffer area		*/
} sw_rbd_t;					/* 0x50 (80) bytes	*/

/*
 * Software Receive Buffer (Producer) Ring Control Block
 * There's one of these for each receiver producer ring (up to 3),
 * but each holds buffers of a different size.
 */
typedef struct buff_ring {
	dma_area_t		desc;		/* (const) related h/w	*/
						/* descriptor area	*/
	dma_area_t		buf[BGE_SPLIT];	/* (const) related	*/
						/* buffer area(s)	*/
	bge_rcb_t		hw_rcb;		/* (const) image of h/w	*/
						/* RCB, and used to	*/
	struct bge		*bgep;		/* (const) containing	*/
						/* driver soft state	*/
						/* initialise same	*/
	volatile uint16_t	*cons_index_p;	/* (const) ptr to h/w	*/
						/* "consumer index"	*/
						/* (in status block)	*/

	/*
	 * The rf_lock must be held when updating the h/w producer index
	 * mailbox register (*chip_mbox_reg), or the s/w producer index
	 * (rf_next).
	 */
	bge_regno_t		chip_mbx_reg;	/* (const) h/w producer	*/
						/* index mailbox offset	*/
	kmutex_t		rf_lock[1];	/* serialize refill	*/
	uint64_t		rf_next;	/* next slot to refill	*/
						/* ("producer index")	*/

	sw_rbd_t		*sw_rbds; 	/* software descriptors	*/
	void			*spare[4];	/* padding		*/
} buff_ring_t;					/* 0x100 (256) bytes	*/

/*
 * Software Receive (Return) Ring Control Block
 * There's one of these for each receiver return ring (up to 16).
 */
typedef struct recv_ring {
	/*
	 * The elements flagged (const) in the comments below are
	 * set up once during initialiation and thereafter unchanged.
	 */
	dma_area_t		desc;		/* (const) related h/w	*/
						/* descriptor area	*/
	bge_rcb_t		hw_rcb;		/* (const) image of h/w	*/
						/* RCB, and used to	*/
						/* initialise same	*/
	struct bge		*bgep;		/* (const) containing	*/
						/* driver soft state	*/
	ddi_softintr_t		rx_softint;	/* (const) per-ring	*/
						/* receive callback	*/
	volatile uint16_t	*prod_index_p;	/* (const) ptr to h/w	*/
						/* "producer index"	*/
						/* (in status block)	*/

	/*
	 * The rx_lock must be held when updating the h/w consumer index
	 * mailbox register (*chip_mbox_reg), or the s/w consumer index
	 * (rx_next).
	 */
	bge_regno_t		chip_mbx_reg;	/* (const) h/w consumer	*/
						/* index mailbox offset	*/
	kmutex_t		rx_lock[1];	/* serialize receive	*/
	uint64_t		rx_next;	/* next slot to examine	*/
	mac_resource_handle_t	handle;		/* per ring cookie	*/
						/* ("producer index")	*/
} recv_ring_t;					/* 0x90 (144) bytes	*/

/*
 * Software version of the Send Buffer Descriptor
 * There's one of these for each send buffer (up to 512 per ring)
 */
typedef struct sw_sbd {
	dma_area_t		desc;		/* (const) related h/w	*/
						/* descriptor area	*/
	dma_area_t		pbuf;		/* (const) related	*/
						/* buffer area		*/

	void			(*recycle)(struct sw_sbd *);
	uint64_t		flags;

	mblk_t			*mp;		/* related mblk, if any	*/
	ddi_dma_handle_t	mblk_hdl;	/* handle for same	*/
} sw_sbd_t;					/* 0xc0 (192) bytes	*/

#define	SW_SBD_FLAG_BUSY	0x0000000000000001
#define	SW_SBD_FLAG_PBUF	0x0000000000000002
#define	SW_SBD_FLAG_BIND	0x0000000000000004

/*
 * Software Send Ring Control Block
 * There's one of these for each of (up to) 16 send rings
 */
typedef struct send_ring {
	/*
	 * The elements flagged (const) in the comments below are
	 * set up once during initialiation and thereafter unchanged.
	 */
	dma_area_t		desc;		/* (const) related h/w	*/
						/* descriptor area	*/
	dma_area_t		buf[BGE_SPLIT];	/* (const) related	*/
						/* buffer area(s)	*/
	bge_rcb_t		hw_rcb;		/* (const) image of h/w	*/
						/* RCB, and used to	*/
						/* initialise same	*/
	struct bge		*bgep;		/* (const) containing	*/
						/* driver soft state	*/
	volatile uint16_t	*cons_index_p;	/* (const) ptr to h/w	*/
						/* "consumer index"	*/
						/* (in status block)	*/

	bge_regno_t		chip_mbx_reg;	/* (const) h/w producer	*/
						/* index mailbox offset	*/
	krwlock_t		tx_lock[1];	/* serialize h/w update	*/
						/* ("producer index")	*/
	uint64_t		tx_next;	/* next slot to use	*/
	uint64_t		tx_flow;	/* # concurrent sends	*/

	/*
	 * These counters/indexes are manipulated in the transmit
	 * path using atomics rather than mutexes for speed
	 */
	uint64_t		tx_free;	/* # of slots available	*/

	/*
	 * The tc_lock must be held while manipulating the s/w consumer
	 * index (tc_next).
	 */
	kmutex_t		tc_lock[1];	/* serialize recycle	*/
	uint64_t		tc_next;	/* next slot to recycle	*/
						/* ("consumer index")	*/

	sw_sbd_t		*sw_sbds; 	/* software descriptors	*/
	uint64_t		mac_resid;	/* special per resource id */
} send_ring_t;					/* 0x100 (256) bytes	*/

typedef struct {
	ether_addr_t		addr;		/* in canonical form	*/
	uint8_t			spare;
	uint8_t			set;		/* nonzero => valid	*/
} bge_mac_addr_t;

/*
 * The original 5700/01 supported only SEEPROMs.  Later chips (5702+)
 * support both SEEPROMs (using the same 2-wire CLK/DATA interface for
 * the hardware and a backwards-compatible software access method), and
 * buffered or unbuffered FLASH devices connected to the 4-wire SPI bus
 * and using a new software access method.
 *
 * The access methods for SEEPROM and Flash are generally similar, with
 * the chip handling the serialisation/deserialisation and handshaking,
 * but the registers used are different, as are a few details of the
 * protocol, and the timing, so we have to determine which (if any) is
 * fitted.
 *
 * The value UNKNOWN means just that; we haven't yet tried to determine
 * the device type.
 *
 * The value NONE can indicate either that a real and definite absence of
 * any NVmem has been detected, or that there may be NVmem but we can't
 * determine its type, perhaps because the NVconfig pins on the chip have
 * been wired up incorrectly.  In either case, access to the NVmem (if any)
 * is not supported.
 */
enum bge_nvmem_type {
	BGE_NVTYPE_NONE = -1,			/* (or indeterminable)	*/
	BGE_NVTYPE_UNKNOWN,			/* not yet checked	*/
	BGE_NVTYPE_SEEPROM,			/* BCM5700/5701 only	*/
	BGE_NVTYPE_LEGACY_SEEPROM,		/* 5702+		*/
	BGE_NVTYPE_UNBUFFERED_FLASH,		/* 5702+		*/
	BGE_NVTYPE_BUFFERED_FLASH		/* 5702+		*/
};

/*
 * Describes the characteristics of a specific chip
 *
 * Note: elements from <businfo> to <latency> are filled in by during
 * the first phase of chip initialisation (see bge_chip_cfg_init()).
 * The remaining ones are determined just after the first RESET, in
 * bge_poll_firmware().  Thereafter, the entire structure is readonly.
 */
typedef struct {
	uint32_t		asic_rev;	/* masked from MHCR	*/
	uint32_t		businfo;	/* from private reg	*/
	uint16_t		command;	/* saved during attach	*/

	uint16_t		vendor;		/* vendor-id		*/
	uint16_t		device;		/* device-id		*/
	uint16_t		subven;		/* subsystem-vendor-id	*/
	uint16_t		subdev;		/* subsystem-id		*/
	uint8_t			revision;	/* revision-id		*/
	uint8_t			clsize;		/* cache-line-size	*/
	uint8_t			latency;	/* latency-timer	*/

	uint8_t			flags;
	uint16_t		chip_label;	/* numeric part only	*/
						/* (e.g. 5703/5794/etc)	*/
	uint32_t		mbuf_base;	/* Mbuf pool parameters */
	uint32_t		mbuf_length;	/* depend on chiptype	*/
	uint32_t		pci_type;
	uint32_t		bge_dma_rwctrl;
	uint32_t		bge_mlcr_default;
	uint32_t		recv_slots;	/* receive ring size    */
	enum bge_nvmem_type	nvtype;		/* SEEPROM or Flash	*/

	uint16_t		jumbo_slots;
	uint16_t		ethmax_size;
	uint16_t		snd_buff_size;
	uint16_t		recv_jumbo_size;
	uint32_t		mbuf_hi_water;
	uint32_t		mbuf_lo_water_rmac;
	uint32_t		mbuf_lo_water_rdma;

	uint64_t		rx_rings;	/* from bge.conf	*/
	uint64_t		tx_rings;	/* from bge.conf	*/

	uint64_t		hw_mac_addr;	/* from chip register	*/
	bge_mac_addr_t		vendor_addr;	/* transform of same	*/
	boolean_t		bge_msi_enabled;	/* default to false */
} chip_id_t;

#define	CHIP_FLAG_SUPPORTED	0x80
#define	CHIP_FLAG_SERDES	0x40
#define	CHIP_FLAG_NO_CSUM	0x20
#define	CHIP_FLAG_NO_JUMBO	0x1

/*
 * Collection of physical-layer functions to:
 *	(re)initialise the physical layer
 *	update it to match software settings
 *	check for link status change
 */
typedef struct {
	void			(*phys_restart)(struct bge *, boolean_t);
	void			(*phys_update)(struct bge *);
	boolean_t		(*phys_check)(struct bge *, boolean_t);
} phys_ops_t;

/*
 * Named Data (ND) Parameter Management Structure
 */
typedef struct {
	int			ndp_info;
	int			ndp_min;
	int			ndp_max;
	int			ndp_val;
	char			*ndp_name;
} nd_param_t;					/* 0x18 (24) bytes	*/

/*
 * NDD parameter indexes, divided into:
 *
 *	read-only parameters describing the hardware's capabilities
 *	read-write parameters controlling the advertised capabilities
 *	read-only parameters describing the partner's capabilities
 *	read-only parameters describing the link state
 */
enum {
	PARAM_AUTONEG_CAP,
	PARAM_PAUSE_CAP,
	PARAM_ASYM_PAUSE_CAP,
	PARAM_1000FDX_CAP,
	PARAM_1000HDX_CAP,
	PARAM_100T4_CAP,
	PARAM_100FDX_CAP,
	PARAM_100HDX_CAP,
	PARAM_10FDX_CAP,
	PARAM_10HDX_CAP,

	PARAM_ADV_AUTONEG_CAP,
	PARAM_ADV_PAUSE_CAP,
	PARAM_ADV_ASYM_PAUSE_CAP,
	PARAM_ADV_1000FDX_CAP,
	PARAM_ADV_1000HDX_CAP,
	PARAM_ADV_100T4_CAP,
	PARAM_ADV_100FDX_CAP,
	PARAM_ADV_100HDX_CAP,
	PARAM_ADV_10FDX_CAP,
	PARAM_ADV_10HDX_CAP,

	PARAM_LP_AUTONEG_CAP,
	PARAM_LP_PAUSE_CAP,
	PARAM_LP_ASYM_PAUSE_CAP,
	PARAM_LP_1000FDX_CAP,
	PARAM_LP_1000HDX_CAP,
	PARAM_LP_100T4_CAP,
	PARAM_LP_100FDX_CAP,
	PARAM_LP_100HDX_CAP,
	PARAM_LP_10FDX_CAP,
	PARAM_LP_10HDX_CAP,

	PARAM_LINK_STATUS,
	PARAM_LINK_SPEED,
	PARAM_LINK_DUPLEX,

	PARAM_LINK_AUTONEG,
	PARAM_LINK_RX_PAUSE,
	PARAM_LINK_TX_PAUSE,

	PARAM_LOOP_MODE,
	PARAM_MSI_CNT,

	PARAM_COUNT
};

/*
 * Actual state of the BCM570x chip
 */
enum bge_chip_state {
	BGE_CHIP_FAULT = -2,			/* fault, need reset	*/
	BGE_CHIP_ERROR,				/* error, want reset	*/
	BGE_CHIP_INITIAL,			/* Initial state only	*/
	BGE_CHIP_RESET,				/* reset, need init	*/
	BGE_CHIP_STOPPED,			/* Tx/Rx stopped	*/
	BGE_CHIP_RUNNING			/* with interrupts	*/
};

enum bge_mac_state {
	BGE_MAC_STOPPED = 0,
	BGE_MAC_STARTED
};

/*
 * (Internal) return values from ioctl subroutines
 */
enum ioc_reply {
	IOC_INVAL = -1,				/* bad, NAK with EINVAL	*/
	IOC_DONE,				/* OK, reply sent	*/
	IOC_ACK,				/* OK, just send ACK	*/
	IOC_REPLY,				/* OK, just send reply	*/
	IOC_RESTART_ACK,			/* OK, restart & ACK	*/
	IOC_RESTART_REPLY			/* OK, restart & reply	*/
};

/*
 * (Internal) return values from send_msg subroutines
 */
enum send_status {
	SEND_FAIL = -1,				/* Not OK		*/
	SEND_KEEP,				/* OK, msg queued	*/
	SEND_FREE				/* OK, free msg		*/
};

/*
 * (Internal) enumeration of this driver's kstats
 */
enum {
	BGE_KSTAT_RAW = 0,
	BGE_KSTAT_STATS,
	BGE_KSTAT_PARAMS,
	BGE_KSTAT_CHIPID,
	BGE_KSTAT_DRIVER,
	BGE_KSTAT_PHYS,
	BGE_KSTAT_MII,

	BGE_KSTAT_COUNT
};

#define	BGE_MAX_RESOURCES 255

/*
 * Per-instance soft-state structure
 */
typedef struct bge {
	/*
	 * These fields are set by attach() and unchanged thereafter ...
	 */
	dev_info_t		*devinfo;	/* device instance	*/
	mac_t			*macp;		/* MAC structure	*/
	ddi_acc_handle_t	cfg_handle;	/* DDI I/O handle	*/
	ddi_acc_handle_t	io_handle;	/* DDI I/O handle	*/
	void			*io_regs;	/* mapped registers	*/
	cyclic_id_t		cyclic_id;	/* cyclic callback	*/
	ddi_softintr_t		factotum_id;	/* factotum callback	*/
	ddi_softintr_t		resched_id;	/* reschedule callback	*/

	ddi_intr_handle_t 	*htable;	/* For array of interrupts */
	int			intr_type;	/* What type of interrupt */
	int			intr_cnt;	/* # of intrs count returned */
	size_t			intr_size;	/* Size of intr array to */
						/* allocate */
	uint_t			intr_pri;	/* Interrupt priority	*/
	int			intr_cap;	/* Interrupt capabilities */
	uint32_t		progress;	/* attach tracking	*/
	uint32_t		debug;		/* per-instance debug	*/
	chip_id_t		chipid;
	const phys_ops_t	*physops;
	char			ifname[8];	/* "bge0" ... "bge999"	*/

	/*
	 * These structures describe the blocks of memory allocated during
	 * attach().  They remain unchanged thereafter, although the memory
	 * they describe is carved up into various separate regions and may
	 * therefore be described by other structures as well.
	 */
	dma_area_t		tx_desc;	/* transmit descriptors	*/
	dma_area_t		rx_desc[BGE_RECV_RINGS_SPLIT];
						/* receive descriptors	*/
	dma_area_t		tx_buff[BGE_SPLIT];
	dma_area_t		rx_buff[BGE_SPLIT];

	/*
	 * The memory described by the <dma_area> structures above
	 * is carved up into various pieces, which are described by
	 * the structures below.
	 */
	dma_area_t		statistics;	/* describes hardware	*/
						/* statistics area	*/
	dma_area_t		status_block;	/* describes hardware	*/
						/* status block		*/

	/*
	 * Runtime read-write data starts here ...
	 *
	 * 3 Buffer Rings (std/jumbo/mini)
	 * 16 Receive (Return) Rings
	 * 16 Send Rings
	 *
	 * Note: they're not necessarily all used.
	 */
	buff_ring_t		buff[BGE_BUFF_RINGS_MAX]; /*  3*0x0100	*/
	recv_ring_t		recv[BGE_RECV_RINGS_MAX]; /* 16*0x0090	*/
	send_ring_t		send[BGE_SEND_RINGS_MAX]; /* 16*0x0100	*/

	/*
	 * Locks:
	 *
	 * Each buffer ring contains its own <rf_lock> which regulates
	 *	ring refilling.
	 *
	 * Each receive (return) ring contains its own <rx_lock> which
	 *	protects the critical cyclic counters etc.
	 *
	 * Each send ring contains two locks: <tx_lock> for the send-path
	 * 	protocol data and <tc_lock> for send-buffer recycling.
	 *
	 * Finally <genlock> is a general lock, protecting most other
	 *	operational data in the state structure and chip register
	 *	accesses.  It is acquired by the interrupt handler and
	 *	most "mode-control" routines.
	 *
	 * Any of the locks can be acquired singly, but where multiple
	 * locks are acquired, they *must* be in the order:
	 *
	 *	genlock >>> rx_lock >>> rf_lock >>> tx_lock >>> tc_lock.
	 *
	 * and within any one class of lock the rings must be locked in
	 * ascending order (send[0].tc_lock >>> send[1].tc_lock), etc.
	 *
	 * Note: actually I don't believe there's any need to acquire
	 * locks on multiple rings, or even locks of all these classes
	 * concurrently; but I've set out the above order so there is a
	 * clear definition of lock hierarchy in case it's ever needed.
	 *
	 * Note: the combinations of locks that are actually held
	 * concurrently are:
	 *
	 *	genlock >>>			(bge_chip_interrupt())
	 *		rx_lock[i] >>>		(bge_receive())
	 *			rf_lock[n]	(bge_refill())
	 *		tc_lock[i]		(bge_recycle())
	 */
	kmutex_t		genlock[1];

	/*
	 * Current Ethernet address and multicast hash (bitmap) and
	 * refcount tables, protected by <genlock>
	 */
	bge_mac_addr_t		curr_addr;
	uint32_t		mcast_hash[BGE_HASH_TABLE_SIZE/32];
	uint8_t			mcast_refs[BGE_HASH_TABLE_SIZE];

	/*
	 * Link state data (protected by genlock)
	 */
	const char		*link_mode_msg;	/* describes link mode	*/
	const char		*link_down_msg;	/* reason for link DOWN	*/
	const char		*link_up_msg;	/* comment on link UP	*/

	link_state_t		link_state;

	/*
	 * Physical layer (PHY/SerDes) state data (protected by genlock)
	 */
	hrtime_t		phys_write_time; /* when last written	*/
	hrtime_t		phys_event_time; /* when status changed	*/
	hrtime_t		phys_delta_time; /* time to settle	*/

	/*
	 * Physical layer: copper only
	 */
	bge_regno_t		phy_mii_addr;	/* should be (const) 1!	*/
	uint16_t		phy_gen_status;
	uint16_t		phy_aux_status;

	/*
	 * Physical layer: serdes only
	 */
	uint32_t		serdes_status;
	uint32_t		serdes_advert;
	uint32_t		serdes_lpadv;

	/*
	 * Driver kstats, protected by <genlock> where necessary
	 */
	kstat_t			*bge_kstats[BGE_KSTAT_COUNT];

	/*
	 * Miscellaneous operating variables (protected by genlock)
	 */
	uint64_t		chip_resets;	/* # of chip RESETs	*/
	uint64_t		missed_dmas;	/* # of missed DMAs	*/
	enum bge_mac_state	bge_mac_state;	/* definitions above	*/
	enum bge_chip_state	bge_chip_state;	/* definitions above	*/
	boolean_t		send_hw_tcp_csum;
	boolean_t		recv_hw_tcp_csum;
	boolean_t		promisc;

	/*
	 * Miscellaneous operating variables (not synchronised)
	 */
	uint32_t		watchdog;	/* watches for Tx stall	*/
	boolean_t		resched_needed;
	uint32_t		factotum_flag;	/* softint pending	*/
	uintptr_t		pagemask;

	/*
	 * NDD parameters (protected by genlock)
	 */
	caddr_t			nd_data_p;
	nd_param_t		nd_params[PARAM_COUNT];

	uintptr_t		resmap[BGE_MAX_RESOURCES];

	/*
	 * Spare space, plus guard element used to check data integrity
	 */
	uint64_t		spare[5];
	uint64_t		bge_guard;

	/*
	 * Receive rules configure
	 */
	bge_recv_rule_t	recv_rules[RECV_RULES_NUM_MAX];
} bge_t;

/*
 * 'Progress' bit flags ...
 */
#define	PROGRESS_CFG		0x0001	/* config space mapped		*/
#define	PROGRESS_REGS		0x0002	/* registers mapped		*/
#define	PROGRESS_RESCHED	0x0010	/* resched softint registered	*/
#define	PROGRESS_FACTOTUM	0x0020	/* factotum softint registered	*/
#define	PROGRESS_HWINT		0x0040	/* h/w interrupt registered	*/
					/* and mutexen initialised	*/
#define	PROGRESS_INTR		0x0080	/* Intrs enabled		*/
#define	PROGRESS_PHY		0x0100	/* PHY initialised		*/
#define	PROGRESS_NDD		0x1000	/* NDD parameters set up	*/
#define	PROGRESS_KSTATS		0x2000	/* kstats created		*/
#define	PROGRESS_READY		0x8000	/* ready for work		*/

/*
 * Shorthand for the NDD parameters
 */
#define	param_adv_autoneg	nd_params[PARAM_ADV_AUTONEG_CAP].ndp_val
#define	param_adv_pause		nd_params[PARAM_ADV_PAUSE_CAP].ndp_val
#define	param_adv_asym_pause	nd_params[PARAM_ADV_ASYM_PAUSE_CAP].ndp_val
#define	param_adv_1000fdx	nd_params[PARAM_ADV_1000FDX_CAP].ndp_val
#define	param_adv_1000hdx	nd_params[PARAM_ADV_1000HDX_CAP].ndp_val
#define	param_adv_100fdx	nd_params[PARAM_ADV_100FDX_CAP].ndp_val
#define	param_adv_100hdx	nd_params[PARAM_ADV_100HDX_CAP].ndp_val
#define	param_adv_10fdx		nd_params[PARAM_ADV_10FDX_CAP].ndp_val
#define	param_adv_10hdx		nd_params[PARAM_ADV_10HDX_CAP].ndp_val

#define	param_lp_autoneg	nd_params[PARAM_LP_AUTONEG_CAP].ndp_val
#define	param_lp_pause		nd_params[PARAM_LP_PAUSE_CAP].ndp_val
#define	param_lp_asym_pause	nd_params[PARAM_LP_ASYM_PAUSE_CAP].ndp_val
#define	param_lp_1000fdx	nd_params[PARAM_LP_1000FDX_CAP].ndp_val
#define	param_lp_1000hdx	nd_params[PARAM_LP_1000HDX_CAP].ndp_val
#define	param_lp_100fdx		nd_params[PARAM_LP_100FDX_CAP].ndp_val
#define	param_lp_100hdx		nd_params[PARAM_LP_100HDX_CAP].ndp_val
#define	param_lp_10fdx		nd_params[PARAM_LP_10FDX_CAP].ndp_val
#define	param_lp_10hdx		nd_params[PARAM_LP_10HDX_CAP].ndp_val

#define	param_link_up		nd_params[PARAM_LINK_STATUS].ndp_val
#define	param_link_speed	nd_params[PARAM_LINK_SPEED].ndp_val
#define	param_link_duplex	nd_params[PARAM_LINK_DUPLEX].ndp_val

#define	param_link_autoneg	nd_params[PARAM_LINK_AUTONEG].ndp_val
#define	param_link_rx_pause	nd_params[PARAM_LINK_RX_PAUSE].ndp_val
#define	param_link_tx_pause	nd_params[PARAM_LINK_TX_PAUSE].ndp_val

#define	param_loop_mode		nd_params[PARAM_LOOP_MODE].ndp_val
#define	param_msi_cnt		nd_params[PARAM_MSI_CNT].ndp_val

/*
 * Sync a DMA area described by a dma_area_t
 */
#define	DMA_SYNC(area, flag)	((void) ddi_dma_sync((area).dma_hdl,	\
				    (area).offset, (area).alength, (flag)))

/*
 * Find the (kernel virtual) address of block of memory
 * described by a dma_area_t
 */
#define	DMA_VPTR(area)		((area).mem_va)

/*
 * Zero a block of memory described by a dma_area_t
 */
#define	DMA_ZERO(area)		bzero(DMA_VPTR(area), (area).alength)

/*
 * Next value of a cyclic index
 */
#define	NEXT(index, limit)	((index)+1 < (limit) ? (index)+1 : 0);

/*
 * Property lookups
 */
#define	BGE_PROP_EXISTS(d, n)	ddi_prop_exists(DDI_DEV_T_ANY, (d),	\
					DDI_PROP_DONTPASS, (n))
#define	BGE_PROP_GET_INT(d, n)	ddi_prop_get_int(DDI_DEV_T_ANY, (d),	\
					DDI_PROP_DONTPASS, (n), -1)

/*
 * Copy an ethernet address
 */
#define	ethaddr_copy(src, dst)	bcopy((src), (dst), ETHERADDRL)

/*
 * Endian swap
 */
/* BEGIN CSTYLED */
#define BGE_BSWAP_32(x)		((((x) & 0xff000000) >> 24)  |		\
                                 (((x) & 0x00ff0000) >> 8)   |		\
                                 (((x) & 0x0000ff00) << 8)   |		\
                                 (((x) & 0x000000ff) << 24))
/* END CSTYLED */

/*
 * Marker value placed at the end of the driver's state
 */
#define	BGE_GUARD		0x1919306009031802

/*
 * Bit flags in the 'debug' word ...
 */
#define	BGE_DBG_STOP		0x00000001	/* early debug_enter()	*/
#define	BGE_DBG_TRACE		0x00000002	/* general flow tracing	*/

#define	BGE_DBG_REGS		0x00000010	/* low-level accesses	*/
#define	BGE_DBG_MII		0x00000020	/* low-level MII access	*/
#define	BGE_DBG_SEEPROM		0x00000040	/* low-level SEEPROM IO	*/
#define	BGE_DBG_CHIP		0x00000080	/* low(ish)-level code	*/

#define	BGE_DBG_RECV		0x00000100	/* receive-side code	*/
#define	BGE_DBG_SEND		0x00000200	/* packet-send code	*/

#define	BGE_DBG_INT		0x00001000	/* interrupt handler	*/
#define	BGE_DBG_FACT		0x00002000	/* factotum (softint)	*/

#define	BGE_DBG_PHY		0x00010000	/* Copper PHY code	*/
#define	BGE_DBG_SERDES		0x00020000	/* SerDes code		*/
#define	BGE_DBG_PHYS		0x00040000	/* Physical layer code	*/
#define	BGE_DBG_LINK		0x00080000	/* Link status check	*/

#define	BGE_DBG_INIT		0x00100000	/* initialisation	*/
#define	BGE_DBG_NEMO		0x00200000	/* nemo interaction	*/
#define	BGE_DBG_ADDR		0x00400000	/* address-setting code	*/
#define	BGE_DBG_STATS		0x00800000	/* statistics		*/

#define	BGE_DBG_IOCTL		0x01000000	/* ioctl handling	*/
#define	BGE_DBG_LOOP		0x02000000	/* loopback ioctl code	*/
#define	BGE_DBG_PPIO		0x04000000	/* Peek/poke ioctls	*/
#define	BGE_DBG_BADIOC		0x08000000	/* unknown ioctls	*/

#define	BGE_DBG_MCTL		0x10000000	/* mctl (csum) code	*/
#define	BGE_DBG_NDD		0x20000000	/* NDD operations	*/

/*
 * Debugging ...
 */
#ifdef	DEBUG
#define	BGE_DEBUGGING		1
#else
#define	BGE_DEBUGGING		0
#endif	/* DEBUG */


/*
 * 'Do-if-debugging' macro.  The parameter <command> should be one or more
 * C statements (but without the *final* semicolon), which will either be
 * compiled inline or completely ignored, depending on the BGE_DEBUGGING
 * compile-time flag.
 *
 * You should get a compile-time error (at least on a DEBUG build) if
 * your statement isn't actually a statement, rather than unexpected
 * run-time behaviour caused by unintended matching of if-then-elses etc.
 *
 * Note that the BGE_DDB() macro itself can only be used as a statement,
 * not an expression, and should always be followed by a semicolon.
 */
#if	BGE_DEBUGGING
#define	BGE_DDB(command)	do {					\
					{ command; }			\
					_NOTE(CONSTANTCONDITION)	\
				} while (0)
#else 	/* BGE_DEBUGGING */
#define	BGE_DDB(command)	do {					\
					{ _NOTE(EMPTY); }		\
					_NOTE(CONSTANTCONDITION)	\
				} while (0)
#endif	/* BGE_DEBUGGING */

/*
 * 'Internal' macros used to construct the TRACE/DEBUG macros below.
 * These provide the primitive conditional-call capability required.
 * Note: the parameter <args> is a parenthesised list of the actual
 * printf-style arguments to be passed to the debug function ...
 */
#define	BGE_XDB(b, w, f, args)	BGE_DDB(if ((b) & (w)) f args)
#define	BGE_GDB(b, args)	BGE_XDB(b, bge_debug, (*bge_gdb()), args)
#define	BGE_LDB(b, args)	BGE_XDB(b, bgep->debug, (*bge_db(bgep)), args)
#define	BGE_CDB(f, args)	BGE_XDB(BGE_DBG, bgep->debug, f, args)

/*
 * Conditional-print macros.
 *
 * Define BGE_DBG to be the relevant member of the set of BGE_DBG_* values
 * above before using the BGE_GDEBUG() or BGE_DEBUG() macros.  The 'G'
 * versions look at the Global debug flag word (bge_debug); the non-G
 * versions look in the per-instance data (bgep->debug) and so require a
 * variable called 'bgep' to be in scope (and initialised!) before use.
 *
 * You could redefine BGE_TRC too if you really need two different
 * flavours of debugging output in the same area of code, but I don't
 * really recommend it.
 *
 * Note: the parameter <args> is a parenthesised list of the actual
 * arguments to be passed to the debug function, usually a printf-style
 * format string and corresponding values to be formatted.
 */

#define	BGE_TRC			BGE_DBG_TRACE	/* default 'trace' bit	*/
#define	BGE_GTRACE(args)	BGE_GDB(BGE_TRC, args)
#define	BGE_GDEBUG(args)	BGE_GDB(BGE_DBG, args)
#define	BGE_TRACE(args)		BGE_LDB(BGE_TRC, args)
#define	BGE_DEBUG(args)		BGE_LDB(BGE_DBG, args)

/*
 * Debug-only action macros
 */
#define	BGE_BRKPT(bgep, s)	BGE_DDB(bge_dbg_enter(bgep, s))
#define	BGE_MARK(bgep)		BGE_DDB(bge_led_mark(bgep))
#define	BGE_PCICHK(bgep)	BGE_DDB(bge_pci_check(bgep))
#define	BGE_PKTDUMP(args)	BGE_DDB(bge_pkt_dump args)
#define	BGE_REPORT(args)	BGE_DDB(bge_log args)

/*
 * Inter-source-file linkage ...
 */

/* bge_chip.c */
uint16_t bge_mii_get16(bge_t *bgep, bge_regno_t regno);
void bge_mii_put16(bge_t *bgep, bge_regno_t regno, uint16_t value);
uint32_t bge_reg_get32(bge_t *bgep, bge_regno_t regno);
void bge_reg_put32(bge_t *bgep, bge_regno_t regno, uint32_t value);
void bge_reg_set32(bge_t *bgep, bge_regno_t regno, uint32_t bits);
void bge_reg_clr32(bge_t *bgep, bge_regno_t regno, uint32_t bits);
void bge_mbx_put(bge_t *bgep, bge_regno_t regno, uint64_t value);
void bge_chip_cfg_init(bge_t *bgep, chip_id_t *cidp, boolean_t enable_dma);
void bge_chip_id_init(bge_t *bgep);
void bge_chip_reset(bge_t *bgep, boolean_t enable_dma);
void bge_chip_start(bge_t *bgep, boolean_t reset_phy);
void bge_chip_stop(bge_t *bgep, boolean_t fault);
void bge_chip_sync(bge_t *bgep);
void bge_chip_blank(void *arg, time_t ticks, uint_t count);
uint_t bge_chip_factotum(caddr_t arg);
void bge_chip_cyclic(void *arg);
enum ioc_reply bge_chip_ioctl(bge_t *bgep, queue_t *wq, mblk_t *mp,
	struct iocblk *iocp);
uint_t bge_intr(caddr_t arg);
extern uint32_t bge_rx_ticks_norm;
extern uint32_t bge_tx_ticks_norm;
extern uint32_t bge_rx_count_norm;
extern uint32_t bge_tx_count_norm;
void   bge_chip_msi_trig(bge_t *bgep);

/* bge_kstats.c */
void bge_init_kstats(bge_t *bgep, int instance);
void bge_fini_kstats(bge_t *bgep);
uint64_t bge_m_stat(void *arg, enum mac_stat stat);

/* bge_log.c */
#if	BGE_DEBUGGING
void (*bge_db(bge_t *bgep))(const char *fmt, ...);
void (*bge_gdb(void))(const char *fmt, ...);
void bge_pkt_dump(bge_t *bgep, bge_rbd_t *hbp, sw_rbd_t *sdp, const char *msg);
void bge_dbg_enter(bge_t *bgep, const char *msg);
#endif	/* BGE_DEBUGGING */
void bge_problem(bge_t *bgep, const char *fmt, ...);
void bge_notice(bge_t *bgep, const char *fmt, ...);
void bge_log(bge_t *bgep, const char *fmt, ...);
void bge_error(bge_t *bgep, const char *fmt, ...);
extern kmutex_t bge_log_mutex[1];
extern uint32_t bge_debug;

/* bge_main.c */
void bge_restart(bge_t *bgep, boolean_t reset_phy);

/* bge_phys.c */
void bge_phys_init(bge_t *bgep);
void bge_phys_reset(bge_t *bgep);
void bge_phys_idle(bge_t *bgep);
void bge_phys_update(bge_t *bgep);
boolean_t bge_phys_check(bge_t *bgep);

/* bge_ndd.c */
int bge_nd_init(bge_t *bgep);
enum ioc_reply bge_nd_ioctl(bge_t *bgep, queue_t *wq, mblk_t *mp,
	struct iocblk *iocp);
void bge_nd_cleanup(bge_t *bgep);

/* bge_recv.c */
void bge_receive(bge_t *bgep, bge_status_t *bsp);

/* bge_send.c */
mblk_t *bge_m_tx(void *arg, mblk_t *mp);
void bge_recycle(bge_t *bgep, bge_status_t *bsp);
uint_t bge_reschedule(caddr_t arg);

/* bge_atomic_sparc.s/bge_atomic_intel.c */
uint64_t bge_atomic_reserve(uint64_t *count_p, uint64_t n);
void bge_atomic_renounce(uint64_t *count_p, uint64_t n);
uint64_t bge_atomic_claim(uint64_t *count_p, uint64_t limit);
uint64_t bge_atomic_clr64(uint64_t *sp, uint64_t bits);
uint32_t bge_atomic_shl32(uint32_t *sp, uint_t count);

#ifdef __cplusplus
}
#endif

#endif	/* _SYS_BGE_IMPL_H */
