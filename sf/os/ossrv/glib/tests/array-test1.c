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

#include <glib.h>
#define LOG_FILE "c:\\logs\\array1_test_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__


void create_xml(int result)
{
    if(result)
        assert_failed = 1;

    testResultXml("array-test");
    close_log_file();
}

static gint sort (gconstpointer a, gconstpointer b)
{
	if(*(guint32*)a == *(guint32*)b)
		return 0;
	else
		return *(guint32*)a < *(guint32*)b ? -1 : 1;
}


static gint sort_userdata (gconstpointer a, gconstpointer b, gpointer user_data)
{
	  if(*(guint32*)a == *(guint32*)b)
			return 0;
	  else
			return *(guint32*)a < *(guint32*)b ? -1 : 1;
}



/*Will return TRUE if input GArray and int[] array 2 are same else FALSE*/
gboolean compare_array(GArray* array1, gint* array2, gint size)
{

    int i;
	if ( size != array1->len)
		return FALSE;
	for ( i = 0; i < size ; i++)
	{
		if ( g_array_index(array1, gint, i) != array2[i])
			return FALSE;
	}

	return TRUE;
}


void test_remove_array_index()
{
    GArray *garray;

    const gint ARRAY_SIZE = 7;
    const gint ARRAY_SIZE_AFTER_REMOVE_INDEX = 6;
    const gint ARRAY_SIZE_AFTER_REMOVE_INDEX_FAST = 5;

    gint array[ARRAY_SIZE] = {4,5,6,7,8,9,10};
    gint array_after_remove_index_1[ARRAY_SIZE_AFTER_REMOVE_INDEX]= {4,6,7,8,9,10};  /*array after removing index 1*/
    gint array_after_remove_index_fast_2[ARRAY_SIZE_AFTER_REMOVE_INDEX_FAST] = {4,6,10,8,9}; /* array after removing index 2 fast. Input is array after removing index 1 */

    int i;
    gboolean ret;

    garray = g_array_new (FALSE,FALSE,sizeof(gint));
    if(garray == NULL)
	{
      std_log(LOG_FILENAME_LINE, "Array not created");
      assert_failed = 1;
	  return ;
	}

	/*Insert values into array*/
    g_array_insert_vals(garray,0,array,ARRAY_SIZE);

    /* test for deleting single element in an array. Removing the element with index 1*/
    std_log(LOG_FILENAME_LINE, "Delete array element at index 1");
    garray = g_array_remove_index(garray,1);

	if(garray == NULL )
	{
		std_log(LOG_FILENAME_LINE, "NULL return by g_array_remove_index");
		assert_failed = 1;
		return ;
	}

    /*Print the array elements after remove*/
    for(i=0; i<garray->len; i++)
    {
          std_log(LOG_FILENAME_LINE, "Current array element at index %d is %d", i,g_array_index(garray, gint, i));
	}

	/*Check if the array size is now 4 and element at index 1 is not 5 after removal*/

	ret = compare_array(garray, array_after_remove_index_1, ARRAY_SIZE_AFTER_REMOVE_INDEX);
	if ( !ret)
	{
		std_log(LOG_FILENAME_LINE, "Array Element not properly deleted by g_array_remove_index");
		assert_failed = 1;
		g_array_free(garray,TRUE);
		return ;
	}


	/* Test to remove index element 2 using g_array_remove_index_fast*/

    std_log(LOG_FILENAME_LINE, "Delete array element at index 2");
    garray =g_array_remove_index_fast(garray,2);
    if(garray == NULL)
    {
		std_log(LOG_FILENAME_LINE, "NULL return by g_array_remove_index_fast");
		assert_failed = 1;
		return ;
	}

    for(i=0; i<garray->len; i++)
	{
		std_log(LOG_FILENAME_LINE, "Current array element at index %d is %d", i,g_array_index(garray, gint, i));
	}

	ret = compare_array(garray, array_after_remove_index_fast_2, ARRAY_SIZE_AFTER_REMOVE_INDEX_FAST);
	if ( !ret)
	{
		std_log(LOG_FILENAME_LINE, "Array Element not properly deleted by g_array_remove_index_fast");
		assert_failed = 1;
		g_array_free(garray,TRUE);
		return;
	}
    g_array_free(garray,TRUE);
}


