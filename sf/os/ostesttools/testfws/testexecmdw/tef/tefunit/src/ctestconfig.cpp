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



/**
 @file CTestConfig.cpp
*/

#include "tefunit.h"

CTestConfig::CTestConfig()
/**
 * Constructor
 */
	{
	}

CTestConfig::CTestConfig( CTestStep* aTestStep )
/**
 * Constructor
 *
 * @param aTestStep - Test step to use for the configuration access
 */
	: iTestStep(aTestStep)
	{
	}

CTestConfig::CTestConfig(const CTestConfig& aTestConfig )
/**
 * Constructor
 *
 * @param aTestConfig - Test config to copy
 */
	{
	iTestStep = aTestConfig.iTestStep;
	}

CTestConfig::~CTestConfig()
/**
 * Destructor
 */
	{
	}

TBool CTestConfig::GetBool(const TDesC& aSectionName, const TDesC& aKey, TBool& aResult) const
/**
 * GetBool
 *
 * @param aSectionName - Section name within the ini file
 * @param aKey - Key within the ini file
 * @param aResult - Retrieved boolean
 * @return - Call result
 */
	{
	return iTestStep->GetBoolFromConfig( aSectionName, aKey, aResult );
	}

TBool CTestConfig::GetInt(const TDesC& aSectionName, const TDesC& aKey, TInt& aResult) const
/**
 * GetInt
 *
 * @param aSectionName - Section name within the ini file
 * @param aKey - Key within the ini file
 * @param aResult - Retrieved int
 * @return - Call result
 */
	{
	return iTestStep->GetIntFromConfig( aSectionName, aKey, aResult );
	}
TBool CTestConfig::GetInt(const TDesC& aSectionName, const TDesC& aKey, TInt64& aResult) const
/**
 * GetInt
 *
 * @param aSectionName - Section name within the ini file
 * @param aKey - Key within the ini file
 * @param aResult - Retrieved int64
 * @return - Call result
 */
	{
	return iTestStep->GetIntFromConfig( aSectionName, aKey, aResult );
	}

TBool CTestConfig::GetInt(const TDesC& aKey, TInt64& aResult) const
/**
 * GetInt
 *
 * @param aKey - Key within the ini file
 * @param aResult - Retrieved int64
 * @return - Call result
 */
	{
	TBool ret = EFalse;
	if( iTestStep->ConfigSection().Length() > 0 )
		{
		ret = iTestStep->GetIntFromConfig( iTestStep->ConfigSection(), aKey, aResult );
		}
	return ret;
	}

TBool CTestConfig::WriteInt(const TDesC& aSectionName, const TDesC& aKey, TInt64& aValue) const
/**
 * WriteInt
 *
 * @param aSectionName - Section name within the ini file
 * @param aKey - Key within the ini file
 * @param aValue - Written int64
 * @return - Call result
 */
	{
	return iTestStep->WriteIntToConfig( aSectionName, aKey, aValue );
	}

TBool CTestConfig::WriteInt(const TDesC& aKey, TInt64& aValue) const
/**
 * WriteInt
 *
 * @param aKey - Key within the ini file
 * @param aValue - Written int
 * @return - Call result
 */
	{
	TBool ret = EFalse;
	if( iTestStep->ConfigSection().Length() > 0 )
		{
		ret = iTestStep->WriteIntToConfig( iTestStep->ConfigSection(), aKey, aValue );
		}
	return ret;
	}
TBool CTestConfig::GetHex(const TDesC& aSectionName, const TDesC& aKey, TInt& aResult) const
/**
 * GetHex
 *
 * @param aSectionName - Section name within the ini file
 * @param aKey - Key within the ini file
 * @param aResult - Retrieved hex
 * @return - Call result
 */
	{
	return iTestStep->GetHexFromConfig( aSectionName, aKey, aResult );
	}

TBool CTestConfig::GetString(const TDesC& aSectionName, const TDesC& aKey, TPtrC& aResult) const
/**
 * GetString
 *
 * @param aSectionName - Section name within the ini file
 * @param aKey - Key within the ini file
 * @param aResult - Retrieved string
 * @return - Call result
 */
	{
	return iTestStep->GetStringFromConfig( aSectionName, aKey, aResult );
	}

TBool CTestConfig::GetBool(const TDesC& aKey, TBool& aResult) const
/**
 * GetBool
 *
 * @param aKey - Key within the ini file
 * @param aResult - Retrieved boolean from section passed in via the script
 * @return - Call result
 */
	{
	TBool ret = EFalse;
	if( iTestStep->ConfigSection().Length() > 0 )
		{
		ret = iTestStep->GetBoolFromConfig( iTestStep->ConfigSection(), aKey, aResult );
		}
	return ret;
	}

