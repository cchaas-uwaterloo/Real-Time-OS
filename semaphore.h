/*
* -----------------------------------------------------------
* Title:          Semaphore declarations
* Project:        RTOS (for MTE 241 Lab 5)
* Authors:        Cameron Haas, Yuchen He
* Date Created:   2019-07-08
* -----------------------------------------------------------
*/

#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H
#include<stdint.h>

typedef struct
{
		uint8_t block_list_bit_vec;
    int8_t count;
    struct osTCB_s *block_list[6];
}sem_t;

void sem_init(sem_t *s, int8_t count);
void wait(sem_t *s);
void signal(sem_t *s);

#endif
