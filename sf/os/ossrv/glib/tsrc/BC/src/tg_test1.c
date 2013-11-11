/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
*
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
*
* Description:
*
*/


#undef	G_LOG_DOMAIN

#undef G_DISABLE_ASSERT
#undef G_DISABLE_CHECKS
#undef G_DISABLE_CAST_CHECKS

#undef VERBOSE
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <gmodule.h>

#include <string.h>

#include <glib.h>
#include <glib-object.h>

#include "testcommon.h"
#include <stdlib.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

typedef struct _BaseObject        BaseObject;

struct _EphyModule
{
	GTypeModule parent_instance;

	GModule *library;

	char *path;
	GType type;
	guint resident : 1;
};

typedef struct _EphyModule EphyModule;

struct _BaseObject
{
  GObject parent_instance;

  gint val1;
  gint val2;
  gint val3;
  gint val4;
};
static guint foo_signal_id = 0;
static guint bar_signal_id = 0;

static GType test_i_get_type (void);
static GType test_a_get_type (void);
static GType test_b_get_type (void);
static GType test_c_get_type (void);

static void  record (const gchar *str);

#define TEST_TYPE_I (test_i_get_type ())
#define DERIVED_TYPE_OBJECT          (derived_object_get_type ())

typedef struct _TestI TestI;
typedef struct _TestIClass TestIClass;

struct _TestIClass
{
  GTypeInterface base_iface;
};

//Added code 
#define TEST_TYPE_IFACE           (test_iface_get_type ())
#define TEST_IFACE(obj)		  (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEST_TYPE_IFACE, TestIface))
#define TEST_IS_IFACE(obj)	  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TEST_TYPE_IFACE))
#define TEST_IFACE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), TEST_TYPE_IFACE, TestIfaceClass))
typedef struct _TestIface      TestIface;
typedef struct _TestIfaceClass TestIfaceClass;
struct _TestIfaceClass
{
  GTypeInterface base_iface;
  void	(*print_string)	(TestIface	*tiobj,
			 const gchar	*string);
};
static void	iface_base_init		(TestIfaceClass	*iface);
static void	iface_base_finalize	(TestIfaceClass	*iface);
static void	print_foo		(TestIface	*tiobj,
					 const gchar	*string);

gpointer my_boxed_copy_function(gpointer boxed)
{
	return boxed;
}

void my_boxed_free_function(gpointer boxed)
{
}

GType
test_iface_get_type (void)
{
  static GType test_iface_type = 0;

  if (!test_iface_type)
    {
      static const GTypeInfo test_iface_info =
      {
	sizeof (TestIfaceClass),
	(GBaseInitFunc)	iface_base_init,		/* base_init */
	(GBaseFinalizeFunc) iface_base_finalize,	/* base_finalize */
      };

      test_iface_type = g_type_register_static (G_TYPE_INTERFACE, "TestIface", &test_iface_info, 0);
      g_type_interface_add_prerequisite (test_iface_type, G_TYPE_OBJECT);
    }

  return test_iface_type;
}
					 
					 
static guint iface_base_init_count = 0;
static void
iface_base_init (TestIfaceClass *iface)
{
  iface_base_init_count++;
  if (iface_base_init_count == 1)
    {
      /* add signals here */
    }
}
static void
iface_base_finalize (TestIfaceClass *iface)
{
  iface_base_init_count--;
  if (iface_base_init_count == 0)
    {
      /* destroy signals here */
    }
}
static void
print_foo (TestIface   *tiobj,
	   const gchar *string)
{
  if (!string)
    string = "<NULL>";
  g_print ("Iface-FOO: \"%s\" from %p\n", string, tiobj);
}
static void
test_object_test_iface_init (gpointer giface,
			     gpointer iface_data)
{
  TestIfaceClass *iface = giface;

  g_assert (iface_data == GUINT_TO_POINTER (42));

  g_assert (G_TYPE_FROM_INTERFACE (iface) == TEST_TYPE_IFACE);

  /* assert iface_base_init() was already called */
  g_assert (iface_base_init_count > 0);

  /* initialize stuff */
  iface->print_string = print_foo;
}

