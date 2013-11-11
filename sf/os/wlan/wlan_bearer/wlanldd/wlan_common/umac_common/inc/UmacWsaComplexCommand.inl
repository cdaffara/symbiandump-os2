/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Implementation of WlanWsaComplexCommand inline methods.
*
*/

/*
* %version: 6 %
*/

inline void WlanWsaComplexCommand::TraverseToHistoryState( 
    WlanContextImpl& aCtxImpl )
    {
    ChangeState( aCtxImpl, 
        *this,                  // previous state
        Dot11History()          // recall history state (new state)
        );                    
    }
