// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
*/
#include "repositoryfn.h"
#include "Te_centrep_SWISuiteDefs.h"
#include "common.h"

// Contains code to perform functions on a repository.

CRepositoryFnStep::~CRepositoryFnStep()
/**
 * Destructor
 */
    {
    }

CRepositoryFnStep::CRepositoryFnStep()
/**
 * Constructor
 */
    {
    // **MUST** call SetTestStepName in the constructor as the controlling
    // framework uses the test step name immediately following construction to set
    // up the step's unique logging ID.
    SetTestStepName(KRepositoryFn);
    }

TVerdict CRepositoryFnStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL()
 * did not leave. That being the case, the current test result value will be
 * EPass.
 * Perform functions on a repository, this can include creation, deletion and
 * setting of repository settings.
 */
    {
    SetTestStepResult(EPass);    

    // Get the key for the repository we're interested in. Leave if absent.
    TInt reposkey=0;
    TInt bRet = GetHexFromConfig(ConfigSection(), KRepositoryKey, reposkey );
    if(bRet!=1)
        {
        SetTestStepResult(EFail);    
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF1(_L("Repository key not specified in .ini file."));
        INFO_PRINTF1(HTML_RED_OFF);
        return TestStepResult();
        }
    INFO_PRINTF2(_L("Repository key: 0x%x"), reposkey );

    // Create the repository object...
    CRepository* repository=NULL;
    TInt leavecode;
    TRAP(leavecode, repository = CRepository::NewL(TUid::Uid(reposkey)));
    if( leavecode != KErrNone )
        {
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF2(_L("Repository 0x%x does not exist."), reposkey );
        INFO_PRINTF1(HTML_RED_OFF);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    CleanupStack::PushL(repository);

    // Look for integer settings to set...
    TInt nints=0;
    bRet = GetIntFromConfig(ConfigSection(), KRepositoryNumInts, nints );
    INFO_PRINTF2(_L("%d ints to modify"), nints );
    if(bRet == 1)
        {
        ModifyRepositoryIntegerSettings(repository, nints);
        }

    // Look for real settings to set...
    TInt nreals=0;
    bRet = GetIntFromConfig(ConfigSection(), KRepositoryNumReals, nreals );
    INFO_PRINTF2(_L("%d reals to modify"), nreals );
    if(bRet == 1)
        {
        ModifyRepositoryRealSettings(repository, nreals);
        }

    // Look for string settings to set...
    TInt nstrings=0;
    bRet = GetIntFromConfig(ConfigSection(), KRepositoryNumStrings, nstrings );
    INFO_PRINTF2(_L("%d strings to modify"), nstrings );
    if(bRet == 1)
        {
        ModifyRepositoryStringSettings(repository, nstrings );
        }

    CleanupStack::PopAndDestroy(repository);
    
    return TestStepResult();
    }


/**
 * @return - nothing
 * Modify repository settings as defined in the config file. For example,
 * we want to set 37 integers in the repository. Their keys will be
 * 'IntKey0' through 'IntKey36'. We'll expect to see hex values for
 * all of these names.
 */
void CRepositoryFnStep::ModifyRepositoryIntegerSettings(CRepository *arepository, TInt anset )
    {
    // Careful if the prefix here changes, we don't want to
    // blow this buffer. Also note the 'Delete' at the end of the loop.
    TBuf<REPCHECKCREATE_KEYBUFLEN> tkey(KIntKeyPrefix);
    TBuf<REPCHECKCREATE_VALBUFLEN> tval(KIntValPrefix);
    TBuf<REPCHECKCREATE_VALBUFLEN> experr(KIntExpectedError);
    TBuf<REPCHECKCREATE_VALBUFLEN> function(KIntFunction);
    TInt keyslen = tkey.Length();
    TInt valslen = tval.Length();
    TInt experrlen = experr.Length();
    TInt fnlen = function.Length();
    for( TInt count=0 ; count < anset ; count++ )
        {
        // Append the count to the name of the key/value/expected error
        // config items..
        tkey.AppendNum(count);            // e.g "IntKey21"
        tval.AppendNum(count);            // e.g "IntVal47"
        experr.AppendNum(count);            // e.g "IntExpErr204"
        function.AppendNum(count);            // e.g "Intfn204"

        // Get the required function config item. We fetch this as a string
        // and convert it..
        TInt fn;
        // Where to put the string from the config file...
        TBuf<REPCHECKCREATE_MAXSTRINGLEN> fnbuf;
        TPtrC fns(fnbuf);

        if(GetStringFromConfig(ConfigSection(), function, fns) )
            fn = FuncToNum(fns);
        else
            fn = fn_set;

        // Get the expected error config item. We fetch this as a string
        // and convert it..
        TInt err;
        // Where to put the string from the config file...
        TBuf<REPCHECKCREATE_MAXSTRINGLEN> errbuf;
        TPtrC errs(errbuf);

        if(GetStringFromConfig(ConfigSection(), experr, errs) )
            err = ErrorToNum(errs);
        else
            err = KErrNone;

        // Get the key. It MUST be defined in the config file.
        TInt r=0, key;
        TInt bRet = GetHexFromConfig(ConfigSection(), tkey, key );
        if(bRet != 1)
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF2(_L("Failed to get key 0x%x"), key );
            INFO_PRINTF1(HTML_RED_OFF);
            SetTestStepResult(EFail);
            continue;
            }
        TInt setval;
        bRet = GetHexFromConfig(ConfigSection(), tval, setval );
        if((bRet != 1) && ( (fn==fn_set) || (fn==fn_create) ) )
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF2(_L("FAIL: Failed to get config intval%d"), count );
            INFO_PRINTF1(HTML_RED_OFF);
            SetTestStepResult(EFail);
            continue;
            }

	TInt vali=-1;
	TInt rr=KErrNone;
        switch(fn)
            {
            case fn_set: 
                INFO_PRINTF3(_L("Setting integer, key 0x%x, to 0x%x "), key, setval );
                r = arepository->Set( key, setval );
                break;
            case fn_create: 
                INFO_PRINTF3(_L("Creating integer, key 0x%x, to 0x%x "), key, setval );
                r = arepository->Create( key, setval );
                break;
            case fn_delete: 
                INFO_PRINTF2(_L("Deleting integer setting, key 0x%x"), key );
                r = arepository->Delete( key );
                rr = arepository->Get( key, vali );
                if( rr != KErrNotFound )
                    {
                    INFO_PRINTF3(_L("Get following delete gave rc %d, val: 0x%x"), rr, vali);
                    }
                break;
            case fn_reset: 
                INFO_PRINTF2(_L("Resetting integer, key 0x%x"), key );
                r = arepository->Reset( key );
                break;
            default:
                INFO_PRINTF1(HTML_RED);
                ERR_PRINTF3(_L("FAIL: Unknown function %d for Int setting %d"), fn, count );
                INFO_PRINTF1(HTML_RED_OFF);
                r = KErrNone;
                SetTestStepResult(EFail);
                break;
            }
        if(r!=err)
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF3(_L("Wrong error code. Got %d, expected %d"), r, err);
            INFO_PRINTF1(HTML_RED_OFF);
            SetTestStepResult(EFail);
            }
        else
            {
            INFO_PRINTF2(_L("Got expected error code %d"), r );
            }

        // Restore the key and value names (i.e strip the digits from the end)
        tkey.Delete(keyslen, REPCHECKCREATE_KEYBUFLEN);
        tval.Delete(valslen, REPCHECKCREATE_VALBUFLEN);
        experr.Delete(experrlen, REPCHECKCREATE_VALBUFLEN);
        function.Delete(fnlen, REPCHECKCREATE_VALBUFLEN);
        }
    // Should we return something?
    return;
    }

