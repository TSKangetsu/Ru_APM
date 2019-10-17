#include <wiringPi.h>
#include <softTone.h>
#define BuzzPin 25

#define CL0 0

#define CL1 131
#define CL2 147
#define CL3 165
#define CL4 175
#define CL5 196
#define CL6 221
#define CL7 248

#define CM1 262
#define CM2 294
#define CM3 330
#define CM4 350
#define CM5 393
#define CM6 441
#define CM7 495

#define CH1 525
#define CH2 589
#define CH3 661
#define CH4 700
#define CH5 786
#define CH6 882
#define CH7 990


int song_1[] = { CM5,CL0,CM5,CM4,CM4,CM6,CM5,CM1,CM4,CM3,CM2,CM3,CM1,CM5,CL0,CM5,CM7,CH1,CM5,CL0};

double beat_1[] = { 2,0.1,1,1,1,1,2,1,1,1,1,1,2,0.1,2,1,1,1,3,2};
inline int yongchang()
{
	int i, j;

	if (wiringPiSetup() == -1) {
		printf("14CORE | WiringPi initialization failed !");
		return 1;
	}

	if (softToneCreate(BuzzPin) == -1) {
		printf("14CORE| Soft Tone Failed !");
		return 1;
	}

	while (1) {
		printf("14CORE| Sound is generated...\n");

		for (i = 0; i < sizeof(song_1) / 4; i++) {
			softToneWrite(BuzzPin, song_1[i]);
			delay(beat_1[i] * 500);
		}
	}

	return 0;
}