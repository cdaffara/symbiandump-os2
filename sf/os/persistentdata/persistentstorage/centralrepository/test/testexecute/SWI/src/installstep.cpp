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
// SWIS test step implementation
// 
//

/**
 @file
*/

#include "installstep.h"
#include "common.h"
#include "tui.h"
#include <e32std.h>
#include <test/testexecutelog.h>
#include <swi/launcher.h>
#include <bautils.h>
#include "swi/sisregistrysession.h"
#include "swi/sisregistrypackage.h"
#include "swi/sisregistryentry.h"
#include "cleanuputils.h"

using namespace Swi;

//
// CinstallStep
//

CinstallStep::~CinstallStep()
    {
    delete iUi;
    }
    

CinstallStep::CinstallStep(TInstallType aInstallType, TBool aDoCancelTest)
    : iInstallType(aInstallType), iDoCancelTest(aDoCancelTest)
    {
    // Call base class method to set up the human readable name for logging
    
    switch (aInstallType)
        {
        case EUseFileHandle:
            SetTestStepName(KInstallFHStep);
            break;
        
        case EUseMemory:
            SetTestStepName(KInstallMemStep);
            break;

        case EUseFileName:
            SetTestStepName(KInstallStep);
            break;
        
        case EUseCAF:
            SetTestStepName(KInstallCAFStep);
            break;

        case EUseOpenFileName:
            SetTestStepName(KInstallOpenFileStep);
            break;
            
        case ECheckExitValue:
            SetTestStepName(KCheckedInstallStep);
            break;     
            
        }
    }

/**
 * Override of base class virtual. Prepares for the test run of SWIS
 * @return TVerdict code
 */