/**
 * @return - nothing
 * Modify repository settings as defined in the config file. For example,
 * we might want to set 37 reals in the repository. Their keys will be
 * 'RealKey0' through 'RealKey36'. 
 */
void CRepositoryFnStep::ModifyRepositoryRealSettings(CRepository *arepository, TInt anset )
    {
    // Careful if the prefix here changes, we don't want to
    // blow this buffer. Also note the 'Delete' at the end of the loop.
    TBuf<REPCHECKCREATE_KEYBUFLEN> tkey(KRealKeyPrefix);
    TBuf<REPCHECKCREATE_VALBUFLEN> tval(KRealValPrefix);
    TBuf<REPCHECKCREATE_VALBUFLEN> experr(KRealExpectedError);
    TBuf<REPCHECKCREATE_VALBUFLEN> function(KRealFunction);
    TInt keyslen = tkey.Length();
    TInt valslen = tval.Length();
    TInt experrlen = experr.Length();
    TInt fnlen = function.Length();
    for( TInt count=0 ; count < anset ; count++ )
        {
        // Append the count to the name of the key/value/expected error
        // config items..
        tkey.AppendNum(count);            // e.g "RealKey21"
        tval.AppendNum(count);            // e.g "RealVal47"
        experr.AppendNum(count);            // e.g "RealExpErr204"
        function.AppendNum(count);            // e.g "Realfn204"

        // Get the required function config item. We fetch this as a string
        // and convert it..
        TInt fn;
        // Where to put the string from the config file...
        TBuf<REPCHECKCREATE_MAXSTRINGLEN> fnbuf;
        TPtrC fns(fnbuf);
        if(GetStringFromConfig(ConfigSection(), function, fns) )
            fn = FuncToNum(fns);
        else
            fn = fn_set;

        // Get the expected error config item. We fetch this as a string
        // and convert it..
        TInt err;
        // Where to put the string from the config file...
        TBuf<REPCHECKCREATE_MAXSTRINGLEN> errbuf;
        TPtrC errs(errbuf);

        if(GetStringFromConfig(ConfigSection(), experr, errs) )
            err = ErrorToNum(errs);
        else
            err = KErrNone;

        // Get the key. It MUST be defined in the config file.
        TInt r=0, key;
        TInt bRet = GetHexFromConfig(ConfigSection(), tkey, key );
        if(bRet != 1)
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF2(_L("Failed to get key 0x%x"), key );
            INFO_PRINTF1(HTML_RED_OFF);
            SetTestStepResult(EFail);
            continue;
            }

        // Where to put the string from the config file...
        TBuf<REPCHECKCREATE_MAXSTRINGLEN> setbuf;
        TPtrC set(setbuf);

        // Get it from the config file...
        bRet = GetStringFromConfig(ConfigSection(), tval, set );
        TLex tlset(set);
        TReal setreal;
        r = tlset.Val(setreal);
        if((bRet != 1) && ( (fn==fn_set) || (fn==fn_create) ) )
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF2(_L("FAIL: Failed to get config realval%d"), count );
            INFO_PRINTF1(HTML_RED_OFF);
            SetTestStepResult(EFail);
            continue;
            }
        else if((r!=KErrNone) && ( (fn==fn_set) || (fn==fn_create) ) )
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF2(_L("Failed to convert realval%d to real"), count);
            INFO_PRINTF1(HTML_RED_OFF);
            SetTestStepResult(EFail);
            continue;
            }
        TReal valr=-1.0;
        TInt rr;
        switch(fn)
            {
            case fn_set: 
                INFO_PRINTF3(_L("Setting real, key 0x%x, to %f "), key, setreal );
                r = arepository->Set( key, setreal );
                break;
            case fn_create: 
                INFO_PRINTF3(_L("Creating real, key 0x%x, to %f "), key, setreal);
                r = arepository->Create( key, setreal );
                break;
            case fn_delete: 
                INFO_PRINTF2(_L("Deleting real, key 0x%x"), key );
                r = arepository->Delete( key );
                rr = arepository->Get( key, valr );
                if( rr != KErrNotFound )
                    {
                    INFO_PRINTF3(_L("Get following delete gave rc %d, val: %f"), rr, valr);
                    }
                break;
            case fn_reset: 
                INFO_PRINTF2(_L("Resetting real, key 0x%x"), key );
                r = arepository->Reset( key );
                break;
            default:
                INFO_PRINTF1(HTML_RED);
                ERR_PRINTF3(_L("FAIL: Unknown function %d for real setting %d"), fn, count );
                INFO_PRINTF1(HTML_RED_OFF);
                r = KErrNone;
                SetTestStepResult(EFail);
                break;
            }

        if(r!=err)
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF3(_L("Wrong error code. Got %d, expected %d"), r, err);
            INFO_PRINTF1(HTML_RED_OFF);
            SetTestStepResult(EFail);
            }
        else
            {
            INFO_PRINTF2(_L("Got expected error code %d"), r );
            }

        // Restore the key and value names (i.e strip the digits from the end)
        tkey.Delete(keyslen, REPCHECKCREATE_KEYBUFLEN);
        tval.Delete(valslen, REPCHECKCREATE_VALBUFLEN);
        experr.Delete(experrlen, REPCHECKCREATE_VALBUFLEN);
        function.Delete(fnlen, REPCHECKCREATE_VALBUFLEN);
        }
    // Should we return something?
    return;
    }

