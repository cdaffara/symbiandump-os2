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

#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#undef G_DISABLE_ASSERT
#undef G_DISABLE_CHECKS
#undef G_DISABLE_CAST_CHECKS

#include <string.h>

#include <glib-object.h>

#include "testcommon.h"

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/


/* Tests for all cclosure_marshal APIs */

/*
 * TestObject, a parent class for TestObject
 */
#define TEST_TYPE_OBJECT          (test_object_get_type ())
typedef struct _TestObject        TestObject;
typedef struct _TestObjectClass   TestObjectClass;

typedef struct 
{
   int a;
   char c;		
	
}TestInputClass;

struct _TestObject
{
   GObject parent_instance;
};

struct _TestObjectClass
{
  GObjectClass parent_class;

  void   (*test_signal1) (TestObject *tobject,
			    gint        param);
  
  //flags as parameter == ulong
  gboolean  (*test_signal2) (TestObject *tobject,
			    gulong        param);
  
  void  (*test_signal3) (TestObject *tobject,
			    gpointer      param);
			    
  void (*test_signal4) (TestObject *tobject,
			    gchar      param);   
			    
  void (*test_signal5) (TestObject *tobject,
			    gdouble    param);  
  
  //enum == long		    
  void (*test_signal6) (TestObject *tobject,
			    glong    param);    			    

  void (*test_signal7) (TestObject *tobject,
			    gulong    param);    	
			    
  void (*test_signal8) (TestObject *tobject,
			    gfloat    param);   				    		    

  void (*test_signal9) (TestObject *tobject,
			    glong    param);  
			    
  //object
  void (*test_signal10) (TestObject *tobject,
			    GObject  *param);  
			     				  
  //pointer
  void (*test_signal11) (TestObject *tobject,
			    gpointer  param); 		     				  	    		    
			    
  void (*test_signal12) (TestObject *tobject,
			    gchar*  param); 		     				  	    		    
  			    
  void (*test_signal13) (TestObject *tobject,
			    guchar  param); 
			    
  void (*test_signal14) (TestObject *tobject,
			    guint  param); 		
  
  void (*test_signal15) (TestObject *tobject,
			    gulong  param); 					    	    		     				  	    		    

  void (*test_signal16) (TestObject *tobject,
			    guint  param1,
			    gpointer param2); 					    	    		     				  	    		    

  void (*test_signal17) (TestObject *tobject,
			    GParamSpec *param); 					    	    		     				  	    		    
};

static GType test_object_get_type (void);

/* Test input for Enums */
enum
{
   TEST_ENUM1,
   TEST_ENUM2
};

//VOID__BOOLEAN
void
test_object_real_signal1 (TestObject *tobject,
			  gint        param)
{
  g_assert(((gint)param == 10));
}

//BOOLEAN__FLAGS
gboolean
test_object_signal2_callback_before (TestObject *tobject,
				     gulong        param)
{
  switch (param)
    {
    case 1: return TRUE;
    case 2: return FALSE;
    case 3: return FALSE;
    case 4: return FALSE;
    }

  g_assert_not_reached ();
  return FALSE;
}

gboolean
test_object_real_signal2 (TestObject *tobject,
			  gulong        param)
{
  switch (param)
    {
    case 1: g_assert_not_reached (); return FALSE;
    case 2: return TRUE;
    case 3: return FALSE;
    case 4: return FALSE;
    }
  
  g_assert_not_reached ();
  return FALSE;
}

gboolean
test_object_signal2_callback_after (TestObject *tobject,
				     gulong        param)
{
  switch (param)
    {
    case 1: g_assert_not_reached (); return FALSE;
    case 2: g_assert_not_reached (); return FALSE;
    case 3: return TRUE;
    case 4: return FALSE;
    }
      
  g_assert_not_reached ();
  return FALSE;
}

//VOID__BOXED
void
test_object_real_signal3 (TestObject *tobject,
			  gpointer        param)
{
  g_assert((strcmp(param, "test-string") == 0));
}

