/* Portion Copyright © 2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.*/
/* This is a process that just sleeps infinitely. */

#include <unistd.h>

int
main ()
{
  while (1)
    sleep (10000000);
  
  return 1;
}
