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
 * @file
 */
#include "repositorycheck.h"
#include "Te_centrep_SWISuiteDefs.h"
#include "common.h"

CRepositoryCheckStep::~CRepositoryCheckStep()
/**
 * Destructor
 */
    {
    }

CRepositoryCheckStep::CRepositoryCheckStep()
/**
 * Constructor
 */
    {
    // **MUST** call SetTestStepName in the constructor as the controlling
    // framework uses the test step name immediately following construction to set
    // up the step's unique logging ID.
    SetTestStepName(KRepositoryCheck);
    }

TVerdict CRepositoryCheckStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL()
 * did not leave. That being the case, the current test result value will be
 * EPass.
 * Check for the absence and presence of repository settings as specified in
 * the config file.
 */
    {
    SetTestStepResult(EPass);    

    // See if we just want to wait for a bit. Bit of a strange place
    // to put this, but convenient..
    TInt pause=0;
    TInt bRet = GetIntFromConfig(ConfigSection(), KPause, pause);
    if(bRet == 1)
        {
        pause *= ipauseincrement;
        User::After(KDefaultEvictionTimeout + pause);
        }

    // Get the key for the repository we're interested in. Return if absent.
    TInt reposkey=0;
    bRet = GetHexFromConfig(ConfigSection(), KRepositoryKey, reposkey );
    if(bRet!=1)
        {
        SetTestStepResult(EFail);    
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF1(_L("Repository key not specified in .ini file."));
        INFO_PRINTF1(HTML_RED_OFF);
        return TestStepResult();
        }
    INFO_PRINTF2(_L("Repository key: 0x%x"), reposkey );

    // Get the flag the indicates whether this repository should exist.
    // We might be checking for absence.. If this isn't given, assume the
    // repository is expected to be present.
    TBool repwanted=ETrue;
    bRet = GetIntFromConfig(ConfigSection(),KRepositoryPresent, repwanted);
    if(bRet != 1) repwanted = ETrue;
    INFO_PRINTF2(_L("Repository wanted: %d"), repwanted );

    // Create the repository object.
    // We have to TRAP here for the case where the repository does not
    // exist. This may be wanted or not.
    TInt leavecode;
    CRepository* repository=NULL;
    TRAP(leavecode, repository = CRepository::NewL(TUid::Uid(reposkey)) );
    if(repository != NULL) CleanupStack::PushL(repository);

    // This is not necessarily an error..
    if(leavecode != KErrNone)
        INFO_PRINTF2(_L("Leave code: %d"), leavecode );
    if(repository == NULL) 
        INFO_PRINTF1(_L("Repository is NULL"));
    else
        INFO_PRINTF1(_L("Repository is not NULL"));
    if(leavecode == KErrAlreadyExists)
        {
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF1(_L("Leavecode is KErrAlreadyExists. Perhaps there's a duplicate key in a keyspace file?"));
        INFO_PRINTF1(HTML_RED_OFF);
        SetTestStepResult(EFail);    
        }

    if(repwanted)
        {
        if(leavecode == KErrNotFound)
            {
            SetTestStepResult(EFail);    
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF1(_L("Required Repository does not exist"));
            INFO_PRINTF1(HTML_RED_OFF);

            return TestStepResult();
            }
        else if(leavecode == KErrNone)
            {
            INFO_PRINTF1(_L("Required Repository exists (correctly)."));
        // Drop through.
            }
        else
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF2(_L("Unexpected leave '%d' creating repository."), leavecode);
            INFO_PRINTF1(HTML_RED_OFF);

            if(repository != NULL)
                CleanupStack::PopAndDestroy(repository);
            SetTestStepResult(EFail);
            return TestStepResult();
            }
        }
    else // !repwanted
        {
        if(leavecode == KErrNone)
            {
            SetTestStepResult(EFail);
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF1(_L("Repository exists when it should not."));
            INFO_PRINTF1(HTML_RED_OFF);
            CleanupStack::PopAndDestroy(repository);
            }
        else
            {
            INFO_PRINTF1(_L("Repository is absent (correctly)."));
            // The following should not be possible.
            if(leavecode != KErrNotFound)
                {
                INFO_PRINTF1(HTML_RED);
                ERR_PRINTF2(_L("Failure to create repository, but error code is not KErrNotFound! Err=%d"), leavecode);
                SetTestStepResult(EFail);
                INFO_PRINTF1(HTML_RED_OFF);
                }
            }
        return TestStepResult();
        }
    // The only way to this point is to have a repository that is wanted.
    // Must remember to pop the cleanupstack later!

    // Look for the expected total number of settings in this repository.
    TInt expectednumsettings;
    bRet = GetIntFromConfig(ConfigSection(), KExpectedNumSettings, expectednumsettings );
    // If bRet is not set then the user isn't interested in the number of
    // settings, which is ok.
    if(bRet == 1)
        {
        RArray<TUint32> foundIds;
        TInt r = KErrNone;
        leavecode = KErrNone;
        INFO_PRINTF2(_L("Looking for number of settings: %d"), expectednumsettings);
        TRAP(leavecode, r = repository->FindL(0, 0, foundIds));
        if(leavecode != KErrNone)
            r = leavecode;
        if(r!=KErrNone)
            SetTestStepResult(EFail);
        INFO_PRINTF2(_L("Number of repository ids: %d"), foundIds.Count());
        if(foundIds.Count() != expectednumsettings)
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF2(_L("Failure: Expected number of keys: %d"), expectednumsettings);
            INFO_PRINTF1(HTML_RED_OFF);
            SetTestStepResult(EFail);
            }
        }

    // Look for settings in the repository which should be absent.
    TInt nabsent;
    bRet = GetIntFromConfig(ConfigSection(), KRepositoryAbsent, nabsent );
    // If bRet is not set then the number of absent settings hasn't been
    // specified, which is ok.
    if(bRet == 1)
        {
        // Careful if the prefix here changes, we don't want to
        // blow this buffer. Also note the 'Delete' a few lines hence.
        TBuf<REPCHECKCREATE_ABSBUFLEN> abs(KAbsentPrefix);
        TInt len = abs.Length();
        for( TInt count=0 ; count < nabsent ; count++ )
            {
            abs.AppendNum(count);            // e.g "Absent27"
            TInt absentkey;
            // All keys from "Absent0" through "Absent<N-1>" MUST be defined
            // in the config file...
            bRet = GetHexFromConfig(ConfigSection(), abs, absentkey );
            if(bRet != 1)
                {
                INFO_PRINTF1(HTML_RED);
                ERR_PRINTF1(_L("Failed to get absent key from config file."));
                INFO_PRINTF1(HTML_RED_OFF);
                SetTestStepResult(EFail);
                }

            // .. but must not exist in the repository.
            TInt vali;
            TReal valr;
            TBuf<REPCHECKCREATE_MAXSTRINGLEN> vals;
            TInt ri = repository->Get( absentkey, vali );
            TInt rr = repository->Get( absentkey, valr );
            TInt rs = repository->Get( absentkey, vals );
            if( (ri==KErrNone) || (rr==KErrNone) || (rs==KErrNone))
                {
                INFO_PRINTF1(HTML_RED);
                ERR_PRINTF2(_L("FAILURE: Repository has key 0x%x which should be absent."), absentkey );
                if(!ri) ERR_PRINTF3(_L("Intrc: %d, Value: 0x%x"), ri, vali );
                if(!rr) ERR_PRINTF3(_L("Realrc: %d, Value: %f"), rr, valr );
                if(!rs) ERR_PRINTF3(_L("Stringrc: %d, Value: %S"), rs, &vals );
                INFO_PRINTF1(HTML_RED_OFF);
                SetTestStepResult(EFail);
                }
            abs.Delete(len, REPCHECKCREATE_ABSBUFLEN);  // Strip the number..
            }
        }

    // Look for settings in the repository which should be present. Integers
    // first..
    TInt nints;
    bRet = GetIntFromConfig(ConfigSection(), KRepositoryNumInts, nints );
    if(bRet == 1)
        {
        CheckRepositoryIntegerSettings(repository, nints);
        }

    // Look for settings in the repository which should be present. Now reals.
    TInt nreals;
    bRet = GetIntFromConfig(ConfigSection(), KRepositoryNumReals, nreals );
    if(bRet == 1)
        {
        CheckRepositoryRealSettings(repository, nreals);
        }

    // Look for settings in the repository which should be present. Now strings.
    TInt nstrings;
    bRet = GetIntFromConfig(ConfigSection(), KRepositoryNumStrings, nstrings );
    if(bRet == 1)
        {
        CheckRepositoryStringSettings(repository, nstrings);
        }

    CleanupStack::PopAndDestroy(repository);
    
    return TestStepResult();
    }