//VOID__CHAR
void
test_object_real_signal4 (TestObject *tobject,
	    	  gchar        param)
{
  g_assert(((gchar)param == 'A'));  
}

//VOID__DOUBLE
void
test_object_real_signal5 (TestObject *tobject,
	    	  gdouble       param)
{
  g_assert(((gdouble)param == 2500000));  
}

 //VOID__ENUM
void
test_object_real_signal6 (TestObject *tobject,
	    	  glong       param)
{
  g_assert(((glong)param == TEST_ENUM2));  
}
 
//VOID__FLAGS
void
test_object_real_signal7 (TestObject *tobject,
	    	  gulong       param)
{
  g_assert(((gulong)param == 1980l));  
}

//VOID__FLOAT
void
test_object_real_signal8 (TestObject *tobject,
	    	  gfloat       param)
{
  g_assert(((gfloat)param == 11.23f));    
}

//VOID__LONG
void
test_object_real_signal9 (TestObject *tobject,
	    	  glong       param)
{
  g_assert(((glong)param == -748l));   
}

//VOID__OBJECT
void
test_object_real_signal10 (TestObject *tobject,
	    	   GObject  *param)
{  
  g_assert(G_IS_OBJECT(param) == TRUE);  
}

//VOID__STRING
void
test_object_real_signal12 (TestObject *tobject,
	    	  gchar*      param)
{  
  g_assert((strcmp(param, "string_input") == 0));  
}

//VOID__UCHAR
void
test_object_real_signal13 (TestObject *tobject,
	    	  guchar      param)
{  
  g_assert(param == '>');  
}


//VOID__UINT
void
test_object_real_signal14 (TestObject *tobject,
	    	  guint     param)
{  
  g_assert(param == 10000);  
}

//VOID__UINT_POINTER
void
test_object_real_signal16 (TestObject *tobject,
	    	  guint     param1,
	    	  gpointer  param2)
{  
  g_assert((param1 == 10000) && (strcmp(param2, "test-string") == 0));  
}

//VOID__PARAM
void
test_object_real_signal17 (TestObject *tobject,
	    	  GParamSpec      *param)	    	  
{  
  g_assert(G_IS_PARAM_SPEC(param) == TRUE);
  g_assert(!strcmp(param->_nick , "Hello"));
  g_assert (!strcmp(param->name , "String1"));  
}

