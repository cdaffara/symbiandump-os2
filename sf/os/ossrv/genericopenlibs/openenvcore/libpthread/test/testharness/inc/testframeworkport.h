/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TESTFRAMEWORKPORT_H__
#define __TESTFRAMEWORKPORT_H__

#include <e32base.h>
#include <e32std.h>
#include <string.h>

#ifdef WINDOWS


typedef int TInt;

#define KErrNone 0


void gettimeofday(void*, void*);

class TTestResult 
    {
    public:
        /**
        * Default constructor of TTestResult
        */
	    TTestResult()
	    :iResult(0)
	    ,iResultDes(NULL)
	    {
	    	
	    };
	    
	    ~TTestResult()
	    {
	     if(iResultDes)
	     {
	     	delete [] iResultDes;
	     	iResultDes = NULL;
	     }
	     iResult = 0;
	    };

        /**
        * Sets result and result description of the test case.
        */
        void SetResult( TInt aResultCode, const TDesC& aResultDes )
        {
    	    this->~TTestResult();
    	    iResult = aResultCode;
    	    int length = aResultDes.Length()+1;
    	    iResultDes = new char [length];
    	    memcpy(iResultDes,(char*)aResultDes.Ptr(),length);
        };

    public:

        // Test case result as Symbian OS error code.
        TInt iResult;
        // Descriptive test case result.
        char* iResultDes;
    };
    
#endif 


    
#endif
