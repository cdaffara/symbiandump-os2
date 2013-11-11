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



#if (!defined __T_IMPLEMENTATION_INFORMATION_API_DATA_H__)
#define __T_IMPLEMENTATION_INFORMATION_API_DATA_H__

//	User Includes
#include "DataWrapperBase.h"

// Classes to get to protected members

//	EPOC includes
#include <w32std.h>
#include <e32std.h>
#include <ecom/implementationinformation.h>

class CT_ImplementationInformationData: public CDataWrapperBase
	{
public:
	/**
	* Two phase constructor
	*/
	static CT_ImplementationInformationData*	NewL();

	/**
	* Destructor
	*/
	~CT_ImplementationInformationData();

	/**
	 * Process a command read from the ini file
	 *
	 * @param aCommand			The command to process
	 * @param aSection			The section in the ini containing data for the command
	 * @param aAsyncErrorIndex	Command index for async calls to return errors to
	 *
	 * @return ETrue if the command is processed
	 *
	 * @leave					System wide error
	 */
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

	/**
	 * @return	Pointer to the object that the data wraps
	 */
	inline virtual TAny*	GetObject() 
		{
		return iImplementationInformation;
		};

	/**
	 * Set the object that the data wraps
	 *
	 * @param	aObject object that the wrapper is testing
	 *
	 * @leave	KErrNotSupported if the the function is not supported
	 */
	virtual inline void	SetObjectL(TAny* aObject)
		{
		iImplementationInformation=static_cast<CImplementationInformation*>(aObject);
		};

protected:
	/**
	 * Public constructor. First phase construction
	 */
	CT_ImplementationInformationData();

	/**
	* Second phase construction
	*/
	void	ConstructL();

private:
	inline void	DoCmdDestroy();
	inline void	DoCmdDisplayName(const TDesC& aSection);
	inline void	DoCmdDataType(const TDesC& aSection);
	inline void	DoCmdOpaqueData(const TDesC& aSection);
	inline void	DoCmdImplementationUid(const TDesC& aSection);
	inline void	DoCmdVersion(const TDesC& aSection);
	inline void	DoCmdDisabled(const TDesC& aSection);
	inline void	DoCmdSetDisabled(const TDesC& aSection);
	inline void	DoCmdDrive(const TDesC& aSection);
	inline void	DoCmdRomOnly(const TDesC& aSection);
	inline void	DoCmdRomBased(const TDesC& aSection);

	TBool	GetUid(const TDesC& aSection, TUid& aUid);
	TBool	GetExpectedString(const TDesC& aSection, TPtrC& aDataType);
	TBool	GetExpectedResult(const TDesC& aSection, TInt& aExpectedResult);
	TBool	GetExpectedHexResult(const TDesC& aSection, TInt& aExpectedResult);
	TBool	GetDisabled(const TDesC& aSection, TBool& aDisabled);
	TBool	GetExpectedBool(const TDesC& aSection, TBool& aBool);
	TBool	GetArrayElement(const TDesC& aSection, TInt& aArrayElement);

	//	Helper methods
	void	DestroyData();

private:
    CImplementationInformation*		iImplementationInformation;
	};

#endif /* __T_IMPLEMENTATION_INFORMATION_API_DATA_H__ */
