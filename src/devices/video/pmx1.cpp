// license:BSD-3-Clause
// copyright-holders:MrBaristaNZ
/**************************************************************************************************

    NEC/VideoLogic PowerVR PMX1 (PowerVR Series 2 "Highlander", PCI 1033:0067) core

    Blocks of the chip: primary core, 2D core, TA/ISP/TSP 3D, cache/SDRAM controller, host interface
    and DAC feed. Cards add their own bus function and output: the Neon 250 has a VGA emulator, AGP and
    power management, the Aristocrat MK6 a plain PCI function. This device is the shared part: VRAM,
    the I/O register block (BAR4 on PCI), the interrupt output and the pixels the DAC is fed with. It
    does not own a screen; its owner calls vblank_w and screen_update.

    I/O registers, as used by the Aristocrat MK6 BIOS (pmx1.c) and matching gaelco/atvtrack.cpp:
    0x00       start the embedded microcode processor (1)
    0x40/0x44  boot code address - 4, low / high 16 bits
    0x70       interrupt pending, write 1 to acknowledge (bit 2 = vertical blank)
    0x74       interrupt mask
    0xa4       control (0x4000 = reset)
    0xb0/0xb4/0xbc  doorbells for the BG0 / VIN / DAC command channels (0x8001 = command queued)
    0xc0       indexed register port: (index | 0x4400) << 16 | value = write, (index | 0x400) << 16 = read
    0xc8       indexed register status: index << 16 | value of the last access

    TODO:
    - everything is a high-level stand-in for the microcode (see below): no 2D core, no 3D, no real
      display controller;
    - VRAM size is fixed at 32MB (the size the MK6 BIOS gives BAR0);
    - neon250 (bus/pci/neon250.cpp) does not use this core yet.

**************************************************************************************************/

#include "emu.h"
#include "pmx1.h"

#include "screen.h"

#include <algorithm>
#include <cstdlib>


DEFINE_DEVICE_TYPE(PMX1, pmx1_device, "pmx1", "NEC PowerVR PMX1")

pmx1_device::pmx1_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: device_t(mconfig, PMX1, tag, owner, clock)
	, m_vram(*this, "vram", 32 * 1024 * 1024, ENDIANNESS_LITTLE)
	, m_irq_cb(*this)
	, m_host_read32(*this, 0)
	, m_ring_locate(*this, 0)
	, m_host_base(0)
	, m_host_mask(0xffffffff)
{
}

void pmx1_device::device_start()
{
	save_item(NAME(m_regs));
	save_item(NAME(m_idx));
	save_item(NAME(m_idx_status));
	save_item(NAME(m_display_base));
	save_item(NAME(m_cif_bg0));
	save_item(NAME(m_ring_woff));
}

void pmx1_device::device_reset()
{
	std::fill(std::begin(m_regs), std::end(m_regs), 0);
	std::fill(std::begin(m_idx), std::end(m_idx), 0);
	m_idx_status = 0;
	m_display_base = 0;
	m_cif_bg0 = 0;
	m_ring_woff = 0;
}

void pmx1_device::vram_map(address_map &map)
{
	map(0x00000000, m_vram.bytes() - 1).ram().share(m_vram);
}

void pmx1_device::io_map(address_map &map)
{
	// registers without a modelled function are a plain register file
	map(0x00, 0xff).lrw32(
			NAME([this] (offs_t offset) -> u32 { return m_regs[offset]; }),
			NAME([this] (offs_t offset, u32 data, u32 mem_mask) { COMBINE_DATA(&m_regs[offset]); }));
	map(0x00, 0x03).w(FUNC(pmx1_device::start_w));
	map(0x70, 0x73).w(FUNC(pmx1_device::irq_ack_w));
	map(0x74, 0x77).w(FUNC(pmx1_device::irq_mask_w));
	map(0xb0, 0xb3).w(FUNC(pmx1_device::bg0_doorbell_w));
	map(0xc0, 0xc3).w(FUNC(pmx1_device::index_w));
	map(0xc8, 0xcb).lr32(NAME([this] () -> u32 { return m_idx_status; }));
}

