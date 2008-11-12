// $Id$

#include "DummyMidiOutDevice.hh"

namespace openmsx {

void DummyMidiOutDevice::recvByte(byte /*value*/, EmuTime::param /*time*/)
{
	// ignore
}

const std::string& DummyMidiOutDevice::getDescription() const
{
	static const std::string EMPTY;
	return EMPTY;
}

void DummyMidiOutDevice::plugHelper(
		Connector& /*connector*/, EmuTime::param /*time*/)
{
}

void DummyMidiOutDevice::unplugHelper(EmuTime::param /*time*/)
{
}

} // namespace openmsx
