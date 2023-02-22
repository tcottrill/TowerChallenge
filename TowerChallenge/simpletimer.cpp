/////////////////////////////////////////////////////////////////////
#include <windows.h>
#include "simpletimer.h"
#include <vector>
#include <timeapi.h>

//OLD SCHOOL
static int timernum = 0;

#pragma comment( lib, "winmm.lib")

std::vector<UINT> timer_item;

static bool initialized = false;

//void WINAPI onTimeFunc (UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2); 

//timeBeginPeriod(1)     timeEndPeriod(1);


UINT install_timer_ex( void *callback, long time)
 {
	 if (!initialized)
	 {
	   timeBeginPeriod(1);
	   initialized = true;
	 }
	// timer_item.push_back() = timeSetEvent (time, 1, (LPTIMECALLBACK) callback, DWORD (1), TIME_PERIODIC); 
   timernum++;
   wrlog("Timernum here is %d size is %d", timernum, timer_item.size());

   UINT evt = timeSetEvent(time, 1, (LPTIMECALLBACK)callback, DWORD(1), TIME_PERIODIC);
   timer_item.push_back(evt);

   wrlog("Timeritem here is %d",evt);
   return evt;
 }


 void stop_timer(UINT timer)
 {
	 timeKillEvent(timer);

 }

 
 void stop_all_timers()
 {
	 if (timer_item.size())
	 {
	   for (unsigned i=0; i < timer_item.size(); i++)
       timeKillEvent (timer_item[i]); 
	 }
	 timer_item.clear();
	 
	 if (initialized){ timeEndPeriod(1); initialized = false;}
 }