TVerdict CinstallStep::doTestStepPreambleL()
    {
    SetTestStepResult(EPass);
    // get step parameters
    TPtrC str;
    if (!GetStringFromConfig(ConfigSection(), _L("sis"), str))
        {
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF1(_L("FAIL: Missing SIS file name"));
        INFO_PRINTF1(HTML_RED_OFF);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    iSisFileName.Copy(str);
    INFO_PRINTF2(_L("Installing '%S'"), &iSisFileName);
            
    // create UI handler and populate the answers from XML file
    iUi = new(ELeave) TUI();
    return TestStepResult();
    }

/**
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 * @return TVerdict code
 */
TVerdict CinstallStep::doTestStepL()
    {
    INFO_PRINTF1(KInstallStep);

    // Try to set up a repository object, we'll need this if any
    // NotifyRequests are listed. Only open the repository if there are
    // notifys.
    CRepository *repository = NULL;
    TInt reposkey=0, notifies;
    TInt bRet = GetIntFromConfig(ConfigSection(), KNumNotifies, notifies);
    if(bRet != 1 )notifies=0;
    bRet = GetHexFromConfig(ConfigSection(), KRepositoryKey, reposkey );
    if((bRet==1) && notifies)
        {
        TInt leavecode;
        TRAP(leavecode, repository = CRepository::NewL(TUid::Uid(reposkey)) );
        if(repository != NULL) CleanupStack::PushL(repository);
        }

    // Before we attempt to install anything, look for any NotifyRequests
    // on Repository settings...
    TRequestStatus treqs[TEFSWICENTREPMAXNOTIFYREQS];
    if( !SetupNotifyRequestsL(repository, notifies, treqs) )
        {
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF1( _L("Failure within SetupNotifyRequestsL") );
        INFO_PRINTF1(HTML_RED_OFF);
        SetTestStepResult(EFail);
        // Don't stop the install by returning...
    }

    // First find out if the install step is supposed to be successful.
    TInt insterr=KErrNone;
    bRet = GetIntFromConfig(ConfigSection(), KExpectedError, insterr );
    if(bRet!=1) insterr=KErrNone;

    // Get an array of timestamps from the affected files.
    RArray<TTime> tstamps;
    SetCheckTimeStampsL(tstamps);

    // Wait a fraction over two seconds. This is necessary because we may just
    // have been messing about with repository settings and if the install
    // happens really quickly we might not see a datestamp change (esp. on
    // hardware where the timestamp granularity is poor).
    User::After(2100000);

    // launch the installation
    CInstallPrefs* prefs = CInstallPrefs::NewLC();
    TInt err = DoInstallL(*prefs);        

    // Wait for the timestamps to change. This should prevent any race
    // with the Central Repository Server - i.e. we won't start checking
    // repository settings until the persists file has been written and
    // therefore (hopefully) the server is ready to answer queries.
    SetCheckTimeStampsL(tstamps);
    tstamps.Close();
    
    // Expected error?
    if(insterr != err)
        {
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3( _L("Installation return wrong error code, expected %d, got %d."), insterr, err );
        INFO_PRINTF1(HTML_RED_OFF);
        SetTestStepResult(EFail);
        }
   else
        {
        INFO_PRINTF2( _L("Installation error code %d (expected)."), err );
        }
    // If we had any notify requests do a 'WaitForRequest' on each of them.
    // Don't even bother if repository isn't set up or if the return code
    // was not what was expected.
    if( (repository != NULL) && (insterr == err))
    	{
		if (WaitOnNotifyRequests(treqs, notifies) == EFail)
	    	{
	    	INFO_PRINTF1(HTML_RED);
	        ERR_PRINTF1( _L("Failure within Install WaitOnNotifyRequests") );
	        INFO_PRINTF1(HTML_RED_OFF);
	        SetTestStepResult(EFail);
	    	}
    	}

    CleanupStack::PopAndDestroy(prefs);
    if(repository != NULL) CleanupStack::PopAndDestroy(repository);
    return TestStepResult();
    }

TInt CinstallStep::DoInstallL(CInstallPrefs& aInstallPrefs)
    {
    TInt err=KErrNone;
    RFs fs;
    RFile file;
    switch (iInstallType)
        {
        case EUseFileName:
            err = Launcher::Install(*iUi, iSisFileName, aInstallPrefs);
            INFO_PRINTF2(_L("EUseFileName: Install return code was %d"), err);
            return err;

        case EUseOpenFileName:
        // open the file as a shared for readers only
            {
            User::LeaveIfError(fs.Connect());
            fs.ShareProtected();
            CleanupClosePushL(fs);
            RFile file;
            User::LeaveIfError(file.Open(fs, iSisFileName, EFileShareReadersOnly));
            CleanupClosePushL(file);
            TInt error = Launcher::Install(*iUi, iSisFileName, aInstallPrefs);
            CleanupStack::PopAndDestroy(2, &fs);
            return error;            
            }

        case EUseFileHandle:
            {
            RFs fs;
            User::LeaveIfError(fs.Connect());
            fs.ShareProtected();
            CleanupClosePushL(fs);
            RFile file;
            User::LeaveIfError(file.Open(fs, iSisFileName, 0));
            CleanupClosePushL(file);
            TInt error=Launcher::Install(*iUi, file, aInstallPrefs);
            CleanupStack::PopAndDestroy(2, &fs);
            return error;
            }

        case ECheckExitValue:
            {
            // This test case does an install and checks for pass or failure
            // TInt err = Launcher::Install(*iUi, iSisFileName, *prefs);
            TInt err = Launcher::Install(*iUi, iSisFileName, aInstallPrefs);
            INFO_PRINTF2(_L("Install return code was %d"), err);
            
            TPtrC expected;
            if (!GetStringFromConfig(ConfigSection(), _L("result"), expected))
                {
                return ETestSuiteError;
                }
            else 
                {
                _LIT(KSucess, "sucess");
                _LIT(KFailure, "failure");
                
                TVerdict result;
                
                if (expected.CompareF(KSucess) == 0)
                    {
                    result = (err == KErrNone ? EPass : EFail);
                    }
                else if (expected.CompareF(KFailure) == 0)
                    {
                    result = (err != KErrNone ? EPass : EFail);
                    }
                else 
                    {
                    result = ETestSuiteError;
                    }
                return result;
                }
            }
            // Unreachable.
//          break;    
		default:
	    	INFO_PRINTF1(HTML_RED);
	        ERR_PRINTF2( _L("CinstallStep::DoInstallL iInstallType %d"), iInstallType );
	        INFO_PRINTF1(HTML_RED_OFF);
			break;
        }
    
    // Shouldn't get here
    return KErrGeneral;
    }

/**
 * Override of base class virtual
 * @return TVerdict code
 */
TVerdict CinstallStep::doTestStepPostambleL()
    {
    CheckFilesExistL();
    CheckFilesNonExistL();
    return TestStepResult();
    }


/* *******************************************************************************
 *  Code below was copies from
 *       \master\common\generic\security\swi\test\tuiscriptadaptors\tswisstep.cpp
 *
 *  If there are any problems with this code, it may be worth contacting the
 *  Security team.
 * *******************************************************************************/

//
// CuninstallStep
//

CuninstallStep::~CuninstallStep()
    {
    delete iUi;
    }

CuninstallStep::CuninstallStep(TUninstallType aType, TBool aDoCancelTest)
    : iType(aType), iDoCancelTest(aDoCancelTest)
    {
    // Call base class method to set up the human readable name for logging
    SetTestStepName(KUninstallStep);
    }

    
    
    
/**
 * Override of base class virtual. Prepares for the test run of SWIS
 * @return TVerdict code
 */
TVerdict CuninstallStep::doTestStepPreambleL()
    {
    // get step parameters
    TInt uid=0;

    TPtrC str;

    if (!GetHexFromConfig(ConfigSection(), _L("uid"), uid))
        {
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF1(_L("Missing uid"));
        INFO_PRINTF1(HTML_RED_OFF);
        SetTestStepResult(EFail);
        return TestStepResult();
        }

    iUid.iUid=uid;
        
    if (iType == EByPackage)
        {
        TPtrC vendorName;
        if (!GetStringFromConfig(ConfigSection(), _L("vendorName"), vendorName))
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF1(_L("Missing Vendor Name"));
            INFO_PRINTF1(HTML_RED_OFF);
            SetTestStepResult(EFail);
            return TestStepResult();
            }
        iVendorName.Set(vendorName);
        
        TPtrC packageName;
        if (!GetStringFromConfig(ConfigSection(), _L("packageName"), packageName))
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF1(_L("Missing Package Name"));
            INFO_PRINTF1(HTML_RED_OFF);
            SetTestStepResult(EFail);
            return TestStepResult();
            }
        iPackageName.Set(packageName);
            
        INFO_PRINTF4(_L("Uninstalling %D, %S, %S"), 
            iUid.iUid, &iPackageName, &iVendorName);
        // create UI handler and populate the answers from XML file
        iUi = new(ELeave) TUI;
        
        }
    else if (iType== EByUid)
        {
        INFO_PRINTF2(_L("Uninstalling '%D'"), iUid.iUid);
        // create UI handler and populate the answers from XML file
        iUi = new(ELeave) TUI;
        
        }

    return TestStepResult();
    }

