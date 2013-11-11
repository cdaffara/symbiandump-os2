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
#include <errno.h>
#define LOG_FILE "c:\\logs\\uri_funcs_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("uri_funcs_log");
    close_log_file();
}

int main (int argc, char *argv[])
{
    gchar *uri = "http:\\\\www.no!ki@a.com";
    gchar *res_str = ":\\";
    char *p;
    char *q;
    char *escape_str;
    
    p = g_uri_parse_scheme(uri);
    
    if(p)
        {
        if(strcmp(p, "http"))
            {
            std_log(LOG_FILENAME_LINE,"g_uri_parse_scheme didnt work as expected");
            assert_failed = 1;
            }
        free(p);
        }
    else
        {
        std_log(LOG_FILENAME_LINE,"g_uri_parse_scheme returnd NULL. errno = %d", errno);
        assert_failed = 1;
        }
    
    
    //escape the uri
    escape_str = g_uri_escape_string(uri, res_str, TRUE);
    
    if(escape_str)
        {
        std_log(LOG_FILENAME_LINE, "escape string %s", escape_str);
        
        //convert back only a segment
        q = g_uri_unescape_segment(escape_str, escape_str+16, NULL);
        if(q)
            {
            std_log(LOG_FILENAME_LINE, "unescape segment string %s", q);
            if(strcmp(q, "http:\\\\www.no!"))
                {
                std_log(LOG_FILENAME_LINE,"g_uri_unescape_segment didnt work as expected");
                assert_failed = 1;
                }
            free(q);
            }
        else
            {
            std_log(LOG_FILENAME_LINE,"g_uri_unescape_segment returned NULL. errno = %d", errno);
            assert_failed = 1;
            }
        
        //convert back the whole string
        p = g_uri_unescape_string(escape_str, NULL);
        if(p)
            {
            std_log(LOG_FILENAME_LINE, "unescape string %s", p);
            
            //converted string should be same as original uri string
            if(strcmp(p, uri))
                {
                std_log(LOG_FILENAME_LINE,"g_uri_unescape_string returned NULL");
                assert_failed = 1;
                }
            
            free(p);
            }
        else
            {
            std_log(LOG_FILENAME_LINE,"g_uri_unescape_string returned NULL. errno = %d", errno);
            assert_failed = 1;
            }
        
        free(escape_str);
        }
    else
        {
        std_log(LOG_FILENAME_LINE,"g_uri_escape_string returned NULL. errno = %d", errno);
        assert_failed = 1;
        }
    
	if(assert_failed)
          std_log(LOG_FILENAME_LINE,"Test Failed");
    else
          std_log(LOG_FILENAME_LINE,"Test Successful");
	
    create_xml(0);

	return 0;
}