/**
 * @return - nothing
 * Check repository settings as defined in the config file. For example,
 * we might expect 37 integers in the repository. Their keys will be
 * 'IntKey0' through 'IntKey36'. We'll expect to see hex values for
 * all of these names (or we will LEAVE). Those keys will be expected
 * to exist in the repository and have values 'IntVal0' through 'IntVal36'
 * (or whatever).
 */
void CRepositoryCheckStep::CheckRepositoryIntegerSettings(CRepository *arepository, TInt ancheck)
    {
    // Careful if the prefix here changes, we don't want to
    // blow this buffer. Also note the 'Delete' at the end of the loop.
    TBuf<REPCHECKCREATE_KEYBUFLEN> tkey(KIntKeyPrefix);
    TBuf<REPCHECKCREATE_VALBUFLEN> tval(KIntValPrefix);
    TBuf<REPCHECKCREATE_VALBUFLEN> tmeta(KIntMetaPrefix);
	TInt keyslen = tkey.Length();
    TInt valslen = tval.Length();
    TInt metalen = tmeta.Length();
    for( TInt count=0 ; count < ancheck ; count++ )
        {
        tkey.AppendNum(count);			// e.g "IntKey21"
        tval.AppendNum(count);			// e.g "IntVal47"
        tmeta.AppendNum(count);			// e.g "IntMeta47"
        TInt r=0;
        TInt key;
        TInt bRet = GetHexFromConfig(ConfigSection(), tkey, key );
        if(bRet != 1)
            {
            ERR_PRINTF2(_L("Failed to get key%d"), count );
            SetTestStepResult(EFail);
            }
        TInt expectedval; 
        bRet = GetHexFromConfig(ConfigSection(), tval, expectedval );
        if(bRet!=1)
            {
            ERR_PRINTF2(_L("Failed to get config intval%d"), count );
            SetTestStepResult(EFail);
            }

        TInt intval;
        r = arepository->Get( key, intval );
        if(r!=KErrNone)
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF2(_L("Repository doesn't have IntKey 0x%x"), key );
            INFO_PRINTF1(HTML_RED_OFF);
            SetTestStepResult(EFail);
            }
        else if(intval!=expectedval)
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF2(_L("Integer match failure, int%d"), count);
            INFO_PRINTF2(_L(" - expectedint 0x%x"), expectedval );
            INFO_PRINTF2(_L(" - actualint 0x%x"), intval );
            INFO_PRINTF1(HTML_RED_OFF);
            SetTestStepResult(EFail);
            }

		TBuf<REPCHECKCREATE_MAXSTRINGLEN> metabuf;
		TPtrC meta(metabuf);
		TUint32 expectedmeta;
        bRet = GetStringFromConfig(ConfigSection(), tmeta, meta );
		if(bRet == 1)
			{
			TLex lex(meta);
			TInt err = lex.Val(expectedmeta, EHex);
			if (err)
				{
				INFO_PRINTF1(HTML_RED);
				ERR_PRINTF2(_L("Meta value couldn't be converted to TUint32 for IntKey 0x%x"), key );
				INFO_PRINTF1(HTML_RED_OFF);
				SetTestStepResult(EFail);				
				}
			
			TUint32 actualmeta;
			r = arepository->GetMeta( key, actualmeta );
			if(r!=KErrNone)
				{
				INFO_PRINTF1(HTML_RED);
				ERR_PRINTF2(_L("MetData doesn't exist for key 0x%x"), key );
				INFO_PRINTF1(HTML_RED_OFF);
				SetTestStepResult(EFail);
				}
			else if(actualmeta!=expectedmeta)
				{
				INFO_PRINTF1(HTML_RED);
				ERR_PRINTF2(_L("Meta match failure, int%d"), count);
				INFO_PRINTF2(_L(" - expectedmeta 0x%x"), expectedmeta );
				INFO_PRINTF2(_L(" - actualmeta 0x%x"), actualmeta );
				INFO_PRINTF1(HTML_RED_OFF);
				SetTestStepResult(EFail);
				}
			}

        // Restore the key and value names (i.e strip the digits from the end)
        tkey.Delete(keyslen, REPCHECKCREATE_KEYBUFLEN);
        tval.Delete(valslen, REPCHECKCREATE_VALBUFLEN);
        tmeta.Delete(metalen, REPCHECKCREATE_VALBUFLEN);
        }
    // Should we return something?
    return;
    }

