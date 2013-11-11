/*
/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TESTCRYPT_H__
#define __TESTCRYPT_H__

#include <test/testexecutestepbase.h>
// For file input/output
#include <stdio.h>
#include <string.h>

_LIT(KEncrypt, "Encrypt");
_LIT(KCrypt, "Crypt");

class CTestCrypt : public CTestStep
	{
public:

	~CTestCrypt(); 
	CTestCrypt(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL(); 
	TVerdict doTestStepPostambleL();
	
private:
	/*
	 * Test method to test encrypt() API
	 */
    virtual TInt Encrypt();
        
    /*
     * Test method to test crypt() API
     */
    virtual TInt Crypt();
        
    /*
     * To reposition the file pointer to the line following the
     * requested string, in this case the "test data ID"
     */
    int RepositionFilePointer(const char *aString);
        
    /*
     * Retrieve's the test data for the encrypt() and setkey()
     * functions
     */
    TInt GetEncryptTestData(char key[], char block[], int *edflag, char output[]);
        
    TInt GetCryptTestData(char[], char[], char[]);

    
private:    // Data
      
	FILE *iTestDataFile;
	};

#endif 

