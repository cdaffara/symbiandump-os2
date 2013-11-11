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



#include "T_EComResolverParamsData.h"


/*@{*/
///	Parameters
_LIT(KExpectedString,										"expected_string");
_LIT(KExpectedBool,											"expected_bool");
_LIT(KDataType,												"data_type");
_LIT(KGenericMatch,											"generic_match");
_LIT(KWildcardMatch,										"wildcard_match");

///	TEcomResolverParams
_LIT(KCmdDataType,											"DataType");
_LIT(KCmdSetDataType,										"SetDataType");
_LIT(KCmdSetGenericMatch,									"SetGenericMatch");
_LIT(KCmdIsGenericMatch,									"IsGenericMatch");
_LIT(KCmdSetWildcardMatch,									"SetWildcardMatch");
_LIT(KCmdIsWildcardMatch,									"IsWildcardMatch");

_LIT( KEmptyString,											"");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * Two phase constructor
 */
CT_EComResolverParamsData* CT_EComResolverParamsData::NewL()
	{
	CT_EComResolverParamsData*	ret=new (ELeave) CT_EComResolverParamsData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

/**
 * Constructor. First phase construction
 */
CT_EComResolverParamsData::CT_EComResolverParamsData()
	{
	}

/**
 * Second phase construction
 */
void CT_EComResolverParamsData::ConstructL()
	{
	}

/**
 * Destructor
 */
CT_EComResolverParamsData::~CT_EComResolverParamsData()
	{
	DestroyData();
	}

void CT_EComResolverParamsData::DestroyData()
	{
	}


TAny* CT_EComResolverParamsData::GetObject() 
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	Pointer to the object that the data wraps
 */
	{
	return &iResolverParams;
	};

//////////////////////////////////////////////////////////////////////
// Read data from INI file
//////////////////////////////////////////////////////////////////////

TBool CT_EComResolverParamsData::GetExpectedString(const TDesC& aSection, TPtrC& aExpectedString)
	{
	aExpectedString.Set(KEmptyString);
	return GetStringFromConfig(aSection, KExpectedString(), aExpectedString);
	}

TBool CT_EComResolverParamsData::GetDataType(const TDesC& aSection, TBuf8<KMaxTestExecuteCommandLength>& aDataType)
	{
	TPtrC	dataType(KEmptyString);
	TBool	ret=GetStringFromConfig(aSection, KDataType(), dataType);
	if ( ret )
		{
		aDataType.Copy(dataType);
		}
	return ret;
	}

TBool CT_EComResolverParamsData::GetExpectedBool(const TDesC& aSection, TBool& aBool)
	{
	aBool = EFalse;
	return GetBoolFromConfig(aSection, KExpectedBool(), aBool );
	}

TBool CT_EComResolverParamsData::GetGenericMatch(const TDesC& aSection, TBool& aGenericMatch)
	{
	aGenericMatch = EFalse;
	return GetBoolFromConfig(aSection, KGenericMatch(), aGenericMatch );
	}

TBool CT_EComResolverParamsData::GetWildcardMatch(const TDesC& aSection, TBool& aWildcardMatch)
	{
	aWildcardMatch = EFalse;
	return GetBoolFromConfig(aSection, KWildcardMatch(), aWildcardMatch );
	}

TBool CT_EComResolverParamsData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
	{
	TBool retVal=ETrue;
	
	if ( aCommand==KCmdDataType )
		{
		DoCmdDataType(aSection);
		}
	else if ( aCommand==KCmdSetDataType )
		{
		DoCmdSetDataType(aSection);
		}
	else if ( aCommand==KCmdSetGenericMatch )
		{
		DoCmdSetGenericMatch(aSection);
		}
	else if ( aCommand==KCmdIsGenericMatch )
		{
		DoCmdIsGenericMatch(aSection);
		}
	else if ( aCommand==KCmdSetWildcardMatch )
		{
		DoCmdSetWildcardMatch(aSection);
		}
	else if ( aCommand==KCmdIsWildcardMatch )
		{
		DoCmdIsWildcardMatch(aSection);
		}
	else
		{
		retVal=EFalse;
		}

	return retVal;
	}

void CT_EComResolverParamsData::DoCmdDataType(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	actualValue;
	actualValue.Copy(iResolverParams.DataType());
	INFO_PRINTF2(_L("DataType %S"), &actualValue);

	TPtrC	expectedValue;
	if (GetExpectedString(aSection, expectedValue))
		{
		if (actualValue!=expectedValue)
			{
			ERR_PRINTF3(_L("Actual value \"%S\" does not match expected value \"%S\""), &actualValue, &expectedValue);
			SetBlockResult(EFail);
			}
		}
	}

void CT_EComResolverParamsData::DoCmdSetDataType(const TDesC& aSection)
	{
	if (!GetDataType(aSection, iDataType))
		{
		ERR_PRINTF1(_L("Not enought arguments"));
		SetBlockResult(EFail);
		}
	else
		{
		iResolverParams.SetDataType(iDataType);
		}
	}

void CT_EComResolverParamsData::DoCmdSetGenericMatch(const TDesC& aSection)
	{
	TBool	setGenericMatch;
	if (!GetGenericMatch(aSection, setGenericMatch))
		{
		ERR_PRINTF1(_L("Not enought arguments"));
		SetBlockResult(EFail);
		}
	else
		{
		iResolverParams.SetGenericMatch(setGenericMatch);
		}
	}

void CT_EComResolverParamsData::DoCmdIsGenericMatch(const TDesC& aSection)
	{
	TBool	actualValue=iResolverParams.IsGenericMatch();
	INFO_PRINTF2(_L("IsGenericMatch %d"), actualValue);

	TBool	expectedValue;
	if (GetExpectedBool(aSection, expectedValue))
		{
		if (actualValue!=expectedValue)
			{
			ERR_PRINTF3(_L("Actual value %d does not match expected value %d"), actualValue, expectedValue);
			SetBlockResult(EFail);
			}
		}
	}

void CT_EComResolverParamsData::DoCmdSetWildcardMatch(const TDesC& aSection)
	{
	TBool	setWildcardMatch;
	if (!GetWildcardMatch(aSection, setWildcardMatch))
		{
		ERR_PRINTF1(_L("Not enought arguments"));
		SetBlockResult(EFail);
		}
	else
		{
		iResolverParams.SetWildcardMatch(setWildcardMatch);
		}
	}

void CT_EComResolverParamsData::DoCmdIsWildcardMatch(const TDesC& aSection)
	{
	TBool	actualValue=iResolverParams.IsWildcardMatch();
	INFO_PRINTF2(_L("IsWildcardMatch %d"), actualValue);

	TBool	expectedValue;
	if (GetExpectedBool(aSection, expectedValue))
		{
		if (actualValue!=expectedValue)
			{
			ERR_PRINTF3(_L("Actual value %d does not match expected value %d"), actualValue, expectedValue);
			SetBlockResult(EFail);
			}
		}
	}

