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


#include "TouchEventClientDll.h"
#include <piprofiler/ProfilerTraces.h>

/*
 *	
 *	class RProfilerTouchEventAnim implementation
 * 
 */
void RProfilerTouchEventAnim::ConstructL( const RWindow& aParent)
{
	LOGTEXT(_L("RProfilerTouchEventAnim::ConstructL - entry"));
	LOGTEXT(_L("RProfilerTouchEventAnim::ConstructL - calling RAnim::Construct..."));
	RAnim::Construct(aParent, 0, TPtrC8());
}


RProfilerTouchEventAnim::RProfilerTouchEventAnim( RAnimDll &aAnimDll ) :
    RAnim( aAnimDll )
{
}

void RProfilerTouchEventAnim::Activate()
{
	LOGTEXT(_L("RProfilerTouchEventAnim::Activate - entry"));
	TInt err = RAnim::CommandReply(KActivate);
	LOGSTRING2("RProfilerTouchEventAnim::Activate - error: %d", err);
}

void RProfilerTouchEventAnim::Deactivate()
{
	LOGTEXT(_L("RProfilerTouchEventAnim::Deactivate - entry"));
	TInt err = RAnim::CommandReply(KDeactivate);
	LOGSTRING2("RProfilerTouchEventAnim::Deactivate - error: %d", err);
}

void RProfilerTouchEventAnim::Close()
	{
	RAnim::Close();
	}



