#include <ESP8266WiFi.h>
#include "Logger.hpp"
#include "flap.hpp"
#include "tempSensor.hpp"

class FlameRegulator
{
public:
	typedef enum Tag_flapMode_e
	{
		FLAP_MODE_MANUAL = 0,
		FLAP_MODE_ECO,
		FLAP_MODE_POWER,
		FLAP_MODE_Count
	}flapMode_e;


	typedef enum tag_FlapProgramState_e
	{
		FLAP_PROGRAM_STATE_NONE = 0,
		FLAP_PROGRAM_STATE_REFILL,
		FLAP_PROGRAM_STATE_HEAT_UP_START,
		FLAP_PROGRAM_STATE_HEAT_UP_1,
		FLAP_PROGRAM_STATE_HEAT_UP_2,
		FLAP_PROGRAM_STATE_HEAT_UP_3,
		FLAP_PROGRAM_STATE_HEAT_UP_4,
		FLAP_PROGRAM_STATE_HEAT_UP_END,
		FLAP_PROGRAM_STATE_HEAT_BURN_IDLE,
		FLAP_PROGRAM_STATE_HEAT_BURN,
		FLAP_PROGRAM_STATE_HEAT_BURN_HOT,
		FLAP_PROGRAM_STATE_HEAT_BURN_HOT2,
		FLAP_PROGRAM_STATE_HEAT_BURN_HOT3,
		FLAP_PROGRAM_STATE_HEAT_OFF,
		FLAP_PROGRAM_STATE_HEAT_RESCUE,
		FLAP_PROGRAM_STATE_COUNT
	}FlapProgramState_e;

	bool mute; // Mute the beep 
	
	// callback functionpointertype for internal memberfunction 
	typedef void (Flap::* flapFctPointer)(void);

	FlameRegulator();

	void ProgramStateMachine(); // oly accessed by timer

	void Refill();	// start "refill wood" program

	void Heat_Up(); // start "heat up" programm

	void SetManual(int pos); // set the flap position manual

	void Beep(bool swOn);
	void ToggleBeep(void);
	void Mute(int seconds);

	// set and get the flap regulation mode	
	void SetFlapRegulationMode(flapMode_e flapRegulationMode);
	flapMode_e GetFlapRegulationMode() { return this->flapRegulationMode; }
	String GetFlapRegulationModeStr();

	// get the programstate (is not setable from outside)
	FlapProgramState_e GetProgramState() { return this->programState; }
	String GetProgramStateStr();

	//wrapper functions for get position of the flap and temperature
	int GetFlapPosition() { return this->flap.GetPosition(); }
	int GetTemperature() { return this->tempSensor.GetTemp(); }

private:
	typedef struct tag_programStateConfig_t
	{
		FlapProgramState_e state;
		FlapProgramState_e nextstate;
		FlapProgramState_e laststate;
		int tempForNextState;
		int tempForLastState;
		int currentFlapPosition;
		int breakTimeSec;
		bool beep;
	}programStateConfig_t;

	static programStateConfig_t programStateConfig[FLAP_PROGRAM_STATE_COUNT];
  static programStateConfig_t programStateConfigEco[FLAP_PROGRAM_STATE_COUNT];

	flapMode_e flapRegulationMode;
	FlapProgramState_e programState;
	Flap flap;
	TempSensor tempSensor;
	os_timer_t timer;
	Logger* logger;
	int action_timeSec;
	int secondsToUnmute;

	// recognizes the initial state after startup
	void RecogniceInitialState();
	
};
