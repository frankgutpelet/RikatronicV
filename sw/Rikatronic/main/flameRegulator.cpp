#include "flameRegulator.hpp"

#define ANALOG_INPUT 0
#define RELAIS_OUTPUT 16
#define PWM_OUTPUT 5

const char* S_ProgramStateString[FlameRegulator::FLAP_PROGRAM_STATE_COUNT] =
{
  "unbekannt",
  "Nachlegen",
  "Anheizen",
  "Anheizen",
  "Anheizen",
  "Anheizen",
  "Anheizen",
  "Anheizen",
  "Automatik",
  "Automatik",
  "Aus",
  "Rettung"
};

const char* S_FlapRegulationModeString[FlameRegulator::FLAP_MODE_Count]
{
  "MANUAL",
  "ECO",
  "POWER"
};

FlameRegulator::programStateConfig_t FlameRegulator::programStateConfig[FLAP_PROGRAM_STATE_COUNT]=
        //state                                 nextstate                       laststate               tempForNextState    tempForLastState    currentFlapPosition     breakTimeSec
{
    {FLAP_PROGRAM_STATE_NONE,           FLAP_PROGRAM_STATE_NONE,            FLAP_PROGRAM_STATE_NONE,            0,                      0,                  0,                  0 },
    {FLAP_PROGRAM_STATE_REFILL,         FLAP_PROGRAM_STATE_HEAT_BURN,       FLAP_PROGRAM_STATE_HEAT_RESCUE,     0,                      0,                  0,                  0 },
    {FLAP_PROGRAM_STATE_HEAT_UP_START,  FLAP_PROGRAM_STATE_HEAT_UP_1,       FLAP_PROGRAM_STATE_HEAT_RESCUE,    40,                     30,                 20,                 60 },
    {FLAP_PROGRAM_STATE_HEAT_UP_1,      FLAP_PROGRAM_STATE_HEAT_UP_2,       FLAP_PROGRAM_STATE_HEAT_RESCUE,    50,                     40,                 40,                 60 },
    {FLAP_PROGRAM_STATE_HEAT_UP_2,      FLAP_PROGRAM_STATE_HEAT_UP_3,       FLAP_PROGRAM_STATE_HEAT_RESCUE,    60,                     50,                 60,                 60 },
    {FLAP_PROGRAM_STATE_HEAT_UP_3,      FLAP_PROGRAM_STATE_HEAT_UP_4,       FLAP_PROGRAM_STATE_HEAT_RESCUE,    70,                     60,                 70,                 60 },
    {FLAP_PROGRAM_STATE_HEAT_UP_4,      FLAP_PROGRAM_STATE_HEAT_UP_END,     FLAP_PROGRAM_STATE_HEAT_RESCUE,    80,                     70,                 75,                 60 },
    {FLAP_PROGRAM_STATE_HEAT_UP_END,    FLAP_PROGRAM_STATE_HEAT_BURN,       FLAP_PROGRAM_STATE_HEAT_RESCUE,    90,                     80,                 80,                 60 },
    {FLAP_PROGRAM_STATE_HEAT_BURN,      FLAP_PROGRAM_STATE_HEAT_BURN_HOT,   FLAP_PROGRAM_STATE_HEAT_RESCUE,   140,                     60,                 85,                 30 },
    {FLAP_PROGRAM_STATE_HEAT_BURN_HOT,  FLAP_PROGRAM_STATE_HEAT_BURN_HOT,   FLAP_PROGRAM_STATE_HEAT_BURN,    1000,                    100,                 88,                 30 },
    {FLAP_PROGRAM_STATE_HEAT_OFF,       FLAP_PROGRAM_STATE_HEAT_UP_START,   FLAP_PROGRAM_STATE_HEAT_OFF,       30,                      0,                  0,                 10 },
    {FLAP_PROGRAM_STATE_HEAT_RESCUE,    FLAP_PROGRAM_STATE_HEAT_UP_START,   FLAP_PROGRAM_STATE_HEAT_OFF,       40,                     25,                  0,                120 }
};