void test_test_remove_array_index_range()
{
	const int ARRAY_SIZE = 10;
	const int ARRAY_SIZE_AFTER_REMOVE_INDEX_RANGE = 8;

	gint array[ARRAY_SIZE]=															{10,5,16,7,11,0,20,1,9,8};
	gint array_after_remove_index_range[ARRAY_SIZE_AFTER_REMOVE_INDEX_RANGE] =  	{10,5,16,0,20,1,9,8};  /*after removing 2 elements from index 3*/

	gboolean ret;
	GArray* garray;
	int i;

	garray = g_array_new (FALSE,FALSE,sizeof(gint));
	if(garray == NULL)
	{
		  std_log(LOG_FILENAME_LINE, "Array not created");
		  assert_failed = 1;
		  return ;
	}

	g_array_insert_vals(garray,0,array,ARRAY_SIZE);
	for(i=0; i<garray->len;i++)
	{
		std_log(LOG_FILENAME_LINE, "Current array elements %d is %d", i,g_array_index(garray, gint, i));
	}

	garray = g_array_remove_range(garray,3,2); /*remove two elements from index 3 */

	if(garray == NULL)
	{
		std_log(LOG_FILENAME_LINE,"Elements not deleted properly by g_array_remove_range");
		assert_failed = 1;
		return ;

	}

	/*print the array elements */
	for(i=0; i<garray->len;i++)
	{
		std_log(LOG_FILENAME_LINE, "Curent array element(after deletion) %d is %d", i,g_array_index(garray, gint, i));
	}


	ret = compare_array(garray, array_after_remove_index_range, ARRAY_SIZE_AFTER_REMOVE_INDEX_RANGE);
	if(!ret)
	{
		std_log(LOG_FILENAME_LINE,"Elements not deleted properly");
		assert_failed = 1;
		g_array_free(garray,TRUE);
		return ;
	}
	g_array_free(garray,TRUE);
}


void test_sort_array()
{

	GArray *garray;
	const int ARRAY_SIZE = 11;
    gint array[ARRAY_SIZE] = {10,5,16,7,11,0,20,1,9,8,9};
	gint sort_array[ARRAY_SIZE] = {0,1,5,7,8,9,9,10,11,16,20};

	gboolean ret;
	int i;

    /* Test for sorting the array elements */

	garray = g_array_new (FALSE,FALSE,sizeof(gint));

	if(garray == NULL)
	{
		std_log(LOG_FILENAME_LINE, "Array not created");
		assert_failed = 1;
		return ;
	}
	g_array_insert_vals(garray,0,array,ARRAY_SIZE);

	g_array_sort(garray, sort);

	if(garray == NULL)
	{
		std_log(LOG_FILENAME_LINE, "Array not sorted");
		assert_failed = 1;
		return ;
	}

	std_log(LOG_FILENAME_LINE,"SORTED ARRAY");

	for(i=0;i<garray->len;i++)
	{
		std_log(LOG_FILENAME_LINE, "Element %d is %d", i,g_array_index(garray, gint, i));
	}

	ret = compare_array(garray, sort_array, ARRAY_SIZE);

	if(!ret)
	{
		std_log(LOG_FILENAME_LINE, "Array not sorted correctly");
		assert_failed = 1;
		return ;
	}

	g_array_free(garray,TRUE);


    garray = g_array_new (FALSE,FALSE,sizeof(gint));
	if(garray == NULL)
	{
      std_log(LOG_FILENAME_LINE, "Array not created");
	  return ;
	}
	g_array_insert_vals(garray,0,array,ARRAY_SIZE);

	g_array_sort_with_data (garray, sort_userdata, NULL);

	if(garray == NULL)
	{
		std_log(LOG_FILENAME_LINE, "Array not sorted with user data");
		assert_failed = 1;
		return ;
	}

	std_log(LOG_FILENAME_LINE,"SORTED ARRAY WITH USERDATA");
	for(i=0;i<garray->len;i++)
	{
		std_log(LOG_FILENAME_LINE, "Element %d is %d", i,g_array_index(garray, gint, i));
	}

	ret = compare_array(garray, sort_array, ARRAY_SIZE);
	if(!ret)
	{
		std_log(LOG_FILENAME_LINE, "Array not sorted correctly with user data");
		assert_failed = 1;
		return ;
	}
    g_array_free(garray,TRUE);
}


int main (void)
{
	test_test_remove_array_index_range();
	test_sort_array();
	test_remove_array_index();

	if(assert_failed)
		std_log(LOG_FILENAME_LINE,"Test Failed");
	else
		std_log(LOG_FILENAME_LINE,"Test Successful");

    create_xml(0);
    return 0;
}