void pmx1_device::start_w(offs_t offset, u32 data, u32 mem_mask)
{
	COMBINE_DATA(&m_regs[0x00 / 4]);
	if (m_regs[0x00 / 4] & 0xffff)
		start_microcode();
}

void pmx1_device::irq_ack_w(offs_t offset, u32 data, u32 mem_mask)
{
	// write 1 to clear
	m_regs[0x70 / 4] &= ~(data & mem_mask);
	update_irq();
}

void pmx1_device::irq_mask_w(offs_t offset, u32 data, u32 mem_mask)
{
	COMBINE_DATA(&m_regs[0x74 / 4]);
	update_irq();
}

void pmx1_device::bg0_doorbell_w(offs_t offset, u32 data, u32 mem_mask)
{
	COMBINE_DATA(&m_regs[0xb0 / 4]);
	if (m_regs[0xb0 / 4] == 0x8001)
		doorbell();             // synchronous command queued on the BG0 channel
	else
		process_ring();         // asynchronous ring kick
}

void pmx1_device::index_w(offs_t offset, u32 data, u32 mem_mask)
{
	COMBINE_DATA(&m_regs[0xc0 / 4]);
	const u32 value = m_regs[0xc0 / 4];
	const u32 index = (value >> 16) & 0x3ff;
	if (BIT(value, 30))   // write
		m_idx[index] = value & 0xffff;
	m_idx_status = (index << 16) | m_idx[index];
}

// called on every change of the pending or mask bits, not only when the result changes
void pmx1_device::update_irq()
{
	m_irq_cb((m_regs[0x70 / 4] & ~m_regs[0x74 / 4] & 0xffff) != 0);
}

void pmx1_device::vblank_w(int state)
{
	if (state)
	{
		process_ring();
		m_regs[0x70 / 4] |= 0x04;
		update_irq();
	}
}

// HLE: 640x480, 32 bpp (xRGB), pitch 640 pixels, as set up by the MK6 BIOS; no display controller registers
u32 pmx1_device::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	const u32 *const vram = m_vram.target();
	const u32 words = m_vram.bytes() / 4;
	for (int y = cliprect.top(); y <= cliprect.bottom(); y++)
	{
		const u32 line = m_display_base / 4 + y * 640;
		u32 *const dst = &bitmap.pix(y);
		for (int x = cliprect.left(); x <= cliprect.right(); x++)
			dst[x] = (line + x < words) ? (vram[line + x] & 0xffffff) : 0;
	}

	return 0;
}


/*
    High-level stand-in for the embedded microcode processor (threads BG0, VIN, TV, DAC). The microcode
    itself is not emulated, only its contract with the host software, as seen with the Aristocrat MK6
    BIOS (pmx1.c). Addresses in command blocks are host addresses (see set_host_vram_address).

    Boot: the host writes 0x55aa to BG0Data.CIFCtl[0] in VRAM, starts the processor and waits for 1.
    Commands: each channel control block holds a linked list of ComIF blocks in VRAM
      +0x00 microcode routine   +0x04 completion word (host 0, microcode 1)
      +0x18 next (PMX address)  +0x1c next (host address)   +0x20 BG0.DeleteComIF: block to unlink
    A delete command is the block itself; the microcode unlinks it from the list.
    Asynchronous ring: packets (word 0 = size rounded up to 64 bytes, word 1 = microcode routine).
    Control block in VRAM: +0x00 read offset, +0x80/+0x84 blit retire counters, +0x88 flip "shown"
    counter (checked by the vblank ISR before it retires a flip), +0x8c fence flag.
    The host keeps the ring write offset in its own memory; the owner says where (ring_locate).

    TODO: ring and routine addresses are fixed: they match the one microcode revision seen so far (MK6
    BIOS us1 and setchip 06.03.04). Derive them from the microcode symbol tables for other revisions.
*/

