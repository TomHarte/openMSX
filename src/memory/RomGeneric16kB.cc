// $Id$

#include "RomGeneric16kB.hh"
#include "Rom.hh"
#include "serialize.hh"

namespace openmsx {

RomGeneric16kB::RomGeneric16kB(
		MSXMotherBoard& motherBoard, const XMLElement& config,
		std::auto_ptr<Rom> rom)
	: Rom16kBBlocks(motherBoard, config, rom)
{
	reset(*static_cast<EmuTime*>(0));
}

void RomGeneric16kB::reset(EmuTime::param /*time*/)
{
	setBank(0, unmappedRead);
	setRom (1, 0);
	setRom (2, 1);
	setBank(3, unmappedRead);
}

void RomGeneric16kB::writeMem(word address, byte value, EmuTime::param /*time*/)
{
	setRom(address >> 14, value);
}

byte* RomGeneric16kB::getWriteCacheLine(word address) const
{
	if ((0x4000 <= address) && (address < 0xC000)) {
		return NULL;
	} else {
		return unmappedWrite;
	}
}

REGISTER_MSXDEVICE(RomGeneric16kB, "RomGeneric16kB");

} // namespace openmsx