/* --- TestObject --- */
#define TEST_TYPE_OBJECT            (test_object_get_type ())
#define TEST_OBJECT(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), TEST_TYPE_OBJECT, TestObject))
#define TEST_OBJECT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TEST_TYPE_OBJECT, TestObjectClass))
#define TEST_IS_OBJECT(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), TEST_TYPE_OBJECT))
#define TEST_IS_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TEST_TYPE_OBJECT))
#define TEST_OBJECT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), TEST_TYPE_OBJECT, TestObjectClass))
#define TEST_OBJECT_GET_PRIVATE(o)  (G_TYPE_INSTANCE_GET_PRIVATE ((o), TEST_TYPE_OBJECT, TestObjectPrivate))
typedef struct _TestObject        TestObject;
typedef struct _TestObjectClass   TestObjectClass;
typedef struct _TestObjectPrivate TestObjectPrivate;
struct _TestObject
{
  GObject parent_instance;
};

struct _TestObjectClass
{
  GObjectClass parent_class;

  gchar* (*test_signal) (TestObject *tobject,
			 TestIface  *iface_object,
			 gpointer    tdata);
};
struct _TestObjectPrivate
{
  int     dummy1;
  gdouble dummy2;
};
static void	test_object_class_init	(TestObjectClass	*class);
static void	test_object_init	(TestObject		*tobject);
static gboolean	test_signal_accumulator	(GSignalInvocationHint	*ihint,
					 GValue            	*return_accu,
					 const GValue       	*handler_return,
					 gpointer                data);
static gchar*	test_object_test_signal	(TestObject		*tobject,
					 TestIface		*iface_object,
					 gpointer		 tdata);


GType
test_object_get_type (void)
{
  static GType test_object_type = 0;

  if (!test_object_type)
    {
      static const GTypeInfo test_object_info =
      {
	sizeof (TestObjectClass),
	NULL,           /* base_init */
	NULL,           /* base_finalize */
	(GClassInitFunc) test_object_class_init,
	NULL,           /* class_finalize */
	NULL,           /* class_data */
	sizeof (TestObject),
	5,              /* n_preallocs */
	(GInstanceInitFunc) test_object_init,
      };
      GInterfaceInfo iface_info = { test_object_test_iface_init, NULL, GUINT_TO_POINTER (42) };

      test_object_type = g_type_register_static (G_TYPE_OBJECT, "TestObject", &test_object_info, 0);
      g_type_add_interface_static (test_object_type, TEST_TYPE_IFACE, &iface_info);
    }

  return test_object_type;
}

static void
test_object_class_init (TestObjectClass *class)
{
  /*  GObjectClass *gobject_class = G_OBJECT_CLASS (class); */
  guint sig_id;
  GQuark detail;
  gulong hook_id;
  GSignalEmissionHook hook_func;
  gpointer hook_data;
  GDestroyNotify data_destroy;
  gulong handler_id;
  GClosure closure;
  gpointer instance;
  GValue args[2];
                                                                                   
  class->test_signal = test_object_test_signal;

  sig_id = g_signal_new ("test-signal",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST | G_SIGNAL_RUN_LAST | G_SIGNAL_RUN_CLEANUP | G_SIGNAL_DETAILED,
		G_STRUCT_OFFSET (TestObjectClass, test_signal),
		test_signal_accumulator, NULL,
		g_cclosure_marshal_STRING__OBJECT_POINTER,
		G_TYPE_STRING, 2, TEST_TYPE_IFACE, G_TYPE_POINTER);
  
  hook_id = g_signal_add_emission_hook      ( sig_id,
                                             detail,
                                             hook_func,
                                             "Data",
                                             data_destroy);
                                             
              
  g_type_class_add_private (class, sizeof (TestObjectPrivate));
}

static void
print_bar (TestIface   *tiobj,
	   const gchar *string)
{
  TestIfaceClass *parent_iface;

  g_return_if_fail (TEST_IS_IFACE (tiobj));

  if (!string)
    string = "<NULL>";
  g_print ("Iface-BAR: \"%s\" from %p\n", string, tiobj);

  g_print ("chaining: ");
  parent_iface = g_type_interface_peek_parent (TEST_IFACE_GET_CLASS (tiobj));
  parent_iface->print_string (tiobj, string);

  g_assert (g_type_interface_peek_parent (parent_iface) == NULL);
}
static void
test_object_init (TestObject *tobject)
{
  TestObjectPrivate *priv;

  priv = TEST_OBJECT_GET_PRIVATE (tobject);

  g_assert (priv);
  g_assert ((gchar *)priv >= (gchar *)tobject + sizeof (TestObject));

  priv->dummy1 = 54321;
}

