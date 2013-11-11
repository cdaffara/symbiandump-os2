/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* test tool step implementation
*
*/


/**
 @file
*/


#include <e32std.h>
#include "ttesttoolstep.h"

_LIT8(KLabel,"label");

const TChar KCarriageReturn = '\r';
const TChar KLineReturn = '\n';
const TChar KDelemeter = ':';

HBufC8* ParseOutputFileLC(const TPtrC& aFileName);
TInt ReadWordL(const TDesC8& aBuffer, TInt& aPos, TPtrC8& aLine);
void ConstuctExpectedArrayL(RPointerArray<HBufC>& aArray, const TDesC8& aBuffer,TBool aCheck, TBool aException = EFalse);
void ResetAndDestroyHBufCList(TAny* aList);

//
// CTestToolListCertStep
//

CTestToolListCertStep::~CTestToolListCertStep()
	{

	}
	

CTestToolListCertStep::CTestToolListCertStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestToolListCertStep);
	}

/**
 * Override of base class virtual. Prepares for the test run of Filetokens
 * @return TVerdict code
 */
TVerdict CTestToolListCertStep::doTestStepPreambleL()
	{
	_LIT(KActualOutput, "actualoutput");
	GetStringFromConfig(ConfigSection(), KActualOutput, iActualOutput);	

	// construct name of the cert
	if (GetStringFromConfig(ConfigSection(), KExpectedOwner, iExpectedOwner))
		{
		iOwnerExist = 1;	
		}
	if (!GetIntFromConfig(ConfigSection(), KExpectedListStore, iListStoreExist))
		{
		iListStoreExist = 0;	
		}
	if(!GetIntFromConfig(ConfigSection(), KStore, iExpectedStore))
		{
		iExpectedStore = -1;
		}
	if(!GetIntFromConfig(ConfigSection(), KExpectedNumLabel, iExpectedNumLabel))
		{
		iExpectedNumLabel = 1;
		}
	if (GetStringFromConfig(ConfigSection(), KExpectedLabel1, iExpectedLabel1))
		{
		iLabel1Exist = 1;		
		}
	if (GetStringFromConfig(ConfigSection(), KExpectedLabel2, iExpectedLabel2))
		{
		iLabel2Exist = 1;		
		}
	return TestStepResult();
	}

/**
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters from an ini file section
 * @return TVerdict code
 */
TVerdict CTestToolListCertStep::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

	CActiveScheduler* sched = NULL;
	sched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);	
	CTestToolEngine* activeEngine = CTestToolEngine::NewLC();
	
	if (iLabel1Exist && !iLabel2Exist)
		{
		iVerdict = activeEngine->ListCerts(iExpectedLabel1, iExpectedNumLabel);
		}
	else if (iLabel2Exist && iLabel1Exist)
		{
		iVerdict = activeEngine->ListCerts(iExpectedLabel1, iExpectedLabel2, iExpectedNumLabel);
		}
	else
		{
		RPointerArray<HBufC> CertList;
		CleanupStack::PushL(TCleanupItem(ResetAndDestroyHBufCList, &CertList));
		HBufC8* fileContents = ParseOutputFileLC(iActualOutput);
		if (iListStoreExist)
			{
			_LIT8(KLabel1, "label:");
			TLex8 lex(*fileContents);
			while(!lex.Eos())
                {
                TPtrC8 token = lex.NextToken();
                if(token.CompareF(KLabel1) == KErrNone)
                	{
                	TInt offset = lex.Offset();
                	TChar c = lex.Get();
                	while(c != KCarriageReturn && c != KLineReturn)
                		{
                		c = lex.Get();
                		} 
                	TInt end = lex.Offset();
                	TPtrC8 newtoken;
                	TInt tokenLen = end-offset-1;
					newtoken.Set(&fileContents->Des()[offset], tokenLen);
		
                	HBufC* tmp = HBufC::NewLC(tokenLen);
                	tmp->Des().Copy(newtoken);
                	tmp->Des().Trim();
                	CertList.Append(tmp);
                	CleanupStack::Pop(tmp);
                	}
                }
			CleanupStack::PopAndDestroy(fileContents);
			iVerdict = activeEngine->ListStore(CertList);
			}
		else
			{
			ConstuctExpectedArrayL(CertList, fileContents->Des(), ETrue);
			CleanupStack::PopAndDestroy(fileContents);
			if (iOwnerExist)
				{
				iVerdict = activeEngine->ListCerts(CertList, iExpectedOwner);
				}
			else if (iExpectedStore != -1)
				{
				iVerdict = activeEngine->ListCerts(CertList, iExpectedStore);
				}
			else
				{
				iVerdict = activeEngine->ListCerts(CertList);	
				}
			}
		CleanupStack::PopAndDestroy(&CertList);
		}
	
	
	CleanupStack::PopAndDestroy(2, sched);

	if (iVerdict == EFail)
		{
		INFO_PRINTF1(_L("Expected Certificate does not Exist"));
		}
	SetTestStepResult(iVerdict);

	return TestStepResult();
	}

