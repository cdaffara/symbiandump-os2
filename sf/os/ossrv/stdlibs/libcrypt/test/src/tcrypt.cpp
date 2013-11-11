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

// INCLUDE FILES
#include <e32svr.h>
#include "tcrypt.h"

// EXTERNAL FUNCTION PROTOTYPES  
extern "C" {
IMPORT_C char *crypt(const char *key, const char *salt);
IMPORT_C void setkey(const char *key);
IMPORT_C void encrypt(char block[], int edflag);
}

// LOCAL FUNCTION PROTOTYPES
LOCAL_C void GetBitVector(char data[], char* buffer);
LOCAL_C char *TrimWhiteSpaces(char *string);

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TrimWhiteSpaces
// To to trim whitespaces in the input string
// -----------------------------------------------------------------------------
//
LOCAL_C char *TrimWhiteSpaces(char *string)
{
	char *pTemp = string;
	for(;*string != '\0'; string++)
	{
		if((*string == ' ') || (*string == '\t'))
		{
			pTemp++;
		}
	}
	
	return pTemp;
}

// -----------------------------------------------------------------------------
// GetBitVector
// This function unpacks the byte to obtain the corresponding bit vector
// -----------------------------------------------------------------------------
//
LOCAL_C void GetBitVector(char data[], char* buffer)
{
	int temp;
	if(buffer != NULL )
	{
		temp = strlen(buffer);
		for(int i = 0 ; i<temp ; ++i, ++buffer)
		{
			data[i] = *buffer - '0';
		}
	}
}


CTestCrypt::~CTestCrypt() 
	{ 
	
	}  

CTestCrypt::CTestCrypt(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestCrypt::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	
	SetTestStepResult(EPass);
	iTestDataFile = fopen("C:\\tcrypt\\test_data.dat", "r");	
	if(iTestDataFile == NULL)
		{
		SetTestStepResult(EFail);	
		}

	return TestStepResult();
	}

TVerdict CTestCrypt::doTestStepPostambleL()
	{
	fclose(iTestDataFile);
	__UHEAP_MARKEND;	
	return TestStepResult();
	}

