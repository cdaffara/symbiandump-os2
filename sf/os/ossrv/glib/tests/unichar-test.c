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
#define LOG_FILE "c:\\logs\\unichar_test_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("unichar_testlog");
    close_log_file();
}

int main (int argc, char *argv[])
{  
    gunichar ret;
    gunichar mark = 0x0BCA; //spacing mark. Tamil letter. 
    gunichar title = 0x01F2; //Latin letter.
    gunichar square = 0x25A1; //white square
    gunichar space = 0x200B; //zero width space
    gunichar tel_symbol = 0x32C0; //telegraphic symbol for january
    gunichar CR = 0x000D; //carriage return
    GUnicodeBreakType type;
    
    g_unichar_get_mirror_char('(', &ret);
    
    if(!(')' == ret))
        {
        std_log(LOG_FILENAME_LINE,"g_unichar_get_mirror_char didnt work as expected");
        assert_failed = 1;
        }
    
    
    if(!g_unichar_ismark(mark))
        {
        std_log(LOG_FILENAME_LINE,"g_unichar_ismark didnt work as expected");
        assert_failed = 1;
        }
    
    if(!g_unichar_istitle(title))
        {
        std_log(LOG_FILENAME_LINE,"g_unichar_istitle didnt work as expected");
        assert_failed = 1;
        }
    
    if(!g_unichar_isdefined(square))
        {
        std_log(LOG_FILENAME_LINE,"g_unichar_isdefined didnt work as expected");
        assert_failed = 1;
        }
    
    if(!g_unichar_iszerowidth(space))
        {
        std_log(LOG_FILENAME_LINE,"g_unichar_iszerowidth didnt work as expected");
        assert_failed = 1;
        }
    
    if(!g_unichar_iswide(tel_symbol))
        {
        std_log(LOG_FILENAME_LINE,"g_unichar_iswide didnt work as expected");
        assert_failed = 1;
        }
    
    if(!g_unichar_iswide_cjk(tel_symbol))
        {
        std_log(LOG_FILENAME_LINE,"g_unichar_iswide_cjk didnt work as expected");
        assert_failed = 1;
        }
    
    ret = g_unichar_totitle('a');
    if(!(ret == 'A'))
        {
        std_log(LOG_FILENAME_LINE,"g_unichar_totitle didnt work as expected");
        assert_failed = 1;
        }
    
    type = g_unichar_break_type(CR);
    if(type != G_UNICODE_BREAK_CARRIAGE_RETURN)
        {
        std_log(LOG_FILENAME_LINE,"g_unichar_break_type didnt work as expected");
        assert_failed = 1;
        }
    
	if(assert_failed)
          std_log(LOG_FILENAME_LINE,"Test Failed");
    else
          std_log(LOG_FILENAME_LINE,"Test Successful");
	
    create_xml(0);

	return 0;
}