/**
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 * @return TVerdict code
 */

TInt CuninstallStep::DoUninstallL()
    {
    TInt err=0;
    
    INFO_PRINTF1(KUninstallStep);

    if (iType == EByUid)
        {
        // launch the installation
        return Launcher::Uninstall(*iUi, iUid); 
        }
    else if (iType == EByPackage)
        {
        err = -1;
        // Go through list of packages from base package to get augmentations.
        CSisRegistryPackage* uninstallPackage=CSisRegistryPackage::NewLC(iUid, iPackageName, iVendorName);
        INFO_PRINTF3(_L("UnInstalling '%S', '%S'"), 
                &iPackageName, &iVendorName);
        
        RSisRegistrySession registrySession;
        User::LeaveIfError(registrySession.Connect());
        CleanupClosePushL(registrySession);

        RSisRegistryEntry registryEntry;
    
        User::LeaveIfError(registryEntry.Open(registrySession, iUid));
        CleanupClosePushL(registryEntry);

        CSisRegistryPackage* package=registryEntry.PackageL();
 
        if (*package == *uninstallPackage)
            {
            // Delete registrySession, registryEntry so package can be uninstalled
            // otherwise we get KErrInUse
            CleanupStack::PopAndDestroy(2);			// registrySession, registryEntry
            err=Launcher::Uninstall(*iUi, *package); 
			delete package;
            }
        else
            {
            // Delete package, no longer needed
            delete package;
            // check augmenations
            RPointerArray<CSisRegistryPackage> augmentationPackages;
            CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(augmentationPackages);

            registryEntry.AugmentationsL(augmentationPackages);
            for (TInt i=0; i < augmentationPackages.Count(); ++i)
                {
                if (*augmentationPackages[i] == *uninstallPackage)
                    {
                    err=Launcher::Uninstall(*iUi, *augmentationPackages[i]);
                    break;
                    }
                }
            CleanupStack::PopAndDestroy(&augmentationPackages);
            CleanupStack::PopAndDestroy(2);			// registrySession, registryEntry
            }  
        CleanupStack::PopAndDestroy(uninstallPackage);

        if(KErrNone!=err)
        	{
	        INFO_PRINTF1(HTML_RED);
	        ERR_PRINTF2( _L("Failure to uninstall err %d"),err );
	        INFO_PRINTF1(HTML_RED_OFF);
	        SetTestStepResult(EFail);
        	}          
        }
    return err;
    }

