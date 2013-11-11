/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef FINDITEMENGINEPANIC_H
#define FINDITEMENGINEPANIC_H

// ========== DATA TYPES ===================================

enum TFindItemEnginePanic
	{
    EItemOutOfDocumentRange = 0x100,
    ENoSearchCase
	};

// ========== FUNCTION PROTOTYPES ==========================

// ---------------------------------------------------------
// Panic
//
// ---------------------------------------------------------
//
GLREF_C void Panic (TFindItemEnginePanic aPanic);

#endif // FINDITEMENGINEPANIC_H

// End of File
