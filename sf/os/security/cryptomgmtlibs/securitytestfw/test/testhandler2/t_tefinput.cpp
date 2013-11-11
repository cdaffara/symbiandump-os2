//
// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
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


#include <s32file.h>
#include "t_tefinput.h"
#include "t_errorconverter.h"
#include "tScriptSetup.h"
#include <securityerr.h>
#include <keystore_errs.h>

#ifdef SYMBIAN_AUTH_SERVER
#include <auth_srv_errs.h>
#endif

_LIT8(KSpace, " ");
_LIT8(KTab, "\t");
_LIT8(KReturn, "\r");
_LIT8(KNewline, "\n");
_LIT8(KOpenBrk, "[");
_LIT8(KCloseBrk, "]");
_LIT8(KEquals, "=");
_LIT8(KUnderScore, "_");
_LIT8(KSectionIDTag, "[SEC");

#define KIniSectionIdLength 512
#define KResultLength 512

// Extracts RUN_TESTSTEP from the whole of script file passed in

EXPORT_C TPtrC8 Tefinput::ParseRunTestStep(const TDesC8& aBuf, 
									const TDesC8& aTag,
									TInt& aPos,
									TInt& aError)
	{
	aError = KErrNone;
	TInt endPos = 0;
	TInt startPos = 0;
	TInt tempPos = 0;

	TPtrC8 data = aBuf.Mid(aPos);
	
	tempPos = data.Find(aTag);
	
	if (tempPos != KErrNotFound)
		{
		tempPos += aTag.Length();
			
		TPtrC8 temprunStepData = data.Mid(tempPos);		
		
		endPos = temprunStepData.Find(KNewline);
		if (endPos == KErrNotFound)
			{
			endPos = temprunStepData.Find(KReturn);
			}
		if (endPos == KErrNotFound)
			{
			endPos = temprunStepData.Length();
			}
		
		TInt len = 0;
		len = (endPos - startPos) + 1;
		TPtrC8 runStepData = temprunStepData.Mid(startPos,len);
		aPos += tempPos + runStepData.Length();
		return runStepData;
		}
	else
		{
		aError = KErrNotFound;
		return TPtrC8();
		}

	// None found
	

	}

// Extracts the nth word from the given descritor
EXPORT_C TPtrC8 Tefinput::ParseNthElement(const TDesC8& aBuf, 
									TInt aWordPos,
									TInt& aError)

	{
	aError = KErrNone;
	TInt startPos = KErrNotFound, endPos = KErrNotFound;
	TInt wordCounter =0 ;
	TBool inWord = EFalse;
	TInt i =0;
	for(i = 0; i < aBuf.Length() ; i ++)
		{
		TPtrC8 tmpChar = aBuf.Mid(i,1);
		if(tmpChar == KSpace || tmpChar == KTab || tmpChar == KReturn || tmpChar == KNewline )
			{
			if(inWord)
				{
				if(wordCounter == aWordPos)
					{
					endPos =i-1;
					break;
					}
				inWord = EFalse;
				}			
			}
		else
			{
			if(inWord == EFalse)
				{
				wordCounter ++;
				inWord = ETrue;
				if(wordCounter == aWordPos)
					{
					startPos =i;
					}
				}
			}
		}	
	
	if(startPos < 0 || endPos < 0)
		{
		aError = KErrNotFound;
		return TPtrC8();
		}
	else
		{
		return aBuf.Mid(startPos,(endPos-startPos+1));
		}
	}

