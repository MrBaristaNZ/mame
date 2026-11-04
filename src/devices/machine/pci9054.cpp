// license:BSD-3-Clause
// copyright-holders:MrBaristaNZ
/**************************************************************************************************

    PLX PCI9054 PCI I/O accelerator

    Modelled as the host bridge of a local bus CPU that reaches PCI through the direct-master
    windows (as on the Aristocrat MK6, an SH-4 board):
    - local configuration registers (local offsets 0x000-0x1ff); the PCI configuration registers
      are shadowed at local 0x00-0x3f
    - direct-master memory window: local address -> PCI memory, remapped by DMRR/DMPBAM
    - direct-master I/O window: PCI I/O, or a PCI configuration cycle when DMCFGA bit 31 is set;
      a configuration cycle nobody answers sets PCISR "received master abort"

    The local base addresses (DMLBAM/DMLBAI) are stored but not decoded: the board maps the
    windows, and the handlers receive window-relative offsets.

    Reference: PLX PCI 9054 Data Book, version 1.0 (1998)

    TODO:
    - DMA channels (logged only)
    - mailbox / doorbell / INTCSR interrupts (stored only)
    - direct slave (PCI-to-local) accesses and the PCIBAR0-3 apertures
    - serial EEPROM loading of the configuration registers

**************************************************************************************************/

#include "emu.h"
#include "pci9054.h"

#define LOG_REGS    (1U << 1)
#define LOG_WINDOW  (1U << 2)

#define VERBOSE (0)
#include "logmacro.h"

#define LOGREGS(...)    LOGMASKED(LOG_REGS, __VA_ARGS__)
#define LOGWINDOW(...)  LOGMASKED(LOG_WINDOW, __VA_ARGS__)


DEFINE_DEVICE_TYPE(PCI9054, pci9054_device, "pci9054", "PLX PCI9054")

namespace {

// local offsets of the registers this device decodes
enum : offs_t
{
	PCIIDR   = 0x000 / 4,
	PCICR    = 0x004 / 4,   // PCICR (low half) / PCISR (high half)
	PCIREV   = 0x008 / 4,
	PCISVID  = 0x02c / 4,
	DMRR     = 0x09c / 4,
	DMLBAM   = 0x0a0 / 4,
	DMLBAI   = 0x0a4 / 4,
	DMPBAM   = 0x0a8 / 4,
	DMCFGA   = 0x0ac / 4,
	DMACSR   = 0x128 / 4    // DMACSR0 (bits 7-0) / DMACSR1 (bits 15-8)
};

// PCISR bits that are write-1-to-clear: parity error, target aborts, master abort, system error
constexpr u16 PCISR_W1C = 0xf900;
constexpr u16 PCISR_RECEIVED_MASTER_ABORT = 0x2000;

} // anonymous namespace


pci9054_device::pci9054_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: pci_host_device(mconfig, PCI9054, tag, owner, clock)
	, m_configuration_config("configuration_space", ENDIANNESS_LITTLE, 32, 20)
	, m_memory_config("memory_space", ENDIANNESS_LITTLE, 32, 32)
	, m_io_config("io_space", ENDIANNESS_LITTLE, 32, 32)
{
	// power-on defaults without a serial EEPROM; revision 0x0b = AB silicon
	set_ids(0x10b59054, 0x0b, 0x068000, 0x10b59054);
}

device_memory_interface::space_config_vector pci9054_device::memory_space_config() const
{
	return space_config_vector{
		std::make_pair(AS_PCI_CONFIG, &m_configuration_config),
		std::make_pair(AS_PCI_MEMORY, &m_memory_config),
		std::make_pair(AS_PCI_IO, &m_io_config)
	};
}

void pci9054_device::device_start()
{
	pci_host_device::device_start();
	set_spaces(&space(AS_PCI_MEMORY), &space(AS_PCI_IO));

	// the whole of both PCI spaces belongs to the bus
	memory_window_start = 0;
	memory_window_end = 0xffffffff;
	memory_offset = 0;
	io_window_start = 0;
	io_window_end = 0xffffffff;
	io_offset = 0;

	save_item(NAME(m_regs));
}

void pci9054_device::device_reset()
{
	pci_host_device::device_reset();

	std::fill(std::begin(m_regs), std::end(m_regs), 0);
	m_regs[DMACSR] = 0x00001010;   // both channels done
	command = 0x0000;
	status = 0x0290;               // capabilities list, fast back-to-back capable, medium DEVSEL#
}

// the 9054 is a type 0 device on its own bus, not a PCI-to-PCI bridge
void pci9054_device::config_map(address_map &map)
{
	pci_device::config_map(map);
	map(0x06, 0x07).w(FUNC(pci9054_device::pcisr_w));
}

void pci9054_device::pcisr_w(offs_t offset, u16 data, u16 mem_mask)
{
	status &= ~(data & mem_mask & PCISR_W1C);
}


/**************************************************************************************************
    Local configuration registers
**************************************************************************************************/

u32 pci9054_device::local_r(offs_t offset, u32 mem_mask)
{
	offset &= 0x1ff / 4;
	switch (offset)
	{
	case PCIIDR:
		return (main_id << 16) | (main_id >> 16);
	case PCICR:
		return (u32(status) << 16) | command;
	case PCIREV:
		return class_rev_r();
	case PCISVID:
		return (subsystem_id << 16) | (subsystem_id >> 16);
	default:
		LOGREGS("%s: local read %03x & %08x = %08x\n", machine().describe_context(), offset * 4, mem_mask, m_regs[offset]);
		return m_regs[offset];
	}
}

