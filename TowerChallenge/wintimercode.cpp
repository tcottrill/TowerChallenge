#include <Windows.h>
#include <mmsystem.h>
#include "log.h"
#include "wintimercode.h"
#include "simpletimer.h"
#include "framework.h"

volatile int logic_counter;       //Counter for keyrepeat
volatile int ani_counter;         //Counter for screen animations
volatile int fps_counter;         //Frame per second counter (Needs removed/revised with new aae code)
volatile int sav_counter;         //Counter for screen saver 


#pragma comment( lib, "winmm.lib") //timeGetTime

#define TIMERS_PER_SECOND   1000
#define SECS_TO_TIMER(x)    ((x) * TIMERS_PER_SECOND)
#define MSEC_TO_TIMER(x)    ((x) * (TIMERS_PER_SECOND / 1000))
#define BPS_TO_TIMER(x)     (TIMERS_PER_SECOND / (x))


void CALLBACK key_timer_callback(UINT uTimerID, UINT uMsg, DWORD_PTR param, DWORD_PTR dw1, DWORD_PTR dw2)
{ 
	logic_counter++;
	//Failsafe
	if (logic_counter > 0xffff) logic_counter=0;
} 


void CALLBACK ani_counter_callback(UINT uTimerID, UINT uMsg, DWORD_PTR param, DWORD_PTR dw1, DWORD_PTR dw2)
{ 
	ani_counter++;
	//Failsave
	if (ani_counter > 0xffff) ani_counter=0;
} 


void CALLBACK sav_counter_callback(UINT uTimerID, UINT uMsg, DWORD_PTR param, DWORD_PTR dw1, DWORD_PTR dw2)
{ 
	sav_counter++;
	//Failsave
	if (sav_counter > 0xffff) sav_counter=0;
} 


void timers_init(void)
{
	logic_counter = 0;
	ani_counter = 0;
	fps_counter = 0;
	sav_counter = 0;

	//Set multimedia timer resolution to 1ms
	//timeBeginPeriod(1);

	install_timer_ex(key_timer_callback, MSEC_TO_TIMER(20));
	install_timer_ex(ani_counter_callback, MSEC_TO_TIMER(275));
	install_timer_ex(sav_counter_callback, SECS_TO_TIMER(60));//MSEC_TO_TIMER(60000));
}


void timers_shutdown()
{
 	stop_all_timers();
}