/**
 * Override of base class virtual
 * @return TVerdict code
 */
TVerdict CTestToolListCertStep::doTestStepPostambleL()
	{
	CActiveScheduler::Install(NULL);
	return TestStepResult();
	}


//
// CTestToolGetTrustAppsStep
//

CTestToolGetTrustAppsStep::~CTestToolGetTrustAppsStep()
	{
	iApps.ResetAndDestroy();
	}
	

CTestToolGetTrustAppsStep::CTestToolGetTrustAppsStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestToolGetTrustAppsStep);
	}

/**
 * Override of base class virtual. Prepares for the test run of Filetokens
 * @return TVerdict code
 */
TVerdict CTestToolGetTrustAppsStep::doTestStepPreambleL()
	{
	_LIT(KNumApps, "numapps"); // this specifies how many commands to read
	_LIT(KAppsBase, "apps"); // + number (0-based) = file to check for
	
	if (!GetStringFromConfig(ConfigSection(), KExpectedLabel1, iExpectedLabel))
		{
		INFO_PRINTF1(_L("label name is missing"));
		SetTestStepResult(EFail);
		}
	TInt numApps = 0;
	if (GetIntFromConfig(ConfigSection(), KNumApps, numApps) && numApps!=0)
		{
		TPtrC appName;
		for (TInt i=0; i<numApps; i++)
			{
			// construct name of the key
			const TInt KKeyBufSize=64;
			TBuf<KKeyBufSize> keyBuf(KAppsBase);
			keyBuf.AppendNum(i);
			
			if (GetStringFromConfig(ConfigSection(), keyBuf, appName))
				{			
				HBufC* apps = appName.AllocLC();
				iApps.AppendL(apps);
				CleanupStack::Pop(apps);								
				}
			else
				{
				SetTestStepResult(EFail);
				// the string must exist, otherwise the config is invalid
				ERR_PRINTF2(_L("Missing apps name for key '%S'"), &keyBuf);
				}
			}
		}
	return TestStepResult();
	}

/**
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters from an ini file section
 * @return TVerdict code
 */
TVerdict CTestToolGetTrustAppsStep::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

	CActiveScheduler* sched = NULL;
	sched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);	
	CTestToolEngine* activeEngine = CTestToolEngine::NewLC();
	iVerdict = activeEngine->CheckApps(iApps, iExpectedLabel);
	
	CleanupStack::PopAndDestroy(2, sched);
	if (iVerdict == EFail)
		{
		INFO_PRINTF1(_L("Certificate is not trusted for the applicaion"));
		}
	SetTestStepResult(iVerdict);

	return TestStepResult();
	}

/**
 * Override of base class virtual
 * @return TVerdict code
 */
TVerdict CTestToolGetTrustAppsStep::doTestStepPostambleL()
	{
	CActiveScheduler::Install(NULL);
	return TestStepResult();
	}

//
// CTestToolListKeyStep
//

CTestToolListKeyStep::~CTestToolListKeyStep()
	{
	}
	

CTestToolListKeyStep::CTestToolListKeyStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestToolListKeyStep);
	}

/**
 * Override of base class virtual. Prepares for the test run of Filetokens
 * @return TVerdict code
 */
TVerdict CTestToolListKeyStep::doTestStepPreambleL()
	{
	// construct name of the key

	GetStringFromConfig(ConfigSection(), KActualOutput, iActualOutput);	
	if (!GetIntFromConfig(ConfigSection(), KExpectedListStore, iListStoreExist))
		{
		iListStoreExist = 0;	
		}
	if(!GetIntFromConfig(ConfigSection(), KStore, iExpectedStore))
		{
		iExpectedStore = -1;
		}
	if(!GetIntFromConfig(ConfigSection(), KExpectedNumLabel, iExpectedNumLabel))
		{
		iExpectedNumLabel = 1;
		}
	if (GetStringFromConfig(ConfigSection(), KExpectedLabel1, iExpectedLabel1))
		{
		iLabel1Exist = 1;		
		}
	if (GetStringFromConfig(ConfigSection(), KExpectedLabel2, iExpectedLabel2))
		{
		iLabel2Exist = 1;		
		}
	return TestStepResult();
	}

