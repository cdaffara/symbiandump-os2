/* GObject - GLib Type, Object, Parameter and Signal Library
 * Copyright (C) 2001, 2003 Red Hat, Inc.
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#undef	G_LOG_DOMAIN
//#define	G_LOG_DOMAIN "TestIfaceProperties"

#undef G_DISABLE_ASSERT
#undef G_DISABLE_CHECKS
#undef G_DISABLE_CAST_CHECKS


#include <string.h>
#include <glib-object.h>

#include <gobject_global.h>

#include "testcommon.h"

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/


#include <glib_global.h>
static guint foo_signal_id = 0; //Amarjeet

enum {
  MAMAN_BAR_CONSTRUCT_NAME = 1,
  MAMAN_BAR_PAPA_NUMBER,
};


/* This test tests interface properties, implementing interface
 * properties and #GParamSpecOverride.
 *
 * Four properties are tested:
 *
 * prop1: Defined in TestIface, Implemented in BaseObject with a GParamSpecOverride
 * prop2: Defined in TestIface, Implemented in BaseObject with a new property
 * prop3: Defined in TestIface, Implemented in BaseObject, Overridden in DerivedObject
 * prop4: Defined in BaseObject, Overridden in DerivedObject
 */

static GType base_object_get_type ();
static GType derived_object_get_type ();

enum {
  BASE_PROP_0,
  BASE_PROP1,
  BASE_PROP2,
  BASE_PROP3,
  BASE_PROP4
};

enum {
  DERIVED_PROP_0,
  DERIVED_PROP3,
  DERIVED_PROP4
};

/*
 * BaseObject, a parent class for DerivedObject
 */
#define BASE_TYPE_OBJECT          (base_object_get_type ())
#define BASE_OBJECT(obj)	  (G_TYPE_CHECK_INSTANCE_CAST ((obj), BASE_TYPE_OBJECT, BaseObject))
typedef struct _BaseObject        BaseObject;
typedef struct _BaseObjectClass   BaseObjectClass;

struct _BaseObject
{
  GObject parent_instance;

  gint val1;
  gint val2;
  gint val3;
  gint val4;
};
struct _BaseObjectClass
{
  GObjectClass parent_class;
};

GObjectClass *base_parent_class;

/*
 * DerivedObject, the child class of DerivedObject
 */
#define DERIVED_TYPE_OBJECT          (derived_object_get_type ())
typedef struct _DerivedObject        DerivedObject;
typedef struct _DerivedObjectClass   DerivedObjectClass;

struct _DerivedObject
{
  BaseObject parent_instance;
};
struct _DerivedObjectClass
{
  BaseObjectClass parent_class;
};

/*
 * The interface
 */
typedef struct _TestIfaceClass TestIfaceClass;

struct _TestIfaceClass
{
  GTypeInterface base_iface;
};

#define TEST_TYPE_IFACE (test_iface_get_type ())

/* The paramspecs installed on our interface
 */
static GParamSpec *iface_spec1, *iface_spec2, *iface_spec3;

/* The paramspecs inherited by our derived object
 */
static GParamSpec *inherited_spec1, *inherited_spec2, *inherited_spec3, *inherited_spec4;