static gchar*
test_object_test_signal (TestObject *tobject,
			 TestIface  *iface_object,
			 gpointer    tdata)
{
  g_message ("::test_signal default_handler called");

  g_return_val_if_fail (TEST_IS_IFACE (iface_object), NULL);
  
  return g_strdup ("<default_handler>");
}
static gboolean
test_signal_accumulator (GSignalInvocationHint *ihint,
			 GValue                *return_accu,
			 const GValue          *handler_return,
			 gpointer               data)
{
 const gchar *accu_string = g_value_get_string (return_accu);
const  gchar *new_string = g_value_get_string (handler_return);
  gchar *result_string;

  if (accu_string)
    result_string = g_strconcat (accu_string, new_string, NULL);
  else if (new_string)
    result_string = g_strdup (new_string);
  else
    result_string = NULL;

  g_value_take_string (return_accu, result_string);

  return TRUE;
}

static void
derived_object_test_iface_init (gpointer giface,
				gpointer iface_data)
{
  TestIfaceClass *iface = giface;

  g_assert (iface_data == GUINT_TO_POINTER (87));

  g_assert (G_TYPE_FROM_INTERFACE (iface) == TEST_TYPE_IFACE);

  /* assert test_object_test_iface_init() was already called */
  g_assert (iface->print_string == print_foo);

  /* override stuff */
  iface->print_string = print_bar;
}

/* --- DerivedObject --- */
#define DERIVED_TYPE_OBJECT            (derived_object_get_type ())
#define DERIVED_OBJECT(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), DERIVED_TYPE_OBJECT, DerivedObject))
#define DERIVED_OBJECT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DERIVED_TYPE_OBJECT, DerivedObjectClass))
#define DERIVED_IS_OBJECT(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), DERIVED_TYPE_OBJECT))
#define DERIVED_IS_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DERIVED_TYPE_OBJECT))
#define DERIVED_OBJECT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), DERIVED_TYPE_OBJECT, DerivedObjectClass))
#define DERIVED_OBJECT_GET_PRIVATE(o)  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DERIVED_TYPE_OBJECT, DerivedObjectPrivate))

typedef struct _DerivedObject        DerivedObject;
typedef struct _TestObjectClass      DerivedObjectClass;
typedef struct _DerivedObjectPrivate DerivedObjectPrivate;
struct _DerivedObject
{
  TestObject parent_instance;
  int  dummy1;
  int  dummy2;
};
struct _DerivedObjectPrivate
{
  char dummy;
};
static void derived_object_class_init (DerivedObjectClass *class);
static void derived_object_init       (DerivedObject      *dobject);


GType
derived_object_get_type (void)
{
  static GType derived_object_type = 0;

  if (!derived_object_type)
    {
      static const GTypeInfo derived_object_info =
      {
	sizeof (DerivedObjectClass),
	NULL,           /* base_init */
	NULL,           /* base_finalize */
	(GClassInitFunc) derived_object_class_init,
	NULL,           /* class_finalize */
	NULL,           /* class_data */
	sizeof (DerivedObject),
	5,              /* n_preallocs */
	(GInstanceInitFunc) derived_object_init,
      };
      GInterfaceInfo iface_info = { derived_object_test_iface_init, NULL, GUINT_TO_POINTER (87) };

      derived_object_type = g_type_register_static (TEST_TYPE_OBJECT, "DerivedObject", &derived_object_info, 0);
      g_type_add_interface_static (derived_object_type, TEST_TYPE_IFACE, &iface_info);
    }

  return derived_object_type;
}

static void
derived_object_class_init (DerivedObjectClass *class)
{
  g_type_class_add_private (class, sizeof (DerivedObjectPrivate));
}
static void
derived_object_init (DerivedObject *dobject)
{
  TestObjectPrivate *test_priv;
  DerivedObjectPrivate *derived_priv;

  derived_priv = DERIVED_OBJECT_GET_PRIVATE (dobject);

  g_assert (derived_priv);
  g_assert ((gchar *)derived_priv >= (gchar *)TEST_OBJECT_GET_PRIVATE (dobject) + sizeof (TestObjectPrivate));

  test_priv = TEST_OBJECT_GET_PRIVATE (dobject);
  
  g_assert (test_priv);
  g_assert ((gchar *)test_priv >= (gchar *)dobject + sizeof (TestObject));

}

//addition over

static void
test_i_foo (TestI *self)
{
  record ("TestI::foo");
}