/**
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters from an ini file section
 * @return TVerdict code
 */
TVerdict CTestToolListKeyStep::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

	CActiveScheduler* sched = NULL;
	sched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);	
	CTestToolEngine* activeEngine = CTestToolEngine::NewLC();
	if (iLabel1Exist && !iLabel2Exist)
		{
		iVerdict = activeEngine->ListKeys(iExpectedLabel1, iExpectedNumLabel);
		}
	else if (iLabel2Exist && iLabel1Exist)
		{
		iVerdict = activeEngine->ListKeys(iExpectedLabel1, iExpectedLabel2, iExpectedNumLabel);
		}
	else
		{
		RPointerArray<HBufC> KeyList;
		CleanupStack::PushL(TCleanupItem(ResetAndDestroyHBufCList, &KeyList));
		HBufC8* fileContents = ParseOutputFileLC(iActualOutput);
		if (iListStoreExist)
			{
			_LIT8(KLabel1, "label:");
			TLex8 lex(*fileContents);
			while(!lex.Eos())
                {
                TPtrC8 token = lex.NextToken();
                if(token.CompareF(KLabel1) == KErrNone)
                	{
                	TInt offset = lex.Offset();
                	TChar c = lex.Get();
                	while(c != KCarriageReturn && c != KLineReturn)
                		{
                		c = lex.Get();
                		} 
                	TInt end = lex.Offset();
                	TPtrC8 newtoken;
                	TInt tokenLen = end-offset-1;
					newtoken.Set(&fileContents->Des()[offset], tokenLen);
		
                	HBufC* tmp = HBufC::NewLC(tokenLen);
                	tmp->Des().Copy(newtoken);
                	tmp->Des().Trim();
                	KeyList.Append(tmp);
                	CleanupStack::Pop(tmp);
                	}
                }
			CleanupStack::PopAndDestroy(fileContents);
			iVerdict = activeEngine->ListStoreKey(KeyList);
			}
		else
			{
			ConstuctExpectedArrayL(KeyList, fileContents->Des(), EFalse);
			CleanupStack::PopAndDestroy(fileContents);
			if (iExpectedStore != -1)
				{
				iVerdict = activeEngine->ListKeys(KeyList, iExpectedStore);
				}
			else
				{
				iVerdict = activeEngine->ListKeys(KeyList);	
				}
			}
		CleanupStack::PopAndDestroy(&KeyList); 
		}
	
	CleanupStack::PopAndDestroy(2, sched);

	if (iVerdict == EFail)
		{
		INFO_PRINTF1(_L("Expected Keys do not Exist"));
		}
	SetTestStepResult(iVerdict);
	
	return TestStepResult();
	}

/**
 * Override of base class virtual
 * @return TVerdict code
 */
TVerdict CTestToolListKeyStep::doTestStepPostambleL()
	{
	CActiveScheduler::Install(NULL);
	return TestStepResult();
	}


//
// CTestToolGetPolicyStep
//

CTestToolGetPolicyStep::~CTestToolGetPolicyStep()
	{
	}
	

CTestToolGetPolicyStep::CTestToolGetPolicyStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestToolGetPolicyStep);
	}

/**
 * Override of base class virtual. Prepares for the test run of Filetokens
 * @return TVerdict code
 */
TVerdict CTestToolGetPolicyStep::doTestStepPreambleL()
	{
	// construct name of the key
	if (GetStringFromConfig(ConfigSection(), KExpectedLabel1, iExpectedLabel))
		{
		iLabelExist = 1;		
		}
	if (!GetStringFromConfig(ConfigSection(), KExpectedUser, iExpectedUser))
		{
		INFO_PRINTF1(_L("policy user name is missing"));
		SetTestStepResult(EFail);
		}
	if (!GetIntFromConfig(ConfigSection(), KExpectedUserExist, iExpectedUserExist))
		{
		iExpectedUserExist = 0;
		}
	return TestStepResult();
	}

