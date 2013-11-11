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
* Description:  Panic codes and definition of a panic function for the Memory Hook
*
*/


#ifndef ANALYZETOOLPANICS_PAN_H
#define ANALYZETOOLPANICS_PAN_H

_LIT( KAnalyzeToolName, "AnalyzeTool" );

/** AnalyzeTool application panic codes */
enum TAnalyzeToolPanics
    {
    ENoMemory = 1,
    EFailedToCreateHeap,
    ECantOpenHandle,
    ECantLoadDevice,
    ECantAppendToTheArray,
    ECantFindRightThread,
    ECantCreateMutex,
    ECantLoadDeviceDriver,
    ECantConnectDeviceDriver
    // add further panics here
    };

inline void AssertPanic(TAnalyzeToolPanics aReason)
    {
    User::Panic( KAnalyzeToolName, aReason );
    }

#endif // ANALYZETOOLPANICS_PAN_H
