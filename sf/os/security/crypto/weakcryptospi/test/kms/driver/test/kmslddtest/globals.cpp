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
* Defines globals shared across translation units.
*
*/


/**
 @file
*/

#include "kmslddtest.h"


RTest test(_L("KmsLddTest"));				///< Test object asserts conditions and prints status.

RKmsChannel KmsChannel;					///< Channel object declared at file level for convenience.
RKmsExtrChannel ExtrChannel;				///< Channel object declared at file level for convenience.

TKeyHandle GenKeyHandle0;				///< When used, refers to generated key.
TKeyHandle GenKeyHandle1;				///< When used, refers to second generated key.

TKeyHandle StoreKeyHandle0;				///< When used, refers to stored key.
TKeyHandle StoreKeyHandle1;				///< When used, refers to second stored key.

RProcess CurrentProcess;					///< Handle to current process, declared at file level for convenience.