// Extracts testCaseID from the whole script file
EXPORT_C TPtrC8 Tefinput::ParseTestCaseID(const TDesC8& aBuf,
										  const TDesC8& aTag,
										  TDes8& aPrevTestID,
										  TInt& aPos, 
										  TInt& aError,
										  TBool& testSet,
										  TBool& startTest
										  )
	{
	aError = KErrNone;
	TInt startPos = 0, endPos = 0;
	
	TPtrC8 datatemp;	
	TPtrC8 testID;
	TPtrC8 data = aBuf.Mid(aPos);
	TInt len = aTag.Length();
	startPos = data.Find(aTag);
	
	TInt filePos = startPos + aPos;
	
	if(startPos != KErrNotFound)
		{
		TInt templen;
		startPos += len;
		filePos += len;
		datatemp.Set(data.Mid(startPos));
		
		endPos = datatemp.Find(KNewline);
		if(endPos == KErrNotFound)
			{
			endPos = datatemp.Find(KReturn);
			TInt temp =0;
			templen = endPos - temp;
			testID.Set(datatemp.Mid(temp,templen));
			filePos += testID.Length();
			aPos = filePos;
			testSet = ETrue;
			startTest = ETrue;
			return Trim(testID);
			}
		else
			{
			TInt temp =0;
			templen = endPos - temp;
			testID.Set(datatemp.Mid(temp,templen));
			filePos += testID.Length();
			aPos = filePos;
			testSet = ETrue;
			startTest = ETrue;
			return Trim(testID);
			}		
		}
	else
		{
		aPos = aPos;
		aError = KErrNotFound;
		testSet = EFalse;
		return aPrevTestID;
		}

	}


// Generates the path for .ini file from the .script file path
EXPORT_C TInt Tefinput::ParseiniPath(const TDesC8& aIniFileName, 
									const TDesC& aScriptPath,
									TDes& aIniFilePath)
	{
	
	TInt err = KErrNone;	
	TInt endPos = aScriptPath.LocateReverse('\\');
	if (endPos == KErrNotFound)
		{
		err = KErrNotFound;
		}
	else
		{
		aIniFilePath.Copy(aIniFileName);
		aIniFilePath.Insert(0, aScriptPath.Left(endPos+1));
		}
	return err;
	}