/* ******************************************************************************
 *  End of copied code.
 * ******************************************************************************/

TVerdict CuninstallStep::doTestStepL()
    {
    INFO_PRINTF1(KUninstallStep);

    // Try to set up a repository object, we'll need this if any
    // NotifyRequests are listed. Only open the repository if there are
    // notifys.
    CRepository *repository = NULL;
    TInt reposkey=0, notifies;
    TInt bRet = GetIntFromConfig(ConfigSection(), KNumNotifies, notifies);
    if(bRet != 1 )notifies=0;
    bRet = GetHexFromConfig(ConfigSection(), KRepositoryKey, reposkey );
    if((bRet==1) && notifies)
        {
        TInt leavecode;
        TRAP(leavecode, repository = CRepository::NewL(TUid::Uid(reposkey)) );
        if(repository != NULL) CleanupStack::PushL(repository);
        }

    // Before we attempt to uninstall anything, look for any NotifyRequests
    // on Repository settings...
    TRequestStatus treqs[TEFSWICENTREPMAXNOTIFYREQS];
    if( !SetupNotifyRequestsL(repository, notifies, treqs) )
        {
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF1( _L("Failure within uninstall SetupNotifyRequestsL") );
        INFO_PRINTF1(HTML_RED_OFF);
        SetTestStepResult(EFail);
        // Don't stop the uninstall by returning...
    }

    // Get an array of timestamps from the affected files.
    RArray<TTime> tstamps;
    SetCheckTimeStampsL(tstamps);

    // Wait a fraction over two seconds. This is necessary because we may just
    // have been messing about with repository settings and if the install
    // happens really quickly we might not see a datestamp change (esp. on
    // hardware where the timestamp granularity is poor).
    User::After(2100000);

    TInt err = DoUninstallL();
    INFO_PRINTF2(_L("DoUninstallL result = %d"), err);
    
    //If the uninstall fails, retry 
    if(err != KErrNone)
    	{
    	INFO_PRINTF1(_L("Retry DoUninstallL"));
    	User::After(1000000);
        err = DoUninstallL();
        INFO_PRINTF2(_L("DoUninstallL result = %d"), err);
        User::LeaveIfError(err);
    	}

    // Wait for the timestamps to change. This should prevent any race
    // with the Central Repository Server - i.e. we won't start checking
    // repository settings until the persists file has been written and
    // therefore (hopefully) the server is ready to answer queries.
    SetCheckTimeStampsL(tstamps);
    tstamps.Close();
    
    // If we had any notify requests do a 'WaitForRequest' on each of them.
    if (WaitOnNotifyRequests(treqs, notifies) == EFail)
    	{
    	INFO_PRINTF1(HTML_RED);
        ERR_PRINTF1( _L("Failure within uninstall WaitOnNotifyRequests") );
        INFO_PRINTF1(HTML_RED_OFF);
        SetTestStepResult(EFail);
    	}

    if(repository != NULL) CleanupStack::PopAndDestroy(repository);

    return TestStepResult();
    }

