// $Id$

/*
 * Password Cartridge
 *
 * Access: write 0x00 to I/O port 0x7e
 *         provide 0xaa, <char1>, <char2>, continuous 0xff sequence reading I/O port 0x7e
 *         write any non-zero to I/O port 0x7e
 *         provide 0xff at all time reading I/O port 0x7e
 */

#include "PasswordCart.hh"
#include "XMLElement.hh"
#include "serialize.hh"
#include <algorithm>

namespace openmsx {

PasswordCart::PasswordCart(MSXMotherBoard& motherBoard, const XMLElement& config)
	: MSXDevice(motherBoard, config)
	, password(config.getChildDataAsInt("password", 0))
{
	reset(*static_cast<EmuTime*>(0));
}

void PasswordCart::reset(EmuTime::param /*time*/)
{
	pointer = 3;
}

void PasswordCart::writeIO(word /*port*/, byte value, EmuTime::param /*time*/)
{
	pointer = (value == 0) ? 0 : 3;
}

byte PasswordCart::readIO(word port, EmuTime::param time)
{
	byte result = peekIO(port, time);
	pointer = std::min(3, pointer + 1);
	return result;
}

byte PasswordCart::peekIO(word /*port*/, EmuTime::param /*time*/) const
{
	switch (pointer) {
	case 0:
		return 0xAA;
	case 1:
		return password >> 8;
	case 2:
		return password & 0xFF;
	default:
		return 0xFF;
	}
}

template<typename Archive>
void PasswordCart::serialize(Archive& ar, unsigned /*version*/)
{
	ar.template serializeBase<MSXDevice>(*this);
	ar.serialize("pointer", pointer);
}
INSTANTIATE_SERIALIZE_METHODS(PasswordCart);
REGISTER_MSXDEVICE(PasswordCart, "PasswordCart");

} // namespace openmsx
