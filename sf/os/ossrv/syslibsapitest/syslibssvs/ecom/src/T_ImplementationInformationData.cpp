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



#include "T_ImplementationInformationData.h"

/*@{*/
///	Parameters
_LIT( KUid,													"uid" );
_LIT( KExpectedString,										"expected_string" );
_LIT( KExpectedResult,										"expected" );
_LIT( KExpectedBool,										"expected_bool" );
_LIT( KDisabled,											"disabled" );
_LIT( KElement,												"element" );

///	CImplementationInformation
_LIT( KCmdDisplayName,										"DisplayName" );
_LIT( KCmdDataType,											"DataType" );
_LIT( KCmdOpaqueData,										"OpaqueData" );
_LIT( KCmdImplementationUid,								"ImplementationUid" );
_LIT( KCmdVersion,											"Version" );
_LIT( KCmdDisabled,											"Disabled" );
_LIT( KCmdSetDisabled,										"SetDisabled" );
_LIT( KCmdDrive,											"Drive" );
_LIT( KCmdRomOnly,											"RomOnly" );
_LIT( KCmdRomBased,											"RomBased" );
_LIT( KCmdDestroy,											"~" );

_LIT( KEmptyString,											"");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_ImplementationInformationData* CT_ImplementationInformationData::NewL( )
	{
	CT_ImplementationInformationData*	ret = new ( ELeave ) CT_ImplementationInformationData( );
	CleanupStack::PushL( ret );
	ret->ConstructL();
	CleanupStack::Pop( ret );
	return ret;
	}

CT_ImplementationInformationData::CT_ImplementationInformationData( )
:	iImplementationInformation(NULL)
	{
	}

void CT_ImplementationInformationData::ConstructL()
	{
	}

CT_ImplementationInformationData::~CT_ImplementationInformationData()
	{
	DestroyData();
	}

void CT_ImplementationInformationData::DestroyData()
	{
	//	this does not own iImplementationInformation
	iImplementationInformation=NULL;
	}

//////////////////////////////////////////////////////////////////////
// Read data from INI file
//////////////////////////////////////////////////////////////////////

TBool CT_ImplementationInformationData::GetUid( const TDesC& aSection, TUid& aUid )
	{
 	TInt	uidValue=0;
 	TBool	ret=GetHexFromConfig( aSection, KUid(), uidValue ) ;
 	aUid = TUid::Uid( uidValue );
 	return ret;
 	}

TBool CT_ImplementationInformationData::GetExpectedString( const TDesC& aSection, TPtrC& aExpectedString)
	{
	aExpectedString.Set(KEmptyString);
	return GetStringFromConfig( aSection, KExpectedString(), aExpectedString);
	}

TBool CT_ImplementationInformationData::GetExpectedResult( const TDesC& aSection, TInt& aExpectedResult )
	{
	aExpectedResult = 0;
	return GetIntFromConfig( aSection, KExpectedResult(), aExpectedResult );
	}

TBool CT_ImplementationInformationData::GetExpectedHexResult( const TDesC& aSection, TInt& aExpectedResult)
	{
	aExpectedResult = 0;
	return GetHexFromConfig( aSection, KExpectedResult(), aExpectedResult );
	}

TBool CT_ImplementationInformationData::GetArrayElement(const TDesC& aSection, TInt& aArrayElement)
	{
	aArrayElement = 0;
	return GetIntFromConfig( aSection, KElement(), aArrayElement);
	}

TBool CT_ImplementationInformationData::GetExpectedBool( const TDesC& aSection, TBool& aBool )
	{
	aBool = EFalse;
	return GetBoolFromConfig( aSection, KExpectedBool(), aBool );
	}

TBool CT_ImplementationInformationData::GetDisabled( const TDesC& aSection, TBool& aDisabled )
	{
	aDisabled = EFalse;
	return GetBoolFromConfig( aSection, KDisabled(), aDisabled );
	}

