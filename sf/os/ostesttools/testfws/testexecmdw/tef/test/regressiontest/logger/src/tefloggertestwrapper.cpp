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
* @file
* This is the test class for TEF (PREQ10437).
*
*/



#include "tefloggertestwrapper.h"

/// Enumerations
_LIT(KFail, "EFail");
_LIT(KPass, "EPass");

/// Commands
_LIT(KPrint, "Print");
_LIT(KPrintNonVALIST, "PrintNonVA_LIST");
_LIT(KUnCoveredFuntions, "UnCoveredFuntions");
_LIT(KGetCPPModuleName, "GetCPPModuleName");
_LIT(KNew, "New");
_LIT(KGetDataObject, "GetDataObject");
_LIT(KGetLongSectionName, "GetLongSectionName");
_LIT(KGetLongKeyName, "GetLongKeyName");

_LIT(KNullExpected, "null_expected");
_LIT(KObjectValue, "object_value");
_LIT(KESevrLevel, "ESevrLevel");

CTEFLoggerTestWrapper::CTEFLoggerTestWrapper() :
	iActiveCallback(NULL), iObject(NULL), iActiveObject(NULL)
	{
	}

CTEFLoggerTestWrapper::~CTEFLoggerTestWrapper()
	{
	delete iActiveCallback;
	iActiveCallback = NULL;

	delete iActiveObject;
	iActiveObject = NULL;

	delete iObject;
	iObject = NULL;
	}

CTEFLoggerTestWrapper* CTEFLoggerTestWrapper::NewL()
	{
	CTEFLoggerTestWrapper* ret = new (ELeave) CTEFLoggerTestWrapper();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

void CTEFLoggerTestWrapper::ConstructL()
	{
	iActiveCallback = CActiveCallback::NewL(*this);
	iObject = new (ELeave) TInt;
	}

TBool CTEFLoggerTestWrapper::DoCommandL(const TTEFFunction& aCommand,
		const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool ret = ETrue;

	// Print out the parameters for debugging
	INFO_PRINTF2( _L("aCommand = %S"), &aCommand );
	INFO_PRINTF2( _L("aSection = %S"), &aSection );
	INFO_PRINTF2( _L("aAsyncErrorIndex = %D"), aAsyncErrorIndex );

	if (KNew() == aCommand)
		{
		DoCmdNewL(aSection);
		}
	else
	if (KGetDataObject() == aCommand)
		{
		DoCmdGetDataObject(aSection);
		}
	else
	if (KPrint() == aCommand)
		{
		DoCmdPrint(aSection);
		}
	else
	if (KPrintNonVALIST() == aCommand)
		{
		DoCmdPrintNoVALIST(aSection);
		}
	else
	if (KUnCoveredFuntions() == aCommand)
		{
		DoCmdKUnCoveredFuntions(aSection);
		}	
	else
	if (KGetCPPModuleName() == aCommand)
		{
		DoCmdKGetCPPModuleName();
		}
	else
	if (KGetLongSectionName()== aCommand)
		{
		GetLongSectionName(aSection);
		}
	else
	if (KGetLongKeyName()== aCommand)
		{
		GetLongKeyName(aSection);
		}
	else
		{
		ret = EFalse;
		}
	return ret;
	}

_LIT(KLoggerServityInfo,"Logger severity test.");
void CTEFLoggerTestWrapper::DoCmdPrint(const TDesC& aSection)
	{
	TInt severity = 0;
	if (!GetIntFromConfig(aSection, KESevrLevel(), severity))
		{
		ERR_PRINTF2(_L("No parameter %S"), &KESevrLevel());
		SetBlockResult(EFail);
		}
	else
		{
		Logger().LogExtra(((TText8*)__FILE__), __LINE__, severity, (KLoggerServityInfo));
		}	
	}
_LIT(KLoggerServityInfoFmt,"Logger severity test(with %2d level @%3d line ).");
void CTEFLoggerTestWrapper::DoCmdPrintNoVALIST(const TDesC& aSection)
	{
	TInt severity = 0;
	if (!GetIntFromConfig(aSection, KESevrLevel(), severity))
		{
		ERR_PRINTF2(_L("No parameter %S"), &KESevrLevel());
		SetBlockResult(EFail);
		}
	else
		{
		Logger().LogExtra(((TText8*)__FILE__), __LINE__, severity, (KLoggerServityInfoFmt),severity,__LINE__);
		}
	}

/**
 Purpose: To test that an wrapped object exists in the dictionary. And optionally to check the object's value.

 @internalComponent
 @param  aSection Current ini file command section
 */
void CTEFLoggerTestWrapper::DoCmdGetDataObject(const TDesC& aSection)
	{
	// check if null is expected
	TBool nullExpected = EFalse;
	GetBoolFromConfig(aSection, KNullExpected(), nullExpected);

	// call GetDataObject() and check that the wrapped object is found
	TInt* wrappedObject= NULL;
	TRAPD(err, wrappedObject = static_cast<TInt*>(GetDataObjectL(aSection)));

	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("GetDataObjectL() error: %d"), err);
		SetBlockResult(EFail);
		SetError(err);
		}

	// check if is null if is expetced to be null and vice versa
	if (wrappedObject == NULL && !nullExpected)
		{
		ERR_PRINTF1(_L("Wrapped object is not expected to be null!"));
		SetBlockResult(EFail);
		}
	else
		if (wrappedObject != NULL)
			{
			if (nullExpected)
				{
				ERR_PRINTF1(_L("Wrapped object is expected to be null!"));
				SetBlockResult(EFail);
				}
			else
				{
				// check expected value
				TInt objectValue = 0;
				if (GetIntFromConfig(aSection, KObjectValue(), objectValue))
					{
					if (*wrappedObject != objectValue)
						{
						ERR_PRINTF3(_L("Wrapped object's value is not as expected. expected=%d, actual=%d"), objectValue, *wrappedObject);
						SetBlockResult(EFail);
						}					
					}
				}
			}
	}
