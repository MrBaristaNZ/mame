// license:BSD-3-Clause
// copyright-holders:MrBaristaNZ
/**************************************************************************************************

    PLX PCI9054 PCI I/O accelerator, as a PCI host bridge in direct-master mode

**************************************************************************************************/

#ifndef MAME_MACHINE_PCI9054_H
#define MAME_MACHINE_PCI9054_H

#pragma once

#include "machine/pci.h"

class pci9054_device : public pci_host_device
{
public:
	pci9054_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

	// local bus side: configuration registers, and the direct-master memory and I/O / configuration windows
	// (window offsets are relative to the window base, the local address decode is done by the board)
	u32 local_r(offs_t offset, u32 mem_mask = ~0);
	void local_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	u32 dm_mem_r(offs_t offset, u32 mem_mask = ~0);
	void dm_mem_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	u32 dm_io_r(offs_t offset, u32 mem_mask = ~0);
	void dm_io_w(offs_t offset, u32 data, u32 mem_mask = ~0);

	virtual void config_map(address_map &map) override ATTR_COLD;
	virtual u8 header_type_r() override { return pci_device::header_type_r(); }

protected:
	enum
	{
		AS_PCI_MEMORY = 1,
		AS_PCI_IO = 2
	};

	virtual void device_start() override ATTR_COLD;
	virtual void device_reset() override ATTR_COLD;

	virtual space_config_vector memory_space_config() const override;

private:
	address_space_config m_configuration_config;
	address_space_config m_memory_config;
	address_space_config m_io_config;

	u32 m_regs[0x200 / 4];

	void pcisr_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	bool config_cycle(u8 &bus, u8 &devfn, u16 &reg);
	offs_t dm_address(offs_t offset, bool io) const;
};

DECLARE_DEVICE_TYPE(PCI9054, pci9054_device)

#endif // MAME_MACHINE_PCI9054_H
