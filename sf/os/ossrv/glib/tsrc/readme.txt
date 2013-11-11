This readme describes the way to run glib test cases.

When running the glib test cases please take care of the following:

1. The hardware image must have the language variants dlls if the ATSINPUT is created using genatsmodules_locale_specific.bat.
Language variant DLLs will not be flashed on to the device if we are using Engineering  English image. So we have to create our own customised image for testing the glib module.

Steps to be followed to test glib module.
a. Create a locale.iby file with following entries in the file.
/*
* ==============================================================================
*  Name        : locale.iby
*  Part of     : locale
*  Description : IBY file for locale
*  Version     : 
*
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
* ==============================================================================
*/



#ifndef LOCALE_IBY
#define LOCALE_IBY

file=\epoc32\release\armv5\urel\elocl.01            SHARED_LIB_DIR\elocl.01
file=\epoc32\release\armv5\urel\elocl.37            SHARED_LIB_DIR\elocl.37
file=\epoc32\release\armv5\urel\elocl.42            SHARED_LIB_DIR\elocl.42
file=\epoc32\release\armv5\urel\elocl.25            SHARED_LIB_DIR\elocl.25
file=\epoc32\release\armv5\urel\elocl.07            SHARED_LIB_DIR\elocl.07
file=\epoc32\release\armv5\urel\elocl.03            SHARED_LIB_DIR\elocl.03
file=\epoc32\release\armv5\urel\elocl.54            SHARED_LIB_DIR\elocl.54
file=\epoc32\release\armv5\urel\elocl.10            SHARED_LIB_DIR\elocl.10
file=\epoc32\release\armv5\urel\elocl.04            SHARED_LIB_DIR\elocl.04
file=\epoc32\release\armv5\urel\elocl.49            SHARED_LIB_DIR\elocl.49
file=\epoc32\release\armv5\urel\elocl.09            SHARED_LIB_DIR\elocl.09
file=\epoc32\release\armv5\urel\elocl.51            SHARED_LIB_DIR\elocl.51
file=\epoc32\release\armv5\urel\elocl.02            SHARED_LIB_DIR\elocl.02
file=\epoc32\release\armv5\urel\elocl.57            SHARED_LIB_DIR\elocl.57
file=\epoc32\release\armv5\urel\elocl.45            SHARED_LIB_DIR\elocl.45
file=\epoc32\release\armv5\urel\elocl.17            SHARED_LIB_DIR\elocl.17
file=\epoc32\release\armv5\urel\elocl.59            SHARED_LIB_DIR\elocl.59
file=\epoc32\release\armv5\urel\elocl.15            SHARED_LIB_DIR\elocl.15
file=\epoc32\release\armv5\urel\elocl.05            SHARED_LIB_DIR\elocl.05
file=\epoc32\release\armv5\urel\elocl.32            SHARED_LIB_DIR\elocl.32
file=\epoc32\release\armv5\urel\elocl.68            SHARED_LIB_DIR\elocl.68
file=\epoc32\release\armv5\urel\elocl.67            SHARED_LIB_DIR\elocl.67
file=\epoc32\release\armv5\urel\elocl.70            SHARED_LIB_DIR\elocl.70
file=\epoc32\release\armv5\urel\elocl.18            SHARED_LIB_DIR\elocl.18
file=\epoc32\release\armv5\urel\elocl.08            SHARED_LIB_DIR\elocl.08
file=\epoc32\release\armv5\urel\elocl.27            SHARED_LIB_DIR\elocl.27
file=\epoc32\release\armv5\urel\elocl.76            SHARED_LIB_DIR\elocl.76
file=\epoc32\release\armv5\urel\elocl.13            SHARED_LIB_DIR\elocl.13
file=\epoc32\release\armv5\urel\elocl.78            SHARED_LIB_DIR\elocl.78
file=\epoc32\release\armv5\urel\elocl.16            SHARED_LIB_DIR\elocl.16
file=\epoc32\release\armv5\urel\elocl.26            SHARED_LIB_DIR\elocl.26
file=\epoc32\release\armv5\urel\elocl.28            SHARED_LIB_DIR\elocl.28
file=\epoc32\release\armv5\urel\elocl.79            SHARED_LIB_DIR\elocl.79
file=\epoc32\release\armv5\urel\elocl.06            SHARED_LIB_DIR\elocl.06
file=\epoc32\release\armv5\urel\elocl.33            SHARED_LIB_DIR\elocl.33
file=\epoc32\release\armv5\urel\elocl.39            SHARED_LIB_DIR\elocl.39
file=\epoc32\release\armv5\urel\elocl.14            SHARED_LIB_DIR\elocl.14
file=\epoc32\release\armv5\urel\elocl.93            SHARED_LIB_DIR\elocl.93
file=\epoc32\release\armv5\urel\elocl.96            SHARED_LIB_DIR\elocl.96
file=\epoc32\release\armv5\urel\elocl.31            SHARED_LIB_DIR\elocl.31
file=\epoc32\release\armv5\urel\elocl.30            SHARED_LIB_DIR\elocl.30

file=\epoc32\release\armv5\urel\SHIFTJIS.DLL            SHARED_LIB_DIR\SHIFTJIS.DLL
file=\epoc32\release\armv5\urel\EUCJP_PACKED.DLL      SHARED_LIB_DIR\EUCJP_PACKED.DLL

#endif


b.Place the locale.iby file in \EPOC32\rom\Include directory
c.Open S60.iby file from  \EPOC32\rom\Include directory and make an entry in the file as given below
   #include <locale.iby>                      
d. Give the following command to build the scarfe image
    isis_build.bat -rom -type=scarfe
 
2. Ensure that locales.txt is present at \epoc32\winscw\c\resource on emulator and at c:\resource on Hardware. If not present, copy the same to the specified location.

3. The following are manual test cases:
        a. tmanual - Prints instructions on screen. Please follow them. If nothing is printed it is considered as failed.

        b. log_manual1 - When the test case is executed, the getchar() API should not be executed. If it is executed the test case is considered as failed.

        c. log_test - prints the following message "test domain-message: test message printed successfully". If it does not print it, the test case is considered as failed.

        d. mem_profile_test - This will print memory profile on the screen. If it does not print the memory profile the test case is considered as failed.
        
        e. error_check_mutexes - This will terminate at some point inside error_check_mutexes function,if, in any case it creates result file then consider is as failed.

4. To generate the ATS test drops for non locale specific APIs use the batch file "genatsmodules.bat" placed in glib\internal. This will generate the "ATSINPUT" folder. 
   For locale specfic APIs use genatsmodules_locale_specific.bat placed in glib\internal.

5. At the end of the all the test cases a log file c:\logtests.txt will be generated which will describe the reason for failures if any.
