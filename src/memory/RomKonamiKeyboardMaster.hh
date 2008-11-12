// $Id$

#ifndef ROMKONAMIKEYBOARDMASTER_HH
#define ROMKONAMIKEYBOARDMASTER_HH

#include "RomBlocks.hh"

namespace openmsx {

class VLM5030;

class RomKonamiKeyboardMaster : public Rom16kBBlocks
{
public:
	RomKonamiKeyboardMaster(MSXMotherBoard& motherBoard, const XMLElement& config,
	                        std::auto_ptr<Rom> rom);
	virtual ~RomKonamiKeyboardMaster();

	virtual void reset(EmuTime::param time);
	virtual void writeIO(word port, byte value, EmuTime::param time);
	virtual byte readIO(word port, EmuTime::param time);

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

private:
	const std::auto_ptr<VLM5030> vlm5030;
};

} // namespace openmsx

#endif