static void
test_i_default_init (gpointer g_class)
{
  gint i;
  const char *name; 
  GSignalQuery query;
  guint retval;
  guint n_ids[4];
  foo_signal_id = g_signal_newv ("foo",
				 TEST_TYPE_I,
				 G_SIGNAL_RUN_LAST,
				 g_cclosure_new(G_CALLBACK(test_i_foo),
						NULL, NULL),
				 NULL, NULL,
				 g_cclosure_marshal_VOID__VOID,
				 G_TYPE_NONE, 0, NULL);
				 
 g_assert(!strcmp(g_signal_name(foo_signal_id), "foo"));
 g_signal_query (foo_signal_id, &query);
 g_assert(query.signal_id == foo_signal_id);
 retval = g_signal_lookup("foo",TEST_TYPE_I);
 g_assert(retval == foo_signal_id);
 
 g_signal_list_ids               (TEST_TYPE_I,
                                             n_ids);                                             
 g_assert(n_ids[0] == foo_signal_id);
 
 i = foo_signal_id ;				 
}

static DEFINE_IFACE (TestI, test_i, NULL, test_i_default_init)

#define TEST_TYPE_A (test_a_get_type())

     typedef struct _TestA TestA;
     typedef struct _TestAClass TestAClass;

struct _TestA {
  GObject parent;
};
struct _TestAClass {
  GObjectClass parent_class;

  void (* bar) (TestA *self);
};

//Dummy function 
gint        GMyCompareDataFunc            (gconstpointer a,
                                             gconstpointer b,
                                             gpointer user_data)
{

return 0;
}
                                             