TBool CTestConfig::GetInt(const TDesC& aKey, TInt& aResult) const
/**
 * GetInt
 *
 * @param aKey - Key within the ini file
 * @param aResult - Retrieved int
 * @return - Call result
 */
	{
	TBool ret = EFalse;
	if( iTestStep->ConfigSection().Length() > 0 )
		{
		ret = iTestStep->GetIntFromConfig( iTestStep->ConfigSection(), aKey, aResult );
		}
	return ret;
	}

TBool CTestConfig::GetHex(const TDesC& aKey, TInt& aResult) const
/**
 * GetHex
 *
 * @param aKey - Key within the ini file
 * @param aResult - Retrieved hex
 * @return - Call result
 */
	{
	TBool ret = EFalse;
	if( iTestStep->ConfigSection().Length() > 0 )
		{
		ret = iTestStep->GetHexFromConfig( iTestStep->ConfigSection(), aKey, aResult );
		}
	return ret;
	}

TBool CTestConfig::GetString(const TDesC& aKey, TPtrC& aResult) const
/**
 * GetString
 *
 * @param aKey - Key within the ini file
 * @param aResult - Retrieved string
 * @return - Call result
 */
	{
	TBool ret = EFalse;
	if( iTestStep->ConfigSection().Length() > 0 )
		{
		ret = iTestStep->GetStringFromConfig( iTestStep->ConfigSection(), aKey, aResult );
		}
	return ret;
	}

// Newly added functions for writing to ini file
TBool CTestConfig::WriteBool(const TDesC& aSectionName, const TDesC& aKey, TBool& aValue) const
/**
 * WriteBool
 *
 * @param aSectionName - Section name within the ini file
 * @param aKey - Key within the ini file
 * @param aValue - Written boolean
 * @return - Call result
 */
	{
	return iTestStep->WriteBoolToConfig( aSectionName, aKey, aValue );
	}

TBool CTestConfig::WriteInt(const TDesC& aSectionName, const TDesC& aKey, TInt& aValue) const
/**
 * WriteInt
 *
 * @param aSectionName - Section name within the ini file
 * @param aKey - Key within the ini file
 * @param aValue - Written int
 * @return - Call result
 */
	{
	return iTestStep->WriteIntToConfig( aSectionName, aKey, aValue );
	}

TBool CTestConfig::WriteHex(const TDesC& aSectionName, const TDesC& aKey, TInt& aValue) const
/**
 * WriteHex
 *
 * @param aSectionName - Section name within the ini file
 * @param aKey - Key within the ini file
 * @param aValue - Written hex
 * @return - Call result
 */
	{
	return iTestStep->WriteHexToConfig( aSectionName, aKey, aValue );
	}

TBool CTestConfig::WriteString(const TDesC& aSectionName, const TDesC& aKey, TPtrC& aValue) const
/**
 * WriteString
 *
 * @param aSectionName - Section name within the ini file
 * @param aKey - Key within the ini file
 * @param aValue - Written string
 * @return - Call result
 */
	{
	return iTestStep->WriteStringToConfig( aSectionName, aKey, aValue );
	}

TBool CTestConfig::WriteBool(const TDesC& aKey, TBool& aValue) const
/**
 * WriteBool
 *
 * @param aKey - Key within the ini file
 * @param aValue - Written boolean from section passed in via the script
 * @return - Call result
 */
	{
	TBool ret = EFalse;
	if( iTestStep->ConfigSection().Length() > 0 )
		{
		ret = iTestStep->WriteBoolToConfig( iTestStep->ConfigSection(), aKey, aValue );
		}
	return ret;
	}

TBool CTestConfig::WriteInt(const TDesC& aKey, TInt& aValue) const
/**
 * WriteInt
 *
 * @param aKey - Key within the ini file
 * @param aValue - Written int
 * @return - Call result
 */
	{
	TBool ret = EFalse;
	if( iTestStep->ConfigSection().Length() > 0 )
		{
		ret = iTestStep->WriteIntToConfig( iTestStep->ConfigSection(), aKey, aValue );
		}
	return ret;
	}

TBool CTestConfig::WriteHex(const TDesC& aKey, TInt& aValue) const
/**
 * WriteHex
 *
 * @param aKey - Key within the ini file
 * @param aValue - Written hex
 * @return - Call result
 */
	{
	TBool ret = EFalse;
	if( iTestStep->ConfigSection().Length() > 0 )
		{
		ret = iTestStep->WriteHexToConfig( iTestStep->ConfigSection(), aKey, aValue );
		}
	return ret;
	}

TBool CTestConfig::WriteString(const TDesC& aKey, TPtrC& aValue) const
/**
 * WriteString
 *
 * @param aKey - Key within the ini file
 * @param aValue - Written string
 * @return - Call result
 */
	{
	TBool ret = EFalse;
	if( iTestStep->ConfigSection().Length() > 0 )
		{
		ret = iTestStep->WriteStringToConfig( iTestStep->ConfigSection(), aKey, aValue );
		}
	return ret;
	}