/**
 * @return - nothing
 * Modify repository settings as defined in the config file. For example,
 * we might want to set 37 strings in the repository. Their keys will be
 * 'StringKey0' through 'StringKey36'. 
 */
void CRepositoryFnStep::ModifyRepositoryStringSettings(CRepository *arepository, TInt anset )
    {
    // Careful if the prefix here changes, we don't want to
    // blow this buffer. Also note the 'Delete' at the end of the loop.
    TBuf<REPCHECKCREATE_KEYBUFLEN> tkey(KStringKeyPrefix);
    TBuf<REPCHECKCREATE_VALBUFLEN> tval(KStringValPrefix);
    TBuf<REPCHECKCREATE_VALBUFLEN> experr(KStringExpectedError);
    TBuf<REPCHECKCREATE_VALBUFLEN> function(KStringFunction);
    TInt keyslen = tkey.Length();
    TInt valslen = tval.Length();
    TInt experrlen = experr.Length();
    TInt fnlen = function.Length();
    for( TInt count=0 ; count < anset ; count++ )
        {
        // Append the count to the name of the key/value/expected error
        // config items..
        tkey.AppendNum(count);            // e.g "StringKey21"
        tval.AppendNum(count);            // e.g "StringVal47"
        experr.AppendNum(count);            // e.g "StringExpErr204"
        function.AppendNum(count);            // e.g "Stringfn204"

        // Get the required function config item. We fetch this as a string
        // and convert it..
        TInt fn;
        // Where to put the string from the config file...
        TBuf<REPCHECKCREATE_MAXSTRINGLEN> fnbuf;
        TPtrC fns(fnbuf);
        if(GetStringFromConfig(ConfigSection(), function, fns) )
            fn = FuncToNum(fns);
        else
            fn = fn_set;

        // Get the expected error config item. We fetch this as a string
        // and convert it..
        TInt err;
        // Where to put the string from the config file...
        TBuf<REPCHECKCREATE_MAXSTRINGLEN> errbuf;
        TPtrC errs(errbuf);

        if(GetStringFromConfig(ConfigSection(), experr, errs) )
            err = ErrorToNum(errs);
        else
            err = KErrNone;

        // Get the key. It MUST be defined in the config file.
        TInt r=0, key;
        TInt bRet = GetHexFromConfig(ConfigSection(), tkey, key );
        if(bRet != 1)
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF2(_L("Failed to get string key %S"), &tkey );
            INFO_PRINTF1(HTML_RED_OFF);
            SetTestStepResult(EFail);
            continue;
            }

        // Where to put the string from the config file...
        TBuf<REPCHECKCREATE_MAXSTRINGLEN> setbuf;
        TPtrC set(setbuf);

        // Get it from the config file...
        bRet = GetStringFromConfig(ConfigSection(), tval, set );
        if((bRet != 1) && ( (fn==fn_set) || (fn==fn_create) ) )
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF2(_L("FAIL: Failed to get config stringval%d"), count );
            INFO_PRINTF1(HTML_RED_OFF);
            SetTestStepResult(EFail);
            continue;
            }

	TInt rr;
        TBuf<REPCHECKCREATE_MAXSTRINGLEN> vals;
        switch(fn)
        {
            case fn_set: 
                INFO_PRINTF3(_L("Setting string, key 0x%x, to %S "), key, &set );
                r = arepository->Set( key, set );
                break;
            case fn_create: 
                INFO_PRINTF3(_L("Creating string, key 0x%x, to %S "), key, &set );
                r = arepository->Create( key, set );
                break;
            case fn_delete: 
                INFO_PRINTF2(_L("Deleting string, key 0x%x"), key );
                r = arepository->Delete( key );
                vals.Delete(0, REPCHECKCREATE_KEYBUFLEN);
                rr = arepository->Get( key, vals );
                if( rr != KErrNotFound )
                    {
                    INFO_PRINTF3(_L("Get following delete gave rc %d, val: %S"), rr, &vals);
                    }
                break;
            case fn_reset: 
                INFO_PRINTF2(_L("Resetting string, key 0x%x"), key );
                r = arepository->Reset( key );
                break;
            default:
                INFO_PRINTF1(HTML_RED);
                ERR_PRINTF3(_L("FAIL: Unknown function %d for string setting %d"), fn, count );
                INFO_PRINTF1(HTML_RED_OFF);
                r = KErrNone;
                SetTestStepResult(EFail);
                break;
            }

        if(r!=err)
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF3(_L("Wrong error code. Got %d, expected %d"), r, err);
            INFO_PRINTF1(HTML_RED_OFF);
            SetTestStepResult(EFail);
            }
        else
            {
            INFO_PRINTF2(_L("Got expected error code %d"), r );
            }

        // Restore the key and value names (i.e strip the digits from the end)
        tkey.Delete(keyslen, REPCHECKCREATE_KEYBUFLEN);
        tval.Delete(valslen, REPCHECKCREATE_VALBUFLEN);
        experr.Delete(experrlen, REPCHECKCREATE_VALBUFLEN);
        function.Delete(fnlen, REPCHECKCREATE_VALBUFLEN);
        }
    // Should we return something?
    return;
    }