//###################################non member function ##############################
void timerCallback(void* pArg);


//################################### member function #################################

FlameRegulator::FlameRegulator(void):
flap{Flap(PWM_OUTPUT, RELAIS_OUTPUT)},
tempSensor{TempSensor(ANALOG_INPUT)}
{
    this->programState = FLAP_PROGRAM_STATE_HEAT_OFF;
    this->flapRegulationMode = FLAP_MODE_ECO;
    this->logger = Logger::instance();

	// initialize tinmer
	os_timer_setfn(&(this->timer), timerCallback, this);
	os_timer_arm(&(this->timer), 1000, true);
}

void FlameRegulator::SetFlapRegulationMode(flapMode_e flapRegulationMode)
{
    this->flapRegulationMode = flapRegulationMode;
}

String FlameRegulator::GetFlapRegulationModeStr(void)
{
    return S_FlapRegulationModeString[this->flapRegulationMode];
}

String FlameRegulator::GetProgramStateStr()
{
    return S_ProgramStateString[this->programState];
}

void FlameRegulator::Heat_Up(void)
{
    flap.SwitchOff();
    flap.SetPosition(0);
    this->programState = FLAP_PROGRAM_STATE_HEAT_UP_START;
    this->action_timeSec = 20;
}

void FlameRegulator::Refill(void)
{
    this->flap.SwitchOff();
    this->programState = FLAP_PROGRAM_STATE_REFILL;
    this->action_timeSec = 20;
}

void FlameRegulator::SetManual(int pos)
{
    if (FLAP_MODE_MANUAL != this->flapRegulationMode)
    {
        logger->Error(String("cannot set the flap position in mode: ") + this->GetFlapRegulationModeStr());
        return;
    }
    if (0 == pos)
    {
        this->flap.SwitchOff();
    }
    else
    {
        this->flap.SwitchOn();
    }

    this->flap.SetPosition(pos);
}

//###################################   state Machine     ##############################

void FlameRegulator::ProgramStateMachine (void)
{
    programStateConfig_t* currentConfig = &this->programStateConfig[(int)this->programState];

    this->logger->Debug("start Statemachine, mode: " + this->GetFlapRegulationMode());
    if (0 != this->action_timeSec)
    {
        this->action_timeSec--;
        return;
    }

    // do nothin if manual regulation
    if (FLAP_MODE_MANUAL == this->flapRegulationMode)
    {
        return;
    }

    // set flap 
    if (0 == currentConfig->currentFlapPosition)
    {
        this->flap.SwitchOff();
    }
    else
    {
        this->flap.SwitchOn();
    }
    this->flap.SetPosition(currentConfig->currentFlapPosition);

    //check fallback temperature
    if (this->tempSensor.GetTemp() < currentConfig->tempForLastState)
    {
        this->programState = currentConfig->laststate;
        this->action_timeSec = 0;
        return;
    }

    // check if temp of next state is reached
    if (this->tempSensor.GetTemp() >= currentConfig->tempForNextState)
    {
        this->programState = currentConfig->nextstate;
        this->action_timeSec = currentConfig->breakTimeSec;
        return;
    }

    this->action_timeSec = 10;    
}

//###################################   private functions    ##############################
void FlameRegulator::RecogniceInitialState(void)
{

    programStateConfig_t config = programStateConfig[FLAP_PROGRAM_STATE_HEAT_OFF];
    int temp = this->tempSensor.GetTemp();

    while (temp >= config.tempForNextState)
    {
        config = programStateConfig[config.nextstate];
    }

    this->programState = config.state;
    this->action_timeSec = 0;
    this->logger->Debug(String("initial state is ") + this->GetProgramStateStr());
}

//###################################non member function ##############################
void timerCallback(void* pArg)
{
	FlameRegulator* instance = (FlameRegulator*)pArg;
	instance->ProgramStateMachine();
}
