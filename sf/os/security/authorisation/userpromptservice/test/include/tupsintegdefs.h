/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


/**
 @file  
 @test
*/

#if (!defined TUPS_SUITEDEFS_H)
#define TUPS_SUITEDEFS_H

// Posible status for Dialog creator
#define KUpsDcStatusReset	       0x0010
#define KUpsDcStatusStarted        0x0011
#define KUpsDcStatusPrepareHold    0x0012
#define KUpsDcStatusDisplayHold    0x0013
#define KUpsDcStatusCompleted      0x0014
const TInt KErrDcLeave=(-60);

// Posible status for Policy evaluator
#define KUpsPeStatusReset          0x0020
#define KUpsPeStatusStarted        0x0021
#define KUpsPeStatusHolding		   0x0022
#define KUpsPeStatusCompleted      0x0023

// status to determine that a Client has completed
#define KUpsClientCompleted		   0x0030

const TInt KErrPeLeave=(-61);

// Bit mask values for buttons to be displayed by dialog creator.
#define KYes        0x0001
#define KNo         0x0002
#define KSessionYes 0x0004
#define KAlways     0x0008
#define KNever      0x0010
#define KSessionNo  0x0020
#define KNone      	0x0000


// For test step panics
_LIT(K_TUPSSuitePanic,"TUPSSuite");

#endif