void CTEFLoggerTestWrapper::DoCmdKUnCoveredFuntions(const TDesC& aSection)
	{
	Logger().Write(aSection); //Write(const TDesC& aLogBuffer)
	TBuf8<256> copy8;
	copy8.Copy(aSection);
	copy8.Append(_L8("8"));
	Logger().Write(copy8);//Write(const TDesC8& aLogBuffer)
	_LIT(KFomarteString,"%S");
	Logger().WriteFormat(KFomarteString,&aSection);//WriteFormat(TRefByValue<const TDesC> aFmt,...)
	_LIT8(KFomarteString8,"%S");
	Logger().WriteFormat(KFomarteString8,&copy8);//WriteFormat(TRefByValue<const TDesC8> aFmt,...)
	_LIT(KTEFUnCovFunXML,"UnCoveredFuntions.xml");
#define UnCovFunXML "UnCoveredFuntions.xml"
	Logger().LogToXml( ((TText8*)__FILE__) , __LINE__, (TInt)ESevrInfo, KTEFUnCovFunXML);
	}

void CTEFLoggerTestWrapper::DoCmdKGetCPPModuleName()
	{
#define KA_Cpp "A.cpp"
	Logger().LogExtra( (TText8*)KA_Cpp , __LINE__, ESevrInfo, (KGetCPPModuleName));
#define KABSLT_Cpp "\\A.cpp"
	Logger().LogExtra( (TText8*)KABSLT_Cpp , __LINE__, ESevrInfo, (KGetCPPModuleName));
#define KB_Cpp "dir\\A.cpp"
	Logger().LogExtra( (TText8*)KB_Cpp , __LINE__, ESevrInfo, (KGetCPPModuleName));
#define KDIR_DIR "DIR\\DIR\\"
	Logger().LogExtra( (TText8*)KDIR_DIR , __LINE__, ESevrInfo, (KGetCPPModuleName));
#define KDIR_DIR1 "DIR\\DIR1"
	Logger().LogExtra( (TText8*)KDIR_DIR1 , __LINE__, ESevrInfo, (KGetCPPModuleName));
#define KDIR_CDRIVER "c:\\"
	Logger().LogExtra( (TText8*)KDIR_CDRIVER , __LINE__, ESevrInfo, (KGetCPPModuleName));
#define KDIR_EMPTY ""
	Logger().LogExtra( (TText8*)KDIR_EMPTY , __LINE__, ESevrInfo, (KGetCPPModuleName));
#define KDIR_FULLPATH "c:\\fulltpath\\to\\file.cpp"
	Logger().LogExtra( (TText8*)KDIR_FULLPATH , __LINE__, ESevrInfo, (KGetCPPModuleName));
	}


_LIT(KTEFTESTIniKey,"Key");
void CTEFLoggerTestWrapper::GetLongSectionName(const TDesC& aSection)
	{
	TPtrC value;
	if (GetStringFromConfig(aSection, KTEFTESTIniKey(), value ))
		{
		Logger().Write(value);
		_LIT(KFomarteString,"%S");
		Logger().WriteFormat(KFomarteString,&value);
		}
	else
		{
		SetBlockResult(EFail);
		}
	}
void CTEFLoggerTestWrapper::GetLongKeyName(const TDesC& aSection)
	{
	TPtrC value;
	if (GetStringFromConfig(aSection, KTEFTESTIniKey(), value ))
		{
		Logger().Write(value);
		_LIT(KFomarteString,"%S");
		Logger().WriteFormat(KFomarteString,&value);
		}
	else
		{
		SetBlockResult(EFail);
		}
	}
/**
 Purpose: To create a new object of the CTEFTest type through the API.

 Ini file options:
 iniData - The data from the ini file at the section provided.

 @internalComponent
 @param  aSection Current ini file command section
 */
void CTEFLoggerTestWrapper::DoCmdNewL(const TDesC& aSection)
	{
	TInt objectValue = 0;
	SetBlockResult(EFail);
	if (!GetIntFromConfig(aSection, KObjectValue(), objectValue))
		{
		ERR_PRINTF2(_L("No parameter %S"), &KObjectValue());
		}
	else
		{
		if (iObject != NULL)
			{
			delete iObject;
			iObject = NULL;
			}
		iObject = new (ELeave) TInt(objectValue);
		SetBlockResult(EPass);
		}
	}
void CTEFLoggerTestWrapper::RunL(CActive* aActive, TInt aIndex)
	{
	INFO_PRINTF1(_L("CTEFLoggerTestWrapper::RunL"));
	TInt err = aActive->iStatus.Int();
	SetAsyncError(aIndex, err);
	DecOutstanding();
	}

/** Reads drive mapping operation name from INI-file */
TBool CTEFLoggerTestWrapper::GetVerdictFromConfig(const TDesC& aSection,
		const TDesC& aParameterName, TVerdict& aVerdict)
	{
	// Read drives mapping operation name from INI file
	TPtrC verdictStr;
	TBool ret = GetStringFromConfig(aSection, aParameterName, verdictStr);
	if (ret)
		{
		if (verdictStr == KPass())
			{
			aVerdict = EPass;
			}
		else
			if (verdictStr == KFail())
				{
				aVerdict = EFail;
				}
			else
				{
				TInt verdict = 0;
				ret = GetIntFromConfig(aSection, aParameterName, verdict);
				if (ret)
					{
					aVerdict = (TVerdict) verdict;
					}
				}
		}

	return ret;
	}

