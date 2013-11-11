/* GObject - GLib Type, Object, Parameter and Signal Library
 * Copyright (C) 2001, 2003 Red Hat, Inc.
 * Portion Copyright © 2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#undef	G_LOG_DOMAIN
#define	G_LOG_DOMAIN "TestDefaultIface"

#undef G_DISABLE_ASSERT
#undef G_DISABLE_CHECKS
#undef G_DISABLE_CAST_CHECKS

#include <glib-object.h>

#include "testcommon.h"
#include "testmodule.h"
#include "mambaz.h"
#include <stdlib.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif //SYMBIAN

static GType test_dynamic_iface_type;
static GType registered_inteface;
static GType registered_enum;
static GType registered_flag;

static gboolean dynamic_iface_init = FALSE;
GType baz_type;


/* This test tests getting the default vtable for an interface
 * and the initialization and finalization of such default
 * interfaces.
 *
 * We test this both for static and for dynamic interfaces.
 */

/**********************************************************************
 * Static interface tests
 **********************************************************************/

typedef struct _TestStaticIfaceClass TestStaticIfaceClass;

struct _TestStaticIfaceClass
{
  GTypeInterface base_iface;
  guint val;
};

#define TEST_TYPE_STATIC_IFACE (test_static_iface_get_type ())

static void
test_static_iface_default_init (TestStaticIfaceClass *iface)
{
  iface->val = 42;
}

DEFINE_IFACE (TestStaticIface, test_static_iface,
	      NULL, test_static_iface_default_init)

static void
test_static_iface (void)
{
  TestStaticIfaceClass *static_iface;

  /* Not loaded until we call ref for the first time */
  static_iface = g_type_default_interface_peek (TEST_TYPE_STATIC_IFACE);
  g_assert (static_iface == NULL);

  /* Ref loads */
  static_iface = g_type_default_interface_ref (TEST_TYPE_STATIC_IFACE);
  g_assert (static_iface && static_iface->val == 42);

  /* Peek then works */
  static_iface = g_type_default_interface_peek (TEST_TYPE_STATIC_IFACE);
  g_assert (static_iface && static_iface->val == 42);
  
  /* Unref does nothing */
  g_type_default_interface_unref (static_iface);
  
  /* And peek still works */
  static_iface = g_type_default_interface_peek (TEST_TYPE_STATIC_IFACE);
  g_assert (static_iface && static_iface->val == 42);
}

/**********************************************************************
 * Dynamic interface tests
 **********************************************************************/

typedef struct _TestDynamicIfaceClass TestDynamicIfaceClass;

struct _TestDynamicIfaceClass
{
  GTypeInterface base_iface;
  guint val;
};


#define TEST_TYPE_DYNAMIC_IFACE (test_dynamic_iface_type)

static void
test_dynamic_iface_default_init (TestStaticIfaceClass *iface)
{
  dynamic_iface_init = TRUE;
  iface->val = 42;
}

static void
test_dynamic_iface_default_finalize (TestStaticIfaceClass *iface)
{
  dynamic_iface_init = FALSE;
}

static void
test_dynamic_iface_register (GTypeModule *module)
{
  static const GTypeInfo iface_info =			
    {								
      sizeof (TestDynamicIfaceClass),
      (GBaseInitFunc)	   NULL,
      (GBaseFinalizeFunc)  NULL,				
      (GClassInitFunc)     test_dynamic_iface_default_init,
      (GClassFinalizeFunc) test_dynamic_iface_default_finalize
    };							

  test_dynamic_iface_type = g_type_module_register_type (module, G_TYPE_INTERFACE,
							 "TestDynamicIface", &iface_info, 0);
}

static void test_module_add_interface(GTypeModule* module)
{
    static const GTypeInfo info = {
      sizeof (MamanBazClass),
      NULL,   /* base_init */
      NULL,   /* base_finalize */
      NULL,   /* class_init */
      NULL,   /* class_finalize */
      NULL,   /* class_data */
      sizeof (MamanBaz),
      0,      /* n_preallocs */
      baz_instance_init    /* instance_init */
    };
    static const GInterfaceInfo ibaz_info = {
      (GInterfaceInitFunc) baz_interface_init,    /* interface_init */
      NULL,               /* interface_finalize */
      NULL                /* interface_data */
    };
    baz_type = g_type_module_register_type (module, G_TYPE_OBJECT,
                                   "MamanBazType",
                                   &info, 0);

	g_type_module_add_interface     (module,baz_type,MAMAN_TYPE_IBAZ, &ibaz_info);
  
}


static void test_g_type_module_register_flags(GTypeModule* module)
{
	static GFlagsValue flags_array[] =
	{
		{ 1, "EOne", "One"},
		{ 2, "ETwo", "Two"},
		{ 4, "EFour", "Four"},
		{ 0, NULL, NULL},
	};

	registered_flag =  g_type_module_register_flags(module, "egFlag",flags_array);
}


