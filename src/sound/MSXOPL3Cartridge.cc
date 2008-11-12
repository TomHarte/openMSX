// $Id$

#include "MSXOPL3Cartridge.hh"
#include "YMF262.hh"
#include "serialize.hh"
#include <cassert>

namespace openmsx {

MSXOPL3Cartridge::MSXOPL3Cartridge(MSXMotherBoard& motherBoard,
                           const XMLElement& config)
	: MSXDevice(motherBoard, config)
	, ymf262(new YMF262(motherBoard, getName(), config))
{
	reset(getCurrentTime());
}

MSXOPL3Cartridge::~MSXOPL3Cartridge()
{
}

void MSXOPL3Cartridge::reset(EmuTime::param time)
{
	ymf262->reset(time);

	// TODO check
	opl3latch = 0;
}

byte MSXOPL3Cartridge::readIO(word port, EmuTime::param /*time*/)
{
	byte result;
	// FM part  0xC4-0xC7 (in MoonSound)
	switch (port & 0x03) {
		case 0: // read status
		case 2:
			result = ymf262->readStatus();
			break;
		case 1:
		case 3: // read fm register
			result = ymf262->readReg(opl3latch);
			break;
		default: // unreachable, avoid warning
			assert(false);
			result = 255;
		}
	return result;
}

byte MSXOPL3Cartridge::peekIO(word port, EmuTime::param /*time*/) const
{
	byte result;
	switch (port & 0x03) {
		case 0: // read status
		case 2:
			result = ymf262->peekStatus();
			break;
		case 1:
		case 3: // read fm register
			result = ymf262->peekReg(opl3latch);
			break;
		default: // unreachable, avoid warning
			assert(false);
			result = 255;
		}
	return result;
}

void MSXOPL3Cartridge::writeIO(word port, byte value, EmuTime::param time)
{
	switch (port & 0x03) {
		case 0: // select register bank 0
			opl3latch = value;
			break;
		case 2: // select register bank 1
			opl3latch = value | 0x100;
			break;
		case 1:
		case 3: // write fm register
			ymf262->writeReg(opl3latch, value, time);
			break;
		default:
			assert(false);
	}
}

template<typename Archive>
void MSXOPL3Cartridge::serialize(Archive& ar, unsigned /*version*/)
{
	ar.template serializeBase<MSXDevice>(*this);
	ar.serialize("ymf262", *ymf262);
	ar.serialize("opl3latch", opl3latch);
}
INSTANTIATE_SERIALIZE_METHODS(MSXOPL3Cartridge);
REGISTER_MSXDEVICE(MSXOPL3Cartridge, "OPL3Cartridge");

} // namespace openmsx
