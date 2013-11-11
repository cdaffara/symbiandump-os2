/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* implementation for NTB build policy base class
*
*/


/**
@file
@internalComponent
*/


#include "ncmntbbuildpolicy.h"

// ======== MEMBER FUNCTIONS ========
//

CNcmNtbBuildPolicy::CNcmNtbBuildPolicy(CNcmNtbBuilder& aBuilder)
    : CActive(CActive::EPriorityStandard), iNtbBuilder(aBuilder)
    {
    }

CNcmNtbBuildPolicy::~CNcmNtbBuildPolicy()
    {
    }





