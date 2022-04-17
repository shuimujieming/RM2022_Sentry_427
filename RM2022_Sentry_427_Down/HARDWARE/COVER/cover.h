#ifndef __COVER_H
#define __COVER_H

#include "main.h"
void Cover_Init();

#define COVER_OPEN_VAL 1200
#define COVER_CLOSE_VAL 1000

#define COVER_ON	TIM_SetCompare4(TIM4,COVER_OPEN_VAL)
#define COVER_OFF	TIM_SetCompare4(TIM4,COVER_CLOSE_VAL)


#endif