TVerdict CTestCrypt::doTestStepL()
	{
	int err;
		
	if(TestStepName() == KEncrypt)
		{
   		INFO_PRINTF1(_L("Encrypt():"));
   		err = Encrypt();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == KCrypt)
		{
   		INFO_PRINTF1(_L("Crypt():"));
   		err = Crypt();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
  		
   	return TestStepResult(); 
	}


// -----------------------------------------------------------------------------
// CTestCrypt::Encrypt
// Encrypt function
// -----------------------------------------------------------------------------
//
TInt CTestCrypt::Encrypt()
{
	static int tId = 0;
	char pTemp[30];
	memset(pTemp, 0, 30);
	sprintf(pTemp, "ENCRYPT_TEST_DATA_%d", ++tId);
    INFO_PRINTF2(_L("Begin: ENCRYPT_TEST_DATA_%d\n"), tId);
 
    // locate "test_data_id" from within the file
    if(!RepositionFilePointer(pTemp))
    {
    	// String not found...invalid test data ID
    	INFO_PRINTF1(_L("Requested test data ID could not be found\n"));
    	return KErrNotFound;
    }

	// Get the key, data block, operation to be performed and the 
	// expected output for the current test data ID
	char key[64] = 
	{
		 0
	};
	char block[64] = 
	{
		0
	};
	char output[64] = 
	{
		0
	};
	int edflag = -1;
	if(GetEncryptTestData(key, block, &edflag, output) != KErrNone)
	{
		// Test data not found or is not present in the expected 
		// format
		INFO_PRINTF1(_L("Test data not found or is not present in the expected format\n"));
		return KErrNotFound;
	}
	
	// Perform encryption/decryption
	
	// Invoke setkey from the libcrypt library
	setkey(key);
	
	// Call the encrypt function
	encrypt(block,edflag);
	
	// Verify if the final output is same as the expected output

	if(!strcmp(block,block))
	{
		INFO_PRINTF2(_L("End: ENCRYPT_TEST_DATA_%d\n"), tId);
		// Test case passed
		return KErrNone;
	}
	INFO_PRINTF1(_L("Output from the encrypt() function does not match the \"expected output\""));
	INFO_PRINTF2(_L("End: ENCRYPT_TEST_DATA_%d\n"), tId);
	return KErrNotFound;
}

// -----------------------------------------------------------------------------
// CTestCrypt::RepositionFilePointer
// This function positions the file pointer to the line immediately following
// the string aString
// -----------------------------------------------------------------------------
//

TInt CTestCrypt::RepositionFilePointer(const char *aString)
{
    char buffer[256];
   	char * ptr = NULL;
    while(fgets((char*)buffer, 256, iTestDataFile) != NULL)
    {
    	 ptr = NULL;
    
    	if((ptr=strchr(buffer,'\r')) || (ptr=strchr(buffer,'\n'))) //check for both
		*ptr='\0';
    	if(!strcmp(buffer, aString))
    	{
    		return 1;
    	}
    	memset(buffer, 0, 256);
    }
    return 0;
}

// -----------------------------------------------------------------------------
// CTestCrypt::GetEncryptTestData
// This function reads the test data for encrypt() API
// -----------------------------------------------------------------------------
//
TInt CTestCrypt::GetEncryptTestData(char key[], char block[], int *edflag, char output[])
{
	char buffer[256];
	char *p = NULL;
	bool bKey = false,		// will be set to true upon reading 'key'
	     bBlock = false,    // will be set to true upon reading 'data block'
	     bEdflag = false,   // will be set to true upon reading 'edflag'
	     bOutput = false;   // will be set to true upon reading 'expected output'
	     
	char *pTemp = NULL;
   	char * ptr = NULL;

	while((p = fgets(buffer, 256, iTestDataFile)) != NULL)
	{

		ptr = NULL;
    
    	if((ptr=strchr(buffer,'\r')) || (ptr=strchr(buffer,'\n'))) //check for both
			*ptr='\0';
		if(strstr(buffer, "//") != NULL)
		{
			continue;
		}
		if(!strcmp(buffer, "END_TEST_DATA"))
		{
			if(bKey && bBlock && bEdflag && bOutput)
			{
				return KErrNone;
			}
			return KErrNotFound;
		}
		if(strstr(buffer, "KEY") != NULL)
		{
			// Read the key
			
			// Get bytes...
			pTemp = strstr(buffer, ":");
			if(pTemp != NULL)
			{
				pTemp++;
			    pTemp = TrimWhiteSpaces(pTemp);
				GetBitVector(key, pTemp);
			}
			bKey = true;
			continue;
		}
		if(strstr(buffer, "DATA_BLOCK") != NULL)
		{
			// Read the data block
			
			pTemp = strstr(buffer, ":");
			if(pTemp != NULL)
			{
				pTemp++;
				pTemp = TrimWhiteSpaces(pTemp);
				GetBitVector(block, pTemp);
			}
			bBlock = true;
			continue;
		}
		if(strstr(buffer, "ED_FLAG") != NULL)
		{
			// Read the ed_flag parameter
			
			pTemp = strstr(buffer, ":");
			if(pTemp != NULL)
			{
				pTemp++;
				pTemp = TrimWhiteSpaces(pTemp);
				*edflag = (*pTemp) - '0';
			}
			bEdflag = true;
			continue;
		}
		if(strstr(buffer, "EXPECTED_OUTPUT") != NULL)
		{
			// Read the bit vector for the expected output
			
			pTemp = strstr(buffer, ":");
			if(pTemp != NULL)
			{
				pTemp++;
				pTemp = TrimWhiteSpaces(pTemp);
				GetBitVector(output, pTemp);
			}
			bOutput = true;
			continue;
		}
	}
	
	return KErrNotFound;
}

// -----------------------------------------------------------------------------
// CTestCrypt::Crypt
// Test function to perform crypt() on the input data
// -----------------------------------------------------------------------------
//
TInt CTestCrypt::Crypt()
{
	static int tId = 0;
	char pTemp[30];
	memset(pTemp, 0, 30);
	sprintf(pTemp, "CRYPT_TEST_DATA_%d", ++tId);
    INFO_PRINTF2(_L("Begin CRYPT_TEST_DATA_%d\n"), tId);
    
    // locate "test_data_id" from within the file
    if(!RepositionFilePointer(pTemp))
    {
    	// String not found...invalid test data ID
    	INFO_PRINTF1(_L("Requested test data ID could not be found\n"));
    	return KErrNotFound;
    }
    
    char password[34] = 
    {
    	'\0'
    };
    char salt[30] = 
    {
    	'\0'
    };
    char output[35] = 
    {
    	'\0'
    };
    
    if(GetCryptTestData(password, salt, output) != KErrNone)
    {
    	// Data not in the expected format or is invalid
    	INFO_PRINTF1(_L("Test data not found or is not present in the expected format\n"));
    	return KErrNotFound;
    }
    
    char *crypt_output = NULL;
    // Invoke crypt()
    crypt_output = crypt(password,salt);
	if(!strcmp(output,""))
	{
		// Since salt is NULL, the expected output is ignored...
		return KErrNone;
	}
    if(!strcmp(salt, ""))
    {
		// salt is NULL, so skip the first byte from the crypt output
		if(crypt_output != NULL)
		{
			crypt_output++;
			if(!strcmp(crypt_output, &output[0]))
			{
				INFO_PRINTF2(_L("End: CRYPT_TEST_DATA_%d\n"), tId);
				return KErrNone;
			}
			INFO_PRINTF1(_L("Output from the crypt() function does not match the \"expected output\""));
			return KErrNotFound;
		}
    }
    else
    {
    	// salt is not NULL
    	if(!strcmp(crypt_output, output))
    	{
    		INFO_PRINTF2(_L("End: CRYPT_TEST_DATA_%d\n"), tId);
    		return KErrNone;
    	}
    	INFO_PRINTF1(_L("Output from the crypt() function does not match the \"expected output\""));
    	return KErrNotFound;
    }
    return KErrNotFound;
}

// -----------------------------------------------------------------------------
// CTestCrypt::GetCryptTestData
// To retrieve the test data for crypt() API
// -----------------------------------------------------------------------------
//
TInt CTestCrypt::GetCryptTestData(char password[], char salt[], char output[])
{
	char buffer[256];
	char *p = NULL;
	char *pTemp = NULL;
	int nLength = 0;
	bool bPassword = false,
	     bSalt = false,
	     bOutput = false;
   	char * ptr = NULL;

	while((p = fgets(buffer, 256, iTestDataFile)) != NULL)
	{
    	ptr = NULL;
    
    	if((ptr=strchr(buffer,'\r')) || (ptr=strchr(buffer,'\n'))) //check for both
			*ptr='\0';
		if(strstr(buffer, "//") != NULL)	// skip the comments
		{
			// "//" could appear within password or salt, so further
			// check is required
			
			// Since judicious use of whitespaces is allowed only from within
			// the comment lines, the comment line will always start with
			// "//"
			if(buffer[0] == '/' && buffer[1] == '/')
			{
				continue;
			}
		}
		if(!strcmp(buffer, "END_TEST_DATA"))
		{
			if(bPassword && bSalt && bOutput)
			{
				return KErrNone;
			}
			return KErrNotFound;
		}

		// Verify if the input buffer has "data". Data is followed by ":"
		pTemp = strstr(buffer, ":");
		if(pTemp != NULL)
		{
			pTemp++;
			pTemp = TrimWhiteSpaces(pTemp);
			nLength = strlen(pTemp);
			if(strstr(buffer, "PASSWORD") != NULL)
			{
				strncpy(password,pTemp,nLength);
				bPassword = true;
				continue;
			}
			else if(strstr(buffer, "SALT") != NULL)
			{
				strncpy(salt,pTemp,nLength);
				bSalt = true;
				continue;
			}
			else if(strstr(buffer, "EXPECTED_OUTPUT") != NULL)
			{
				strncpy(output,pTemp,nLength);
				bOutput = true;
				continue;
			}
			else
			{
				// Unexpected output
				return KErrNotFound;
			}
		}
	}
	return KErrNotFound;
}

//  End of File

