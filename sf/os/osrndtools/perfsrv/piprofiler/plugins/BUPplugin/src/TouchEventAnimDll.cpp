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



#include <e32base.h>
#include <e32cons.h>
#include <w32std.h>
#include <in_sock.h>
#include <txtfrmat.h>
#include <e32property.h>
#include <piprofiler/ProfilerTraces.h>

// user includes
#include "TouchEventAnimDll.h"

// control commands
static const TInt KActivate   = 70002;
static const TInt KDeactivate = 70003;

// touch events
static const TInt KTouchEventDown = 69999;
static const TInt KTouchEventUp = 70000;

static const TInt KUpEventOffset = 70000;

_LIT( KTouchEventServer, "PIProfilerTouchEvent server" );
enum TAnimationPanics
    {
    EProfilerTouchEventServerPanic = 100
    };

const TUid KProfilerKeyEventPropertyCat={0x2001E5AD};
enum TProfilerKeyEventPropertyKeys
	{
	EProfilerKeyEventPropertySample = 7
	};

/*
*
* Implementation of CProfilerTouchEventAnim
*
*/
CProfilerTouchEventAnim::CProfilerTouchEventAnim() : iState(EFalse)
    {
	LOGTEXT(_L("CProfilerTouchEventAnim::CProfilerTouchEventAnim - entry"));
    }

CProfilerTouchEventAnim::~CProfilerTouchEventAnim()
    {
	LOGTEXT(_L("CProfilerTouchEventAnim::~CProfilerTouchEventAnim - entry"));
	//iFunctions->GetRawEvents( EFalse );	// disable capture
	LOGTEXT(_L("CProfilerTouchEventAnim::~CProfilerTouchEventAnim - exit"));
    }

void CProfilerTouchEventAnim::ConstructL(TAny* /*aArgs*/, TBool /*aHasFocus*/)
    {
	LOGTEXT(_L("CProfilerTouchEventAnim::ConstructL - entry"));
    iFunctions->GetRawEvents( ETrue );
	LOGTEXT(_L("CProfilerTouchEventAnim::ConstructL - exit"));
    }

void CProfilerTouchEventAnim::Command(TInt /*aOpcode*/, TAny* /*aArgs*/)
    {

    }

TInt CProfilerTouchEventAnim::CommandReplyL(TInt aOpcode, TAny* /*aArgs*/)
    {
	LOGSTRING2("CProfilerTouchEventAnim::CommandReplyL - entry, aOpcode: %d", aOpcode);
	switch(aOpcode)
	    {
		case KActivate:	// activate
			iState = ETrue;
			LOGTEXT(_L("CProfilerTouchEventAnim::CommandReplyL - activate"));
			break;
		case KDeactivate: // deactivate
			iState = EFalse;
			iFunctions->GetRawEvents( EFalse );	// disable capture
			LOGTEXT(_L("CProfilerTouchEventAnim::CommandReplyL - deactivate"));
			break;
		default:
			User::Panic( KTouchEventServer, EProfilerTouchEventServerPanic );
			LOGSTRING2("CProfilerTouchEventAnim::CommandReplyL - panic, code %d", EProfilerTouchEventServerPanic);
			return EProfilerTouchEventServerPanic;

        }
	return KErrNone;
    }


TBool CProfilerTouchEventAnim::OfferRawEvent(const TRawEvent& aRawEvent)
    {
	LOGTEXT(_L("CProfilerTouchEventAnim::OfferRawEvent - entry"));
	if(iState == EFalse)
		return EFalse; // if not activated yet just pass through
	

	switch(aRawEvent.Type())
	    {
        // handle the pointer events here
        case TRawEvent::EButton1Down:
            {
            LOGTEXT(_L("CProfilerTouchEventAnim::OfferRawEvent - pointer down"));
            return HandlePointerDown(aRawEvent.Pos());
            }
        case TRawEvent::EButton1Up:
            {
            LOGTEXT(_L("CProfilerTouchEventAnim::OfferRawEvent - pointer up"));
            return HandlePointerUp(aRawEvent.Pos());
            }
                
            // handle the key events here, replacing the BUP trace functionality
        case TRawEvent::EKeyDown:
            {
            TInt scan = aRawEvent.ScanCode() & 0xFFFF;
                    return HandleKeyDown(scan);
            }
        case TRawEvent::EKeyUp:
            {
            TInt scan = (aRawEvent.ScanCode() & 0xFFFF)+KUpEventOffset;	// 
                    return HandleKeyUp(scan);
            }
            default:
                return EFalse;	// no action
        }
    }

TBool CProfilerTouchEventAnim::HandlePointerDown( TPoint /*aPoint*/ )
    {
	RProperty::Set(KProfilerKeyEventPropertyCat,EProfilerKeyEventPropertySample, KTouchEventDown);
	return EFalse;
    }

TBool CProfilerTouchEventAnim::HandlePointerUp( TPoint /*aPoint*/ )
    {
	RProperty::Set(KProfilerKeyEventPropertyCat,EProfilerKeyEventPropertySample, KTouchEventUp);
	return EFalse;
    }

TBool CProfilerTouchEventAnim::HandleKeyDown( TInt aScanCode )
    {
	LOGSTRING2("CProfilerTouchEventAnim::HandleKeyDown - scancode = %d", aScanCode);
	RProperty::Set(KProfilerKeyEventPropertyCat,EProfilerKeyEventPropertySample, aScanCode);
	return EFalse;
    }

TBool CProfilerTouchEventAnim::HandleKeyUp( TInt aScanCode )
    {
	LOGSTRING2("CProfilerTouchEventAnim::HandleKeyUp - scancode = %d", aScanCode);
	RProperty::Set(KProfilerKeyEventPropertyCat,EProfilerKeyEventPropertySample, aScanCode);
	return EFalse;
    }


void CProfilerTouchEventAnim::Animate(TDateTime* /*aDateTime*/)
    {
    }

void CProfilerTouchEventAnim::Redraw()
    {
    }

void CProfilerTouchEventAnim::FocusChanged(TBool /*aState*/)
    {
    }


/*
*
* Implementation of CProfilerTouchEventAnimDll
*
*/
CProfilerTouchEventAnimDll::CProfilerTouchEventAnimDll() : CAnimDll()
    {
    }

CAnim* CProfilerTouchEventAnimDll::CreateInstanceL(TInt /*aType*/)
    {
	LOGTEXT(_L("CProfilerTouchEventAnimDll::CreateInstanceL - entry"));
	return (new (ELeave) CProfilerTouchEventAnim());
    }


// DLL entry
EXPORT_C CAnimDll* CreateCAnimDllL()
    {
	return (new (ELeave) CProfilerTouchEventAnimDll);
    }


