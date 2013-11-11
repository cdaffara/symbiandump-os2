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
// Started by DWW, March 1997
// BAFL specialization of CActiveScheduler
// 
//

#include <basched.h>
#include <baerrhan.h>

/**
 * @publishedAll
 */
EXPORT_C CBaActiveScheduler::CBaActiveScheduler()
	{
	}

/**
 * @publishedAll
 */
EXPORT_C CBaActiveScheduler::~CBaActiveScheduler()
	{
	}

/**
 * @internalAll
 */
EXPORT_C void CBaActiveScheduler::Exit()
	{ // static
	User::Leave(KLeaveExit);
	}

/**
 * @internalAll
 */
EXPORT_C void CBaActiveScheduler::Error(TInt aError) const
	{
	if (aError==KLeaveExit)
		User::Leave(KLeaveExit);
	DisplayError(aError);
	}

/**
 * @internalAll
 */
EXPORT_C void CBaActiveScheduler::DisplayError(TInt /*aError*/) const
	{ // for subclassers to replace
	}

/**
 * @internalAll
 */
EXPORT_C void CBaActiveScheduler::DisplayExtendedError(TUid aComponent,TInt aErrorNumber)
	{ // static
	CBaActiveScheduler* pS=((CBaActiveScheduler*)CActiveScheduler::Current());
	pS->iExtendedError.iInformation=EFalse;
	pS->iExtendedError.iComponent=aComponent;
	pS->iExtendedError.iErrorNumber=aErrorNumber;
	pS->DisplayError(KErrExtended);
	}

EXPORT_C void CBaActiveScheduler::LeaveNoAlert()
	{ // static
	User::Leave(KLeaveWithoutAlert);
	}

/**
 * @internalAll
 */
EXPORT_C void CBaActiveScheduler::LeaveForAlert(TUid aComponent,TInt aErrorNumber)
	{ // static
	ExtendedLeave(aComponent,aErrorNumber,EFalse);
	}

/**
 * @internalAll
 */
EXPORT_C void CBaActiveScheduler::LeaveForInfoPrint(TUid aComponent,TInt aErrorNumber)
	{ // static
	ExtendedLeave(aComponent,aErrorNumber,ETrue);
	}

/**
 * @internalAll
 */
EXPORT_C void CBaActiveScheduler::LeaveForErrorHandler(const CBaErrorHandler *aHandler)
	{ // static
	ExtendedLeave(KUidBaflErrorHandler,(TInt)(aHandler),EFalse);
	}

void CBaActiveScheduler::ExtendedLeave(TUid aComponent,TInt aErrorNumber,TBool aLeaveForInfoPrint)
	{ // static
	CBaActiveScheduler* pS=((CBaActiveScheduler*)CActiveScheduler::Current());
	pS->iExtendedError.iInformation=aLeaveForInfoPrint;
	pS->iExtendedError.iComponent=aComponent;
	pS->iExtendedError.iErrorNumber=aErrorNumber;
	User::Leave(KErrExtended);
	}

/**
 * @internalAll
 */
EXPORT_C const SExtendedError& CBaActiveScheduler::ExtendedError()
	{
	CBaActiveScheduler* pS=((CBaActiveScheduler*)CActiveScheduler::Current());
	return(pS->iExtendedError);
	}

/**
 * @internalAll
 */
EXPORT_C void CBaActiveScheduler::Reserved_1()
//
// Reserved for future development
//
	{}

/**
 * @internalAll
 */
EXPORT_C void CBaActiveScheduler::Reserved_2()
//
// Reserved for future development
//
	{}