//Class init function
static void
test_object_class_init (TestObjectClass *class)
{
  class->test_signal1 = test_object_real_signal1;
  class->test_signal2 = test_object_real_signal2;
  class->test_signal3 = test_object_real_signal3;
  class->test_signal4 = test_object_real_signal4;
  class->test_signal5 = test_object_real_signal5;
  class->test_signal6 = test_object_real_signal6;
  class->test_signal7 = test_object_real_signal7;
  class->test_signal8 = test_object_real_signal8;
  class->test_signal9 = test_object_real_signal9;  
  class->test_signal10 = test_object_real_signal10;  
  class->test_signal11= test_object_real_signal3; //since both are doing the same
  class->test_signal12= test_object_real_signal12;
  class->test_signal13= test_object_real_signal13;
  class->test_signal14= test_object_real_signal14;
  class->test_signal15= test_object_real_signal7; //both are doing the same comparison
  class->test_signal16= test_object_real_signal16; 
  class->test_signal17= test_object_real_signal17; 
  
  g_signal_new ("test-signal1",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TestObjectClass, test_signal1),
		NULL, NULL,
		g_cclosure_marshal_VOID__BOOLEAN ,
		G_TYPE_NONE, 1, G_TYPE_INT);
  
  g_signal_new ("test-signal2",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TestObjectClass, test_signal2),
		g_signal_accumulator_true_handled, NULL,
		g_cclosure_marshal_BOOLEAN__FLAGS,
		G_TYPE_BOOLEAN, 1, G_TYPE_ULONG);
		
  g_signal_new ("test-signal3",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TestObjectClass, test_signal3),
		NULL, NULL,
		g_cclosure_marshal_VOID__BOXED,
		G_TYPE_NONE, 1, G_TYPE_POINTER);
		
  g_signal_new ("test-signal4",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TestObjectClass, test_signal4),
		NULL, NULL,
		g_cclosure_marshal_VOID__CHAR,
		G_TYPE_NONE, 1, G_TYPE_CHAR);

  g_signal_new ("test-signal5",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TestObjectClass, test_signal5),
		NULL, NULL,
		g_cclosure_marshal_VOID__DOUBLE,
		G_TYPE_NONE, 1, G_TYPE_DOUBLE);

  g_signal_new ("test-signal6",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TestObjectClass, test_signal6),
		NULL, NULL,
		g_cclosure_marshal_VOID__ENUM,
		G_TYPE_NONE, 1, G_TYPE_LONG);

  g_signal_new ("test-signal7",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TestObjectClass, test_signal7),
		NULL, NULL,
		g_cclosure_marshal_VOID__FLAGS,
		G_TYPE_NONE, 1, G_TYPE_ULONG);		
			
  g_signal_new ("test-signal8",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TestObjectClass, test_signal8),
		NULL, NULL,
		g_cclosure_marshal_VOID__FLOAT,
		G_TYPE_NONE, 1, G_TYPE_FLOAT);		
		
  g_signal_new ("test-signal9",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TestObjectClass, test_signal9),
		NULL, NULL,
		g_cclosure_marshal_VOID__LONG,
		G_TYPE_NONE, 1, G_TYPE_LONG);
		
  g_signal_new ("test-signal10",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TestObjectClass, test_signal10),
		NULL, NULL,
		g_cclosure_marshal_VOID__OBJECT,
		G_TYPE_NONE, 1, TEST_TYPE_OBJECT);	
		
   g_signal_new ("test-signal11",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TestObjectClass, test_signal11),
		NULL, NULL,
		g_cclosure_marshal_VOID__POINTER,
		G_TYPE_NONE, 1, G_TYPE_POINTER);
		
   g_signal_new ("test-signal12",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TestObjectClass, test_signal12),
		NULL, NULL,
		g_cclosure_marshal_VOID__STRING,
		G_TYPE_NONE, 1, G_TYPE_STRING);	
		
	g_signal_new ("test-signal13",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TestObjectClass, test_signal13),
		NULL, NULL,
		g_cclosure_marshal_VOID__UCHAR,
		G_TYPE_NONE, 1, G_TYPE_UCHAR);	
		
   	g_signal_new ("test-signal14",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TestObjectClass, test_signal14),
		NULL, NULL,
		g_cclosure_marshal_VOID__UINT,
		G_TYPE_NONE, 1, G_TYPE_UINT);
	
	g_signal_new ("test-signal15",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TestObjectClass, test_signal15),
		NULL, NULL,
		g_cclosure_marshal_VOID__ULONG,
		G_TYPE_NONE, 1, G_TYPE_ULONG);
		
    g_signal_new ("test-signal16",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TestObjectClass, test_signal16),
		NULL, NULL,
		g_cclosure_marshal_VOID__UINT_POINTER,
		G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_POINTER);	
		
	g_signal_new ("test-signal17",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (TestObjectClass, test_signal17),
		NULL, NULL,
		g_cclosure_marshal_VOID__PARAM,
		G_TYPE_NONE, 1, G_TYPE_PARAM);											
}

static DEFINE_TYPE(TestObject, test_object,
		   test_object_class_init, NULL, NULL,
		   G_TYPE_OBJECT)		   