//Function to test the apis :
static void
test_set_val_apis (void)
{ 
  GType type; 
  gchar c;
  gchar *p="foo";
  gconstpointer ptr;
  gpointer ret_val = NULL;
  gpointer v_object ;
  gchar *retVal;
  GObject *object;
  GParamFlags flags;
  GParamSpec* paramSpec;
  GParamSpec *gpSpec = (GParamSpec * )g_malloc(sizeof (GParamSpec));
  GValue orig = { 0, };
  GValue orig1 = { 1, };
  GValue xform = { 0, };
  GValue param1 = { 0, };
  GTypeModule module ;
  GEnumValue values[] = { {0,"0","0"}, {1,"1","1"}}; 
  GValueArray value_array = {
  {
   0 
  },
  {
   0 
  }};
  
  GValueArray *ret_value_array;
  GCompareFunc compare_func;
  GCompareDataFunc compare_func1 = GMyCompareDataFunc;
  GValue *ret_value; 
  gpointer user_data;
  GParamSpec *param;
  GQuark detail;
  GValue *return_value;
  GObject *ret_object;
  guint t;
  guint n_children = 10;
  GType *ret_gtype;
  GTypePlugin * ret_plugin;
  guint n_prerequisites = 1;
  const GInterfaceInfo interface_info;
  GType flag_type,boxed_type;
  GFlagsValue flag_value = {0,"0","0"};
  int dummy;
  gpointer boxed_value = &dummy;
  GParamSpec *spec;
  gchar *boxed_variable = "abc";

  type = g_enum_register_static ("TestEnum", values); 
 g_value_init (&orig, type); 
 g_value_set_enum (&orig, 5); 
 t = g_value_get_enum(&orig);
 g_assert(t == 5);

 memset (&xform, 0, sizeof (GValue));
 g_value_init (&xform, G_TYPE_CHAR); 
 g_value_set_char (&xform, 10);
 g_assert (xform.data[0].v_int == '\n');

 memset (&xform, 0, sizeof (GValue));
 g_value_init (&xform, G_TYPE_DOUBLE); 
 g_value_set_double (&xform, 100.00);
 g_assert (xform.data[0].v_double == 100.00);
 g_assert (g_value_get_double (&xform) == 100.00);
 
 
 memset (&xform, 0, sizeof (GValue));
 g_value_init (&xform, G_TYPE_FLOAT); 
 g_value_set_float (&xform, 1.0);
 g_assert (xform.data[0].v_float == 1.0);
 g_assert (g_value_get_float(&xform) ==  1.0);

 memset (&xform, 0, sizeof (GValue));
 g_value_init (&xform, G_TYPE_INT64); 
 g_value_set_int64 (&xform, 2555);
 g_assert (xform.data[0].v_int64 == 2555);

 memset (&xform, 0, sizeof (GValue));
 g_value_init (&xform, G_TYPE_LONG); 
 g_value_set_long (&xform, 2555);
 g_assert (xform.data[0].v_long == 2555);

 memset (&xform, 0, sizeof (GValue));
 g_value_init (&xform, G_TYPE_UCHAR); 
 g_value_set_uchar (&xform, 10);
 g_assert (xform.data[0].v_uint == '\n');
 
 memset (&xform, 0, sizeof (GValue));
 g_value_init (&xform, G_TYPE_UINT); 
 g_value_set_uint (&xform, 100);
 g_assert (xform.data[0].v_uint == 100);
 
 memset (&xform, 0, sizeof (GValue));
 g_value_init (&xform, G_TYPE_UINT64); 
 g_value_set_uint64 (&xform, 1000000);
 g_assert (xform.data[0].v_uint64 == 1000000);
 
 memset (&xform, 0, sizeof (GValue));
 g_value_init (&xform, G_TYPE_ULONG); 
 g_value_set_ulong (&xform, 1000000);
 g_assert (xform.data[0].v_ulong == 1000000);
 
 memset (&xform, 0, sizeof (GValue));
 g_value_init (&xform, G_TYPE_STRING); 
 g_value_set_string (&xform, "Hello");
 g_assert (!strcmp(xform.data[0].v_pointer , "Hello"));
 
 memset (&xform, 0, sizeof (GValue));
 g_value_init (&xform, G_TYPE_STRING); 
 g_value_set_static_string (&xform, "Hello");
 g_assert (!strcmp(xform.data[0].v_pointer , "Hello"));
 
 memset (&xform, 0, sizeof (GValue));
 g_value_init (&xform, G_TYPE_POINTER); 
 g_value_set_pointer (&xform, p);
 g_assert (xform.data[0].v_pointer == p);
 
 memset (&xform, 0, sizeof (GValue));
 g_value_init (&xform, G_TYPE_STRING);
 g_value_set_string (&xform, "World"); 
 retVal = g_value_dup_string (&xform);
 g_assert (!strcmp(retVal , "World"));
 
 ret_value_array = g_value_array_copy (&value_array);
 
 g_assert (ret_value_array != NULL );
 
 g_value_array_free(ret_value_array);
 
 ret_value_array = g_value_array_prepend(&value_array, &orig);
 
 g_assert (ret_value_array != NULL );
 
 ret_value = g_value_array_get_nth (&value_array, 0);
 
 g_assert (ret_value_array != NULL );
 
 ret_value_array = g_value_array_sort(&value_array, compare_func);

 g_assert (ret_value_array != NULL ); 

 ret_value_array = g_value_array_sort_with_data  (&value_array,
                                              compare_func1,
                                              user_data);
 
 g_assert (ret_value_array != NULL );

 
 memset (&xform, 0, sizeof (GValue));
 g_value_init (&xform, G_TYPE_PARAM);
 
 g_value_set_param (&xform, NULL);
 
 g_assert (&xform != NULL );
 
 g_value_set_param_take_ownership(&xform, NULL);
 
 g_assert (&xform != NULL ); 
 
 paramSpec = g_value_dup_param (&xform); 
 
 g_assert (paramSpec == NULL );
 
 //g_signal_emitv (&orig, 1, detail, return_value);
 memset (&xform, 0, sizeof (GValue));
 g_value_init (&xform, G_TYPE_POINTER);
 ret_val = g_value_get_pointer(&xform);
 g_assert (ret_val == NULL );
 
 memset (&xform, 0, sizeof (GValue));
 g_value_init (&xform, G_TYPE_OBJECT);
 ret_object = g_value_dup_object(&xform);
 g_assert (ret_object == NULL );
 g_value_set_object_take_ownership (&xform, NULL);
 
 memset (&xform, 0, sizeof (GValue));
 flag_type = g_flags_register_static("TestFlag",&flag_value);
 g_value_init(&xform , flag_type);
 g_value_set_flags (&xform , 1);
 t = g_value_get_flags (&xform);
 g_assert (t == 1);

 memset (&xform, 0, sizeof (GValue));
 boxed_type = g_boxed_type_register_static("TestBoxed",my_boxed_copy_function,my_boxed_free_function);
 g_value_init (&xform, boxed_type);
 g_value_set_boxed (&xform,boxed_variable);
 ret_val = g_value_get_boxed (&xform);
 g_assert (ret_val == boxed_variable);

 g_value_set_static_boxed (&xform, "abc");
 g_value_set_boxed_take_ownership (&xform, "abc");
 g_assert (&xform != NULL);
 ret_val = g_value_dup_boxed (&xform);
 g_assert (ret_val == boxed_variable);
   
 memset (&xform, 0, sizeof (GValue)); 
 g_value_init(&xform,G_TYPE_OBJECT);
 ret_val = g_value_get_object(&xform);
 g_assert (ret_val == NULL);

 memset (&xform, 0, sizeof (GValue)); 
 g_value_init (&xform, G_TYPE_PARAM);
 spec = g_param_spec_int
	(
		"property-a",				/* property name */
 		"a",					/* nickname */
		"Mysterty value 1",			/* description */
		5,					/* minimum */
		10,					/* maximum */
		5,					/* default */
		G_PARAM_READABLE |G_PARAM_WRITABLE	/* GParamSpecFlags */
	);
 g_value_set_param(&xform,spec);
 param = g_value_get_param(&xform);
 g_assert (param == spec);
 
 ret_gtype = g_type_children (G_TYPE_INT, &n_children);
 g_assert (ret_gtype != NULL);
 free(ret_gtype);
 
 ret_plugin = g_type_interface_get_plugin (G_TYPE_INT, G_TYPE_INTERFACE);
 g_assert (ret_plugin == NULL);
 
 ret_gtype = g_type_interface_prerequisites  (G_TYPE_INTERFACE,&n_prerequisites);
 g_assert (ret_gtype != NULL);
 free(ret_gtype);
 
 //g_type_module_set_name (&module, "Name");
 //g_object_set_property (object,
 //                                            NULL,
 //                                            &xform); 
// g_type_class_unref_uncached     (NULL);*/
    
}

