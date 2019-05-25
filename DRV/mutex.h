#pragma once

#include <stdbool.h>

typedef volatile bool mutex_t;

#define unlock(mutex) do{mutex=false;}while(0)
#define lock(mutex) do{mutex=true;}while(0)
#define iflockret(mutex) do{if(mutex)return;}while(0)
