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

#if !defined(__BASCHED_H__)
#define __BASCHED_H__

#if !defined(__e32std_h__)
#include <e32std.h>
#endif

#if !defined(__e32base_h__)
#include <e32base.h>
#endif

/**
@publishedAll
@released
*/
const TInt KErrExtended=(-1000);
const TInt KErrExtendedWithText=(-1001);
const TInt KLeaveWithoutAlert=(-1002);
const TInt KLeaveExit=(-1003);

/**
@publishedAll
*/
struct SExtendedError
	{
	TUid iComponent;
	TInt iErrorNumber;
	TBool iInformation;
	};

class CBaErrorHandler;

class CBaActiveScheduler : public CActiveScheduler
/**
@publishedAll
@released
*/
	{
public:
	IMPORT_C static void LeaveNoAlert();
public: // Internal to Symbian
	IMPORT_C CBaActiveScheduler();
	IMPORT_C ~CBaActiveScheduler();
	IMPORT_C static void Exit();
	IMPORT_C static void LeaveForAlert(TUid aComponent,TInt aErrorNumber);
	IMPORT_C static void LeaveForInfoPrint(TUid aComponent,TInt aErrorNumber);
	IMPORT_C static void LeaveForErrorHandler(const CBaErrorHandler *aHandler);
	IMPORT_C static const SExtendedError& ExtendedError();
	IMPORT_C static void DisplayExtendedError(TUid aComponent,TInt aErrorNumber);
	IMPORT_C virtual void DisplayError(TInt aError) const;
public:// from CActiveScheduler
	IMPORT_C void Error(TInt aError) const;
private:
// reserved virtual function space
	IMPORT_C virtual void Reserved_1();
	IMPORT_C virtual void Reserved_2();
// internal
	static void ExtendedLeave(TUid aComponent,TInt aErrorNumber,TBool aLeaveForInfoPrint);
protected:
	SExtendedError iExtendedError;
	};

#endif
