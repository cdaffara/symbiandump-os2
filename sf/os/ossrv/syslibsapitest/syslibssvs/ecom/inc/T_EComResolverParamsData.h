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



#if (!defined __T_ECOM_RESOLVER_PARAMS_API_DATA_H__)
#define __T_ECOM_RESOLVER_PARAMS_API_DATA_H__

//	User Includes
#include "DataWrapperBase.h"

// Classes to get to protected members

//	EPOC includes
#include <w32std.h>
#include <e32std.h>
#include <ecom/implementationinformation.h>

class CT_EComResolverParamsData: public CDataWrapperBase
	{
public:
	static CT_EComResolverParamsData*	NewL();

	~CT_EComResolverParamsData();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	CT_EComResolverParamsData();
	void	ConstructL();

private:
	void	DoCmdDataType(const TDesC& aSection);
	void	DoCmdSetDataType(const TDesC& aSection);
	void	DoCmdSetGenericMatch(const TDesC& aSection);
	void	DoCmdIsGenericMatch(const TDesC& aSection);
	void	DoCmdSetWildcardMatch(const TDesC& aSection);
	void	DoCmdIsWildcardMatch(const TDesC& aSection);

	TBool	GetExpectedString(const TDesC& aSection, TPtrC& aExpectedString);
	TBool	GetDataType(const TDesC& aSection, TBuf8<KMaxTestExecuteCommandLength>& aDataType);
	TBool	GetGenericMatch(const TDesC& aSection, TBool& aDisabled);
	TBool	GetWildcardMatch(const TDesC& aSection, TBool& aDisabled);
	TBool	GetExpectedBool(const TDesC& aSection, TBool& aBool);

	//	Helper methods
	void	DestroyData();

private:
	TEComResolverParams					iResolverParams;
	TBuf8<KMaxTestExecuteCommandLength>	iDataType;
	};

#endif /* __T_ECOM_RESOLVER_PARAMS_API_DATA_H__ */
