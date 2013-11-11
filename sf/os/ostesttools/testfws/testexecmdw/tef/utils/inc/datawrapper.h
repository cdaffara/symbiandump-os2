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
* This contains DataWrapper.h
*
*/



#ifndef __DATA_WRAPPER_H__
#define __DATA_WRAPPER_H__

#include <e32def.h>
#include <e32cmn.h>

#include <test/datadictionary.h>
#include <test/dataaccess.h>
#include <test/tefresult.h>
#include <test/activecallback.h>
#include <test/testblockcontroller.h>
#include <test/blockitems.h>


class CDataWrapper : public CBase, public MActiveCallback, public MDataAccess, public MTEFResult
/**
 * @publishedPartner
 * @test
 *
 * @see		CBase
 * @see		MActiveCallback
 * @see		MDataAccess
 * @see		MTEFResult
 *
 * Wrapper for an object in the data dictionary
 */
	{
public:
	IMPORT_C virtual				~CDataWrapper();

	/**
	 * Execute a command on the object
	 *
	 * @param	aCommand - command to execute
	 * @param	aSection - section with data for command
	 * @param	aAsyncErrorIndex - command index
	 *
	 * @see		TTEFFunction
	 * @see		TTEFSectionName
	 *
	 * @leave	system wide error
	 *
	 * @return	ETrue if the command is processed, EFalse if not
	 */
	IMPORT_C virtual TBool			DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex) = 0;

	/**
	 * Get the object that the wrapper contains
	 *
	 * @return	The object that the wrapper contains. NULL if it does not contain one.
	 */
	virtual TAny*					GetObject() = 0;

	inline virtual TCleanupOperation	CleanupOperation();
	inline virtual void					SetObjectL(TAny* aObject);
	inline virtual void					DisownObjectL();
	IMPORT_C void						SetTestBlockController(CTestBlockController* aTestBlockController);
	IMPORT_C void						SetDataDictionary(CDataDictionary* aDataDictionary);
	inline virtual void					InitialiseL();

	inline void						IncOutstanding();
	inline void						DecOutstanding();
	inline TBool					Outstanding();

	// MDataAccess implementation
	IMPORT_C virtual TBool			GetBoolFromConfig(const TDesC&  aSectName, const TDesC& aKeyName, TBool& aResult);
	IMPORT_C virtual TBool			GetIntFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult);
	IMPORT_C virtual TBool			GetStringFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TPtrC& aResult);
	IMPORT_C virtual TBool			GetHexFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult);
	IMPORT_C virtual TBool			WriteBoolToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TBool& aValue);
	IMPORT_C virtual TBool			WriteIntToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TInt& aValue);
	IMPORT_C virtual TBool			WriteStringToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TPtrC& aValue);
	IMPORT_C virtual TBool			WriteHexToConfig(const TDesC&  aSectName, const TDesC& aKeyName, const TInt& aValue);
	IMPORT_C virtual void			ReadSharedDataL(const TDesC& aShareName, TDes& aSharedDataVal);
	IMPORT_C virtual void			WriteSharedDataL(const TDesC& aShareName, TDesC& aSharedDataVal, TModeCopy aModeCopy = ESetText);
	IMPORT_C virtual CDataWrapper*	GetDataWrapperL(const TDesC& aSectionName);
	IMPORT_C virtual TAny*			GetDataObjectL(const TDesC& aSectionName);
	IMPORT_C virtual void			SetDataObjectL(const TDesC& aSectionName, TAny* aObject);
	IMPORT_C virtual CTestExecuteLogger&	Logger();

	// MActiveCallback implementation
	IMPORT_C virtual void			RunL(CActive* aActive, TInt aIndex);
	IMPORT_C virtual void			DoCancel(CActive* aActive, TInt aIndex);

	// MTEFResult implementation
	IMPORT_C virtual void			SetError(const TInt aError);
	IMPORT_C virtual void			SetAsyncError(const TInt aIndex, const TInt aError);
	IMPORT_C virtual void			SetBlockResult(const TVerdict aResult);
	IMPORT_C virtual TVerdict		BlockResult();

protected:
	IMPORT_C CDataWrapper();

private:
	CDataDictionary*		iDataDictionary;
	CTestBlockController*	iTestBlockController;
	TInt					iOutstanding;
	};

#include <test/datawrapper.inl>

#endif // __DATA_WRAPPER_H__