/**
 Test script command entry point
 @internalComponent

 @return Explanation of the object returned
 @param aCommand descriptor containing the command value
 @param aSection descriptor containing the command parameter
 @pre N/A 
 @post N/A
*/
TBool CT_ImplementationInformationData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool								retVal = ETrue;
	
	if ( aCommand == KCmdDestroy )
		{
		DoCmdDestroy();
		}
	else if ( aCommand == KCmdDisplayName )
		{
		DoCmdDisplayName( aSection );
		}
	else if ( aCommand == KCmdDataType )
		{
		DoCmdDataType( aSection );
		}
	else if ( aCommand == KCmdOpaqueData )
		{
		DoCmdOpaqueData( aSection );
		}
	else if ( aCommand == KCmdImplementationUid )
		{
		DoCmdImplementationUid( aSection );
		}
	else if ( aCommand == KCmdVersion )
		{
		DoCmdVersion( aSection );
		}
	else if ( aCommand == KCmdDisabled )
		{
		DoCmdDisabled( aSection );
		}
	else if ( aCommand == KCmdSetDisabled )
		{
		DoCmdSetDisabled( aSection );
		}
	else if ( aCommand == KCmdDrive )
		{
		DoCmdDrive( aSection );
		}
	else if ( aCommand == KCmdRomOnly )
		{
		DoCmdRomOnly( aSection );
		}
	else if ( aCommand == KCmdRomBased )
		{
		DoCmdRomBased( aSection );
		}
	else
		{
		retVal = EFalse;
		}

	return retVal;
	}

void CT_ImplementationInformationData::DoCmdDisplayName(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	actualValue;
	actualValue.Copy(iImplementationInformation->DisplayName());
	TInt	pos=actualValue.Find(_L("|"));
	if (pos>0)
		{
		actualValue.Copy(actualValue.Left(pos));
		};
	INFO_PRINTF2(_L("DisplayName %S"), &actualValue);

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

void CT_ImplementationInformationData::DoCmdDataType(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	actualValue;
	actualValue.Copy(iImplementationInformation->DataType());
	TInt	pos=actualValue.Find(_L("|"));
	if (pos>0)
		{
		actualValue.Copy(actualValue.Left(pos));
		};
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

void CT_ImplementationInformationData::DoCmdOpaqueData(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	actualValue;
	actualValue.Copy(iImplementationInformation->OpaqueData());
	TInt	pos=actualValue.Find(_L("|"));
	if (pos>0)
		{
		actualValue.Copy(actualValue.Left(pos));
		};
	INFO_PRINTF2(_L("OpaqueData %S"), &actualValue);
	
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

void CT_ImplementationInformationData::DoCmdImplementationUid(const TDesC& aSection)
	{
	TInt	actualValue=iImplementationInformation->ImplementationUid().iUid;
	INFO_PRINTF2(_L("ImplementationUid %d"), actualValue);

	TInt	expectedValue;
	if (GetExpectedHexResult(aSection, expectedValue))
		{
		if (actualValue!=expectedValue)
			{
			ERR_PRINTF3(_L("Actual value %d does not match expected value %d"), actualValue, expectedValue);
			SetBlockResult(EFail);
			}
		}
	}

void CT_ImplementationInformationData::DoCmdVersion(const TDesC& aSection)
	{
	TInt	actualValue=iImplementationInformation->Version();
	INFO_PRINTF2(_L("Version %d"), actualValue);

	TInt	expectedValue;
	if (GetExpectedResult(aSection, expectedValue))
		{
		if (actualValue!=expectedValue)
			{
			ERR_PRINTF3(_L("Actual value %d does not match expected value %d"), actualValue, expectedValue);
			SetBlockResult(EFail);
			}
		}
	}

void CT_ImplementationInformationData::DoCmdDisabled(const TDesC& aSection)
	{
	TBool	actualValue=iImplementationInformation->Disabled();
	INFO_PRINTF2(_L("Disabled %d"), actualValue);

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

void CT_ImplementationInformationData::DoCmdSetDisabled(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("SetDisabled"));

	TBool	disabled;
	if (!GetDisabled(aSection, disabled))
		{
		ERR_PRINTF1(_L("Not enought arguments"));
		SetBlockResult( EFail );
		}
	else
		{
		iImplementationInformation->SetDisabled(disabled);
		}
	}

void CT_ImplementationInformationData::DoCmdDrive(const TDesC& aSection)
	{
	TDriveName	actualValue(iImplementationInformation->Drive().Name());
	INFO_PRINTF2(_L("Drive Name %s"), &actualValue);

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

void CT_ImplementationInformationData::DoCmdRomOnly(const TDesC& aSection)
	{
	TBool	actualValue=iImplementationInformation->RomOnly();
	INFO_PRINTF2(_L("RomOnly %d"), actualValue);

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

void CT_ImplementationInformationData::DoCmdRomBased(const TDesC& aSection)
	{
	TBool	actualValue=iImplementationInformation->RomBased();
	INFO_PRINTF2(_L("RomBased %d"), actualValue);

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

void CT_ImplementationInformationData::DoCmdDestroy()
	{
	INFO_PRINTF1( _L( "Destroyed" ) );
	DestroyData();
	}
