/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/




#include <liboil/liboil.h>
#include <liboil/liboildebug.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include <liboil/globals.h>

#define LOG_FILE "c:\\logs\\examples_oil-random_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("examples_oil-random");
    close_log_file();
}
typedef struct _OilRandomState OilRandomState;

struct _OilRandomState {
  int index;
  uint32_t mt[624];
  uint32_t bits[624];
};

OilRandomState state;

static void _oil_random_init (void);
uint32_t oil_random_get_int (OilRandomState *state);
void oil_random_get_bits (OilRandomState *state, uint8_t *dest, int n);

int main (int argc, char *argv[])
{
  int i;
  uint32_t a[10];

  oil_init();

  _oil_random_init();

  for(i=0;i<10;i++){
  std_log(LOG_FILENAME_LINE,"%d\n", oil_random_get_int(&state));
  }

  oil_random_get_bits (&state, (void *)a, 10*4);
  for(i=0;i<10;i++){
  std_log(LOG_FILENAME_LINE,"%d\n", a[i]);
  }
    if(assert_failed)
          std_log(LOG_FILENAME_LINE,"Test Fail");
    else
          std_log(LOG_FILENAME_LINE,"Test Successful");
    create_xml(0);

}


void
oil_random_state_seed (OilRandomState *state, uint32_t seed)
{
  uint32_t *mt = state->mt;
  int i;

  mt[0] = seed;
  for(i=1;i<624;i++) {
    mt[i] = (1812433253UL * (mt[i-1] ^ (mt[i-1] >> 30)) + i);
  }
  oil_mt19937 (state->bits, state->mt);
  state->index = 0;
}



static void
_oil_random_init (void)
{
  int seed;

  seed = time(NULL);
  OIL_ERROR("seed is %d", seed);

  oil_random_state_seed (&state, seed);
}

uint32_t
oil_random_get_int (OilRandomState *state)
{
  if (state->index >= 624) {
    oil_mt19937 (state->bits, state->mt);
    state->index = 0;
  }
  return state->bits[state->index++];
}

void
oil_random_get_bits (OilRandomState *state, uint8_t *dest, int n)
{
  int i = state->index * 4;
  int m;

  while(n > 0) {
    if (i >= 624*4) {
      oil_mt19937 (state->bits, state->mt);
      i = 0;
    }

    m = n;
    if (m > 624*4 - i) m = 624*4 - i;

    memcpy (dest, ((uint8_t *)state->bits) + i, m);
    i += m;
    n -= m;
  }
  state->index = (i+3)/4;
}


