/*
* -----------------------------------------------------------
* Title:          Mutex declarations
* Project:        RTOS (for MTE 241 Lab 5)
* Authors:        Cameron Haas, Yuchen He
* Date Created:   2019-07-08
* -----------------------------------------------------------
*/

#ifndef __MUTEX_H
#define __MUTEX_H
#include<stdint.h>

typedef struct
{
		uint8_t block_list_bit_vec;
    int8_t count;
    struct osTCB_s *block_list[6];
		uint8_t owner_index; 
		uint8_t owner_priority;
		uint8_t owner_inherited_priority_flag;
		uint8_t inherited_priority;
}mutex_t;

void mutex_init(mutex_t *m);
void acquire_mutex(mutex_t *m);
void release_mutex(mutex_t *m);

#endif
