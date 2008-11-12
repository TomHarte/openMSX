// $Id$

#ifndef CASSETTEPORT_HH
#define CASSETTEPORT_HH

#include "Connector.hh"
#include "components.hh"
#include <memory>

namespace openmsx {

class MSXMotherBoard;
class CassetteDevice;
class CassettePlayer;
#ifdef COMPONENT_JACK
class CassetteJack;
#endif

class CassettePortInterface
{
public:
	virtual ~CassettePortInterface();

	/**
	* Sets the cassette motor relay
	*  false = off   true = on
	*/
	virtual void setMotor(bool status, EmuTime::param time) = 0;

	/**
	* Writes one bit to the cassette port.
	* From the RedBook:
	*   The CasOut bit is filtered and attenuated before being
	*   taken to the cassette DIN socket as the MIC signal. All
	*   cassette tone generation is performed in software.
	*/
	virtual void cassetteOut(bool output, EmuTime::param time) = 0;

	/**
	 * last bit written to CasOut.
	 * for use in Pluggable::plugHelper()
	 */
	virtual bool lastOut() const = 0;

	/**
	* Reads one bit from the cassette port.
	* From the RedBook:
	*   The cassette input is used to read the signal from the
	*   cassette EAR output. This is passed through a comparator
	*   to clean the edges and to convert to digital levels,
	*   but is otherwise unprocessed.
	*/
	virtual bool cassetteIn(EmuTime::param time) = 0;
};

class CassettePort : public CassettePortInterface, public Connector
{
public:
	explicit CassettePort(MSXMotherBoard& motherBoard);
	virtual ~CassettePort();
	virtual void setMotor(bool status, EmuTime::param time);
	virtual void cassetteOut(bool output, EmuTime::param time);
	virtual bool cassetteIn(EmuTime::param time);
	virtual bool lastOut() const;

	// Connector
	virtual const std::string& getDescription() const;
	virtual const std::string& getClass() const;
	virtual void unplug(EmuTime::param time);

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

private:
	CassetteDevice& getPluggedCasDev() const;

	MSXMotherBoard& motherBoard;

	std::auto_ptr<CassettePlayer> cassettePlayer;
#ifdef COMPONENT_JACK
	std::auto_ptr<CassetteJack> cassetteJack;
#endif

	bool lastOutput;
};

class DummyCassettePort : public CassettePortInterface
{
public:
	virtual void setMotor(bool status, EmuTime::param time);
	virtual void cassetteOut(bool output, EmuTime::param time);
	virtual bool cassetteIn(EmuTime::param time);
	virtual bool lastOut() const;
};

} // namespace openmsx

#endif