/**
 * @return - nothing
 * Check repository settings as defined in the config file. For example,
 * we might expect 37 reals in the repository. Their keys will be
 * 'RealKey0' through 'RealKey36'. We'll expect to see hex values for
 * all of these names. Those keys will be expected
 * to exist in the repository and have values 'RealVal0' through 'RealVal36'
 * (or whatever).
 */
void CRepositoryCheckStep::CheckRepositoryRealSettings(CRepository *arepository,
    TInt ancheck)
    {
    // Careful if the prefix here changes, we don't want to
    // blow this buffer. Also note the 'Delete' at the end of the loop.
    TBuf<REPCHECKCREATE_KEYBUFLEN> tkey(KRealKeyPrefix);
    TBuf<REPCHECKCREATE_VALBUFLEN> tval(KRealValPrefix);
    TBuf<REPCHECKCREATE_VALBUFLEN> tmeta(KRealMetaPrefix);
    TInt keyslen = tkey.Length();
    TInt valslen = tval.Length();
    TInt metalen = tmeta.Length();
    for( TInt count=0 ; count < ancheck ; count++ )
        {
        tkey.AppendNum(count);            // e.g "RealKey21"
        tval.AppendNum(count);            // e.g "RealVal47"
        tmeta.AppendNum(count);			  // e.g "RealMeta47"
        TInt r=0;
        TInt key;
        TInt bRet = GetHexFromConfig(ConfigSection(), tkey, key );
        if(bRet != 1)
            {
            ERR_PRINTF2(_L("Failed to get key%d"), count );
            SetTestStepResult(EFail);
            }
        TBuf<REPCHECKCREATE_MAXSTRINGLEN> expectedbuf;
        TPtrC expected(expectedbuf);

        // Get the real stored in the repository.
        TReal rval;
        r = arepository->Get( key, rval );
        if(r!=KErrNone)
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF2(_L("Repository doesn't have RealKey 0x%x"), key );
            INFO_PRINTF1(HTML_RED_OFF);
            SetTestStepResult(EFail);
            }
        else
            {
            // Unfortunately there is no 'GetRealFromConfig'
            // so we must fetch the real number as a 
            // string.
            bRet = GetStringFromConfig(ConfigSection(), tval, expected );
            if(bRet!=1)
                {
                INFO_PRINTF1(HTML_RED);
                ERR_PRINTF2(_L("Config doesn't have RealVal%d"), count );
                INFO_PRINTF1(HTML_RED_OFF);
                SetTestStepResult(EFail);
                }
            else
                {
                TLex tlexp(expected);
                TReal expectedreal;
                r = tlexp.Val(expectedreal);
    
                // This is of course a little problematic -
                // obviously one should never compare reals.
                if(rval!=expectedreal)
                    {
                    INFO_PRINTF1(HTML_RED);
                    ERR_PRINTF4(_L("Real match failure, key 0x%x, got %f, expected %f"), key, rval, expectedreal);
                    INFO_PRINTF1(HTML_RED_OFF);
                    SetTestStepResult(EFail);
                    }
                }
            }

		TBuf<REPCHECKCREATE_MAXSTRINGLEN> metabuf;
		TPtrC meta(metabuf);
		TUint32 expectedmeta;
        bRet = GetStringFromConfig(ConfigSection(), tmeta, meta );
        if(bRet == 1)
			{
			TLex lex(meta);
			TInt err = lex.Val(expectedmeta, EHex);
			if (err)
				{
				INFO_PRINTF1(HTML_RED);
				ERR_PRINTF2(_L("Meta value couldn't be converted to TUint32 for IntKey 0x%x"), key );
				INFO_PRINTF1(HTML_RED_OFF);
				SetTestStepResult(EFail);				
				}
			
			TUint32 actualmeta;
			r = arepository->GetMeta( key, actualmeta );
			if(r!=KErrNone)
				{
				INFO_PRINTF1(HTML_RED);
				ERR_PRINTF2(_L("MetData doesn't exist for key 0x%x"), key );
				INFO_PRINTF1(HTML_RED_OFF);
				SetTestStepResult(EFail);
				}
			else if(actualmeta!=expectedmeta)
				{
				INFO_PRINTF1(HTML_RED);
				ERR_PRINTF2(_L("Meta match failure, int%d"), count);
				INFO_PRINTF2(_L(" - expectedmeta 0x%x"), expectedmeta );
				INFO_PRINTF2(_L(" - actualmeta 0x%x"), actualmeta );
				INFO_PRINTF1(HTML_RED_OFF);
				SetTestStepResult(EFail);
				}
			}

        // Restore the key and value names (i.e strip the digits from the end)
        tkey.Delete(keyslen, REPCHECKCREATE_KEYBUFLEN);
        tval.Delete(valslen, REPCHECKCREATE_VALBUFLEN);
        tmeta.Delete(metalen, REPCHECKCREATE_VALBUFLEN);
        }
    // Should we return something?
    return;
    }

