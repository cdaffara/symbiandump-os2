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
* NTB build base class inl file
*
*/


/**
@file
@internalComponent
*/


#ifndef NCMNTBBUILDER_INL
#define NCMNTBBUILDER_INL

const TInt KMinNtbInMaxSize = 2048;

inline TInt CNcmNtbBuilder::NtbInMaxSize()
    {
    return iNtbInMaxSize;
    }

inline void CNcmNtbBuilder::SetBuildPolicy(CNcmNtbBuildPolicy& aPolicy)
    {
    iBuildPolicy = &aPolicy;
    }

inline TBool CNcmNtbBuilder::IsNtbStarted()
    {
    return iNtbStarted;
    }

inline TInt CNcmNtbBuilder::MinNtbInMaxSize()
    {
    return KMinNtbInMaxSize;
    }



#endif