/**
 * Override of base class virtual
 * @return TVerdict code
 */
TVerdict CuninstallStep::doTestStepPostambleL()
    {
    CheckFilesExistL();
    CheckFilesNonExistL();
    return TestStepResult();
    }


Cinstallers::~Cinstallers()
/**
 * Destructor
 */
    {
    }

Cinstallers::Cinstallers()
/**
 * Constructor
 */
    {
    }

void Cinstallers::CheckFilesL(const TDesC& aNumEntries,
                                   const TDesC& aEntryBase, TBool aCheckExist)
    {
    TInt numEntries=0;
    TInt nErr=0;

    _LIT(Report_CheckFiles, "CheckFilesL");
    INFO_PRINTF1(Report_CheckFiles);

    RFs fs;
    User::LeaveIfError(fs.Connect());
    fs.ShareProtected();
    CleanupClosePushL(fs);

    if (GetIntFromConfig(ConfigSection(), aNumEntries, numEntries) && numEntries!=0)
        {
        INFO_PRINTF1(Report_CheckFiles);
        TPtrC fname;
        for (TInt i=0; i<numEntries; i++)
            {
            // construct name of the key
            const TInt KKeyBufSize=64;
            TBuf<KKeyBufSize> keyBuf(aEntryBase);
            keyBuf.AppendNum(i);
            
            if (GetStringFromConfig(ConfigSection(), keyBuf, fname))
                {            
                TInt timeout=10000000;	// 10 seconds.
                TInt wait=250000;		// 0.25 seconds.
                // check if the file (doesn't) exist. Give it some time if
                // we see a failure in case there's a race with the
                // (un)installer or Central Repository server (if
                // perhaps we're looking at a persists file).
                if (aCheckExist)
                    {
                    TInt sec=timeout;
                    while (!BaflUtils::FileExists(fs, fname) && (sec>0))
                        {
                        User::After(wait);
                        sec -= wait;
                        };
                    if (!BaflUtils::FileExists(fs, fname))
                        {
                        INFO_PRINTF1(HTML_RED);
                        ERR_PRINTF2(_L("File missing: %S"), &fname);
                        INFO_PRINTF1(HTML_RED_OFF);
                        nErr++;
                        }
                    }
                else
                    {
                    TInt sec=timeout;
                    while (BaflUtils::FileExists(fs, fname) && (sec>0))
                        {
                        User::After(wait);
                        sec -= wait;
                        };
                    if (BaflUtils::FileExists(fs, fname))
                        {
                        INFO_PRINTF1(HTML_RED);
                        ERR_PRINTF2(_L("File exists (but shouldn't): %S"), &fname);
                        INFO_PRINTF1(HTML_RED_OFF);
                        nErr++;
                        }
                    }
                }
            else
                {
                // the string must exist, otherwise the config is invalid
                INFO_PRINTF1(HTML_RED);
                ERR_PRINTF2(_L("Missing file name for key '%S'"), &keyBuf);
                INFO_PRINTF1(HTML_RED_OFF);
                nErr++;
                }
            }
        }

        if (nErr)
            SetTestStepResult(EFail);
        CleanupStack::PopAndDestroy(1, &fs);
    }

