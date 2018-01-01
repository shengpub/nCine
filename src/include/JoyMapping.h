#ifndef CLASS_NCINE_JOYMAPPING
#define CLASS_NCINE_JOYMAPPING

#include <cstdint>
#include "Array.h"
#include "StaticArray.h"
#include "InputEvents.h"

namespace ncine {

class IInputManager;
class JoyButtonEvent;
class JoyAxisEvent;
class JoyConnectionEvent;
class IInputEventHandler;
template <class T> class Vector2;
typedef class Vector2<float> Vector2f;

/// Information about a mapped joystick state (implementation)
class JoyMappedStateImpl : public JoyMappedState
{
  public:
	JoyMappedStateImpl()
	{
		for (unsigned int i = 0; i < JoyMappedState::NumButtons; i++)
			buttons_[i] = false;
		for (unsigned int i = 0; i < JoyMappedState::NumAxes; i++)
			axesValues_[i] = 0.0f;
	}

	bool isButtonPressed(ButtonName name) const
	{
		bool pressed = false;
		if (name != BUTTON_UNKNOWN)
			pressed = buttons_[static_cast<int>(name)];
		return pressed;
	}

	float axisValue(AxisName name) const
	{
		float value = 0.0f;
		if (name != AXIS_UNKNOWN)
			value = axesValues_[static_cast<int>(name)];
		return value;
	}

  private:
	bool buttons_[JoyMappedState::NumButtons];
	float axesValues_[JoyMappedState::NumAxes];

	friend class JoyMapping;
};

class JoyMapping
{
  public:
	JoyMapping();
	~JoyMapping() { }

	void init(const IInputManager *inputManager);
	inline void setHandler(IInputEventHandler *inputEventHandler) { inputEventHandler_ = inputEventHandler; }

	bool addMappingFromString(const char *mappingString);
	void addMappingsFromStrings(const char **mappingStrings);
	void addMappingsFromFile(const char *filename);

	void onJoyButtonPressed(const JoyButtonEvent &event);
	void onJoyButtonReleased(const JoyButtonEvent &event);
	void onJoyAxisMoved(const JoyAxisEvent &event);
	bool onJoyConnected(const JoyConnectionEvent &event);
	void onJoyDisconnected(const JoyConnectionEvent &event);

	bool isJoyMapped(int joyId) const;
	const JoyMappedStateImpl &joyMappedState(int joyId) const;
	void deadZoneNormalize(Vector2f &joyVector, float deadZoneValue) const;

  private:
	static const unsigned int MaxNameLength = 64;

	struct MappedJoystick
	{
		struct Axis
		{
			AxisName name;
			float min;
			float max;
		};

		static const int MaxNumAxes = 16;
		static const int MaxNumButtons = 16;

		class Guid
		{
		  public:
			Guid();
			Guid(const char *string) { fromString(string); }
			void fromString(const char *string);

			bool operator==(const Guid &guid) const;

		  private:
			uint32_t array_[4];
		} guid;

		char name[MaxNameLength];
		Axis axes[MaxNumAxes];
		ButtonName buttons[MaxNumButtons];
		int hats[MaxNumButtons];

		MappedJoystick();
	};

	static const char *AxesStrings[JoyMappedState::NumAxes];
	static const char *ButtonsStrings[JoyMappedState::NumButtons];

	static const int MaxNumJoysticks = 4;
	int mappingIndex_[MaxNumJoysticks];
	nc::Array<MappedJoystick> mappings_;

	static JoyMappedStateImpl nullMappedJoyState_;
	static StaticArray<JoyMappedStateImpl, MaxNumJoysticks> mappedJoyStates_;
	static JoyMappedButtonEvent mappedButtonEvent_;
	static JoyMappedAxisEvent mappedAxisEvent_;

	const IInputManager *inputManager_;
	IInputEventHandler *inputEventHandler_;

	void checkConnectedJoystics();
	int findMappingByGuid(const MappedJoystick::Guid &guid);
	int findMappingByName(const char *name);
	bool parseMappingFromString(const char *mappingString, MappedJoystick &map);
	bool parsePlatformKeyword(const char *start, const char *end) const;
	bool parsePlatformName(const char *start, const char *end) const;
	int parseAxisName(const char *start, const char *end) const;
	int parseButtonName(const char *start, const char *end) const;
	int  parseAxisMapping(const char *start, const char *end, MappedJoystick::Axis &axis) const;
	int parseButtonMapping(const char *start, const char *end) const;
	int parseHatMapping(const char *start, const char *end) const;
};

}

#endif