// Convert an error code that's been read as a string to the equivalent
// integer value.
int CRepositoryFnStep::ErrorToNum(TDesC& ads)
    {
    if(ads == RFN_KErrNone)
        {
        return KErrNone;
        }
    if(ads == RFN_KErrNotFound)
        {
        return KErrNotFound;
        }
    if(ads == RFN_KErrNotSupported)
        {
        return KErrNotSupported;
        }
    if(ads == RFN_KErrAbort)
        {
        return KErrAbort;
        }
    if(ads == RFN_KErrLocked)
        {
        return KErrLocked;
        }
    if(ads == RFN_KErrOverflow)
        {
        return KErrOverflow;
        }
    if(ads == RFN_KErrPermissionDenied)
        {
        return KErrPermissionDenied;
        }
    if(ads == RFN_KErrAlreadyExists)
        {
        return KErrAlreadyExists;
        }
    if(ads == RFN_KErrArgument)
        {
        return KErrArgument;
        }

    return KErrUnknown;
    }

// Convert a required function into the appropriate enum.
int CRepositoryFnStep::FuncToNum(TDesC &ads)
    {
    if(ads == RFN_Set)
        {
        return fn_set;
        }
    if(ads == RFN_Create)
        {
        return fn_create;
        }
    if(ads == RFN_Delete)
        {
        return fn_delete;
        }
    if(ads == RFN_Reset)
        {
        return fn_reset;
        }
    return unknown_func;
    }
