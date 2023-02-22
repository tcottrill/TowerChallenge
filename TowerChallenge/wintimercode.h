#ifndef WINTIMERCODE_H
#define WINTIMERCODE_H

extern volatile int logic_counter;       //Counter for keyrepeat
extern volatile int ani_counter;         //Counter for screen animations
extern volatile int fps_counter;         //Frame per second counter (Needs removed/revised with new aae code)
extern volatile int sav_counter;         //Counter for screen saver 

extern void timers_init();
extern void timers_shutdown();

#endif