/**
 * @return - nothing
 * Check repository settings as defined in the config file. For example,
 * we might expect 37 strings in the repository. Their keys will be
 * 'StringKey0' through 'StringKey36'. We'll expect to see hex values for
 * all of these names (or we will LEAVE). Those keys will be expected
 * to exist in the repository and have values 'StringVal0' through 'StringVal36'
 * (or whatever).
 */
void CRepositoryCheckStep::CheckRepositoryStringSettings(CRepository *arepository, TInt ancheck)
    {
    // Careful if the prefix here changes, we don't want to
    // blow this buffer. Also note the 'Delete' at the end of the loop.
    TBuf<REPCHECKCREATE_KEYBUFLEN> tkey(KStringKeyPrefix);
    TBuf<REPCHECKCREATE_VALBUFLEN> tval(KStringValPrefix);
    TInt keyslen = tkey.Length();
    TInt valslen = tval.Length();
    for( TInt count=0 ; count < ancheck ; count++ )
        {
        tkey.AppendNum(count);            // e.g "StringKey21"
        tval.AppendNum(count);            // e.g "StringVal47"
        TInt r=0;
        TInt key;
        TInt bRet = GetHexFromConfig(ConfigSection(), tkey, key );
        if(bRet != 1)
            {
            ERR_PRINTF2(_L("Failed to get key%d"), count );
            SetTestStepResult(EFail);
            }
        TBuf<REPCHECKCREATE_MAXSTRINGLEN> expectedbuf;
        TPtrC expected(expectedbuf);

        bRet = GetStringFromConfig(ConfigSection(), tval, expected );
        TBuf<REPCHECKCREATE_MAXSTRINGLEN> sval;
        r = arepository->Get( key, sval );
        if(r!=KErrNone)
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF2(_L("Repository doesn't have StringKey 0x%x"), key );
            INFO_PRINTF1(HTML_RED_OFF);
            SetTestStepResult(EFail);
            }
        else if(sval!=expected)
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF4(_L("String match failure, key 0x%x, got %S, expected %S"), key, &sval, &expected);
            INFO_PRINTF1(HTML_RED_OFF);
            SetTestStepResult(EFail);
            }

        // Restore the key and value names (i.e strip the digits from the end)
        tkey.Delete(keyslen, REPCHECKCREATE_KEYBUFLEN);
        tval.Delete(valslen, REPCHECKCREATE_VALBUFLEN);
        }
    // Should we return something?
    return;
    }
