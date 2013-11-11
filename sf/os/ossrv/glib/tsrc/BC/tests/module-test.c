/* module-test.c - test program for GMODULE
 * Copyright (C) 1998 Tim Janik
 * Portion Copyright © 2008-09 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/. 
 */

#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#include <gmodule.h>
#include <string.h>
#include <stdio.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/


typedef	int (*SimpleFunc) (void);

int
main (int   arg,
      char *argv[])
{
  GModule *module_a, *module_b;
  gchar *plugin_a, *plugin_b;
  SimpleFunc f_a, f_b;
  int retVal;
  void *fun1,*fun2;
  gchar *build_path = NULL;
  gchar temp_build_path[100];
  gchar *lib_name = "xyz";

  #ifdef SYMBIAN
  g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
  g_set_print_handler(mrtPrintHandler);
  #endif /*SYMBIAN*/
	  

  if (!g_module_supported ())
    g_print ("dynamic modules not supported");
 
  plugin_a  = "libmoduletestplugin_a.dll";
  plugin_b  = "libmoduletestplugin_b.dll";
  
  build_path = g_module_build_path("c:\\sys\\bin",lib_name);
  g_assert(!strcmp(build_path,"c:\\sys\\bin\\xyz.dll"));
  g_free(build_path);
  
  build_path = g_module_build_path(NULL,lib_name);
  g_assert(!strcmp(build_path,"xyz.dll"));
  g_free(build_path);

  /* module handles */

  module_a = g_module_open (plugin_a, G_MODULE_BIND_LAZY);
  if (!module_a)
    g_print ("error: %s", g_module_error ());

  module_b = g_module_open (plugin_b, G_MODULE_BIND_LAZY);
  if (!module_b)
    g_print ("error: %s", g_module_error ());

  /* get plugin specific symbols and call them
   */
  if (!g_module_symbol (module_a, /*"gplugin_a_func1"*/"1", /*(gpointer *) &f_a*/&fun1))
    g_print ("error: %s", g_module_error ());
   
  if (!g_module_symbol (module_b, /*"gplugin_b_func1"*/"1", /*(gpointer *) &f_b)*/&fun2))
      g_print ("error: %s", g_module_error ());
   
  f_a = (SimpleFunc)fun1;
  f_b = (SimpleFunc)fun2;
  
  retVal = f_a ();
  g_assert(retVal == 1);
  
  retVal = f_b ();
  g_assert(retVal == 1);
  
  if (!g_module_symbol (module_a, /*"gplugin_a_func2"*/"2", (gpointer *) &f_a))
    g_print ("error: %s", g_module_error ());
   
  if (!g_module_symbol (module_b, /*"gplugin_b_func2"*/"2", (gpointer *) &f_b))
    g_print ("error: %s", g_module_error ());
  
  retVal = f_a ();
  g_assert(retVal == 2);
  
  retVal = f_b ();
  g_assert(retVal == 2);
  
  //checks g_module_name
  g_assert(!strcmp(g_module_name(module_a),"libmoduletestplugin_a.dll"));
  
  g_module_make_resident(module_a);
  
  /* unload plugins  */
  
  //g_module_close is ignored because g_module_make_resident(module_a) is called
  if (!g_module_close (module_a))
    g_print ("error: %s", g_module_error ());

  if (!g_module_close (module_b))
    g_print ("error: %s", g_module_error ());
  
  
  // As g_module_make_resident(module_a) is called g_module_close is ignored and therefore
  // we are able to get the symbol from the library and execute the funtion also
  if (!g_module_symbol (module_a, /*"gplugin_a_func1"*/"1", /*(gpointer *) &f_a*/&fun1))
    g_print ("error: %s", g_module_error ());
      
  f_a = (SimpleFunc)fun1;
  
  retVal = f_a ();
  g_assert(retVal == 1);
  
  #ifdef SYMBIAN
  testResultXml("module-test");
  #endif /* EMULATOR */
 
  return 0;
}
