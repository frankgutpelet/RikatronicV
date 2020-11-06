extern "C" {
#include "user_interface.h"
}

class Flap
{
public:
  typedef enum tag_error_e
  {
    ERROR_NOT_CALIBRATED = 255,
    ERROR_WRONG_CALIBRATED = 254
  }error_e;
  typedef void (Flap::*flapFctPointer)(void);

  typedef enum tag_FlapProgramState_e
  {
    FLAP_PROGRAM_STATE_NONE = 0,
    FLAP_PROGRAM_STATE_CALIBRATION_LOW,
    FLAP_PROGRAM_STATE_CALIBRATION_HIGH,
    FLAP_PROGRAM_STATE_COUNT
  }FlapProgramState_e;

  typedef enum Tag_flapMode_e
  {
    FLAP_MODE_MANUAL,
    FLAP_MODE_ECO,
    FLAP_MODE_POWER,
    FLAP_MODE_Count
  }flapMode_e;
    
	Flap(int pwmPin, int relaisPin, int adcPin,  int adcTmpPin);
	void SetPosition(int percentage);
	void SwitchOn();
	void SwitchOff();
	int GetPosition();
  void Calibrate(); //Dauert 2:10 min
  bool IsCalibrated();
  int GetTemp();
  void ProgramStateMachine();
  void SetMode(flapMode_e state);
  flapMode_e GetMode(){return this->state;}
  FlapProgramState_e GetState(){return this->programState;}
   
private:
  double GetTmpResistance();
  double GetTmpVoltage();
  
  int action_timeSec;
  FlapProgramState_e programState;
  bool isCalibrated;
  int calibrationFactorLow;
  int calibrationFactorHigh;
  int adcPin, pwmPin, relaisPin, adcTmpPin;
	int position;
	bool isOn;
  flapMode_e state;
  os_timer_t timer;
 
};