// Reads the whole of .ini file contents, calls ParseActionbody()
//	to extract info between <actionbody></actionbody>tags
EXPORT_C HBufC8* Tefinput::GetiniFile(RFs& aFs, 
										const TDesC& aIniFilePath, 
										TInt& err)
	{
	RFile iniFile;
	err = iniFile.Open(aFs, aIniFilePath, EFileStream | EFileRead | EFileShareReadersOnly);
	if (err != KErrNone)
		{	
		return NULL;	// Bad exit ...
		}
	// gets size of ini file
	TInt size;
	iniFile.Size(size);
	iniFile.Close();
			
	// reads ini into iTestInput
	HBufC8* testInput = HBufC8::NewLC(size);
	
	TPtr8	aInput = testInput->Des();   // Appoint our local 8 bit pointer descriptor to the heap area to work with
	aInput.SetLength(size); 
	
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(aFs, aIniFilePath, EFileStream | EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(stream);
	stream.ReadL(aInput, size);		// Load in the inifile into Heap buffer
		
//	TPtrC8 iniFileread = ParseElement(pInput, tempsectID, pos, err );
//	TPtrC8 iniFileread = ParseActionbody(pInput, aIniSectionName, err);	
//	iniFileread.TDesC8();
	
	CleanupStack::PopAndDestroy(1); // stream, 
	
	return testInput;  // Return a valid HBuf8 pointer where the ini file has been loaded (on the heap)
	}

// Extracts info between <actionbody></actionbody>tags from the inifile contents
EXPORT_C TInt Tefinput::ParseActionbody( TPtrC8& aBuf, 
											const TDesC8& aIniTag, 
											TPtrC8& aSectionBody)
	{
	TInt len =0, startPos =0, endPos = 0;
	TPtrC8 inifile = aBuf.Mid(startPos);
	
	TBuf8<KIniSectionIdLength> tempsectID(KOpenBrk);
	tempsectID.Append(aIniTag);
	tempsectID.Append(KCloseBrk);
	
	startPos = inifile.Find(tempsectID);
	
	if (startPos != KErrNotFound)
		{
		TPtrC8 actionblk = inifile.Mid(startPos + tempsectID.Length());		
		endPos = actionblk.Find(KSectionIDTag);
		if (endPos == KErrNotFound)
			{
			endPos = actionblk.Length();
			}		
		len = endPos - 0;
		
		aSectionBody.Set(aBuf.Mid(startPos + tempsectID.Length(),len));
		return KErrNone;
		}
	else
		{
//		aSectionBody();
		return KErrNotFound;
		}
	}

// Takes in an integer and returns the appropriate error code with <return></return> tags
EXPORT_C TInt Tefinput::GetActionResult(TInt aErrCode, TDes8& aReturnTag)
	{
	TInt err = KErrNone;
	
	if(aErrCode == KErrNone)
		{
		aReturnTag.Copy(_L8("\r\n\t\t<return>KErrNone</return>"));
		}	
	else if(aErrCode == KErrNotFound)
		{
		aReturnTag.Copy(_L8("\r\n\t\t<return>KErrNotFound</return>"));
		}
	else if(aErrCode == KErrGeneral)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrGeneral</return>"));
			}	 
	else if(aErrCode == KErrCancel)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrCancel</return>"));
			}	
	else if(aErrCode == KErrNoMemory)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrNoMemory</return>"));
			}	

	else if(aErrCode == KErrNotSupported)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrNotSupported</return>"));
			}	

	else if(aErrCode == KErrArgument)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrArgument</return>"));
			}	

	else if(aErrCode == KErrTotalLossOfPrecision)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrTotalLossOfPrecision</return>"));
			}	

	else if(aErrCode == KErrBadHandle)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrBadHandle</return>"));
			}	

	else if(aErrCode == KErrOverflow)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrOverflow</return>"));
			}	

	else if(aErrCode == KErrUnderflow)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrUnderflow</return>"));
			}	

	else if(aErrCode == KErrAlreadyExists)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAlreadyExists</return>"));
			}	

	else if(aErrCode == KErrPathNotFound)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrPathNotFound</return>"));
			}	

	else if(aErrCode == KErrDied)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrDied</return>"));
			}	

	else if(aErrCode == KErrInUse)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrInUse</return>"));
			}	

	else if(aErrCode == KErrServerTerminated)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrServerTerminated</return>"));
			}	

	else if(aErrCode == KErrServerBusy)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrServerBusy</return>"));
			}	

	else if(aErrCode == KErrCompletion)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrCompletion</return>"));
			}	

	else if(aErrCode == KErrNotReady)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrNotReady</return>"));
			}	

	else if(aErrCode == KErrUnknown)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrUnknown</return>"));
			}	

	else if(aErrCode == KErrCorrupt)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrCorrupt</return>"));
			}	

	else if(aErrCode == KErrAccessDenied)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAccessDenied</return>"));
			}	

	else if(aErrCode == KErrLocked)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrLocked</return>"));
			}	

	else if(aErrCode == KErrWrite)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrWrite</return>"));
			}	

	else if(aErrCode == KErrDisMounted)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrDisMounted</return>"));
			}	

	else if(aErrCode == KErrEof)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrEof</return>"));
			}	

	else if(aErrCode == KErrDiskFull)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrDiskFull</return>"));
			}	

	else if(aErrCode == KErrBadDriver)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrBadDriver</return>"));
			}	

	else if(aErrCode == KErrBadName)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrBadName</return>"));
			}	

	else if(aErrCode == KErrCommsLineFail)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrCommsLineFail</return>"));
			}	

	else if(aErrCode == KErrCommsFrame)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrCommsFrame</return>"));
			}	

	else if(aErrCode == KErrCommsOverrun)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrCommsOverrun</return>"));
			}	

	else if(aErrCode == KErrCommsParity)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrCommsParity</return>"));
			}	

	else if(aErrCode == KErrTimedOut)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrTimedOut</return>"));
			}	

	else if(aErrCode == KErrCouldNotConnect)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrCouldNotConnect</return>"));
			}	

	else if(aErrCode == KErrCouldNotDisconnect)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrCouldNotDisconnect</return>"));
			}	

	else if(aErrCode == KErrDisconnected)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrDisconnected</return>"));
			}	

	else if(aErrCode == KErrBadLibraryEntryPoint)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrBadLibraryEntryPoint</return>"));
			}	

	else if(aErrCode == KErrBadDescriptor)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrBadDescriptor</return>"));
			}	

	else if(aErrCode == KErrAbort)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAbort</return>"));
			}	

	else if(aErrCode == KErrTooBig)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrTooBig</return>"));
			}	

	else if(aErrCode == KErrDivideByZero)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrDivideByZero</return>"));
			}	

	else if(aErrCode == KErrBadPower)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrBadPower</return>"));
			}	

	else if(aErrCode == KErrDirFull)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrDirFull</return>"));
			}	

	else if(aErrCode == KErrHardwareNotAvailable)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrHardwareNotAvailable</return>"));
			}		
	else if(aErrCode == KErrPermissionDenied)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrPermissionDenied</return>"));
			}	
	else if(aErrCode == KErrBadPassphrase)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrBadPassphrase</return>"));
			}
	else if(aErrCode == KErrNotSecure)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrNotSecure</return>"));
			}
	else if(aErrCode == KErrKeyNotWeakEnough)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrKeyNotWeakEnough</return>"));
			}
	else if(aErrCode == KErrInvalidPadding)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrInvalidPadding</return>"));
			}
	else if(aErrCode == KErrWeakKey)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrWeakKey</return>"));
			}
	else if(aErrCode == KErrNegativeExportNotSupported)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrNegativeExportNotSupported</return>"));
			}
	else if(aErrCode == KErrKeyAlgorithm)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrKeyAlgorithm</return>"));
			}
	else if(aErrCode == KErrKeyUsage)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrKeyUsage</return>"));
			}
	else if(aErrCode == KErrKeyValidity)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrKeyValidity</return>"));
			}
	else if(aErrCode == KErrKeySize)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrKeySize</return>"));
			}
	else if(aErrCode == KErrKeyAccess)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrKeyAccess</return>"));
			}
	else if(aErrCode == KErrPrivateKeyNotFound)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrPrivateKeyNotFound</return>"));
			}
	else if(aErrCode == KErrAuthenticationFailure)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAuthenticationFailure</return>"));
			}