//Function to test the apis

static void
test_param_spec_apis (void)
{ 
  GParamFlags flags;
  GParamSpec* paramSpec;
  gchar *qdata = "abc";
  GQuark quark = g_quark_from_string(qdata);
  gpointer retVal;
  gpointer data= "MY data";
  gchar *testData;
  GDestroyNotify destroy;
  GType type;
  gboolean ret;
  GValue orig = { 0, };
  GValue xform = { 0, }; 
  GEnumValue values[] = { {0,"0","0"}, {1,"1","1"}}; 
  GFlagsValue flag_value = {1,"1","1"};
  GType enum_type,flags_type,boxed_type;
  memset (&orig, 0, sizeof (GValue));
  g_value_init (&orig, G_TYPE_POINTER);
  
  memset (&xform, 0, sizeof (GValue));
  g_value_init (&xform, G_TYPE_POINTER);
  
  paramSpec = g_param_spec_uchar("String1","Hello","World", 0,100,10, flags);

  g_assert (!strcmp(paramSpec->_nick , "Hello"));
 
  paramSpec = g_param_spec_uint("String2","Hello","World", 0,255,50, flags);

  g_assert (!strcmp(paramSpec->name , "String2"));
  
  paramSpec = g_param_spec_uint64("String3","Hello","World", 0,255555,1000, flags);
  
  g_assert (!strcmp(paramSpec->name , "String3"));
  
  paramSpec = g_param_spec_ulong("String4","Hello","World", 0,255,100, flags);
  
  g_assert (!strcmp(paramSpec->name , "String4"));
  
  paramSpec = g_param_spec_unichar("String5","Hello","World",'X', flags);
  
  g_assert (!strcmp(paramSpec->name , "String5"));
  
  paramSpec = g_param_spec_string("String6","Hello","World","String", flags);
  
  g_assert (!strcmp(paramSpec->name , "String6"));
  
  paramSpec = g_param_spec_int64("String7","Hello","World",-100,25555,500, flags);
  
  g_assert (!strcmp(paramSpec->name , "String7"));
  
  paramSpec = g_param_spec_long("String8","Hello","World",-100,2555,100, flags);
  
  g_assert (!strcmp(paramSpec->name , "String8"));
  
  g_param_spec_set_qdata(paramSpec,quark,qdata);
  retVal = g_param_spec_steal_qdata(paramSpec, quark);
  g_assert (retVal == qdata);
  
  g_param_spec_set_qdata(paramSpec,100, data);
  
  retVal = g_param_spec_get_qdata(paramSpec,100);
  
  testData = (gchar *)retVal;
  
  g_assert (!strcmp(testData , "MY data"));
  
  g_param_spec_set_qdata_full (paramSpec, 200, data, destroy);
  
  retVal = g_param_spec_get_qdata(paramSpec, 200);
  testData = (gchar *)retVal;
  g_assert (!strcmp(testData , "MY data"));
  
  paramSpec = g_param_spec_object("String9","Hello","World",G_TYPE_OBJECT,flags);
  g_assert (!strcmp(paramSpec->name , "String9"));
  
  paramSpec = g_param_spec_param("String10","Hello","World",G_TYPE_PARAM,flags);
  
  g_assert (!strcmp(paramSpec->name , "String10"));
  
  paramSpec = g_param_spec_pointer("String11", "Hello", "World", flags);
  
  g_assert (!strcmp(paramSpec->name , "String11"));
  
  ret = g_param_value_convert (paramSpec, &orig, &xform, TRUE); 
                     
  g_assert (ret == TRUE);
  
  ret = g_param_value_defaults(paramSpec , &xform);
  
  g_assert (ret == TRUE);
  
  paramSpec = g_param_spec_value_array ("String12", "Hello", "World", NULL, flags);
  
  g_assert (!strcmp(paramSpec->name , "String12"));
  
  paramSpec = g_param_spec_float("String13","Hello", "World",0.00,255.00,100.00,flags);
  
  g_assert (!strcmp(paramSpec->name , "String13"));
  
  paramSpec = g_param_spec_flags("String14","Hello", "World",G_TYPE_FLAGS,0,flags);
  
  g_assert (!strcmp(paramSpec->name , "String14"));
  
  paramSpec = g_param_spec_double("String15","Hello", "World", 0.00,25554.00,100.00,flags);
  
  g_assert (!strcmp(paramSpec->name , "String15"));
  
  paramSpec = g_param_spec_char("String16","Hello", "World", 'a','z','c',flags);
  
  g_assert (!strcmp(paramSpec->name , "String16"));
  
  paramSpec = g_param_spec_boolean("String17", "Hello", "World",TRUE,flags);
  
  g_assert (!strcmp(paramSpec->name , "String17"));

  flags_type = g_flags_register_static("TestFlag1",&flag_value); 
  paramSpec = g_param_spec_flags   ("String18", "Hello", "World", flags_type, 1, flags);                                       
  g_assert (!strcmp(paramSpec->name , "String18"));

  enum_type = g_enum_register_static("TestEnum1",values); 
  paramSpec = g_param_spec_enum ("String19", "Hello", "World", enum_type, 0, flags);
  g_assert (!strcmp(paramSpec->name , "String19"));
  
  boxed_type = g_boxed_type_register_static("TestBoxed1",my_boxed_copy_function,my_boxed_free_function);
  paramSpec =  g_param_spec_boxed ("String20", "Hello", "World", boxed_type, flags);
  g_assert (!strcmp(paramSpec->name , "String20"));
                                                 
}

