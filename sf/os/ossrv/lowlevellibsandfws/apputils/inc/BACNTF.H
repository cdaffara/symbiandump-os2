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
//

// Started by BLB, October 1996
#if !defined(__BACNTF_H__)
#define __BACNTF_H__

#include <e32base.h>


class CEnvironmentChangeNotifier : public CActive
/** Environment change notifier. This is an active object and can be used to handle 
environment change events. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CEnvironmentChangeNotifier* NewL(TInt aPriority,const TCallBack& aCallBack);
	IMPORT_C ~CEnvironmentChangeNotifier();
	IMPORT_C void Start();
	IMPORT_C TInt Set(const TCallBack& aCallBack);
	inline TInt Change() const;
private:
	inline CEnvironmentChangeNotifier(TInt aPriority);
	void RunL();
	void DoCancel();
private:
	TInt iChange;
	TCallBack iCallBack;
	RChangeNotifier iChangeNotifier;
	};



inline TInt CEnvironmentChangeNotifier::Change() const
	/** Returns the last set of change events.
	
	If the last outstanding request completed normally, the function returns a 
	bit pattern where each bit value corresponds to one of the enumerators defined 
	by TChanges. A set bit indicates that the corresponding change event occurred.
	
	For example, if the bit value TChanges::EChangesMidnightCrossover is set, 
	then the system time has passed midnight.
	
	@return A set of bits consisting of one or more of the values defined by TChanges, 
	or KErrCancel if the last outstanding request was cancelled. 
	@see TChanges */
	{return iChange;}

#endif


