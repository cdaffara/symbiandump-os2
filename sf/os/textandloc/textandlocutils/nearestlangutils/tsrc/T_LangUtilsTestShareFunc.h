// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __T_LANGUTILS_TEST_SHAREFUNC_H__
#define __T_LANGUTILS_TEST_SHAREFUNC_H__

#include <e32std.h>
#include <f32file.h>



class LangUtilsTest
    {
public:
    static TInt DeleteFile(RFs& aFs, const TDesC& aSourceFullName, TUint aSwitch = 0);
    static TBool FileExists(const RFs& aFs,const TDesC& aFileName);
    static TInt CopyFile(RFs& aFs, const TDesC& aSourceFullName, const TDesC& aTargetFullName, TUint aSwitch = CFileMan::EOverWrite);

    };



#endif // __T_LANGUTILS_TEST_SHAREFUNC_H__

