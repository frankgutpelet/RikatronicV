class Flap
{
public:
  typedef enum tag_error_e
  {
    ERROR_NOT_CALIBRATED = 255
  }error_e;
  
	Flap(int pwmPin, int relaisPin, int adcPin);
	void SetPosition(int percentage);
	void SwitchOn();
	void SwitchOff();
	int GetPosition();
  void Calibrate(); //Dauert 2:10 min
private:
	int position;
  int adcPin, pwmPin, relaisPin; 
	bool isOn;
  bool isCalibrated;
  int calibrationFactorLow;
  int calibrationFactorHigh;
};
