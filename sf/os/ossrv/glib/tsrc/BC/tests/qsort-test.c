/* Portion Copyright © 2008-09 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.*/
#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#include <glib.h>

#include <stdio.h>
#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/


#define SIZE 1000//00

guint32 array[SIZE];

static gint
sort (gconstpointer a, gconstpointer b, gpointer user_data)
{
  return *(guint32*)a < *(guint32*)b ? -1 : 1;
}

int
main ()
{
  int i;

  #ifdef SYMBIAN
  g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
  g_set_print_handler(mrtPrintHandler);
  #endif /*SYMBIAN*/
	  

  for (i = 0; i < SIZE; i++)
    array[i] = g_random_int ();

  g_qsort_with_data (array, SIZE, sizeof (guint32), sort, NULL);

  for (i = 0; i < SIZE - 1; i++)
    g_assert (array[i] <= array[i+1]);

  /* 0 elemenents is a valid case */
  g_qsort_with_data (array, 0, sizeof (guint32), sort, NULL);

#ifdef SYMBIAN
  testResultXml("qsort-test");
#endif /* EMULATOR */
  return 0;
}
