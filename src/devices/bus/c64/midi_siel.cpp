// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Siel/JMS/DATEL MIDI Interface cartridge emulation

**********************************************************************/

#include "emu.h"
#include "midi_siel.h"
#include "machine/clock.h"
#include "bus/midi/midi.h"



//**************************************************************************
//  MACROS/CONSTANTS
//**************************************************************************

#define MC6850_TAG      "mc6850"



//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

DEFINE_DEVICE_TYPE(C64_MIDI_SIEL, c64_siel_midi_cartridge_device, "c64_midisiel", "C64 Siel MIDI")


WRITE_LINE_MEMBER( c64_siel_midi_cartridge_device::acia_irq_w )
{
	m_slot->irq_w(state);
}

WRITE_LINE_MEMBER( c64_siel_midi_cartridge_device::write_acia_clock )
{
	m_acia->write_txc(state);
	m_acia->write_rxc(state);
}


//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

MACHINE_CONFIG_START(c64_siel_midi_cartridge_device::device_add_mconfig)
	ACIA6850(config, m_acia, 0);
	m_acia->txd_handler().set("mdout", FUNC(midi_port_device::write_txd));
	m_acia->irq_handler().set(FUNC(c64_siel_midi_cartridge_device::acia_irq_w));

	MCFG_MIDI_PORT_ADD("mdin", midiin_slot, "midiin")
	MCFG_MIDI_RX_HANDLER(WRITELINE(MC6850_TAG, acia6850_device, write_rxd))

	MCFG_MIDI_PORT_ADD("mdout", midiout_slot, "midiout")

	MCFG_DEVICE_ADD("acia_clock", CLOCK, 31250*16)
	MCFG_CLOCK_SIGNAL_HANDLER(WRITELINE(*this, c64_siel_midi_cartridge_device, write_acia_clock))
MACHINE_CONFIG_END



//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  c64_siel_midi_cartridge_device - constructor
//-------------------------------------------------

c64_siel_midi_cartridge_device::c64_siel_midi_cartridge_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, C64_MIDI_SIEL, tag, owner, clock),
	device_c64_expansion_card_interface(mconfig, *this),
	m_acia(*this, MC6850_TAG)
{
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void c64_siel_midi_cartridge_device::device_start()
{
	// state saving
}


//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void c64_siel_midi_cartridge_device::device_reset()
{
	m_acia->reset();
}


//-------------------------------------------------
//  c64_cd_r - cartridge data read
//-------------------------------------------------

uint8_t c64_siel_midi_cartridge_device::c64_cd_r(address_space &space, offs_t offset, uint8_t data, int sphi2, int ba, int roml, int romh, int io1, int io2)
{
	if (!io1)
	{
		switch (offset & 0xff)
		{
		case 6:
		case 7:
			data = m_acia->read(space, offset & 1);
			break;
		}
	}

	return data;
}


//-------------------------------------------------
//  c64_cd_w - cartridge data write
//-------------------------------------------------

void c64_siel_midi_cartridge_device::c64_cd_w(address_space &space, offs_t offset, uint8_t data, int sphi2, int ba, int roml, int romh, int io1, int io2)
{
	if (!io1)
	{
		switch (offset & 0xff)
		{
		case 4:
		case 5:
			m_acia->write(space, offset & 1, data);
			break;
		}
	}
}
