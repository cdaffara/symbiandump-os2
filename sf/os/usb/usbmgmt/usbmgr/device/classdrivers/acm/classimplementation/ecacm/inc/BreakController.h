/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __BREAKCONTROLLER_H__
#define __BREAKCONTROLLER_H__

#include <e32base.h>
 
class CCdcAcmClass;
class MHostPushedChangeObserver;

NONSHARABLE_CLASS(CBreakController) : public CActive
/**
 * Break (timed signal) handler
 *
 * This class provides a simple active object that manages the timing aspect 
 * of a break: there is only one of these, that is used to manage both 
 * client-induced breaks (routed in via the ACM port) and break commands sent 
 * here from the (USB) Host.
 *
 * Note that only one of the device and the host can be 'in charge of' a 
 * break. If the host, say, is dealing with a timed break, then it is illegal 
 * for the device to issue a timed break, though the host may override the 
 * timed break with a new timed break or a locked break. 
 */
	{
public:
	enum TRequester
		{
		ENone,
		EDevice,
		EHost,
		};

	enum TState
		{
		EInactive,
		ETiming,
		ELocked,
		ENumStates
		};

public:
	static CBreakController* NewL(CCdcAcmClass& aParentAcm);
	~CBreakController();

public: // API for requesting breaks
	TInt BreakRequest(TRequester aRequester, 
		TState aState, 
		TTimeIntervalMicroSeconds32 aDelay = 0);

private:
	CBreakController(CCdcAcmClass& aParentAcm);
	void ConstructL();

private:
	void StateMachine(TState aBreakState, TTimeIntervalMicroSeconds32 aDelay);

	void Publish(TState aNewState);

	static TBool ScInvalid(CBreakController *aThis, 
		TTimeIntervalMicroSeconds32 aDelay);
	static TBool ScInactive(CBreakController *aThis, 
		TTimeIntervalMicroSeconds32 aDelay);
	static TBool ScSetTimer(CBreakController *aThis, 
		TTimeIntervalMicroSeconds32 aDelay);
	static TBool ScLocked(CBreakController *aThis, 
		TTimeIntervalMicroSeconds32 aDelay);

private: // from CActive
	void RunL();
	void DoCancel();

private: // owned
	typedef TBool ( *PBFNT ) (CBreakController *aThis, 
		TTimeIntervalMicroSeconds32 aDelay);
	PBFNT StateDispatcher[ENumStates][ENumStates];
	RTimer iTimer;
	TState iBreakState;
	// Whoever's in charge of the current break.
	TRequester iRequester;

private: // unowned
	CCdcAcmClass&	  iParentAcm;	///< use to tell (USB) Host about changes
	};

#endif // __BREAKCONTROLLER_H__
