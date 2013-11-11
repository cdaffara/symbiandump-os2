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
* This contains CDataWrapper
*
*/



/**
 @prototype
 @test
*/

#include "datawrapper.h"

_LIT(KDictionaryName,			"name");

EXPORT_C CDataWrapper::CDataWrapper()
/**
 * Constructor
 */
:	iDataDictionary(NULL)
,	iTestBlockController(NULL)
,	iOutstanding(0)
	{
	}

EXPORT_C CDataWrapper::~CDataWrapper()
/**
 * Destructor
 */
	{
	}

EXPORT_C void CDataWrapper::SetTestBlockController(CTestBlockController* aTestBlockController)
/**
 * Set the test block controller
 *
 * @param	aTestBlockController - the controller
 *
 * @see		CTestBlockController
 */
	{
	iTestBlockController = aTestBlockController;
	}

EXPORT_C void CDataWrapper::SetDataDictionary(CDataDictionary* aDataDictionary)
/**
 * Set the data dictionary
 *
 * @param	aDataDictionary - the data dictionary
 *
 * @see		CDataDictionary
 */
	{
	iDataDictionary = aDataDictionary;
	}

// MDataAccess implementation
EXPORT_C TBool CDataWrapper::GetBoolFromConfig(const TDesC&  aSectName, const TDesC& aKeyName, TBool& aResult)
	{
	return iTestBlockController?iTestBlockController->GetBoolFromConfig(aSectName, aKeyName, aResult):EFalse;
	}

EXPORT_C TBool CDataWrapper::GetIntFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult)
	{
	return iTestBlockController?iTestBlockController->GetIntFromConfig(aSectName, aKeyName, aResult):EFalse;
	}

EXPORT_C TBool CDataWrapper::GetStringFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TPtrC& aResult)
	{
	return iTestBlockController?iTestBlockController->GetStringFromConfig(aSectName, aKeyName, aResult):EFalse;
	}

EXPORT_C TBool CDataWrapper::GetHexFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult)
	{
	return iTestBlockController?iTestBlockController->GetHexFromConfig(aSectName, aKeyName, aResult):EFalse;
	}

EXPORT_C TBool CDataWrapper::WriteBoolToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TBool& aValue)
	{
	return iTestBlockController?iTestBlockController->WriteBoolToConfig(aSectName, aKeyName, aValue):EFalse;
	}

EXPORT_C TBool CDataWrapper::WriteIntToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TInt& aValue)
	{
	return iTestBlockController?iTestBlockController->WriteIntToConfig(aSectName, aKeyName, aValue):EFalse;
	}

EXPORT_C TBool CDataWrapper::WriteStringToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TPtrC& aValue)
	{
	return iTestBlockController?iTestBlockController->WriteStringToConfig(aSectName, aKeyName, aValue):EFalse;
	}

EXPORT_C TBool CDataWrapper::WriteHexToConfig(const TDesC&  aSectName, const TDesC& aKeyName, const TInt& aValue)
	{
	return iTestBlockController?iTestBlockController->WriteHexToConfig(aSectName, aKeyName, aValue):EFalse;
	}

EXPORT_C void CDataWrapper::ReadSharedDataL(const TDesC& aShareName, TDes& aSharedDataVal)
	{
	if( iTestBlockController )
		{
		iTestBlockController->ReadSharedDataL(aShareName, aSharedDataVal);
		}
	}

EXPORT_C void CDataWrapper::WriteSharedDataL(const TDesC& aShareName, TDesC& aSharedDataVal, TModeCopy aModeCopy)
	{
	if( iTestBlockController )
		{
		iTestBlockController->WriteSharedDataL( aShareName, aSharedDataVal, aModeCopy );
		}
	}

EXPORT_C CTestExecuteLogger& CDataWrapper::Logger()
	{
	return iTestBlockController->Logger();
	}

EXPORT_C CDataWrapper* CDataWrapper::GetDataWrapperL(const TDesC& aSectionName)
	{
	CDataWrapper* dataWrapper = NULL;
	if( iDataDictionary )
		{
		// Lookup the dictionary name in the ini file
		TPtrC dictionaryName;
		TBool res = GetStringFromConfig(aSectionName, KDictionaryName, dictionaryName );
		if( KErrNone != res )
			{
			// Retrieve the object from the dictionary
			dataWrapper = iDataDictionary->GetDataL(dictionaryName);
			}
		else
			{
			User::Leave( KErrBadName );
			}
		}
	return dataWrapper;
	}

EXPORT_C TAny* CDataWrapper::GetDataObjectL(const TDesC& aSectionName)
	{
	TAny* dataObject = NULL;
	if( iDataDictionary )
		{
		// Lookup the dictionary name in the ini file
		TPtrC dictionaryName;
		TBool res = GetStringFromConfig(aSectionName, KDictionaryName, dictionaryName );
		if( KErrNone != res )
			{
			// Retrieve the object from the dictionary
			dataObject = iDataDictionary->GetObjectL(dictionaryName);
			}
		else
			{
			User::Leave( KErrBadName );
			}
		}
	return dataObject;
	}

EXPORT_C void CDataWrapper::SetDataObjectL(const TDesC& aSectionName, TAny* aObject)
	{
	if( iDataDictionary )
		{
		// Lookup the dictionary name in the ini file
		TPtrC dictionaryName;
		TBool res = GetStringFromConfig(aSectionName, KDictionaryName, dictionaryName );
		if( KErrNone != res )
			{
			// Set the object in the wrapper
			iDataDictionary->SetObjectL(dictionaryName, aObject);
			}
		else
			{
			User::Leave( KErrBadName );
			}
		}
	}

// MTEFResult implementation
EXPORT_C void CDataWrapper::SetError(const TInt aError)
	{
	if( iTestBlockController )
		{
		iTestBlockController->SetError( aError );
		}
	}

EXPORT_C void CDataWrapper::SetAsyncError(const TInt aIndex, const TInt aError)
	{
	if( iTestBlockController )
		{
		iTestBlockController->SetAsyncError( aIndex, aError );
		}
	}

EXPORT_C void CDataWrapper::SetBlockResult(const TVerdict aResult)
	{
	if( iTestBlockController )
		{
		iTestBlockController->SetBlockResult( aResult );
		}
	}

EXPORT_C TVerdict CDataWrapper::BlockResult()
	{
	return iTestBlockController->BlockResult();
	}

// MActiveCallback implementation
EXPORT_C void CDataWrapper::RunL(CActive* /*aActive*/, TInt /*aIndex*/)
	{
	// This should never be called, if it has been then warn the user!
	//  They need to implement this function when using CActiveCallback's
	//  and handle the request completion!
	SetBlockResult( EFail );
	}

EXPORT_C void CDataWrapper::DoCancel(CActive* /*aActive*/, TInt /*aIndex*/)
	{
	// This should never be called, if it has been then warn the user!
	//  They need to implement this function when using CActiveCallback's
	//  and handle the request cancellation!
	SetBlockResult( EFail );
	}
