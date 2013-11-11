// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#define LOG_FILENAME_LINE __FILE__, __LINE__
#include <stdio.h>
#include <glib.h>

#define LOG_FILE "c:\\logs\\array2_test_log.txt"
#include "std_log_result.h"


void create_xml(int result)
{
    if(result)
        assert_failed = 1;

    testResultXml("ptrarray-test");
    close_log_file();
}

static gint psort (gconstpointer a, gconstpointer b)
{
  if(**(guint32**)a == **(guint32**)b)
	  return 0;
  else
	return **(guint32**)a < **(guint32**)b ? -1 : 1;
}


static gint psort_userdata (gconstpointer a, gconstpointer b, gpointer user_data)
{
    if(**(guint32**)a == **(guint32**)b)
	   return 0;
	else
	   return **(guint32**)a < **(guint32**)b ? -1 : 1;
}

gboolean compare_pointer_array(GPtrArray *parray1, gint* array2, gint size )
{

	int i;
	int* val;
	if ( size != parray1->len)
		return FALSE;
	for ( i = 0; i < size ; i++)
	{
		val = (int*)g_ptr_array_index(parray1, i) ;
		if(val == NULL)
			return FALSE;
		if ( *val != array2[i])
			return FALSE;
	}

	return TRUE;


}
void test_pointer_array_remove_range()
{
	GPtrArray *gparray;
    const gint ARRAY_SIZE = 15;
    const gint ARRAY_SIZE_AFTER_REMOVE_RANGE = 12; /*removing 3 elements starting from index 3*/

    gint array[ARRAY_SIZE]= {99,88,77,33,44,11,66,22,0,39,1,9,100,2,73};
    gint array_after_remove_range[ARRAY_SIZE_AFTER_REMOVE_RANGE ]= {99,88,77,66,22,0,39,1,9,100,2,73};
    gboolean ret;
    int i;


	gparray = g_ptr_array_new ();
 	if(gparray == NULL)
 	{
 		std_log(LOG_FILENAME_LINE, "Pointer Array Not created");
 		assert_failed = 1;
 		return;
 	}

 	/*Add elements to array*/
	for (i = 0; i < ARRAY_SIZE; i++)
	{
		g_ptr_array_add (gparray, &(array[i]));
		std_log(LOG_FILENAME_LINE, "Ptr Array element at index %d is %d",i, array[i]);
	}

	g_ptr_array_remove_range(gparray,3,3);

	std_log(LOG_FILENAME_LINE, "AFTER DELETING THE RANGE");

	/*Print the garray pointer->values*/
	for(i=0;i<gparray->len;i++)
	{
        gpointer val = g_ptr_array_index (gparray,i);	
        std_log(LOG_FILENAME_LINE, "Ptr Array element at index %d is %d",i,  *((int*)val));	
	}

	ret = compare_pointer_array(gparray, array_after_remove_range, ARRAY_SIZE_AFTER_REMOVE_RANGE );

    if(ret != TRUE)
    {
		std_log(LOG_FILENAME_LINE, "Pointer Array Elements not properly deleted by g_ptr_array_remove_range");
		assert_failed = 1;
		g_ptr_array_free(gparray,TRUE);
		return ;
	}
 	g_ptr_array_free (gparray, TRUE);
}


void sort_pointer_array()
{

	GPtrArray *gparray;
    const gint ARRAY_SIZE = 15;

    gint array[ARRAY_SIZE]= {99,88,77,33,44,11,66,22,0,39,1,9,100,2,73};
    gint sorted_array[ARRAY_SIZE]= { 0,1,2,9,11,22,33,39,44,66,73,77,88,99,100};
    int i;
    gboolean ret;

	/* Test to sort the pointer array*/
	gparray = g_ptr_array_new ();
	if(gparray == NULL)
	{
		std_log(LOG_FILENAME_LINE, "Pointer Array not created");
		assert_failed = 1;
		g_ptr_array_free(gparray,TRUE);
		return ;
	}

	for (i = 0; i < ARRAY_SIZE; i++)
	{
		g_ptr_array_add (gparray, &array[i]);
		std_log(LOG_FILENAME_LINE, "Ptr Array element at index %d is %d",i, array[i]);
	}

	g_ptr_array_sort(gparray,psort);

	/*Print the sorted Array*/
	std_log(LOG_FILENAME_LINE, "SORTED ARRAY");

	for(i=0;i<gparray->len;i++)
	{
        gpointer val = g_ptr_array_index (gparray,i);
        std_log(LOG_FILENAME_LINE, "Ptr Array element at index %d is %d",i, *((int*)val));
	}


	ret = compare_pointer_array(gparray, sorted_array, ARRAY_SIZE);
    if(ret != TRUE)
    {
		std_log(LOG_FILENAME_LINE, "Pointer Array Elements not sorted by g_ptr_array_sort");
		assert_failed = 1;
		g_ptr_array_free(gparray,TRUE);
		return ;
	}
	g_ptr_array_free (gparray, TRUE);
}


void sort_pointer_array_with_data()
{

	GPtrArray *gparray;
    const gint ARRAY_SIZE = 15;

    gint array[ARRAY_SIZE]= {99,88,77,33,44,11,66,22,0,39,1,9,100,2,73};
    gint sorted_array[ARRAY_SIZE]= { 0,1,2,9,11,22,33,39,44,66,73,77,88,99,100};
    int i;
    gboolean ret;

	/* Test to sort the pointer array*/
	gparray = g_ptr_array_new ();
	if(gparray == NULL)
	{
		std_log(LOG_FILENAME_LINE, "Pointer Array not created");
		assert_failed = 1;
		g_ptr_array_free(gparray,TRUE);
		return ;
	}

	for (i = 0; i < ARRAY_SIZE; i++)
	{
		g_ptr_array_add (gparray, &array[i]);
		
		std_log(LOG_FILENAME_LINE, "Ptr Array element at index %d is %d",i, array[i]);
	}

	g_ptr_array_sort_with_data(gparray,psort_userdata, NULL);


	/*Print the sorted Array*/
	std_log(LOG_FILENAME_LINE, "SORTED ARRAY");

	for(i=0;i<gparray->len;i++)
	{
        gpointer val = g_ptr_array_index (gparray,i);
        std_log(LOG_FILENAME_LINE, "Ptr Array element at index %d is %d",i, *((int*)val) );
	}


	ret = compare_pointer_array(gparray, sorted_array, ARRAY_SIZE);

    if(ret != TRUE)
    {
		std_log(LOG_FILENAME_LINE, "Pointer Array Elements not sorted by g_ptr_array_sort");
		assert_failed = 1;
		g_ptr_array_free(gparray,TRUE);
		return ;
	}
	g_ptr_array_free (gparray, TRUE);
}


int main (void)
{
	test_pointer_array_remove_range();
	sort_pointer_array();
	sort_pointer_array_with_data();

	if(assert_failed)
		std_log(LOG_FILENAME_LINE,"Test Failed");
	else
		std_log(LOG_FILENAME_LINE,"Test Successful");

	create_xml(0);
	return 0;
}