namespace {

// HLE: microcode routine entry points as VRAM offsets, for the one microcode revision seen so far
constexpr u32 ROUTINE_FLIP = 0x278ff0;
constexpr u32 ROUTINE_BLTA = 0x27b624;   // opaque blit
constexpr u32 ROUTINE_BLTB = 0x279110;   // blit with per-pixel alpha; if word 2 bit 1 is set, top byte = fade
constexpr u32 ROUTINE_FILL = 0x278534;   // solid rectangle
constexpr u32 ROUTINE_TEXT = 0x273454;   // string of 1bpp glyphs
constexpr u32 ROUTINE_LINE = 0x276d58;   // one pixel wide line
constexpr u32 ROUTINE_COPY = 0x273770;   // word 2 = count, then (source, destination, bytes)
constexpr u32 ROUTINE_SYNC = 0x271c00;   // writes word 3 to address word 2; later words are stale ring data

} // anonymous namespace

u32 pmx1_device::mem_r(u32 addr) const
{
	const u32 offset = (addr & m_host_mask) - m_host_base;
	return (offset < m_vram.bytes()) ? m_vram[offset / 4] : 0;
}

void pmx1_device::mem_w(u32 addr, u32 data)
{
	const u32 offset = (addr & m_host_mask) - m_host_base;
	if (offset < m_vram.bytes())
		m_vram[offset / 4] = data;
}

void pmx1_device::start_microcode()
{
	// the handshake word sits in the microcode data area at the top of VRAM
	const u32 *const vram = m_vram.target();
	for (int i = m_vram.bytes() / 4 - 1; i >= 0; i--)
	{
		if (vram[i] == 0x000055aa)
		{
			m_cif_bg0 = m_host_base + i * 4;
			m_ring_woff = 0;
			mem_w(m_cif_bg0, 1);
			logerror("microcode started, BG0 control block at %08x\n", m_cif_bg0);
			return;
		}
	}
	logerror("microcode started, handshake word not found\n");
}

void pmx1_device::doorbell()
{
	if (m_cif_bg0 == 0)
		return;

	u32 prev = 0;
	u32 cif = mem_r(m_cif_bg0 + 4);
	for (int count = 0; cif != 0 && count < 256; count++)
	{
		const u32 next = mem_r(cif + 0x1c);
		if (mem_r(cif + 0x00) != 0 && mem_r(cif + 0x04) == 0)
		{
			if (prev != 0 && (mem_r(cif + 0x20) & m_host_mask) == (cif & m_host_mask))
			{
				// BG0.DeleteComIF
				mem_w(prev + 0x1c, next);
				mem_w(prev + 0x18, mem_r(cif + 0x18));
				mem_w(cif + 0x20, 0);
				mem_w(cif + 0x04, 1);
				cif = next;
				continue;
			}
			mem_w(cif + 0x04, 1);
		}
		prev = cif;
		cif = next;
	}
}

void pmx1_device::blit(u32 src, int sx, int sy, int spitch, u32 dst, int dx, int dy, int dpitch, int w, int h, bool key0, int fade)
{
	const u32 bytes = m_vram.bytes();
	const u32 soffs = (src & m_host_mask) - m_host_base + (u32(sy) * u32(spitch) + u32(sx)) * 4;
	const u32 doffs = (dst & m_host_mask) - m_host_base + (u32(dy) * u32(dpitch) + u32(dx)) * 4;
	if (w <= 0 || h <= 0 || spitch <= 0 || dpitch <= 0)
		return;
	if (u64(soffs) + (u64(h - 1) * spitch + w) * 4 > bytes || u64(doffs) + (u64(h - 1) * dpitch + w) * 4 > bytes)
	{
		logerror("blit out of range src %08x dst %08x %dx%d\n", src, dst, w, h);
		return;
	}

	const u32 *s = m_vram.target() + soffs / 4;
	u32 *d = m_vram.target() + doffs / 4;
	for (int y = 0; y < h; y++, s += spitch, d += dpitch)
	{
		for (int x = 0; x < w; x++)
		{
			const u32 src = s[x];
			u32 alpha = src >> 24;
			if (fade >= 0)
				alpha = alpha * fade / 255;
			if (!key0 || alpha == 0xff)
				d[x] = src;
			else if (alpha != 0)
			{
				// source over, 8 bits of alpha per pixel
				const u32 dst = d[x];
				u32 result = src & 0xff000000;
				for (int shift = 0; shift < 24; shift += 8)
				{
					const u32 c = (((src >> shift) & 0xff) * alpha + ((dst >> shift) & 0xff) * (255 - alpha) + 127) / 255;
					result |= c << shift;
				}
				d[x] = result;
			}
		}
	}
}

