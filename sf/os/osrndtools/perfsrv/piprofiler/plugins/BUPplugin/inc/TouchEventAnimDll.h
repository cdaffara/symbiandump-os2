/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef _PROFILER_TOUCH_EVENT_ANIM_DLL_
#define _PROFILER_TOUCH_EVENT_ANIM_DLL_

/*
*
*	TouchEventAnimDll.h
*
*/

// system includes
#include <w32adll.h>
#include <e32def.h>



/*
*
* Class definition of CProfilerTouchEventAnim
*
*/
class CProfilerTouchEventAnim : public CWindowAnim
{
public:
	CProfilerTouchEventAnim();
	virtual ~CProfilerTouchEventAnim();

	// from CWindowAnim
	void ConstructL(TAny*	aAny, TBool aHasFocus);
	void Redraw();
	void FocusChanged(TBool aState);
	// from MEventHandler
	TBool OfferRawEvent(const TRawEvent& aRawEvent);
	// from CAnim
	void Animate(TDateTime* aDateTime);
	void Command(TInt aOpcode, TAny* aArgs);
	TInt CommandReplyL(TInt aOpcode, TAny* aArgs);
	
private:
	TBool HandlePointerDown(TPoint aPoint);
	TBool HandlePointerUp(TPoint aPoint);
	TBool HandleKeyDown(TInt aScanCode);
	TBool HandleKeyUp(TInt aScanCode);
private:
	TInt iState;
};


/*
*
* Class definition of CProfilerTouchEventAnimDll
*
*/

class CProfilerTouchEventAnimDll : public CAnimDll
{
public:
	CProfilerTouchEventAnimDll();

public:
	 IMPORT_C CAnim* CreateInstanceL(TInt aType);
	
};


#endif