void Cinstallers::CheckFilesExistL()
    {
    _LIT(KNumExist, "numexist"); // this specifies how many files to check for
    _LIT(KExistBase, "exist");   // + number (0-based) = file to check for
    CheckFilesL(KNumExist, KExistBase, ETrue);
    }

void Cinstallers::CheckFilesNonExistL()
    {
    _LIT(KNumNonExist, "numnonexist"); // this specifies how many files to check for
    _LIT(KNonExistBase, "nonexist"); // + number (0-based) = file to check for
    CheckFilesL(KNumNonExist, KNonExistBase, EFalse);
    }

TBool Cinstallers::SetupNotifyRequestsL(CRepository *arepository, TInt anots,
                                       TRequestStatus *areqs)
    {
	INFO_PRINTF1(_L("SetupNotifyRequests"));
	INFO_PRINTF2(_L("Number Of Notification (%d)"), anots);

    TBool rc = ETrue;
    if(anots == 0)return rc;
    if(anots > TEFSWICENTREPMAXNOTIFYREQS)
        {
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF2( _L("Too many notify requests. Max %d."),
                                            TEFSWICENTREPMAXNOTIFYREQS);
        INFO_PRINTF1(HTML_RED_OFF);
        anots = TEFSWICENTREPMAXNOTIFYREQS - 1;
        rc = EFalse;
        }

    // Get the repository key...
    if(arepository == NULL)
        {
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF1( _L("Cannot find repository key for notifies."));
        INFO_PRINTF1(HTML_RED_OFF);
        return EFalse;
        }

    const TInt KKeyBufSize=64;
    TBuf<KKeyBufSize> notBuf(KNotifyPrefix);
    TInt nlen = notBuf.Length();

    for (TInt i=0; i<anots; i++)
        {
        // Construct the name of the notify config option.
        notBuf.AppendNum(i);
        TInt notkey;

        if( !GetHexFromConfig(ConfigSection(), notBuf, notkey))
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF2( _L("Cannot get notify key %S"), &notBuf );
            INFO_PRINTF1(HTML_RED_OFF);
            rc = EFalse;
            break;
            }
        INFO_PRINTF2( _L("Notify request on 0x%x"), notkey );
        TInt nreq = arepository->NotifyRequest(notkey, areqs[i]);
        TESTL(areqs[i] == KRequestPending);
        if( nreq != KErrNone )
            {
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF4( _L("Bad NotifyRequest status %d on key 0x%x, count %d"), nreq, notkey, i );
            INFO_PRINTF1(HTML_RED_OFF);
            rc = EFalse;
            break;
            }
        notBuf.Delete(nlen, 64);
        }
    return rc;
    }

// All a bit ugly - wait for any TRequestStatus objects in passed 
// array. Change TEFSWICENTREPMAXNOTIFYREQS to increase the limit.
TBool Cinstallers::WaitOnNotifyRequests(TRequestStatus *areqs, TInt anots)
    {
	RTimer timer;

    INFO_PRINTF1(_L("WaitOnNotifyRequests"));
    // Don't bother warning if this is > 9, that warning will already
    // have been issued.
    if(anots >= TEFSWICENTREPMAXNOTIFYREQS)
        anots = TEFSWICENTREPMAXNOTIFYREQS - 1;
    for( TInt i=0 ; i<anots ; i++ )
        {
        timer.CreateLocal();
        TRequestStatus stats=KRequestPending;
        timer.After(stats, 5 * 1000000);
        INFO_PRINTF2( _L("WaitForRequest: %d"), i );
        // Wait for 1st request to complete (notify) or 5 secs timer
        User::WaitForRequest(areqs[i], stats);
        if (stats == KRequestPending)	// Notify occured before 5 secs elapsed.
        	{
        	INFO_PRINTF1(_L("WaitOnNotify Succeed"));
        	timer.Close();
        	}
        else	// Notify has not completed after 5 secs - something wrong.
        	{
        	INFO_PRINTF1(_L("WaitOnNotify Timeout"));
        	timer.Close();
			return EFail;
        	}
        }
    return EPass;
    }

