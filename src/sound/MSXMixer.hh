// $Id$

#ifndef MSXMIXER_HH
#define MSXMIXER_HH

#include "Schedulable.hh"
#include "Observer.hh"
#include "EmuTime.hh"
#include "EmuDuration.hh"
#include <vector>
#include <map>
#include <memory>

namespace openmsx {

class SoundDevice;
class Mixer;
class Scheduler;
class MSXCommandController;
class CommandController;
class ThrottleManager;
class IntegerSetting;
class StringSetting;
class BooleanSetting;
class Setting;
class SoundDeviceInfoTopic;
class AviRecorder;

class MSXMixer : private Schedulable, private Observer<Setting>
               , private Observer<ThrottleManager>
{
public:
	MSXMixer(Mixer& mixer, Scheduler& scheduler,
	         MSXCommandController& msxCommandController);
	virtual ~MSXMixer();

	/**
	 * Use this method to register a given sounddevice.
	 *
	 * While registering, the device its setSampleRate() method is
	 * called (see SoundDevice for more info).
	 * After registration the device its updateBuffer() method is
	 * 'regularly' called (see SoundDevice for more info).
	 */
	void registerSound(SoundDevice& device, double volume,
	                   int balance, unsigned numChannels);

	/**
	 * Every sounddevice must unregister before it is destructed
	 */
	void unregisterSound(SoundDevice& device);

	/**
	 * Use this method to force an 'early' call to all
	 * updateBuffer() methods.
	 */
	void updateStream(EmuTime::param time);

	// TODO
	void setSynchronousMode(bool synchronous);

	/** TODO
	 * This methods (un)mute the sound.
	 * These methods may be called multiple times, as long as
	 * you never call unmute() more than mute()
	 */
	void mute();
	void unmute();

	// Called by Mixer or SoundDriver

	/** Set new fragment size and sample frequency.
	 * A fragment size of zero means the Mixer is muted.
	 */
	void setMixerParams(unsigned fragmentSize, unsigned sampleRate);

	// Called by AviRecorder

	void setRecorder(AviRecorder* recorder);
	unsigned getSampleRate() const;

	SoundDevice* findDevice(const std::string& name) const;

	void reschedule();

private:
	struct SoundDeviceInfo {
		double defaultVolume;
		IntegerSetting* volumeSetting;
		IntegerSetting* balanceSetting;
		struct ChannelSettings {
			StringSetting* recordSetting;
			BooleanSetting* muteSetting;
		};
		std::vector<ChannelSettings> channelSettings;
		int left1, right1, left2, right2;
	};
	typedef std::map<SoundDevice*, SoundDeviceInfo> Infos;

	void updateVolumeParams(Infos::iterator it);
	void updateMasterVolume();
	void reInit();
	void updateStream2(EmuTime::param time);
	void generate(short* buffer, unsigned samples, EmuTime::param start,
	              EmuDuration::param sampDur);

	// Schedulable
	void executeUntil(EmuTime::param time, int userData);
	const std::string& schedName() const;

	// Observer<Setting>
	virtual void update(const Setting& setting);
	// Observer<ThrottleManager>
	virtual void update(const ThrottleManager& throttleManager);

	void changeRecordSetting(const Setting& setting);
	void changeMuteSetting(const Setting& setting);

	unsigned sampleRate;
	unsigned fragmentSize;

	Infos infos;

	Mixer& mixer;
	CommandController& commandController;

	IntegerSetting& masterVolume;
	IntegerSetting& speedSetting;
	ThrottleManager& throttleManager;

	EmuTime prevTime;
	EmuDuration interval1; ///<  (Estimated) duration for one sample
	EmuDuration interval1min;
	EmuDuration interval1max;

	friend class SoundDeviceInfoTopic;
	const std::auto_ptr<SoundDeviceInfoTopic> soundDeviceInfo;

	AviRecorder* recorder;
	unsigned synchronousCounter;

	unsigned muteCount;
	int prevLeft, prevRight;
	int outLeft, outRight;
};

} // namespace openmsx

#endif