int
main (int   argc,
      char *argv[])
{
  TestObject *object;  
  gboolean bool_result;
  gpointer i_string = "test-string";
  gchar    i_char   = 'A';
  gdouble  i_double = 2500000; //typically 8 bytes  
  gfloat   i_float  = 11.23f;
  gulong   i_ulong  = 1980l;
  glong    i_long 	= -748l;
  gchar*   i_char_string = "string_input";
  guchar   i_uchar  ='>'; 
  TestObject *i_input_obj;  	
  GParamFlags i_flags;
  GParamSpec* i_paramSpec;	
  	
  #ifdef SYMBIAN
  g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
  #endif /*SYMBIAN*/
	  

  /*g_log_set_always_fatal (g_log_set_always_fatal (G_LOG_FATAL_MASK) |
			  G_LOG_LEVEL_WARNING |
			  G_LOG_LEVEL_CRITICAL);*/
  g_type_init ();

  object = g_object_new (TEST_TYPE_OBJECT, NULL);

  //VOID__BOOLEAN  
  g_signal_emit_by_name (object, "test-signal1", 10, 0);  
  

  //BOOLEAN__FLAGS
  g_signal_connect (object, "test-signal2",
		    G_CALLBACK (test_object_signal2_callback_before), NULL);
  g_signal_connect_after (object, "test-signal2",
			  G_CALLBACK (test_object_signal2_callback_after), NULL);
  
  bool_result = FALSE;
  g_signal_emit_by_name (object, "test-signal2", 1, &bool_result);
  g_assert (bool_result == TRUE);
  bool_result = FALSE;
  g_signal_emit_by_name (object, "test-signal2", 2, &bool_result);
  g_assert (bool_result == TRUE);
  bool_result = FALSE;
  g_signal_emit_by_name (object, "test-signal2", 3, &bool_result);
  g_assert (bool_result == TRUE);
  bool_result = TRUE;
  g_signal_emit_by_name (object, "test-signal2", 4, &bool_result);
  g_assert (bool_result == FALSE);
  
 
  //VOID__BOXED
  g_signal_emit_by_name (object, "test-signal3", i_string, NULL);
  
  //VOID__CHAR
  g_signal_emit_by_name (object, "test-signal4", i_char, NULL);
  
  //VOID__DOUBLE
  g_signal_emit_by_name (object, "test-signal5", i_double, NULL);
  
  //VOID__ENUM
  g_signal_emit_by_name (object, "test-signal6", TEST_ENUM2, NULL);

  //VOID__FLAGS
  g_signal_emit_by_name (object, "test-signal7", i_ulong, NULL);
  
  //VOID__FLOAT
  g_signal_emit_by_name (object, "test-signal8", i_float, NULL);
  
  //VOID__FLOAT
  g_signal_emit_by_name (object, "test-signal9", i_long, NULL);  
  
  //VOID__OBJECT
  i_input_obj = g_object_new (TEST_TYPE_OBJECT, NULL);
  g_signal_emit_by_name (object, "test-signal10", i_input_obj, NULL);  
  
  //VOID__POINTER
  g_signal_emit_by_name (object, "test-signal11", i_string, NULL);
  
  //VOID__STRING
  g_signal_emit_by_name (object, "test-signal12", i_char_string, NULL);
  
  //VOID__UCHAR  
  g_signal_emit_by_name (object, "test-signal13", i_uchar, NULL);
  
  //VOID__UINT
  g_signal_emit_by_name (object, "test-signal14", 10000, NULL);
  
  //VOID__ULONG
  g_signal_emit_by_name (object, "test-signal15", i_ulong, NULL);
  
  //VOID_UNIT__POINTER
  g_signal_emit_by_name (object, "test-signal16", 10000, i_string,NULL);  
 
  //VOID__PARAM  
  i_paramSpec = g_param_spec_uchar("String1","Hello","World", 0,100,10, i_flags);
  g_signal_emit_by_name (object, "test-signal17", i_paramSpec, NULL);   
  
  #if SYMBIAN
  testResultXml("closure-marshal");
  #endif
  
  return 0;
}
