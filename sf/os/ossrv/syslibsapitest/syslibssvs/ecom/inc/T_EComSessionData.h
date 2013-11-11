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



#if (!defined __T_ECOM_SESSION_API_DATA_H__)
#define __T_ECOM_SESSION_API_DATA_H__

//	User Includes
#include "DataWrapperBase.h"

// Classes to get to protected members

//	EPOC includes
#include <w32std.h>
#include <e32std.h>
#include <ecom/ecom.h>

class CT_EComSessionData: public CDataWrapperBase
	{
private:
	struct TTestInterfaceInitParams
		{
		TInt			iInteger;
		const TDesC*	iDescriptor;
		};

public:
	static CT_EComSessionData*	NewL();

	~CT_EComSessionData();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

	//	MTPActiveCallback implementation
	virtual void	RunL(CActive* aActive, const TInt aIndex);

protected:
	CT_EComSessionData();
	void	ConstructL();

private:
	inline void	DoCmdOpenL();
	inline void	DoCmdClose();
	inline void	DoCmdFinalClose();
	inline void	DoCmdNotifyOnChange(const TInt aIndex);
	inline void	DoCmdCancelNotifyOnChange();
	inline void	DoCmdListImplementationsL(const TDesC& aSection);
	inline void	DoCmdCreateImplementationL(const TDesC& aSection);
	inline void	DoCmdDestroyedImplementation(const TDesC& aSection);

	inline void	RunNotifyOnChangeL(const TInt aIndex);

	//	Helper methods
	void	DestroyData();
	
	void	GetUid(const TDesC& aSection, TUid& aUid);
	TBool	GetResolver(const TDesC& aSection, TPtrC& aResolver);
	TBool	GetResolverUid(const TDesC& aSection, TUid& aUid);
	TBool	GetExpectedResult(const TDesC& aSection, TInt& aExpectedResult);
	TBool	GetOffset(const TDesC& aSection, TInt& aOffset);
	TBool	GetDescriptor(const TDesC& aSection, TPtrC& aDescriptor);
	TBool	GetInteger(const TDesC& aSection, TInt& aInt);
	TBool	GetEmpty(const TDesC& aSection, TBool& aEmpty);

private:
    REComSession            			iSession;
    TUid								iDTorKey;
	CActiveCallback*					iActiveNotifyOnChange;
	TTestInterfaceInitParams			iInitParams;
	TBuf<KMaxTestExecuteCommandLength>	iInterfaceDesc;
	RImplInfoPtrArray					iInfoArray;

	//Initially iImplementationInterface==NULL. Object can be created by DoCmdCreateImplementationL.
    TAny*								iImplementationInterface;
	};
#endif /* __T_ECOM_SESSION_API_DATA_H__ */