#ifdef SYMBIAN_AUTH_SERVER
	else if(aErrCode == KErrAuthServUnsupportedExprVersion)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAuthServUnsupportedExprVersion</return>"));
			}
	else if(aErrCode == KErrAuthServNoSuchPlugin)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAuthServNoSuchPlugin</return>"));
			}
	else if(aErrCode == KErrAuthServNoSuchIdentity)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAuthServNoSuchIdentity</return>"));
			}
	else if(aErrCode == KErrAuthServPluginCancelled)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAuthServPluginCancelled</return>"));
			}
	else if(aErrCode == KErrAuthServPluginQuit)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAuthServPluginQuit</return>"));
			}
	else if(aErrCode == KErrAuthServAuthenticationRequired)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAuthServAuthenticationRequired</return>"));
			}
	else if(aErrCode == KErrAuthServIdentityAlreadyExists)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAuthServIdentityAlreadyExists</return>"));
			}
	else if(aErrCode == KErrAuthServIdentityNotFound)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAuthServIdentityNotFound</return>"));
			}
	else if(aErrCode == KErrAuthServTrainingNotFound)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAuthServTrainingNotFound</return>"));
			}
	else if(aErrCode == KErrAuthServRegistrationFailed)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAuthServRegistrationFailed</return>"));
			}
	else if(aErrCode == KErrAuthServCanNotRemoveLastIdentity)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAuthServCanNotRemoveLastIdentity</return>"));
			}
	else if(aErrCode == KErrAuthServCanNotRemoveLastPlugin)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAuthServCanNotRemoveLastPlugin</return>"));
			}
	else if(aErrCode == KErrAuthServPluginNotActive)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAuthServPluginNotActive</return>"));
			}
	else if(aErrCode == KErrAuthServDescTooLong)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAuthServDescTooLong</return>"));
			}
	else if(aErrCode == KErrUnknownAuthStrengthAlias)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrUnknownAuthStrengthAlias</return>"));
			}
	else if(aErrCode == KErrAuthStrengthAliasNotDefined)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAuthStrengthAliasNotDefined</return>"));
			}
	else if(aErrCode == KErrAuthServInvalidAliasStringExpression)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAuthServInvalidAliasStringExpression/return>"));
			}
	else if(aErrCode == KErrAuthServResetMayLoseIdentity)
			{
			aReturnTag.Copy(_L8("\r\n\t\t<return>KErrAuthServResetMayLoseIdentity/return>"));
			}