static void test_g_type_module_register_enum(GTypeModule* module)
{
	static GEnumValue enum_array[] =
	{
		{ 1, "EOne", "One"},
		{ 2, "ETwo", "Two"},
		{ 4, "EFour", "Four"},
		{ 0, NULL, NULL},
	};

	registered_enum =  g_type_module_register_enum(module, "egEnum", enum_array);
		
}


static void
module_register (GTypeModule *module)
{
  test_dynamic_iface_register (module);
  test_module_add_interface(module);
  test_g_type_module_register_flags(module);
  test_g_type_module_register_enum(module);
}

static void
test_dynamic_iface (void)
{
  GTypeModule *module;
  TestDynamicIfaceClass *dynamic_iface;

  module = test_module_new (module_register);

  /* Not loaded until we call ref for the first time */
  dynamic_iface = g_type_default_interface_peek (TEST_TYPE_DYNAMIC_IFACE);
  g_assert (dynamic_iface == NULL);

  /* Ref loads */
  dynamic_iface = g_type_default_interface_ref (TEST_TYPE_DYNAMIC_IFACE);
  g_assert (dynamic_iface_init);
  g_assert (dynamic_iface && dynamic_iface->val == 42);

  /* Peek then works */
  dynamic_iface = g_type_default_interface_peek (TEST_TYPE_DYNAMIC_IFACE);
  g_assert (dynamic_iface && dynamic_iface->val == 42);
  
  /* Unref causes finalize */
  g_type_default_interface_unref (dynamic_iface);
  g_assert (!dynamic_iface_init);

  /* Peek returns NULL */
  dynamic_iface = g_type_default_interface_peek (TEST_TYPE_DYNAMIC_IFACE);
  g_assert (dynamic_iface == NULL);
  
  /* Ref reloads */
  dynamic_iface = g_type_default_interface_ref (TEST_TYPE_DYNAMIC_IFACE);
  g_assert (dynamic_iface_init);
  g_assert (dynamic_iface && dynamic_iface->val == 42);

  /* And Unref causes finalize once more*/
  g_type_default_interface_unref (dynamic_iface);
  g_assert (!dynamic_iface_init);
}


void test_flags()
{
	GFlagsValue* retrievedValue;
	GFlagsClass* pPointer = g_type_class_ref(registered_flag);
	if(pPointer)
	{
		retrievedValue = g_flags_get_value_by_name(pPointer,"EOne");
		g_assert(retrievedValue && retrievedValue->value == 1);
		retrievedValue = g_flags_get_value_by_name(pPointer,"EFive");
		g_assert(retrievedValue == NULL);
		g_type_class_unref(pPointer);
	}
}

void test_enum()
{
	GEnumValue* retrievedValue;
	GEnumClass* pPointer = g_type_class_ref(registered_enum);
	if(pPointer)
	{
		retrievedValue = g_enum_get_value_by_name(pPointer,"EOne");
		g_assert(retrievedValue && retrievedValue->value == 1);
		retrievedValue = g_enum_get_value_by_name(pPointer,"EFive");
		g_assert(retrievedValue == NULL);
		g_type_class_unref(pPointer);
	}
}

void test_interface()
{
	MamanBaz* pPointer =     g_object_new(baz_type,NULL);
	if(pPointer)
	{
		g_assert(0xdeadbeaf == pPointer->instance_member);
		maman_ibaz_do_action (MAMAN_IBAZ(pPointer));
		g_assert(10 == pPointer->instance_member);
		g_object_unref(pPointer);
	}
	
}


gboolean    my_cache_func(gpointer cache_data, GTypeClass *g_class)
{
	if(MAMAN_IS_BAZ_CLASS(g_class))
	{
		g_assert(strcmp("hello",(char*) cache_data) == 0);
		g_type_class_ref(baz_type); 

		g_type_class_unref_uncached (g_class);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int
main (int   argc,
      char *argv[])
{
  char* data;	
  #ifdef SYMBIAN
  g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
  g_set_print_handler(mrtPrintHandler);
  #endif /*SYMBIAN*/
  
  g_log_set_always_fatal (g_log_set_always_fatal (G_LOG_FATAL_MASK) |
			  G_LOG_LEVEL_WARNING |
			  G_LOG_LEVEL_CRITICAL);
  g_type_init ();

  data = (char*) malloc(6);
  strcpy(data,"hello");

  g_type_add_class_cache_func(data, my_cache_func);

  test_static_iface ();
  test_dynamic_iface ();
  test_flags();
  test_enum();
  test_interface();

  #ifdef SYMBIAN
  testResultXml("defaultiface");
  #endif //SYMBIAN
  
  return 0;
}
