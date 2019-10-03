#include "M_RECsetter.hpp"

I2Ccontrol_REC::PCA9685_AirSet::PCA9685_AirSet()
{
	PCA9658_fd = pca9685Setup(PCA9658_PinBase, PCA9658_Address, PWM_State.APWM_Freq);

	pca9685PWMWrite(PCA9658_fd, PWM_State.PWM_Throttle_PIN, 0, PWM_State.PWM_Throttle_StartUP);
	pca9685PWMWrite(PCA9658_fd, PWM_State.PWM_Course_PIN, 0, PWM_State.PWM_Course__StartUP);
	pca9685PWMWrite(PCA9658_fd, PWM_State.PWM_Front_Subduction_PIN, 0, PWM_State.PWM_Front_Subduction__StartUP);
	pca9685PWMWrite(PCA9658_fd, PWM_State.PWM_Rear_Subduction_PIN, 0, PWM_State.PWM_Rear_Subduction_StartUP);
}