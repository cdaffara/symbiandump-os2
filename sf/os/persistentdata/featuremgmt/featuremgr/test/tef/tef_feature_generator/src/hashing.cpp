// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "hashing.h"
#include "efmmain.h"

//
// This code is for setting up and accessing hashes. These are used within
// the test harness to associate a word with an integer value.
// The base class CEFMHashUtil contains the accessor and destructor
// methods which do 99% of the work. All of the other classes merely
// contain constructors to set up their hashes.
//

// Basic functionality first...
TPtrC* CEFMHashUtil::GetStringFromNum(TInt aErrNum)
    {
    return ihash.Find(aErrNum);
    }

TInt CEFMHashUtil::GetNumFromString(TPtrC aErrMsg)
    {
    THashMapIter<TInt, TPtrC> it(ihash);
    it.NextValue();
    while (it.CurrentValue())
        {
        if (*it.CurrentValue() == aErrMsg)
            return *it.CurrentKey();
        it.NextValue();
        }
    return (KErrNotFound);
    }

CEFMHashUtil::~CEFMHashUtil()
    {
    while(ihash.Count())
        {
        THashMapIter<TInt, TPtrC> it(ihash);
        const TInt *curkey = it.NextKey();
        ihash.Remove(*curkey);
        }
    }

// Now do the enumerations for all of the functions in efmmain..
CEFMTEFAction::CEFMTEFAction()
    {
    // Enumerations from efmmain.h
    ihash.Insert(CEFMFnStep::Efn_undefined, _L("fn_undefined"));
    ihash.Insert(CEFMFnStep::Efn_nop, _L("NoOperation"));
    ihash.Insert(CEFMFnStep::Efn_loadfromini, _L("LoadFromIni"));
    ihash.Insert(CEFMFnStep::Efn_loadfromdat, _L("LoadFromDat"));
    ihash.Insert(CEFMFnStep::Efn_loadfromcfg, _L("LoadFromCfg"));
    ihash.Insert(CEFMFnStep::Efn_writefile, _L("WriteFile"));
    ihash.Insert(CEFMFnStep::Efn_compareini, _L("CompareToIni"));
    ihash.Insert(CEFMFnStep::Efn_comparedat, _L("CompareToDat"));
    ihash.Insert(CEFMFnStep::Efn_setndsrs, _L("SetNDSRs"));
    TInt err = ihash.Insert(CEFMFnStep::Ectrl_endblock, _L("EndBlock"));
    if( err == KErrNone )
        return;

    User::Panic(_L("Unable to create action hash"), 1414);
    }

