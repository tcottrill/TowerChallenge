#pragma once

#ifndef SMTIMER_H
#define SMTIMER_H

#include <mmsystem.h>
#include "log.h"

#define TIMER_PROC(name) void WINAPI name (UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2) 

#define TIMERS_PER_SECOND   1000
#define SECS_TO_TIMER(x)    ((x) * TIMERS_PER_SECOND)
#define MSEC_TO_TIMER(x)    ((x) * (TIMERS_PER_SECOND / 1000))
#define BPS_TO_TIMER(x)     (TIMERS_PER_SECOND / (x))

void stop_timer(UINT timer);
UINT install_timer_ex( void *callback, long time);
void stop_all_timers();


#endif