/**
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters from an ini file section
 * @return TVerdict code
 */
TVerdict CTestToolGetPolicyStep::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

	CActiveScheduler* sched = NULL;
	sched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);	
	CTestToolEngine* activeEngine = CTestToolEngine::NewLC();
    if (iLabelExist)
		{
		iVerdict = activeEngine->GetPolicy(iExpectedLabel, iExpectedUser, iExpectedUserExist);
		}

	CleanupStack::PopAndDestroy(2, sched);
	if (iVerdict == EFail)
		{
		INFO_PRINTF1(_L("Exptected Keys are not Exist"));
		}
	SetTestStepResult(iVerdict);
	
	return TestStepResult();
	}

/**
 * Override of base class virtual
 * @return TVerdict code
 */
TVerdict CTestToolGetPolicyStep::doTestStepPostambleL()
	{
	CActiveScheduler::Install(NULL);
	return TestStepResult();
	}



//
// CTestToolParseFileStep
//

CTestToolParseFileStep::~CTestToolParseFileStep()
	{
	iArgs.ResetAndDestroy();
	}
	

CTestToolParseFileStep::CTestToolParseFileStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestToolParseFileStep);
	}

/**
 * Override of base class virtual. Prepares for the test run of Filetokens
 * @return TVerdict code
 */
TVerdict CTestToolParseFileStep::doTestStepPreambleL()
	{
	_LIT(KNumLines, "numlines"); // this specifies how many commands to read
	_LIT(KLineBase, "line"); // + number (0-based) = file to check for
	

	TInt numlines = 0;
	if (GetIntFromConfig(ConfigSection(), KNumLines, numlines) && numlines!=0)
		{
		TPtrC lineContent;
		for (TInt i=0; i<numlines; i++)
			{
			// construct name of the key
			const TInt KKeyBufSize=64;
			TBuf<KKeyBufSize> keyBuf(KLineBase);
			keyBuf.AppendNum(i);
			
			if (GetStringFromConfig(ConfigSection(), keyBuf, lineContent))
				{			
				HBufC* line = lineContent.AllocLC();
				line->Des().Trim();
				iArgs.AppendL(line);
				CleanupStack::Pop(line);								
				}
			else
				{
				SetTestStepResult(EFail);
				// the string must exist, otherwise the config is invalid
				ERR_PRINTF2(_L("Missing apps name for key '%S'"), &keyBuf);
				}
			}
		}
	if (!GetStringFromConfig(ConfigSection(), KActualOutput, iActualOutput))
			{
			INFO_PRINTF1(_L("actual output filename is missing"));
			SetTestStepResult(EFail);	
			}
	if (numlines == 0)
		{
		if (!GetStringFromConfig(ConfigSection(), KExpectedError, iExpectedError))
			{
			INFO_PRINTF1(_L("error value is missing"));
			SetTestStepResult(EFail);
			}
		}
	return TestStepResult();
	}

/**
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters from an ini file section
 * @return TVerdict code
 */
TVerdict CTestToolParseFileStep::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
	if (iArgs.Count() == 0)
		{
		HBufC8* fileContents = ParseOutputFileLC(iActualOutput);
		HBufC8* actualError = GetErrorFromOutputFileLC(*fileContents);
		if (actualError == NULL)
			{
			INFO_PRINTF1(_L("Failed to parse the output File"));
			SetTestStepResult(EFail);
			}
		
		HBufC* actual16 = HBufC::NewLC(actualError->Length());
		actual16->Des().Copy(*actualError);
	
		if (iExpectedError.CompareF(*actual16) != KErrNone)
			{
			INFO_PRINTF1(_L("Expected Error is Different from Actual"));
			SetTestStepResult(EFail);
			}
		CleanupStack::PopAndDestroy(3, fileContents); //actual16, actualError, fileContents
		}
	else
		{
		HBufC8* fileContents = ParseOutputFileLC(iActualOutput);
		RPointerArray<HBufC> actualoutput;
		CleanupStack::PushL(TCleanupItem(ResetAndDestroyHBufCList, &actualoutput));
		ConstuctExpectedArrayL(actualoutput, fileContents->Des(), EFalse, ETrue);
		TBool result = EFalse;
		for (TInt i = 0; i < iArgs.Count(); i++)
			{
			result = EFalse;
			for (TInt j = 5; j < actualoutput.Count(); j++)
				{
				if (iArgs[i]->Des().CompareF(actualoutput[j]->Des()) == KErrNone)
					{
					result = ETrue;
					break;
					}
				}
			if (!result)
				{
				break;
				}
			}
		if (!result)
			{
			INFO_PRINTF1(_L("expected output and actual output is not matching"));
			SetTestStepResult(EFail);
			}
		CleanupStack::PopAndDestroy(2, fileContents); // fileContents ,actualoutput
		}
		
	return TestStepResult();
	}


