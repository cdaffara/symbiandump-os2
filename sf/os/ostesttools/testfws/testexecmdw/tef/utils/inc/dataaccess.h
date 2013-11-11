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
* This contains DataAccess.h
*
*/



#ifndef __DATA_ACCESS_H__
#define __DATA_ACCESS_H__

#include <test/testexecutestepbase.h>

#include <test/datawrapper.h>

class MDataAccess
/**
 * @internalComponent
 * @test
 *
 * Data access interface
 */
	{
public:
	/**
	 * Read a boolean value from the configuration file.
	 *
	 * @param	aSectName - Section name in the config file
	 * @param	aKeyName - text name for the value in the config file
	 * @param	aResult - value found in config file. ETrue is represented by the case insensitive string "true"
	 *
	 * @return	ETrue if the section/key name was found
	 */
	virtual TBool			GetBoolFromConfig(const TDesC&  aSectName, const TDesC& aKeyName, TBool& aResult) =0;

	/**
	 * Read an integer value from the configuration file.
	 *
	 * @param	aSectName - Section name in the config file
	 * @param	aKeyName - text name for the value in the config file
	 * @param	aResult - value found in config file. ETrue is represented by the case insensitive string "true"
	 *
	 * @return	ETrue if the section/key name was found
	 */
	virtual TBool			GetIntFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult) =0;

	/**
	 * Read a string value from the configuration file.
	 *
	 * @param	aSectName - Section name in the config file
	 * @param	aKeyName - text name for the value in the config file
	 * @param	aResult - value found in config file. ETrue is represented by the case insensitive string "true"
	 *
	 * @return	ETrue if the section/key name was found
	 */
	virtual TBool			GetStringFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TPtrC& aResult) =0;

	/**
	 * Read a hexidecimal value from the configuration file.
	 *
	 * @param	aSectName - Section name in the config file
	 * @param	aKeyName - text name for the value in the config file
	 * @param	aResult - value found in config file. ETrue is represented by the case insensitive string "true"
	 *
	 * @return	ETrue if the section/key name was found
	 */
	virtual TBool			GetHexFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult) =0;

	/**
	 * Write an integer value to the configuration file.
	 *
	 * @param	aSectName - Section name in the config file
	 * @param	aKeyName - text name for the value in the config file
	 * @param	aValue - value to write into the config file.
	 *
	 * @return	ETrue if successful
	 */
	virtual TBool			WriteIntToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TInt& aValue) =0;

	/**
	 * Write a string value to the configuration file.
	 *
	 * @param	aSectName - Section name in the config file
	 * @param	aKeyName - text name for the value in the config file
	 * @param	aValue - value to write into the config file.
	 *
	 * @return	ETrue if successful
	 */
	virtual TBool			WriteStringToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TPtrC& aValue) =0;

	/**
	 * Write a boolean value to the configuration file.
	 *
	 * @param	aSectName - Section name in the config file
	 * @param	aKeyName - text name for the value in the config file
	 * @param	aValue - value to write into the config file.
	 *
	 * @return	ETrue if successful
	 */
	virtual TBool			WriteBoolToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TBool& aValue) =0;

	/**
	 * Write a hexidecimal value to the configuration file.
	 *
	 * @param	aSectName - Section name in the config file
	 * @param	aKeyName - text name for the value in the config file
	 * @param	aValue - value to write into the config file.
	 *
	 * @return	ETrue if successful
	 */
	virtual TBool			WriteHexToConfig(const TDesC&  aSectName, const TDesC& aKeyName, const TInt& aValue) =0;

	/**
	 * Updates the data to the shared data name passed either in Copy Mode/Append Mode
	 *
	 * @param	aShareName - global shared memory name
	 * @param	aSharedDataVal - data to write
	 * @param	aModeCopy - copy or append data
	 *
	 * @leave	system wide error
	 */
	virtual void			WriteSharedDataL(const TDesC& aShareName, TDesC& aSharedDataVal, TModeCopy aModeCopy) =0;

	/**
	 * Read data from the shared data name passed
	 *
	 * @param	aShareName - global shared memory name
	 * @param	aSharedDataVal - data read
	 *
	 * @leave	system wide error
	 */
	virtual void			ReadSharedDataL(const TDesC& aShareName, TDes& aSharedDataVal) =0;

	/**
	 * Read name of data dictionary entry from configuration file and return the data wrapper
	 *
	 * @param	aSectionName - section in configuration file to read name of dictionary entry
	 *
	 * @leave	KErrBadName - name not found in configuration file
	 * @leave	KErrNotFound - entry not found in dictinary
	 *
	 * @return	Data wrapper for the object
	 */
	virtual CDataWrapper*	GetDataWrapperL(const TDesC& aSectionName) =0;

	/**
	 * Read name of data dictionary entry from configuration file and return the data wrapper
	 *
	 * @param	aSectionName - section in configuration file to read name of dictionary entry
	 *
	 * @leave	KErrBadName - name not found in configuration file
	 * @leave	KErrNotFound - entry not found in dictinary
	 *
	 * @return	The object stored in the data wrapper
	 */
	virtual TAny*			GetDataObjectL(const TDesC& aSectionName) =0;

	/**
	 * Read name of data dictionary entry from configuration file set the data wrappers object
	 *
	 * @param	aSectionName - section in configuration file to read name of dictionary entry
	 * @param	aObject - object to set in the data wrapper
	 *
	 * @leave	KErrBadName - name not found in configuration file
	 * @leave	KErrNotFound - entry not found in dictinary
	 */
	virtual void			SetDataObjectL(const TDesC& aSectionName, TAny* aObject) =0;

	/**
	 * Get the logger
	 *
	 * @see		CTestExecuteLogger
	 *
	 * @return	the logger
	 */
	virtual CTestExecuteLogger&	Logger() =0;
	};

#endif // __DATA_ACCESS_H__
