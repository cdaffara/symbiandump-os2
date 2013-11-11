/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/



// Test that checks multi threading with stdcpp library
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <e32base.h>
#include <unistd.h>
#include <pthread.h>
#include <limits>
#ifdef LIBC_OLD_BACKEND
#include <mrtlib.h>
#endif

#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

using namespace std;

const char name_01[] = "c:\\testframework\\tstdcpp\\infile_manual.txt"; // file with data in it
const char name_02[] = "c:\\testframework\\tstdcpp\\Mainoutfile_manual.txt"; // file with data in it
const char name_03[] = "c:\\testframework\\tstdcpp\\Rthreadoutfile_manual.txt"; // file with data in it
const char name_04[] = "c:\\testframework\\tstdcpp\\Pthreadoutfile_manual.txt"; // file with data in it

//pthread id
TInt iThreadId;

void RExecuteL()
{
	cout << "Rthread: Now file ops" << endl;
	ifstream in(name_01);
    ofstream out(name_03);
    out << in.rdbuf(); // Copy file to out
    in.close();
    out.close();	  
}

TInt RThreadEntryL(TAny */*args*/)
{
	CTrapCleanup * cleanup = CTrapCleanup::New();
	TInt err = KErrNone;
#ifdef LIBC_OLD_BACKEND
	TRAP(err, MrtInit());
#endif
	TRAP(err, RExecuteL());
#ifdef LIBC_OLD_BACKEND
    TRAP(err,MrtClose());
#endif
	delete cleanup;
	return 0;
}

void RCreateThreadL()
{
	RThread thread;
	_LIT(KName, "RThread");
	thread.Create(KName, RThreadEntryL, 2000, NULL, NULL);
	thread.Resume();
}


void* PThreadEntryL(TAny */*args*/)
{
	int b = 1;
	cout << "Pthread.... Enter an integer: " << endl;
	cin >> b;
	cout <<"Pthread: Value of b is :" << b << endl;

	cout << "Pthread:Now file ops"  << endl;
	ifstream in(name_01);
    ofstream out(name_04);
    out << in.rdbuf(); // Copy file to out
    in.close();
    out.close();

    //testing stls
    int failures = 0;
    std::string s("abcde");

   	std::string::iterator i1(s.begin());

   	 ++i1;
    if (*i1 != 'b')
      ++failures;

    if (*i1++ != 'b')
       ++failures;
    if (*i1 != 'c')
      ++failures;

    ++ ++i1;
    if (*i1 != 'e')
      ++failures;

    --i1;
    if (*i1 != 'd')
      ++failures;

    if (*i1-- != 'd')
      ++failures;

    if (*i1 != 'c')
      ++failures;

    -- --i1;
    if (*i1 != 'a')
      ++failures;

	if(!failures)
    	std_log(LOG_FILENAME_LINE,"Result : Passed");
 	else
 		std_log(LOG_FILENAME_LINE,"Result : Failed");

 	return 0;
}

void PCreateThreadL()
{
	//create the pthread
	TInt err = pthread_create((unsigned int *)&iThreadId,(pthread_attr_t *)NULL,PThreadEntryL,NULL);
}

int main()
{
    std_log(LOG_FILENAME_LINE,"[ Test Case for multi_thread ]");
    
    cout << "Main thread: Enter a character :"<< endl;
    char k;
    cin >> k;
	cout << "Main thread the character entered is" << k <<endl;
	
	cout  << "Main thread:file ops " << endl;	
	ifstream in(name_01);
    ofstream out(name_02);
    out << in.rdbuf(); // Copy file to out
    in.close();
    out.close();    
    
	//Create a Rthread
    RCreateThreadL();
   	User::After(200000); //Allow Rthread to execute

	//Create a Pthread
	PCreateThreadL();	
	//Wait for the thread to terminate
	pthread_join(iThreadId, NULL);

	std_log(LOG_FILENAME_LINE,"[End Test Case ]");
	testResultXml("multi_thread_manual");
  	close_log_file();
	return 0;
}

