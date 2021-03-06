#ifndef KEYBOARD_HH
#define KEYBOARD_HH

#include "KeyboardSettings.hh"
#include "UnicodeKeymap.hh"
#include "MSXEventListener.hh"
#include "StateChangeListener.hh"
#include "Schedulable.hh"
#include "RecordedCommand.hh"
#include "SimpleDebuggable.hh"
#include "EventListener.hh"
#include "serialize_meta.hh"
#include "array_ref.hh"
#include "string_ref.hh"
#include "openmsx.hh"
#include <array>
#include <deque>
#include <vector>
#include <memory>

namespace openmsx {

class MSXMotherBoard;
class Scheduler;
class CommandController;
class DeviceConfig;
class EventDistributor;
class MSXEventDistributor;
class StateChangeDistributor;
class KeyEvent;
class StateChange;
class TclObject;
class Interpreter;

class Keyboard final : private MSXEventListener, private StateChangeListener
                     , private Schedulable
{
public:
	enum MatrixType { MATRIX_MSX, MATRIX_SVI };

	/** Constructs a new Keyboard object.
	 * @param motherBoard ref to the motherBoard
	 * @param scheduler ref to the scheduler
	 * @param commandController ref to the command controller
	 * @param eventDistributor ref to the emu event distributor
	 * @param msxEventDistributor ref to the user input event distributor
	 * @param stateChangeDistributor ref to the state change distributor
	 * @param matrix which system's keyboard matrix to use
	 * @param config ref to the device configuration
	 */
	Keyboard(MSXMotherBoard& motherBoard, Scheduler& scheduler,
	         CommandController& commandController,
	         EventDistributor& eventDistributor,
	         MSXEventDistributor& msxEventDistributor,
	         StateChangeDistributor& stateChangeDistributor,
	         MatrixType matrix, const DeviceConfig& config);

	~Keyboard();

	/** Returns a pointer to the current KeyBoard matrix
	 */
	const byte* getKeys();

	void transferHostKeyMatrix(const Keyboard& source);

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

private:
	// MSXEventListener
	void signalEvent(const std::shared_ptr<const Event>& event,
	                 EmuTime::param time) override;
	// StateChangeListener
	void signalStateChange(const std::shared_ptr<StateChange>& event) override;
	void stopReplay(EmuTime::param time) override;

	// Schedulable
	void executeUntil(EmuTime::param time) override;

	void pressKeyMatrixEvent(EmuTime::param time, KeyMatrixPosition pos);
	void releaseKeyMatrixEvent(EmuTime::param time, KeyMatrixPosition pos);
	void changeKeyMatrixEvent (EmuTime::param time, byte row, byte newValue);

	void processDeadKeyEvent(unsigned n, EmuTime::param time, bool down);
	void processCapslockEvent(EmuTime::param time, bool down);
	void processCodeKanaChange(EmuTime::param time, bool down);
	void processGraphChange(EmuTime::param time, bool down);
	void processKeypadEnterKey(EmuTime::param time, bool down);
	void processSdlKey(EmuTime::param time, bool down, Keys::KeyCode key);
	bool processQueuedEvent(const Event& event, EmuTime::param time);
	bool processKeyEvent(EmuTime::param time, bool down, const KeyEvent& keyEvent);
	void updateKeyMatrix(EmuTime::param time, bool down, KeyMatrixPosition pos);
	void doKeyGhosting();
	void processCmd(Interpreter& interp, array_ref<TclObject> tokens, bool up);
	bool pressUnicodeByUser(EmuTime::param time, unsigned unicode, bool down);
	int pressAscii(unsigned unicode, bool down);
	void pressLockKeys(byte lockKeysMask, bool down);
	bool commonKeys(unsigned unicode1, unsigned unicode2);
	void debug(const char* format, ...);

	/** Returns a bit vector in which the bit for a modifier is set iff that
	  * modifier is a lock key and must be toggled before the given key input
	  * can be produced.
	  */
	byte needsLockToggle(const UnicodeKeymap::KeyInfo& keyInfo) const;

	CommandController& commandController;
	MSXEventDistributor& msxEventDistributor;
	StateChangeDistributor& stateChangeDistributor;

	static const int MAX_KEYSYM = 0x150;
	static const KeyMatrixPosition keyTabs[][MAX_KEYSYM];
	const KeyMatrixPosition* keyTab;

	const std::array<KeyMatrixPosition, UnicodeKeymap::KeyInfo::NUM_MODIFIERS> modifierPos;

	struct KeyMatrixUpCmd final : RecordedCommand {
		KeyMatrixUpCmd(CommandController& commandController,
			       StateChangeDistributor& stateChangeDistributor,
			       Scheduler& scheduler);
		void execute(array_ref<TclObject> tokens, TclObject& result,
			     EmuTime::param time) override;
		std::string help(const std::vector<std::string>& tokens) const override;
	} keyMatrixUpCmd;

	struct KeyMatrixDownCmd final : RecordedCommand {
		KeyMatrixDownCmd(CommandController& commandController,
				 StateChangeDistributor& stateChangeDistributor,
				 Scheduler& scheduler);
		void execute(array_ref<TclObject> tokens, TclObject& result,
			     EmuTime::param time) override;
		std::string help(const std::vector<std::string>& tokens) const override;
	} keyMatrixDownCmd;

	class KeyInserter final : public RecordedCommand, public Schedulable {
	public:
		KeyInserter(CommandController& commandController,
			    StateChangeDistributor& stateChangeDistributor,
			    Scheduler& scheduler);
		template<typename Archive>
		void serialize(Archive& ar, unsigned version);

	private:
		void type(string_ref str);
		void reschedule(EmuTime::param time);

		// Command
		void execute(array_ref<TclObject> tokens, TclObject& result,
			     EmuTime::param time) override;
		std::string help(const std::vector<std::string>& tokens) const override;
		void tabCompletion(std::vector<std::string>& tokens) const override;

		// Schedulable
		void executeUntil(EmuTime::param time) override;

		std::string text_utf8;
		unsigned last;
		byte lockKeysMask;
		bool releaseLast;
		byte oldLocksOn;

		bool releaseBeforePress;
		unsigned typingFrequency;
	} keyTypeCmd;

	class CapsLockAligner final : private EventListener, private Schedulable {
	public:
		CapsLockAligner(EventDistributor& eventDistributor,
				Scheduler& scheduler);
		~CapsLockAligner();

	private:
		// EventListener
		int signalEvent(const std::shared_ptr<const Event>& event) override;

		// Schedulable
		void executeUntil(EmuTime::param time) override;

		void alignCapsLock(EmuTime::param time);

		EventDistributor& eventDistributor;

		enum CapsLockAlignerStateType {
			MUST_ALIGN_CAPSLOCK, MUST_DISTRIBUTE_KEY_RELEASE, IDLE
		} state;
	} capsLockAligner;

	KeyboardSettings keyboardSettings;

	class MsxKeyEventQueue final : public Schedulable {
	public:
		MsxKeyEventQueue(Scheduler& scheduler, Interpreter& interp);
		void process_asap(EmuTime::param time,
		                  const std::shared_ptr<const Event>& event);
		void clear();
		template<typename Archive>
		void serialize(Archive& ar, unsigned version);
	private:
		// Schedulable
		void executeUntil(EmuTime::param time) override;
		std::deque<std::shared_ptr<const Event>> eventQueue;
		Interpreter& interp;
	} msxKeyEventQueue;

	struct KeybDebuggable final : SimpleDebuggable {
		explicit KeybDebuggable(MSXMotherBoard& motherBoard);
		byte read(unsigned address) override;
		void write(unsigned address, byte value) override;
	} keybDebuggable;

	UnicodeKeymap unicodeKeymap;
	unsigned dynKeymap[MAX_KEYSYM];

	/** Keyboard matrix state for keymatrix/type command. */
	byte cmdKeyMatrix [KeyMatrixPosition::NUM_ROWS];
	/** Keyboard matrix state for pressed user keys (live or replay). */
	byte userKeyMatrix[KeyMatrixPosition::NUM_ROWS];
	/** Keyboard matrix state that is always in sync with host keyb, also during replay. */
	byte hostKeyMatrix[KeyMatrixPosition::NUM_ROWS];
	/** Combination of cmdKeyMatrix and userKeyMatrix. */
	byte keyMatrix[KeyMatrixPosition::NUM_ROWS];

	byte msxmodifiers;

	/** True iff keyboard includes a numeric keypad. */
	const bool hasKeypad;
	/** True iff changes to keyboard row 11 must be rejected.
	  * On MSX, row 11 contains the (Japanese) Yes/No keys.
	  */
	const bool blockRow11;
	/** True iff pressing multiple keys at once can add ghost key presses. */
	const bool keyGhosting;
	/** True iff Shift, Graph and Code are protected against key ghosting. */
	const bool keyGhostingSGCprotected;
	/** Bit vector where each modifier's bit (using KeyInfo::Modifier's
	  * numbering) is set iff it is a lock key.
	  */
	const byte modifierIsLock;
	const bool sdlReleasesCapslock;
	bool keysChanged;
	/** Bit vector where each modifier's bit (using KeyInfo::Modifier's
	  * numbering) is set iff it is a lock key that is currently on in
	  * the emulated machine.
	  */
	byte locksOn;
};
SERIALIZE_CLASS_VERSION(Keyboard, 2);

} // namespace openmsx

#endif