static void
test_iface_default_init (TestIfaceClass *iface_vtable)
{
  inherited_spec1 = iface_spec1 = g_param_spec_int ("prop1",
						    "Prop1",
						    "Property 1",
						    G_MININT, /* min */
						    0xFFFF,  /* max */
						    42,       /* default */
						    (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
  g_object_interface_install_property (iface_vtable, iface_spec1);

  iface_spec2 = g_param_spec_int ("prop2",
				  "Prop2",
				  "Property 2",
				  G_MININT, /* min */
				  G_MAXINT, /* max */
				  0,	       /* default */
				  G_PARAM_WRITABLE);
  g_object_interface_install_property (iface_vtable, iface_spec2);
    
  inherited_spec3 = iface_spec3 = g_param_spec_int ("prop3",
						    "Prop3",
						    "Property 3",
						    G_MININT, /* min */
						    G_MAXINT, /* max */
						    0,	       /* default */
						    G_PARAM_READWRITE);
  g_object_interface_install_property (iface_vtable, iface_spec3);
}

static DEFINE_IFACE (TestIface, test_iface, NULL, test_iface_default_init)


static GObject*
base_object_constructor  (GType                  type,
			  guint                  n_construct_properties,
			  GObjectConstructParam *construct_properties)
{
  /* The constructor is the one place where a GParamSpecOverride is visible
   * to the outside world, so we do a bunch of checks here
   */
  GValue value1 = { 0, };
  GValue value2 = { 0, };
  GParamSpec *pspec;

  g_assert (n_construct_properties == 1);

  pspec = construct_properties->pspec;

  /* Check we got the param spec we expected
   */
  g_assert (G_IS_PARAM_SPEC_OVERRIDE (pspec));
  g_assert (pspec->param_id == BASE_PROP1);
  g_assert (strcmp (g_param_spec_get_name (pspec), "prop1") == 0);
  g_assert (g_param_spec_get_redirect_target (pspec) == iface_spec1);

  /* Test redirection of the nick and blurb to the redirect target
   */
  g_assert (strcmp (g_param_spec_get_nick (pspec), "Prop1") == 0);
  g_assert (strcmp (g_param_spec_get_blurb (pspec), "Property 1") == 0);

  /* Test forwarding of the various GParamSpec methods to the redirect target
   */
  g_value_init (&value1, G_TYPE_INT);
  g_value_init (&value2, G_TYPE_INT);
  
  g_param_value_set_default (pspec, &value1);
  g_assert (g_value_get_int (&value1) == 42);

  g_value_reset (&value1);
  g_value_set_int (&value1, 0x10000);
  g_assert (g_param_value_validate (pspec, &value1));
  g_assert (g_value_get_int (&value1) == 0xFFFF);
  g_assert (!g_param_value_validate (pspec, &value1));
  
  g_value_reset (&value1);
  g_value_set_int (&value1, 1);
  g_value_set_int (&value2, 2);
  g_assert (g_param_values_cmp (pspec, &value1, &value2) < 0);
  g_assert (g_param_values_cmp (pspec, &value2, &value1) > 0);
  
  g_value_unset (&value1);
  g_value_unset (&value2);

  return base_parent_class->constructor (type,
					 n_construct_properties,
					 construct_properties);
}

static void
base_object_set_property (GObject      *object,
			  guint         prop_id,
			  const GValue *value,
			  GParamSpec   *pspec)
{
  BaseObject *base_object = BASE_OBJECT (object);
  
  switch (prop_id)
    {
    case BASE_PROP1:
      g_assert (pspec == inherited_spec1);
      base_object->val1 = g_value_get_int (value);
      break;
    case BASE_PROP2:
      g_assert (pspec == inherited_spec2);
      base_object->val2 = g_value_get_int (value);
      break;
    case BASE_PROP3:
      g_assert_not_reached ();
      break;
    case BASE_PROP4:
      g_assert_not_reached ();
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
base_object_get_property (GObject                *object,
			  guint                   prop_id,
			  GValue                 *value,
			  GParamSpec             *pspec)
{
  BaseObject *base_object = BASE_OBJECT (object);

  switch (prop_id)
    {
    case BASE_PROP1:
      g_assert (pspec == inherited_spec1);
      g_value_set_int (value, base_object->val1);
      break;
    case BASE_PROP2:
      g_assert (pspec == inherited_spec2);
      g_value_set_int (value, base_object->val2);
      break;
    case BASE_PROP3:
      g_assert_not_reached ();
      break;
    case BASE_PROP4:
      g_assert_not_reached ();
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
base_object_notify (GObject    *object,
		    GParamSpec *pspec)
{
  /* The property passed to notify is the redirect target, not the
   * GParamSpecOverride
   */
  g_assert (pspec == inherited_spec1 ||
	    pspec == inherited_spec2 ||
	    pspec == inherited_spec3 ||
	    pspec == inherited_spec4);
}

static void
base_object_class_init (BaseObjectClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  base_parent_class= g_type_class_peek_parent (class);

  object_class->constructor = base_object_constructor;
  object_class->set_property = base_object_set_property;
  object_class->get_property = base_object_get_property;
  object_class->notify = base_object_notify;

  g_object_class_override_property (object_class, BASE_PROP1, "prop1");

  /* We override this one using a real property, not GParamSpecOverride
   * We change the flags from READONLY to READWRITE to show that we
   * can make the flags less restrictive
   */
  inherited_spec2 = g_param_spec_int ("prop2",
				      "Prop2",
				      "Property 2",
				      G_MININT, /* min */
				      G_MAXINT, /* max */
				      0,	       /* default */
				      G_PARAM_READWRITE);
  g_object_class_install_property (object_class, BASE_PROP2, inherited_spec2);
  
  g_object_class_override_property (object_class, BASE_PROP3, "prop3");
  
  inherited_spec4 = g_param_spec_int ("prop4",
				      "Prop4",
				      "Property 4",
				      G_MININT, /* min */
				      G_MAXINT, /* max */
				      0,	       /* default */
				      G_PARAM_READWRITE);
  g_object_class_install_property (object_class, BASE_PROP4, inherited_spec4);
}

static void
base_object_init (BaseObject *base_object)
{
  base_object->val1 = 42;
}

static DEFINE_TYPE_FULL (BaseObject, base_object,
			 base_object_class_init, NULL, base_object_init,
			 G_TYPE_OBJECT,
			 INTERFACE (NULL, TEST_TYPE_IFACE))

static void
derived_object_set_property (GObject      *object,
			     guint         prop_id,
			     const GValue *value,
			     GParamSpec   *pspec)
{
  BaseObject *base_object = BASE_OBJECT (object);

  switch (prop_id)
    {
    case DERIVED_PROP3:
      g_assert (pspec == inherited_spec3);
      base_object->val3 = g_value_get_int (value);
      break;
    case DERIVED_PROP4:
      g_assert (pspec == inherited_spec4);
      base_object->val4 = g_value_get_int (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
derived_object_get_property (GObject                *object,
			     guint                   prop_id,
			     GValue                 *value,
			     GParamSpec             *pspec)
{
  BaseObject *base_object = BASE_OBJECT (object);

  switch (prop_id)
    {
    case DERIVED_PROP3:
      g_assert (pspec == inherited_spec3);
      g_value_set_int (value, base_object->val3);
      break;
    case DERIVED_PROP4:
      g_assert (pspec == inherited_spec4);
      g_value_set_int (value, base_object->val4);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
derived_object_class_init (DerivedObjectClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->set_property = derived_object_set_property;
  object_class->get_property = derived_object_get_property;

  /* Overriding a property that is itself overridding an interface property */
  g_object_class_override_property (object_class, DERIVED_PROP3, "prop3");

  /* Overriding a property not from an interface */
  g_object_class_override_property (object_class, DERIVED_PROP4, "prop4");
}

static DEFINE_TYPE (DerivedObject, derived_object,
		    derived_object_class_init, NULL, NULL,
		    BASE_TYPE_OBJECT)

/* Helper function for testing ...list_properties()
 */
static void
assert_in_properties (GParamSpec  *param_spec,
		      GParamSpec **properties,
		      gint         n_properties)
{
  gint i;
  gboolean found = FALSE;

  for (i = 0; i < n_properties; i++)
    {
      if (properties[i] == param_spec)
	  found = TRUE;
    }

  g_assert (found);
}
/**** Dummy function which does nothing *******/
gboolean MyFunc	     (gpointer	       cache_data,
					      GTypeClass      *g_class)
{
    return FALSE;
};

void       MyFunc1               (gpointer data,
                                             GClosure *closure)
{
  return ;
}
                                             


int
main (gint   argc,
      gchar *argv[])
{
  GObject *object, *object1;
  GObjectClass *object_class;
  GParamSpec **properties;
  gpointer p ="Data",p1 = "Data1",ret;
  gchar *str;
  GValue val;
  GParamSpec* paramSpec;
  GParamFlags flags;
  GDestroyNotify destroy;
  gpointer wkPtr= "Weak Pointer";
  GType t;
  GTypeQuery query; 
  GTypeClassCacheFunc cache_func = MyFunc;
  GTypeModule module;
  GEnumClass *type_class;
  GEnumValue enum_ret;
  GCallback callback_func;
  GClosure *closure_ret;
  gpointer notify_data= "Notify";
  gpointer notify_data1;
  GClosureNotify notify_func;
  gpointer pre_marshal_data = "MarshalData";
  GClosureNotify pre_marshal_notify;
  gpointer post_marshal_data = "PostMarshal";
  GClosureNotify post_marshal_notify = MyFunc1;
  GFlagsClass *flags_class;
  GFlagsValue* flagValue;
  GTypeModule *module1;
  GSource  source;
  gpointer      marshal_data;
  gpointer      invocation_hint;
  GClosure closure;
  GObject *bar;
  GValue val1 = {0,};
  gpointer user_data= "name";
  GValue args[2];
  GObject *object_set;
  const GEnumValue static_values;
  static const GFlagsValue const_static_values[] = {
	{ G_IO_IN,   "G_IO_IN",   "in" },
	{ G_IO_OUT,  "G_IO_OUT",  "out" },
	{ G_IO_PRI,  "G_IO_PRI",  "pri" },
	{ G_IO_ERR,  "G_IO_ERR",  "err" },
	{ G_IO_HUP,  "G_IO_HUP",  "hup" },
	{ G_IO_NVAL, "G_IO_NVAL", "nval" },
	{ 0, NULL, NULL }
      };
 
  guint n_properties;
  
  gint  val2, val3, val4;
  GQuark detail;
  GSignalEmissionHook hook_func;
  gpointer hook_data;
  GDestroyNotify data_destroy;
  
  GValue orig = { 0, };
  GValue xform = { 0, }; 
  GEnumValue values[] = { {0,"0","0"}, {1,"1","1"}}; 
	
  #ifdef SYMBIAN
      g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
  #endif /*SYMBIAN*/
  
  g_log_set_always_fatal (g_log_set_always_fatal (G_LOG_FATAL_MASK) |
			  G_LOG_LEVEL_WARNING |
			  G_LOG_LEVEL_CRITICAL);
  g_type_init ();

  object = g_object_new (DERIVED_TYPE_OBJECT, NULL);

  /* Test setting and getting the properties
   */
  
  
  memset (&xform, 0, sizeof (GValue));
  g_value_init (&xform, G_TYPE_CHAR); 
  
  g_object_set_data(object , "Key",p);
  ret = g_object_get_data(object , "Key");
  str = (gchar *)ret;
  g_assert (!strcmp(str , "Data"));
     
  g_object_set_data_full          (object, "Key1", p1, destroy);
  ret = g_object_get_data(object , "Key1");
  str = (gchar *)ret;
  g_assert (!strcmp(str , "Data1"));
  
  g_object_set_qdata (object, 1, p1);
  
  g_object_set_qdata_full (object, 1, p1, destroy);
  
  ret = g_object_get_qdata (object, 1);
  
  str = (gchar *)ret;
  
  g_assert (!strcmp(str , "Data1"));
  
  ret = g_object_steal_data (object , "Key1");
  
  str = (gchar *)ret;
  
  g_assert (!strcmp(str , "Data1"));
  
  ret = g_object_steal_qdata (object , 1);
  
  str = (gchar *)ret;
  
  g_assert (!strcmp(str , "Data1"));
  
  g_object_run_dispose(object);
  
  g_object_add_weak_pointer (object, wkPtr);

  g_object_remove_weak_pointer(object, wkPtr);
  
  g_assert (g_type_qname (G_TYPE_INTERFACE) == 5);
                
  g_type_query (G_TYPE_OBJECT, &query);
  
  g_assert (query.type == 80);
  
  g_type_set_qdata (G_TYPE_OBJECT ,1 , wkPtr);
  
  ret = g_type_get_qdata (G_TYPE_OBJECT ,1);
  
  str = (gchar *)ret;
  
  g_assert (!strcmp(str , "Weak Pointer"));
  
  g_type_add_class_cache_func (wkPtr, cache_func);
 
  g_type_remove_class_cache_func (wkPtr, cache_func);
  
  g_assert (g_type_next_base (G_TYPE_OBJECT, G_TYPE_INT) == 0); 
  
  type_class = g_type_class_ref (G_TYPE_ENUM);
  
  g_assert (g_enum_get_value_by_nick (G_ENUM_CLASS (type_class),"Nick") == NULL) ;
  
  g_assert ((g_enum_register_static ("Name", &enum_ret)) != 0);
  
  g_assert((g_cclosure_new_object((GCallback)MyFunc1, object)) != NULL);
  
  closure_ret = g_closure_new_object(sizeof(GClosure),object);
  
  g_assert((g_closure_new_object(sizeof(GClosure),object)) != NULL);
  
  g_closure_add_finalize_notifier (closure_ret, notify_data, MyFunc1);
  
  g_closure_remove_finalize_notifier (closure_ret, notify_data, MyFunc1);
  
  closure_ret->n_guards = 0; //Changed the structure to pass the below api.
  
  g_closure_add_marshal_guards (closure_ret, pre_marshal_data, MyFunc1, post_marshal_data, MyFunc1);
  
  g_assert(g_pointer_type_register_static  ("G_TYPE_OBJECT") != 0);
  
  paramSpec = g_param_spec_boolean("String1", "Hello", "World", FALSE, flags);  
  
  g_assert( !strcmp(paramSpec->name , "String1"));
 
  flags_class = g_type_class_ref(G_TYPE_FLAGS); 
  
  flagValue = g_flags_get_value_by_name (flags_class, "Name");
  
  g_assert (flagValue == NULL);
  
  flagValue = g_flags_get_value_by_nick (flags_class, "Name");
  
  g_assert (flagValue == NULL);
  
  object1 = g_object_new (DERIVED_TYPE_OBJECT, NULL);     
  
  t = g_flags_register_static ("GIOCondition", const_static_values);                                  
  
  g_assert (t != 0);
  
  ret = g_object_connect (object1, NULL);
  
  g_assert (ret != NULL);
  
  g_object_disconnect(object1, NULL);
  
  paramSpec = g_param_spec_uchar ("papa-number",
                              "Papa",
                              "Set/Get papa's number",
                              0  /* minimum value */,
                              10 /* maximum value */,
                              2  /* default value */,
                              G_PARAM_READWRITE);
   
  object_set = G_OBJECT (NULL);
  
  g_value_init (&val1, paramSpec->value_type);
  
  #if SYMBIAN
  testResultXml("g_test2");
  #endif /* EMULATOR */
  
  return 0;
}