#endif


//	
	
	return err;
	}
	
// Checks if the first word in RUN_TEST_STEP is an error code. If, then calls GetActionResult().
EXPORT_C TInt Tefinput::ParseActionResult(const TDesC8& aBuf, TDes8& aActionResult)
	{
	TInt errorCode = 0;
	TInt startPos =0;
	
	TPtrC8 data = aBuf.Mid(startPos);
	startPos = data.Find(KEquals);
	if (startPos != KErrNotFound)
		{
		TInt tmp;
		TPtrC8 tmpErrCode = data.Mid(startPos+1);	
		TLex8 lex8(tmpErrCode); 
		tmp = lex8.Val(errorCode);	
		if(tmp != KErrNone)
			{
			return KErrNotFound;
			}
		}
	
	return GetActionResult(errorCode, aActionResult);
	}
	
// Removes any /t, /n and empty spaces from any extracted data.
EXPORT_C TPtrC8 Tefinput::Trim(const TDesC8& aBuf)
	{
	TInt startPos = 0;
	TInt endPos = 0;
	TInt i = 0, j = 0;
		
	for(i = 0; i < aBuf.Length() ; i ++)
		{
		TPtrC8 tmpChar = aBuf.Mid(i,1);
		if(tmpChar != KSpace && tmpChar != KTab && tmpChar != KReturn && tmpChar != KNewline )
			{
			startPos = i;
			break;
			}
		}
	for(j = aBuf.Length()-1; j >= 0 ; j --)
		{
		TPtrC8 tmpChar1 = aBuf.Mid(j,1);
		if(tmpChar1 != KSpace && tmpChar1 != KTab && tmpChar1 != KReturn && tmpChar1 != KNewline )
			{
			endPos = j;
			break;
			}
		}
	if(endPos < startPos)
		{
		endPos = aBuf.Length();
		}
	

	return aBuf.Mid(startPos, endPos - startPos + 1);
	
	}
	
//Removes '_' between 2 words and adds a space 
EXPORT_C  TDes8 Tefinput::TrimActionType(const TDesC8& aBuf, TDes8& aType)
	{
	TInt startPos = 0;
	TInt pos = 0;
	
        // Allocating on heap - since 1024 bytes on stack could be too much for the target.
	HBufC* temp = HBufC::NewLC(512); 
	HBufC* temp2 = HBufC::NewLC(512);	
	
	startPos = aBuf.Find(KUnderScore);
	if(startPos != KErrNotFound)
		{
		temp->Des().Copy(aBuf.Mid(pos,(startPos-pos)));
		temp2->Des().Copy(aBuf.Mid(startPos+1));
		aType.Copy(temp->Des());
		aType.Append(KSpace);
		aType.Append(temp2->Des());
		}
	else
		{
		aType.Copy(aBuf);
		}

        CleanupStack::PopAndDestroy(temp2);                 		
        CleanupStack::PopAndDestroy(temp);                 		                 
        return aType;          
	}

//Checks if END_TESTCASE has been reached
EXPORT_C TBool Tefinput::EndTestCase(const TDesC8& aBuf, const TDesC8& aRunStep, const TDesC8& aTag, TInt& aPos)
	{
	TInt startPos =0;
	
	TInt lenRunStep = aRunStep.Length();
	
	TPtrC8 tempScriptFile;
	tempScriptFile.Set(aBuf.Mid(aPos));
	
	startPos = tempScriptFile.Find(aTag);
	if(startPos != KErrNotFound)
		{
		startPos += aPos;
		if(startPos == aPos)
			{
			return ETrue;
			}
		else
			{
			return EFalse;
			}
		}
	else
		{
		return EFalse;
		}
	
	}

