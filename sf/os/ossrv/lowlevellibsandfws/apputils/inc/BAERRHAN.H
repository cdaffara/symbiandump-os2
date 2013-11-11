// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Started by DWW, May 1997
// Error handler
// 
//

#if !defined(__BAERRHAN_H__)
#define __BAERRHAN_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif
/**
@internalComponent
*/
#ifdef _UNICODE
#define KUidBaflErrorHandlerValue KUidBaflErrorHandlerValue16
#define KUidBaflErrorHandler KUidBaflErrorHandler16
#else
#define KUidBaflErrorHandlerValue KUidBaflErrorHandlerValue8
#define KUidBaflErrorHandler KUidBaflErrorHandler8
#endif


/**
@internalComponent
*/
const TInt KUidBaflErrorHandlerValue8=0x10000257;
const TUid KUidBaflErrorHandler8={KUidBaflErrorHandlerValue8};
const TInt KUidBaflErrorHandlerValue16=0x10003A13;
const TUid KUidBaflErrorHandler16={KUidBaflErrorHandlerValue16};

/**
@internalComponent
*/
enum TErrorHandlerResponse
	{
	EErrorNotHandled,
	ENoDisplay,
	EAlertDisplay,
	EInfoDisplay
	};

class CBaErrorHandler : public CBase
/**
deliberately *not* mixable - else couldn't be freely cast
@internalComponent
*/
	{ 
public:
	virtual TErrorHandlerResponse HandleError(TDes& aErrorText,TDes& aContextText)=0;
	inline static TErrorHandlerResponse CallBack(TInt aErrorNumber,TDes& aErrorText,TDes& aContextText);
	};

inline TErrorHandlerResponse CBaErrorHandler::CallBack(TInt aErrorNumber,TDes& aErrorText,TDes& aContextText)
	{
	return((CBaErrorHandler*)(aErrorNumber))->HandleError(aErrorText,aContextText);
	}

#endif