HBufC8* CTestToolParseFileStep::GetErrorFromOutputFileLC(const TDesC8& aBuffer)
	{
	_LIT8(KOutput, "output");
	TInt readPos = 0;
	TPtrC8 wordContents;

	HBufC8* wordBuf = NULL;
	while (!ReadWordL(aBuffer, readPos, wordContents))
		{
		wordBuf = wordContents.AllocLC();
		wordBuf->Des().Trim();
		if (wordBuf->Des().CompareF(KOutput) == KErrNone)
			{
			readPos+=1;
			if (!ReadWordL(aBuffer, readPos, wordContents))
				{
				CleanupStack::PopAndDestroy(wordBuf);
				wordBuf = wordContents.AllocLC();
				wordBuf->Des().Trim();
				break;
				}
			}
		CleanupStack::PopAndDestroy(wordBuf);
		}
	return wordBuf;		
	}
	
/**
 * Override of base class virtual
 * @return TVerdict code
 */
TVerdict CTestToolParseFileStep::doTestStepPostambleL()
	{
	CActiveScheduler::Install(NULL);
	return TestStepResult();
	}



//
// global public methods
//

// Reads the words from the Buffer either ended with : or \n 
// if the word is "label", then read the next word and add to the array 
// if the word is not label read upto the end of the line then go to next loop
// if exception is on , reads the entire line
void ConstuctExpectedArrayL(RPointerArray<HBufC>& aArray, const TDesC8& aBuffer,TBool aCheck, TBool aException)
	{
	TInt readPos = 0;
	TPtrC8 wordContents;

	TInt bufferLength = aBuffer.Length();
	while (!ReadWordL(aBuffer, readPos, wordContents))
		{
		HBufC8* wordBuf = wordContents.AllocLC();
		wordBuf->Des().Trim();
		if (aException)
			{
			HBufC* currentLabel = HBufC::NewLC(wordBuf->Length());
			currentLabel->Des().Copy(*wordBuf);
			currentLabel->Des().Trim();
			aArray.AppendL(currentLabel);
			CleanupStack::Pop(currentLabel);			
			}
		else if (wordBuf->Des().CompareF(KLabel) == KErrNone)
			{
			if (!ReadWordL(aBuffer, readPos, wordContents))
				{
				TPtrC8 word;
				TInt len = wordContents.Length();
				// aCheck is on for reading the certool output file,because the output will be like this
				// label: abc      format:.... ,  so we need to exclude format
				if (aCheck) // certdetails 
					{
					len = len - 6;	
					}		
				word.Set(wordContents.Ptr(), len);
				HBufC* currentLabel = HBufC::NewLC(word.Length());
				currentLabel->Des().Copy(word);
				currentLabel->Des().Trim();
				aArray.AppendL(currentLabel);
				CleanupStack::Pop(currentLabel);			
				}
			else
				{
				break;
				}
			}
		else
			{
			readPos--;
			while (readPos < bufferLength)
				{
				TChar c = aBuffer[readPos];
	
				if (c == KCarriageReturn || c == KLineReturn) 
					{
					readPos += 1;
					break;
					}	
				readPos++;
				}	
			}
		CleanupStack::PopAndDestroy(wordBuf);
		}
	}

// Reads the output file and return into the buffer.
HBufC8* ParseOutputFileLC(const TPtrC& aFileName)
	{
	RFs fs;
	RFile file;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(file.Open(fs, aFileName, EFileRead));
	CleanupClosePushL(file);
	TInt fSize;
	file.Size(fSize);
	
	HBufC8* fileContents = HBufC8::NewLC(fSize);
	TPtr8 ptr(fileContents->Des());
	ptr.SetLength(fSize);
	
	// create file stream and Read the content from the file
	RFileReadStream inputFileStream(file);
    CleanupClosePushL(inputFileStream);
	inputFileStream.ReadL(ptr, fSize);
	CleanupStack::PopAndDestroy(&inputFileStream);
	CleanupStack::Pop(fileContents);
	CleanupStack::PopAndDestroy(2, &fs);
	CleanupStack::PushL(fileContents);

	return fileContents;
	}
	
