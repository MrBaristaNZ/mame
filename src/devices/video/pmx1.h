// license:BSD-3-Clause
// copyright-holders:MrBaristaNZ
/**************************************************************************************************

    NEC/VideoLogic PowerVR PMX1 (PowerVR Series 2 "Highlander", PCI 1033:0067) core

    The chip without a bus face or a video output: the board or card that contains it provides the
    PCI/AGP function, the display timing and the screen.

**************************************************************************************************/

#ifndef MAME_VIDEO_PMX1_H
#define MAME_VIDEO_PMX1_H

#pragma once


class pmx1_device : public device_t
{
public:
	// HLE: VRAM offsets of the asynchronous ring control block and ring, as laid out by the one
	// microcode revision seen so far (Aristocrat MK6 BIOS us1 and setchip 06.03.04)
	static constexpr u32 HLE_ASYNC_CTRL = 0x25d000;
	static constexpr u32 HLE_ASYNC_RING = 0x25e000;

	pmx1_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock = 0);

	// interrupt output (INTA# on PCI)
	auto irq_cb() { return m_irq_cb.bind(); }

	// HLE: the microcode stand-in reads host memory through these. host_read32 reads one dword at a
	// host address; ring_locate returns the host address where the host software keeps the
	// asynchronous ring write offset, or 0 while it is not known yet. Both are host-software specific.
	auto host_read32() { return m_host_read32.bind(); }
	auto ring_locate() { return m_ring_locate.bind(); }

	// HLE: host address of VRAM as the host software writes it into command blocks, and the mask
	// applied to such addresses first (e.g. to drop SH-4 cache area bits)
	void set_host_vram_address(u32 base, u32 mask) { m_host_base = base; m_host_mask = mask; }

	// EXPERIMENT (dual PMX1), HLE: VRAM address on the chip's own bus (PCI BAR0), as the microcode sees it in
	// ring packets; 0 = same as the host address. The owner updates it when the BAR moves.
	void set_bus_vram_address(u32 base) { m_bus_base = base; }

	void vram_map(address_map &map) ATTR_COLD;
	void io_map(address_map &map) ATTR_COLD;

	u32 *vram() const { return m_vram.target(); }
	u32 vram_bytes() const { return m_vram.bytes(); }

	void vblank_w(int state);
	u32 screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

protected:
	// device_t implementation
	virtual void device_start() override ATTR_COLD;
	virtual void device_reset() override ATTR_COLD;

private:
	memory_share_creator<u32> m_vram;
	devcb_write_line m_irq_cb;
	devcb_read32 m_host_read32;
	devcb_read32 m_ring_locate;
	u32 m_host_base;
	u32 m_host_mask;
	u32 m_bus_base;

	u32 m_regs[0x100 / 4];          // I/O register block
	u16 m_idx[0x400];               // indexed registers behind I/O 0xc0/0xc8
	u32 m_idx_status;               // value read back at I/O 0xc8
	u32 m_display_base;             // VRAM offset of the displayed framebuffer
	u32 m_cif_bg0;                  // host address of BG0Data.CIFCtl, learned from the boot handshake
	u32 m_ring_woff;                // host address of the asynchronous ring write offset

	void start_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	void irq_ack_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	void irq_mask_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	void bg0_doorbell_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	void index_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	void update_irq();

	// high-level stand-in for the embedded microcode processor
	u32 vram_offset(u32 addr) const;
	u32 mem_r(u32 addr) const;
	void mem_w(u32 addr, u32 data);
	void start_microcode();
	void doorbell();
	void process_ring();
	void blit(u32 src, int sx, int sy, int spitch, u32 dst, int dx, int dy, int dpitch, int w, int h, bool key0, int fade = -1);
	void fill(u32 dst, int dx, int dy, int dpitch, int w, int h, u32 color);
	void text(u32 pkt);
	void line(u32 dst, int pitch, int x0, int y0, int x1, int y1, int clipw, int cliph, u32 color);
};

DECLARE_DEVICE_TYPE(PMX1, pmx1_device)

#endif // MAME_VIDEO_PMX1_H
