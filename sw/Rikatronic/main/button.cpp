#include <Arduino.h>
#include "button.hpp"
#define BUTTON_PIN 14

Button::Button()
{

}

bool Button::IsPressed()
{
	if (LOW == digitalRead(BUTTON_PIN))
		return true;
	return false;
}