static void test_object_apis()
{
  BaseObject *object;
  EphyModule *module;
  GObjectClass *object_class;
  GParamSpec **properties;
  gint n_properties;
  gpointer p = "Hello";
  
  gint val1, val2, val3, val4;
	
  g_log_set_always_fatal (g_log_set_always_fatal (G_LOG_FATAL_MASK) |
			  G_LOG_LEVEL_WARNING |
			  G_LOG_LEVEL_CRITICAL);
			  
  g_type_init ();
  
  object = g_object_new (DERIVED_TYPE_OBJECT , NULL);
#if 0
  module = g_object_new (EPHY_TYPE_MODULE, NULL);
  
  ephy_module_new_object(module);
  g_type_module_set_name          (G_TYPE_MODULE (module),
                                             "name");
  
#endif
      
}



static void
test_a_foo (TestI *self)
{
  GValue args[1] = { { 0, } };

  record ("TestA::foo");

  g_value_init (&args[0], TEST_TYPE_A);
  g_value_set_object (&args[0], self);

  g_assert (g_signal_get_invocation_hint (self)->signal_id == foo_signal_id);
  
  g_signal_chain_from_overridden (args, NULL);

  g_value_unset (&args[0]);
}

static void
test_a_bar (TestA *self)
{
  record ("TestA::bar");
}

static void
test_a_class_init (TestAClass *class)
{
  class->bar = test_a_bar;

  bar_signal_id = g_signal_new ("bar",
				TEST_TYPE_A,
				G_SIGNAL_RUN_LAST,
				G_STRUCT_OFFSET (TestAClass, bar),
				NULL, NULL,
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE, 0, NULL);
}