void pci9054_device::local_w(offs_t offset, u32 data, u32 mem_mask)
{
	offset &= 0x1ff / 4;
	switch (offset)
	{
	case PCIIDR:
	case PCIREV:
	case PCISVID:
		logerror("%s: write to read-only ID register %03x = %08x & %08x\n", machine().describe_context(), offset * 4, data, mem_mask);
		break;

	case PCICR:
		if (ACCESSING_BITS_0_15)
			command_w(0, data, mem_mask);
		if (ACCESSING_BITS_16_31)
			pcisr_w(0, data >> 16, mem_mask >> 16);
		break;

	case DMRR:
	case DMLBAM:
	case DMLBAI:
		mem_mask &= 0xffff0000;   // bits 15-0 reserved, read as 0
		LOGREGS("%s: local write %03x = %08x & %08x\n", machine().describe_context(), offset * 4, data, mem_mask);
		COMBINE_DATA(&m_regs[offset]);
		break;

	case DMCFGA:
		mem_mask &= 0x80ffffff;   // bits 30-24 reserved, read as 0
		LOGREGS("%s: local write %03x = %08x & %08x\n", machine().describe_context(), offset * 4, data, mem_mask);
		COMBINE_DATA(&m_regs[offset]);
		break;

	case DMACSR:
		COMBINE_DATA(&m_regs[offset]);
		if ((data & mem_mask) & 0x00000202)
			logerror("%s: DMA start %08x not emulated (DMAMODE0 %08x, DMAMODE1 %08x)\n", machine().describe_context(), data & mem_mask, m_regs[0x100 / 4], m_regs[0x114 / 4]);
		m_regs[offset] = (m_regs[offset] & ~0x00000e0e) | 0x00001010;   // start/abort/clear self-clear, channels stay done
		break;

	default:
		LOGREGS("%s: local write %03x = %08x & %08x\n", machine().describe_context(), offset * 4, data, mem_mask);
		COMBINE_DATA(&m_regs[offset]);
		break;
	}
}


/**************************************************************************************************
    Direct-master windows
**************************************************************************************************/

offs_t pci9054_device::dm_address(offs_t offset, bool io) const
{
	// DMRR selects the local address bits replaced by the DMPBAM remap bits
	const u32 range = m_regs[DMRR] & 0xffff0000;
	const u32 address = (offset << 2) & ~range;
	if (io && BIT(m_regs[DMPBAM], 13))
		return address & 0xffff;   // I/O remap select: PCI address bits 31-16 forced to 0
	return (m_regs[DMPBAM] & range) | address;
}

u32 pci9054_device::dm_mem_r(offs_t offset, u32 mem_mask)
{
	if (!BIT(m_regs[DMPBAM], 0))
	{
		logerror("%s: direct-master memory read %08x with memory access disabled\n", machine().describe_context(), offset << 2);
		return 0;
	}
	return space(AS_PCI_MEMORY).read_dword(dm_address(offset, false), mem_mask);
}

void pci9054_device::dm_mem_w(offs_t offset, u32 data, u32 mem_mask)
{
	if (!BIT(m_regs[DMPBAM], 0))
	{
		logerror("%s: direct-master memory write %08x = %08x with memory access disabled\n", machine().describe_context(), offset << 2, data);
		return;
	}
	space(AS_PCI_MEMORY).write_dword(dm_address(offset, false), data, mem_mask);
}

// decode DMCFGA for a configuration cycle; false (and master abort) if no device answers
bool pci9054_device::config_cycle(u8 &bus, u8 &devfn, u16 &reg)
{
	const u32 dmcfga = m_regs[DMCFGA];
	const bool type1 = BIT(dmcfga, 0);
	bus = type1 ? BIT(dmcfga, 16, 8) : 0;
	devfn = BIT(dmcfga, 8, 8);
	reg = dmcfga & 0xfc;

	// type 0 cycles reach the devices on this bus; the 9054 does not decode its own
	const bool claimed = bus ? !all_bridges.empty() : (sub_devices[devfn] && sub_devices[devfn] != this);
	if (!claimed)
	{
		LOGWINDOW("%s: configuration cycle %02x:%02x.%x:%02x master abort\n", machine().describe_context(), bus, devfn >> 3, devfn & 7, reg);
		status |= PCISR_RECEIVED_MASTER_ABORT;
	}
	return claimed;
}

u32 pci9054_device::dm_io_r(offs_t offset, u32 mem_mask)
{
	if (!BIT(m_regs[DMPBAM], 1))
	{
		logerror("%s: direct-master I/O read %08x with I/O access disabled\n", machine().describe_context(), offset << 2);
		return 0;
	}

	if (BIT(m_regs[DMCFGA], 31))
	{
		u8 bus, devfn;
		u16 reg;
		if (!config_cycle(bus, devfn, reg))
			return 0xffffffff;
		return root_config_read(bus, devfn, reg, mem_mask);
	}

	return space(AS_PCI_IO).read_dword(dm_address(offset, true), mem_mask);
}

void pci9054_device::dm_io_w(offs_t offset, u32 data, u32 mem_mask)
{
	if (!BIT(m_regs[DMPBAM], 1))
	{
		logerror("%s: direct-master I/O write %08x = %08x with I/O access disabled\n", machine().describe_context(), offset << 2, data);
		return;
	}

	if (BIT(m_regs[DMCFGA], 31))
	{
		u8 bus, devfn;
		u16 reg;
		if (config_cycle(bus, devfn, reg))
			root_config_write(bus, devfn, reg, data, mem_mask);
		return;
	}

	space(AS_PCI_IO).write_dword(dm_address(offset, true), data, mem_mask);
}