TInt Cinstallers::GetFileTimeStamp(const TDesC& aFullName, TTime& aTime)
    {
    RFs fsSession;
    TInt err = fsSession.Connect();
    if(err == KErrNone)
        {
        TEntry entry;
        if(fsSession.Entry(aFullName, entry) == KErrNone)
            {
            aTime=entry.iModified;
            }
        fsSession.Close();
        }
    else
        {
        ERR_PRINTF3(_L("Error %d connecting file session. File %S.\n"), err, &aFullName);
        }
    return err;
    }


// If the passed array is empty, fill it with the timestamps of the files
// listed in the config file with the 'KAffectedFile' prefix. If the array
// has content, check the current timestamps against the passed array. If
// the files have an unchanged timestamp (or earlier) generate a warning
// message and a test failure.
void Cinstallers::SetCheckTimeStampsL(RArray<TTime> &atstamps)
    {
    // See if there are 'affected files' defined. These should be files
    // that are introduced or modified by the installation. The idea is
    // to verify that the install has done something.
    TBuf<20> fprefix(KAffectedFile);
    TPtrC filename;
    TInt len = fprefix.Length();
    TBool checking=EFalse;
    if(atstamps.Count()) checking=ETrue;

    // Drop out of this loop when we fail to find an 'AffectFileN' in the
    // config file.
    for( TInt count=0 ; ; count++ )
        {
        fprefix.AppendNum(count);     // e.g AffectedFile22

        // Get the file we're interested in. If none if defined for this
        // count then drop out.
        // Then get the current timestamp on the file. If the file doesn't
        // exist (possible if it's going to be introduced by an install)
        // then the timestamp will be zero.
        TInt bRet = GetStringFromConfig(ConfigSection(), fprefix, filename );
        TTime ttm2(0);
        if(bRet!=1)
            break;
        (void)GetFileTimeStamp( filename, ttm2 );

        TInt maxdelay=2000000; // 2 seconds.
        TInt delayinc=250000; // 0.25 seconds.

        if(checking)
            {
            // Check that the timestamp has changed.
            TTime ttm1 = atstamps[count];

            INFO_PRINTF2(_L("Checking timestamp on %S..."), &filename );
            // If it hasn't, wait around for a maximum of 2 seconds until
            // it does.
            do
                {
                if(ttm2 > ttm1)
                    break;
                INFO_PRINTF2(_L("Timestamp on %S still unchanged, waiting..."), &filename );
                User::After(delayinc);
                maxdelay -= delayinc;
                (void)GetFileTimeStamp( filename, ttm2 );
                }
            while(maxdelay > 0);

            // If the timestamp still hasn't been updated we have problems.
            if(ttm2 <= ttm1)
                {
                INFO_PRINTF1(HTML_RED);
                ERR_PRINTF2(_L("File %S has the same or earlier modification date as before the install."), &filename);
                ERR_PRINTF1(_L("It may be, on hardware, that the timestamps do not have sufficient granularity to detect a difference."));
                // Suppress the test failure. There seems to be a problem, possibly with granularity of timestamps on
                // hardware, which may be giving us false failures (on the persists file particularly). Until this
                // is better understood this will not be a failure. Any real problem will show up as a failure within
                // Central Repository setting checks anyway.
                // SetTestStepResult(EFail);
                ERR_PRINTF1(_L("Test failure suppressed."));
                }
            TBuf<50> date;
            _LIT(format, "%F%D-%M-%Y %H-%T-%S-%C");
            ttm1.FormatL( date, format );
            INFO_PRINTF2(_L("Date1: %S"), &date );
            ttm2.FormatL( date, format );
            INFO_PRINTF2(_L("Date2: %S"), &date );
            INFO_PRINTF1(HTML_RED_OFF);
            }else // we must be setting the timestamps before installation..
            {
            atstamps.Append(ttm2);
            }
        fprefix.Delete(len, 20);  // Knock off the digit(s) from 'AffectedFileNN'
        }
    }