// Reads the words from the aBuffer either ended with : or \n or \r
TInt ReadWordL(const TDesC8& aBuffer, TInt& aPos, TPtrC8& aLine)
	{
	TBool endOfBuffer = EFalse;

	TInt bufferLength = aBuffer.Length();
	if ( aPos > bufferLength || aPos < 0 )
		{	
		return ETrue; // End of buffer
		}
	
	TInt endPos = aPos;
	// find the position of the next delimeter		
	endPos = aPos;	
	while (endPos < bufferLength)
		{
		TChar c = aBuffer[endPos];

		if (c == KCarriageReturn || c == KLineReturn || c == KDelemeter) 
			{
			// exception when comes like c:\filename
			if ((bufferLength > (endPos + 1)) && aBuffer[endPos+1] != '\\')
				{
				break;	
				}		
			}	
		endPos++;
		}

	if (endPos != aPos)	
		{
		TInt tokenLen = endPos - aPos;
		aLine.Set(&aBuffer[aPos], tokenLen);
		endPos += 1;
		}
	else if (endPos == bufferLength)
		{
		return ETrue; // End of buffer
		}
	else
		{
		endPos++;	
		}
	aPos = endPos;
	return endOfBuffer;
	}


void ResetAndDestroyHBufCList(TAny* aList)
	{
	RPointerArray<HBufC>* list = static_cast<RPointerArray<HBufC>*>(aList);

	list->ResetAndDestroy();
	list->Close();
	}









//
// CTestToolGetTrustStep
//

CTestToolGetTrustStep::~CTestToolGetTrustStep()
	{
	
	}
	

CTestToolGetTrustStep::CTestToolGetTrustStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestToolGetTrustStep);
	}

/**
 * Override of base class virtual. Prepares for the test run of Filetokens
 * @return TVerdict code
 */
TVerdict CTestToolGetTrustStep::doTestStepPreambleL()
	{
	
	if (!GetStringFromConfig(ConfigSection(), KExpectedLabel1, iExpectedLabel))
		{
		INFO_PRINTF1(_L("label name is missing"));
		SetTestStepResult(EFail);
		}
	if (!GetIntFromConfig(ConfigSection(), KExpectedTrust, iExpectedTrust))
		{
		INFO_PRINTF1(_L("Expected trust is missing"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

/**
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters from an ini file section
 * @return TVerdict code
 */
TVerdict CTestToolGetTrustStep::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

	CActiveScheduler* sched = NULL;
	sched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);	
	CTestToolEngine* activeEngine = CTestToolEngine::NewLC();
	iVerdict = activeEngine->CheckTrust(iExpectedLabel,iExpectedTrust);
	CleanupStack::PopAndDestroy(2, sched);
	if (iVerdict == EFail)
		{
		INFO_PRINTF1(_L("Certificate is not trusted for the applicaion"));
		}
	SetTestStepResult(iVerdict);

	return TestStepResult();
	}

/**
 * Override of base class virtual
 * @return TVerdict code
 */
TVerdict CTestToolGetTrustStep::doTestStepPostambleL()
	{
	CActiveScheduler::Install(NULL);
	return TestStepResult();
	}

CTestToolCheckFileStep::CTestToolCheckFileStep()
	{}

CTestToolCheckFileStep::~CTestToolCheckFileStep()
	{}

TVerdict CTestToolCheckFileStep::doTestStepPreambleL()
	{
	if (!GetStringFromConfig(ConfigSection(), KFileName,iFileName))
		{
		INFO_PRINTF1(_L("file name is missing"));
		SetTestStepResult(EFail);
		}
	if (!GetStringFromConfig(ConfigSection(), KCheckType,iCheckType))
		{
		INFO_PRINTF1(_L("chek type for file is missing"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestToolCheckFileStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CTestToolCheckFileStep::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFile file;
	CleanupClosePushL(file);
	
	TInt error = file.Open(fs,iFileName,EFileRead);
	if( ( iCheckType.Compare(_L("present")) == 0 && error == KErrNone ) || 	
		( iCheckType.Compare(_L("absent")) == 0 && error == KErrNotFound )
		)
		{
		SetTestStepResult(EPass);
		}
	else
		{
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(2,&fs); // file
	return TestStepResult();
	}
// End of file