void pmx1_device::fill(u32 dst, int dx, int dy, int dpitch, int w, int h, u32 color)
{
	const u32 doffs = (dst & m_host_mask) - m_host_base + (u32(dy) * u32(dpitch) + u32(dx)) * 4;
	if (w <= 0 || h <= 0 || dpitch <= 0 || u64(doffs) + (u64(h - 1) * dpitch + w) * 4 > m_vram.bytes())
	{
		logerror("fill out of range dst %08x %dx%d\n", dst, w, h);
		return;
	}

	u32 *d = m_vram.target() + doffs / 4;
	for (int y = 0; y < h; y++, d += dpitch)
		std::fill_n(d, w, color);
}

/*
    Text packet: +0x08 destination, +0x0c pitch in bytes << 1 | 1, +0x10 y << 16 | x, +0x14 glyph count,
    +0x1c clip height << 16 | width, +0x20 foreground, +0x24 background, then (glyph, advance) pairs.
    A glyph is 2 bytes of padding, width, height and 4 bytes of flags, then 1bpp rows, MSB left. The
    host fills the background first, so only foreground pixels are drawn here.
*/
void pmx1_device::text(u32 pkt)
{
	const u32 dst = mem_r(pkt + 0x08);
	const int pitch = mem_r(pkt + 0x0c) >> 3;
	const u32 pos = mem_r(pkt + 0x10);
	const u32 count = mem_r(pkt + 0x14);
	const u32 clip = mem_r(pkt + 0x1c);
	const u32 color = mem_r(pkt + 0x20);
	const int clipw = clip & 0xffff;
	const int cliph = clip >> 16;
	const u8 *const vram = reinterpret_cast<const u8 *>(m_vram.target());
	u32 *const fb = m_vram.target();
	const u32 base = ((dst & m_host_mask) - m_host_base) / 4;
	if (pitch <= 0 || pitch > 4096)
		return;

	int x = pos & 0xffff;
	const int y = pos >> 16;
	for (u32 i = 0; i < count && i < 0x200; i++)
	{
		const u32 glyph = (mem_r(pkt + 0x28 + i * 8) & m_host_mask) - m_host_base;
		const u32 advance = mem_r(pkt + 0x2c + i * 8);
		if (glyph + 8 < m_vram.bytes())
		{
			const int gw = vram[glyph + 2];
			const int gh = vram[glyph + 3];
			const int rowbytes = (gw + 7) / 8;
			if (glyph + 8 + gh * rowbytes <= m_vram.bytes())
			{
				for (int gy = 0; gy < gh; gy++)
				{
					const u8 *const row = &vram[glyph + 8 + gy * rowbytes];
					const int py = y + gy;
					for (int gx = 0; gx < gw; gx++)
					{
						const int px = x + gx;
						const u32 offs = base + py * pitch + px;
						if (BIT(row[gx >> 3], 7 - (gx & 7)) && px < clipw && py < cliph && offs < m_vram.bytes() / 4)
							fb[offs] = color;
					}
				}
			}
		}
		x += advance;
	}
}

void pmx1_device::line(u32 dst, int pitch, int x0, int y0, int x1, int y1, int clipw, int cliph, u32 color)
{
	const u32 base = ((dst & m_host_mask) - m_host_base) / 4;
	u32 *const fb = m_vram.target();
	if (pitch <= 0)
		return;

	const int dx = std::abs(x1 - x0);
	const int dy = -std::abs(y1 - y0);
	const int sx = (x0 < x1) ? 1 : -1;
	const int sy = (y0 < y1) ? 1 : -1;
	int err = dx + dy;
	for (;;)
	{
		const u32 offs = base + u32(y0) * u32(pitch) + u32(x0);
		if (x0 >= 0 && y0 >= 0 && x0 < clipw && y0 < cliph && offs < m_vram.bytes() / 4)
			fb[offs] = color;
		if (x0 == x1 && y0 == y1)
			break;
		const int e2 = 2 * err;
		if (e2 >= dy)
		{
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx)
		{
			err += dx;
			y0 += sy;
		}
	}
}

