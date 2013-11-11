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


#include<stdio.h>
#include <glib.h>
#define LOG_FILE "c:\\logs\\bookmark_test1_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#define in_FILE "file.xbel"


void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("bookmark-test1");
    close_log_file();
}

int
main (int   argc,
      char *argv[])
    {
    GBookmarkFile* bookmark;
    gsize uris_len;
    gchar *bookmark_data = NULL;
    gboolean file_written,file_load;
    const gchar *rel_path;
    time_t time;
    const gchar *bookmark_filename = argv[1];
    GError        *error = NULL;
    gchar **bookmark_uri = NULL;
        
    bookmark = g_bookmark_file_new();
    file_load= g_bookmark_file_load_from_file(bookmark, bookmark_filename, &error);
    if(file_load != TRUE)
        {
        std_log(LOG_FILENAME_LINE,"g_bookmark_file_load_from_file fails with err:%s",error->message);
        assert_failed = 1;
        }
    bookmark_data = g_bookmark_file_to_data(bookmark,&uris_len,&error);
    if(bookmark_data == NULL)
        {
        std_log(LOG_FILENAME_LINE,"Bg_bookmark_file_to_data fails with err:%s",error->message);
        assert_failed = 1;
        }
    rel_path = in_FILE; // Code changed to Hardcode the data file in the c:\ private path. 
    file_written = g_bookmark_file_load_from_data_dirs(bookmark, rel_path,NULL, &error);
    if(file_written == FALSE)
           {
           std_log(LOG_FILENAME_LINE,"g_bookmark_file_load_from_data_dirs fails with err:%s",error->message);
           assert_failed = 1;
           }
    
    bookmark_uri = g_bookmark_file_get_uris(bookmark, NULL);
    if(bookmark_uri == NULL)
            {
            std_log(LOG_FILENAME_LINE,"g_bookmark_file_get_uris fails");
            assert_failed = 1;
            }
          
    bookmark_data = g_bookmark_file_get_mime_type(bookmark,*bookmark_uri,&error);
    if(bookmark_data == NULL)
        {
        std_log(LOG_FILENAME_LINE,"URI cannot be found and the error code:%s",error->message);
        assert_failed = 1;
        }
    g_bookmark_file_set_is_private(bookmark,*bookmark_uri, TRUE);
    file_written = g_bookmark_file_get_is_private(bookmark,*bookmark_uri, &error);
    if(file_written == FALSE)
        {
        std_log(LOG_FILENAME_LINE,"Private flag is not set in the URI and fails with :%s",error->message);
        assert_failed = 1;
        }
    
    g_bookmark_file_set_added(bookmark,*bookmark_uri,-1);//-1, To use the current time.
    time = g_bookmark_file_get_added(bookmark,*bookmark_uri,&error);
    if(time == -1)
        {
        std_log(LOG_FILENAME_LINE,"URI cannot be found and fails with :%s",error->message);
        assert_failed = 1;
        } 
    g_strfreev(bookmark_uri);  
    g_bookmark_file_free(bookmark);
    
    if(assert_failed)
        std_log(LOG_FILENAME_LINE,"Test Fail");
    else
        std_log(LOG_FILENAME_LINE,"Test Successful");
         
   
    create_xml(assert_failed);
    return 0;
    }