static void
test_a_interface_init (TestIClass *iface)
{
  g_signal_override_class_closure (foo_signal_id,
				   TEST_TYPE_A,
				   g_cclosure_new (G_CALLBACK (test_a_foo),
				   		   NULL, NULL));
}

static DEFINE_TYPE_FULL (TestA, test_a,
			 test_a_class_init, NULL, NULL,
			 G_TYPE_OBJECT,
			 INTERFACE (test_a_interface_init, TEST_TYPE_I))
     
#define TEST_TYPE_B (test_b_get_type())

typedef struct _TestB TestB;
typedef struct _TestBClass TestBClass;

struct _TestB {
  TestA parent;
};
struct _TestBClass {
  TestAClass parent_class;
};

static void
test_b_foo (TestA *self)
{
  GValue args[1] = { { 0, } };

  record ("TestB::foo");

  g_value_init (&args[0], TEST_TYPE_A);
  g_value_set_object (&args[0], self);

  g_assert (g_signal_get_invocation_hint (self)->signal_id == foo_signal_id);
  g_signal_chain_from_overridden (args, NULL);

  g_value_unset (&args[0]);
}

static void
test_b_bar (TestI *self)
{
  GValue args[1] = { { 0, } };

  record ("TestB::bar");

  g_value_init (&args[0], TEST_TYPE_A);
  g_value_set_object (&args[0], self);

  g_assert (g_signal_get_invocation_hint (self)->signal_id == bar_signal_id);
  g_signal_chain_from_overridden (args, NULL);

  g_value_unset (&args[0]);
}

static void
test_b_class_init (TestBClass *class)
{
  g_signal_override_class_closure (foo_signal_id,
				   TEST_TYPE_B,
				   g_cclosure_new (G_CALLBACK (test_b_foo),
						   NULL, NULL));
  g_signal_override_class_closure (bar_signal_id,
				   TEST_TYPE_B,
				   g_cclosure_new (G_CALLBACK (test_b_bar),
						   NULL, NULL));
}

static DEFINE_TYPE (TestB, test_b,
		    test_b_class_init, NULL, NULL,
		    TEST_TYPE_A)

#define TEST_TYPE_C (test_c_get_type())

typedef struct _TestC TestC;
typedef struct _TestCClass TestCClass;

struct _TestC {
  TestB parent;
};
struct _TestCClass {
  TestBClass parent_class;
};

static void
test_c_foo (TestA *self)
{
  GValue args[1] = { { 0, } };

  record ("TestC::foo");

  g_value_init (&args[0], TEST_TYPE_A);
  g_value_set_object (&args[0], self);

  g_assert (g_signal_get_invocation_hint (self)->signal_id == foo_signal_id);
  g_signal_chain_from_overridden (args, NULL);

  g_value_unset (&args[0]);
}

static void
test_c_bar (TestI *self)
{
  GValue args[1] = { { 0, } };
   
  record ("TestC::bar");
  
  g_value_init (&args[0], TEST_TYPE_A);
  g_value_set_object (&args[0], self);
  
  g_assert (g_signal_get_invocation_hint (self)->signal_id == bar_signal_id);
  g_signal_chain_from_overridden (args, NULL);
  
  g_value_unset (&args[0]);
}

static void
test_c_class_init (TestBClass *class)
{
  g_signal_override_class_closure (foo_signal_id,
				   TEST_TYPE_C,
				   g_cclosure_new (G_CALLBACK (test_c_foo),
						   NULL, NULL));
  g_signal_override_class_closure (bar_signal_id,
				   TEST_TYPE_C,
				   g_cclosure_new (G_CALLBACK (test_c_bar),
						   NULL, NULL));
}


static DEFINE_TYPE (TestC, test_c,
		    test_c_class_init, NULL, NULL,
		    TEST_TYPE_B)

static GString *test_string = NULL;
gboolean failed = FALSE;
     
static void
record (const gchar *str)
{
  if (test_string->len)
    g_string_append_c (test_string, ',');
  g_string_append (test_string, str);
}
   
int
main (int argc, char **argv)
{
  gpointer p;

#ifdef SYMBIAN  
  g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
#endif /*SYMBIAN*/
                                                                                       
  g_type_init();
    
  test_i_default_init(p);
  
  test_set_val_apis();
  
  test_param_spec_apis();
  
  test_object_apis();
  
  #if SYMBIAN
  testResultXml("g_test1");
  #endif /* EMULATOR */
  
  return failed ? 1 : 0;
}