void pmx1_device::process_ring()
{
	const u32 ctrl = m_host_base + HLE_ASYNC_CTRL;

	if (m_ring_woff == 0)
	{
		if (m_regs[0xb0 / 4] == 0)
			return;
		m_ring_woff = m_ring_locate();
		if (m_ring_woff == 0)
			return;
		logerror("async write offset at %08x\n", m_ring_woff);
	}

	const u32 write = m_host_read32(m_ring_woff) & 0xffff;
	u32 read = mem_r(ctrl) & 0xffff;

	for (int count = 0; read != write && count < 4096; count++)
	{
		const u32 pkt = m_host_base + HLE_ASYNC_RING + read;
		const u32 size = mem_r(pkt);
		const u32 routine = mem_r(pkt + 4);
		if (size < 8 || size > 0x1000)
		{
			logerror("bad ring packet at %04x, size %x\n", read, size);
			read = write;
			break;
		}

		const auto w = [this, pkt] (int i) { return mem_r(pkt + i * 4); };
		if (size == 0x14)
		{
			// fence: everything queued before it has been executed
			mem_w(ctrl + 0x8c, 1);
			read = (read + 0x40) & 0xffff;
			continue;
		}

		switch (routine - m_host_base)
		{
		case ROUTINE_BLTB:
			blit(w(8), w(12) >> 16, w(13) & 0xffff, w(11) >> 16,
					w(9), w(10) & 0xffff, w(13) >> 16, w(11) & 0xffff,
					w(10) >> 16, w(12) & 0xffff, true, BIT(w(2), 1) ? int(w(2) >> 24) : -1);
			mem_w(w(15), w(14));
			mem_w(w(17), w(16));
			break;

		case ROUTINE_BLTA:
			blit(w(2), w(4) >> 16, w(5) & 0xffff, w(7) >> 16,
					w(3), w(6) & 0xffff, w(5) >> 16, w(7) & 0xffff,
					w(6) >> 16, w(4) & 0xffff, false);
			mem_w(w(13), w(12));
			mem_w(w(15), w(14));
			break;

		case ROUTINE_FILL:
			fill(w(2), w(4) >> 16, w(4) & 0xffff, (w(3) >> 16) / 4, w(5) >> 16, w(5) & 0xffff, w(7));
			break;

		case ROUTINE_TEXT:
			text(pkt);
			break;

		case ROUTINE_LINE:
			line(w(2), w(8) / 4, w(3) & 0xffff, w(3) >> 16, w(4) & 0xffff, w(4) >> 16, w(6) & 0xffff, w(6) >> 16, w(9));
			break;

		case ROUTINE_COPY:
			for (u32 i = 0; i < w(2) && i < 3; i++)
			{
				const u32 src = (w(3 + i * 3) & m_host_mask) - m_host_base;
				const u32 dst = (w(4 + i * 3) & m_host_mask) - m_host_base;
				const u32 bytes = w(5 + i * 3);
				if (src < m_vram.bytes() && dst < m_vram.bytes() && bytes <= m_vram.bytes() - std::max(src, dst))
				{
					u8 *const base = reinterpret_cast<u8 *>(m_vram.target());
					if (dst < src)
						std::copy_n(base + src, bytes, base + dst);
					else if (dst > src)
						std::copy_backward(base + src, base + src + bytes, base + dst + bytes);
				}
			}
			break;

		case ROUTINE_SYNC:
			mem_w(w(2), w(3));
			break;

		case ROUTINE_FLIP:
			// HLE: word 2 selects the page: 0 or 0x100, the MK6 BIOS framebuffers are consecutive
			m_display_base = BIT(w(2), 8) ? 640 * 480 * 4 : 0;
			mem_w(w(7), w(6));
			break;

		default:
			logerror("unhandled routine %08x size %x: %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x\n", routine, size,
					w(2), w(3), w(4), w(5), w(6), w(7), w(8), w(9), w(10), w(11));
			break;
		}

		read = (read + ((size + 63) & ~63)) & 0xffff;
	}

	mem_w(ctrl, read);
}
