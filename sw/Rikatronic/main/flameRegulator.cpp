#include "flameRegulator.hpp"

#define ANALOG_INPUT 0
#define RELAIS_OUTPUT 16
#define PWM_OUTPUT 5
#define BEEP_PIN 4

const char* S_PushMessage_DeviceId = "vCDFC7D7572A7731";
const char* S_logServer = "api.pushingbox.com";


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
  "Erhalten",
  "Automatik",
  "Automatik",
  "Automatik",
  "Automatik",
  "Aus",
  "Rettung"
};

const char* S_FlapRegulationModeString[FlameRegulator::FLAP_MODE_Count] =
{
  "MANUAL",
  "ECO",
  "POWER"
};


FlameRegulator::programStateConfig_t FlameRegulator::programStateConfig[FLAP_PROGRAM_STATE_COUNT]=
        //state                                 nextstate                       laststate               tempForNextState    tempForLastState    currentFlapPosition     breakTimeSec
{
    {FLAP_PROGRAM_STATE_NONE,           FLAP_PROGRAM_STATE_NONE,            FLAP_PROGRAM_STATE_NONE,            0,                      0,                  0,                  0,   false },
    {FLAP_PROGRAM_STATE_REFILL,         FLAP_PROGRAM_STATE_HEAT_BURN,       FLAP_PROGRAM_STATE_HEAT_OFF,        0,                      0,                  0,                  0,   false },
    {FLAP_PROGRAM_STATE_HEAT_UP_START,  FLAP_PROGRAM_STATE_HEAT_UP_1,       FLAP_PROGRAM_STATE_HEAT_OFF,        50,                     30,                 20,                 60,  false },
    {FLAP_PROGRAM_STATE_HEAT_UP_1,      FLAP_PROGRAM_STATE_HEAT_UP_2,       FLAP_PROGRAM_STATE_HEAT_UP_START,   60,                     50,                 40,                 60,  false },
    {FLAP_PROGRAM_STATE_HEAT_UP_2,      FLAP_PROGRAM_STATE_HEAT_UP_3,       FLAP_PROGRAM_STATE_HEAT_UP_1,       70,                     60,                 60,                 60,  false },
    {FLAP_PROGRAM_STATE_HEAT_UP_3,      FLAP_PROGRAM_STATE_HEAT_UP_4,       FLAP_PROGRAM_STATE_HEAT_UP_2,       80,                     70,                 70,                 60,  false },
    {FLAP_PROGRAM_STATE_HEAT_UP_4,      FLAP_PROGRAM_STATE_HEAT_UP_END,     FLAP_PROGRAM_STATE_HEAT_UP_3,       90,                     80,                 75,                 60,  false },
    {FLAP_PROGRAM_STATE_HEAT_UP_END,    FLAP_PROGRAM_STATE_HEAT_BURN,       FLAP_PROGRAM_STATE_HEAT_UP_4,       100,                    90,                 80,                 60,  false },
    {FLAP_PROGRAM_STATE_HEAT_BURN_IDLE, FLAP_PROGRAM_STATE_HEAT_BURN,       FLAP_PROGRAM_STATE_HEAT_RESCUE,     150,                    140,                75,                 20,  true },
    {FLAP_PROGRAM_STATE_HEAT_BURN,      FLAP_PROGRAM_STATE_HEAT_BURN_HOT,   FLAP_PROGRAM_STATE_HEAT_BURN_IDLE,  160,                    140,                80,                 30,  false },
    {FLAP_PROGRAM_STATE_HEAT_BURN_HOT,  FLAP_PROGRAM_STATE_HEAT_BURN_HOT2,  FLAP_PROGRAM_STATE_HEAT_BURN,       175,                    150,                87,                 30,  false },
    {FLAP_PROGRAM_STATE_HEAT_BURN_HOT2, FLAP_PROGRAM_STATE_HEAT_BURN_HOT3,  FLAP_PROGRAM_STATE_HEAT_BURN_HOT,   250,                    170,                90,                 30,  false },
    {FLAP_PROGRAM_STATE_HEAT_BURN_HOT3, FLAP_PROGRAM_STATE_HEAT_BURN_HOT3,  FLAP_PROGRAM_STATE_HEAT_BURN_HOT2,  1000,                   200,                92,                 30,  false },
    {FLAP_PROGRAM_STATE_HEAT_OFF,       FLAP_PROGRAM_STATE_HEAT_UP_START,   FLAP_PROGRAM_STATE_HEAT_OFF,        50,                      0,                  0,                 10,  false },
    {FLAP_PROGRAM_STATE_HEAT_RESCUE,    FLAP_PROGRAM_STATE_HEAT_BURN,   FLAP_PROGRAM_STATE_HEAT_OFF,            90,                     40,                 60,                120,  true }

FlameRegulator::programStateConfig_t FlameRegulator::programStateConfigEco[FLAP_PROGRAM_STATE_COUNT]=
        //state                                 nextstate                       laststate               tempForNextState    tempForLastState    currentFlapPosition     breakTimeSec
{
    {FLAP_PROGRAM_STATE_NONE,           FLAP_PROGRAM_STATE_NONE,            FLAP_PROGRAM_STATE_NONE,            0,                      0,                  0,                  0,   false },
    {FLAP_PROGRAM_STATE_REFILL,         FLAP_PROGRAM_STATE_HEAT_BURN,       FLAP_PROGRAM_STATE_HEAT_OFF,        0,                      0,                  0,                  0,   false },
    {FLAP_PROGRAM_STATE_HEAT_UP_START,  FLAP_PROGRAM_STATE_HEAT_UP_1,       FLAP_PROGRAM_STATE_HEAT_OFF,        50,                     30,                 20,                 60,  false },
    {FLAP_PROGRAM_STATE_HEAT_UP_1,      FLAP_PROGRAM_STATE_HEAT_UP_2,       FLAP_PROGRAM_STATE_HEAT_UP_START,   60,                     50,                 40,                 60,  false },
    {FLAP_PROGRAM_STATE_HEAT_UP_2,      FLAP_PROGRAM_STATE_HEAT_UP_3,       FLAP_PROGRAM_STATE_HEAT_UP_1,       70,                     60,                 60,                 60,  false },
    {FLAP_PROGRAM_STATE_HEAT_UP_3,      FLAP_PROGRAM_STATE_HEAT_UP_4,       FLAP_PROGRAM_STATE_HEAT_UP_2,       80,                     70,                 70,                 60,  false },
    {FLAP_PROGRAM_STATE_HEAT_UP_4,      FLAP_PROGRAM_STATE_HEAT_UP_END,     FLAP_PROGRAM_STATE_HEAT_UP_3,       90,                     80,                 75,                 60,  false },
    {FLAP_PROGRAM_STATE_HEAT_UP_END,    FLAP_PROGRAM_STATE_HEAT_BURN,       FLAP_PROGRAM_STATE_HEAT_UP_4,       100,                    90,                 80,                 60,  false },
    {FLAP_PROGRAM_STATE_HEAT_BURN_IDLE, FLAP_PROGRAM_STATE_HEAT_BURN,       FLAP_PROGRAM_STATE_HEAT_RESCUE,     150,                    140,                75,                 20,  true },
    {FLAP_PROGRAM_STATE_HEAT_BURN,      FLAP_PROGRAM_STATE_HEAT_BURN_HOT,   FLAP_PROGRAM_STATE_HEAT_BURN_IDLE,  160,                    140,                86,                 30,  false },
    {FLAP_PROGRAM_STATE_HEAT_BURN_HOT,  FLAP_PROGRAM_STATE_HEAT_BURN_HOT2,  FLAP_PROGRAM_STATE_HEAT_BURN,       175,                    150,                95,                 30,  false },
    {FLAP_PROGRAM_STATE_HEAT_BURN_HOT2, FLAP_PROGRAM_STATE_HEAT_BURN_HOT3,  FLAP_PROGRAM_STATE_HEAT_BURN_HOT,   250,                    170,                98,                 30,  false },
    {FLAP_PROGRAM_STATE_HEAT_BURN_HOT3, FLAP_PROGRAM_STATE_HEAT_BURN_HOT3,  FLAP_PROGRAM_STATE_HEAT_BURN_HOT2,  1000,                   200,               100,                 30,  false },
    {FLAP_PROGRAM_STATE_HEAT_OFF,       FLAP_PROGRAM_STATE_HEAT_UP_START,   FLAP_PROGRAM_STATE_HEAT_OFF,        50,                      0,                  0,                 10,  false },
    {FLAP_PROGRAM_STATE_HEAT_RESCUE,    FLAP_PROGRAM_STATE_HEAT_BURN,   FLAP_PROGRAM_STATE_HEAT_OFF,            90,                     40,                 60,                120,  true }
};


//###################################non member function ##############################
void timerCallback(void* pArg);


//################################### member function #################################

FlameRegulator::FlameRegulator(base* indexPage):
flap{Flap(PWM_OUTPUT, RELAIS_OUTPUT)},
tempSensor{TempSensor(ANALOG_INPUT)}
{
    this->programState = FLAP_PROGRAM_STATE_HEAT_OFF;
    this->flapRegulationMode = FLAP_MODE_POWER;
    this->logger = Logger::instance();
    this->RecogniceInitialState();
    this->mute = true;
    this->secondsToUnmute = -1;
    this->indexPage = indexPage;
    pinMode(BEEP_PIN, OUTPUT);
    
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
    this->action_timeSec = 60;
    this->Mute(600); //mute for 10 minutes
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

    this->logger->Debug("Set manual position");
    this->flap.SetPosition(pos);
}
void FlameRegulator::Beep(bool swOn)
{
  if (swOn)
  {
    digitalWrite(BEEP_PIN, HIGH);
  }
  else
  {
    digitalWrite(BEEP_PIN, LOW);
  }
}

void FlameRegulator::ToggleBeep(void)
{
    static bool beepIsOn = false;
    if (false == beepIsOn)
    {
        digitalWrite(BEEP_PIN, HIGH);
        beepIsOn = true;

    }
    else
    {
        digitalWrite(BEEP_PIN, LOW);
        beepIsOn = false;
    }
}

void FlameRegulator::Mute(int seconds)
{
    if (false == this->mute)
    {
        this->mute = true;
        this->secondsToUnmute = seconds;
    } 
    else
    {
        this->secondsToUnmute = -1;
    }
}

//###################################   state Machine     ##############################

void FlameRegulator::ProgramStateMachine (void)
{
    programStateConfig_t* currentConfig;
    
    if (FLAP_MODE_ECO == this->flapRegulationMode)
    {
        currentConfig = &this->programStateConfigEco[(int)this->programState];
    }
    else
    {
        currentConfig = &this->programStateConfig[(int)this->programState];
    }

    if ((int) 0 <= this->secondsToUnmute)
    {
        this->logger->Debug(String(secondsToUnmute));
        if (0 == this->secondsToUnmute)
        {
            this->mute = false;
        }
        this->secondsToUnmute--;
    }    

    if ((false == this->mute) && (currentConfig->beep))
    {
        this->logger->Debug("Beep");
        this->ToggleBeep();
    }
    else
    {
        this->Beep(false);
    }

    if (0 != this->action_timeSec)
    {
        this->action_timeSec--;
        return;
    }


    // do nothin if manual regulation
    if (FLAP_MODE_MANUAL == this->flapRegulationMode)
    {
        this->logger->Debug("Flap Mode Manual - statemachine inactive");      
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

    this->indexPage->Set_flap(String(this->flap.GetPosition()));
    this->indexPage->Set_slider(String(this->flap.GetPosition()));
    this->indexPage->Set_temp(String(this->tempSensor.GetTemp()));
    this->indexPage->Set_program(this->GetProgramStateStr());
    this->indexPage->Set_state(this->GetFlapRegulationModeStr());
    this->action_timeSec = 10;    

    //check fallback temperature
    if (this->tempSensor.GetTemp() < currentConfig->tempForLastState)
    {
        this->logger->Debug("Set last state");
        this->programState = currentConfig->laststate;
        this->action_timeSec = 0;
        return;
    }

    // check if temp of next state is reached
    if (this->tempSensor.GetTemp() >= currentConfig->tempForNextState)
    {
        this->logger->Debug("Set next state");
        this->programState = currentConfig->nextstate;
        this->action_timeSec = currentConfig->breakTimeSec;
        return;
    }
}

//###################################   private functions    ##############################
void FlameRegulator::RecogniceInitialState(void)
{

    programStateConfig_t config = programStateConfig[FLAP_PROGRAM_STATE_HEAT_OFF];
    int temp = this->tempSensor.GetTemp();
    this->logger->Debug("recognize initial state");

    while (temp >= config.tempForNextState)
    {
        this->logger->Debug(String("new state: ") + this->GetProgramStateStr());
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
