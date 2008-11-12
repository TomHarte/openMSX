// $Id$

#ifndef NINJATAP_HH
#define NINJATAP_HH

#include "JoyTap.hh"

namespace openmsx {

class NinjaTap : public JoyTap
{
public:
	NinjaTap(PluggingController& pluggingController,
	         const std::string& name);

	// Pluggable
	virtual const std::string& getDescription() const;

	// JoystickDevice
	virtual byte read(EmuTime::param time);
	virtual void write(byte value, EmuTime::param time);

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

private:
	byte status;
	byte previous;
	byte buf[4];
};

} // namespace openmsx

#endif
