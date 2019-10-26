#include "M_RECsetter.hpp"

#ifdef linux
I2Ccontrol_REC::flight_control_connector::flight_control_connector()
{
	PCA9658_fd = pca9685Setup(PCA9658_PinBase, PCA9658_Address, PWM_State.APWM_Freq);
	PWM_State.Free_Throttle = (600 - 270) / 2;
}

void I2Ccontrol_REC::flight_control_connector::MotorStartUP()
{
	pca9685PWMWrite(PCA9658_fd, PWM_State.PWM_Throttle_PIN, 0, PWM_State.PWM_Throttle_StartUP);
	pca9685PWMWrite(PCA9658_fd, PWM_State.PWM_Course_PIN, 0, PWM_State.PWM_Course__StartUP);
	pca9685PWMWrite(PCA9658_fd, PWM_State.PWM_Front_Subduction_PIN, 0, PWM_State.PWM_Front_Subduction__StartUP);
	pca9685PWMWrite(PCA9658_fd, PWM_State.PWM_Rear_Subduction_PIN, 0, PWM_State.PWM_Rear_Subduction_StartUP);
	PWM_State.PWM_Throttle = PWM_State.PWM_Throttle_StartUP;
	PWM_State.PWM_Course = PWM_State.PWM_Course__StartUP;
	PWM_State.PWM_Front_Subduction = PWM_State.PWM_Front_Subduction__StartUP;
	PWM_State.PWM_Rear_Subduction = PWM_State.PWM_Rear_Subduction_StartUP;
	sleep(1);

	pca9685PWMWrite(PCA9658_fd, PWM_State.PWM_Course_PIN, 0, PWM_State.Free_Throttle);
	pca9685PWMWrite(PCA9658_fd, PWM_State.PWM_Front_Subduction_PIN, 0, PWM_State.Free_Throttle);
	pca9685PWMWrite(PCA9658_fd, PWM_State.PWM_Rear_Subduction_PIN, 0, PWM_State.Free_Throttle);
	PWM_State.PWM_Course = PWM_State.Free_Throttle;
	PWM_State.PWM_Front_Subduction = PWM_State.Free_Throttle;
	PWM_State.PWM_Rear_Subduction = PWM_State.Free_Throttle;

	PWM_State.PWM_STARTED = true;
}

void I2Ccontrol_REC::flight_control_connector::MotorStop()
{
	pca9685PWMWrite(PCA9658_fd, PWM_State.PWM_Throttle_PIN, 0, PWM_State.PWM_Throttle_StartUP);
	pca9685PWMWrite(PCA9658_fd, PWM_State.PWM_Course_PIN, 0, PWM_State.PWM_Course__StartUP);
	pca9685PWMWrite(PCA9658_fd, PWM_State.PWM_Front_Subduction_PIN, 0, PWM_State.PWM_Front_Subduction__StartUP);
	pca9685PWMWrite(PCA9658_fd, PWM_State.PWM_Rear_Subduction_PIN, 0, PWM_State.PWM_Rear_Subduction_StartUP);
	PWM_State.PWM_Throttle = PWM_State.PWM_Throttle_StartUP;
	PWM_State.PWM_Course = PWM_State.PWM_Course__StartUP;
	PWM_State.PWM_Front_Subduction = PWM_State.PWM_Front_Subduction__StartUP;
	PWM_State.PWM_Rear_Subduction = PWM_State.PWM_Rear_Subduction_StartUP;
	sleep(1);

	pca9685PWMWrite(PCA9658_fd, PWM_State.PWM_Course_PIN, 0, PWM_State.Free_Throttle);
	pca9685PWMWrite(PCA9658_fd, PWM_State.PWM_Front_Subduction_PIN, 0, PWM_State.Free_Throttle);
	pca9685PWMWrite(PCA9658_fd, PWM_State.PWM_Rear_Subduction_PIN, 0, PWM_State.Free_Throttle);
	PWM_State.PWM_Course = PWM_State.Free_Throttle;
	PWM_State.PWM_Front_Subduction = PWM_State.Free_Throttle;
	PWM_State.PWM_Rear_Subduction = PWM_State.Free_Throttle;

	PWM_State.PWM_STARTED = false;
}
#endif