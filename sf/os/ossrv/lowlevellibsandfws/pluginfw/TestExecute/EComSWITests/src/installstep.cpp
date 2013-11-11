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

#include "installStep.h"
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
    : iInstallType(aInstallType), iDoCancelTest(aDoCancelTest),
	  iInstallSuccess(EFalse)
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
  	
 	TInt bRet;

    // First find out if the install step is supposed to be successful.
    TInt insterr=KErrNone;
    bRet = GetIntFromConfig(ConfigSection(), KExpectedError, insterr );
    if(bRet!=1) insterr=KErrNone;

    CInstallPrefs* prefs = CInstallPrefs::NewLC();
    TInt err = DoInstallL(*prefs);        
	iInstallSuccess = (err == KErrNone);
    
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
 
 
    CleanupStack::PopAndDestroy(prefs);

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

/** Need to wait a few seconds for ECOM to
discover the change. Otherwise the next step may fail.
@pre caller has checked the install/uninstall
is successful. Otherwise will waste 30 s.
*/
void Cinstallers::WaitForEComReDiscoveryL()
	{
    REComSession& ecomSession = REComSession::OpenL();
	CleanupClosePushL(ecomSession);
	TRequestStatus ecomstatus;
	ecomSession.NotifyOnChange(ecomstatus);

	RTimer timer;
	User::LeaveIfError(timer.CreateLocal());
	CleanupClosePushL(timer);
	const TInt KInterval = 30000000; // 30 s
	TRequestStatus timerstatus;
	timer.After(timerstatus, KInterval);

	User::WaitForRequest(ecomstatus, timerstatus);

	if (ecomstatus == KRequestPending)
		{
		ecomSession.CancelNotifyOnChange(ecomstatus);
		User::WaitForRequest(ecomstatus);

        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF1(_L("No notification from ECOM"));
        INFO_PRINTF1(HTML_RED_OFF);
		// does not affect test result as this is the equivalent of
		// DELAY 30000 in the script.
		}
	else
		{
		timer.Cancel();
		User::WaitForRequest(timerstatus);
        INFO_PRINTF1(_L("ECOM has discovered the change"));
		}

	CleanupStack::PopAndDestroy(2); // ecomsession, RTimer
	REComSession::FinalClose();
	}

/** Resolver installation step */

/** constructor */
CResolverInstallStep::CResolverInstallStep(CinstallStep::TInstallType aInstallType)
	: CinstallStep(aInstallType)
	{
	}

/** destructor */
CResolverInstallStep::~CResolverInstallStep()
	{
	iLibrary.Close();
	}

/** uses CinstallStep::dotestStepL to do the install.
But adds an optional step of loading a DLL before, and
a step to wait for ECom rediscovery after.
*/
TVerdict CResolverInstallStep::doTestStepL()
	{
	TPtrC libraryPath;
    if (GetStringFromConfig(ConfigSection(), _L("loadresolver"), libraryPath))
		{
		TUidType nullUid;
		TInt err = iLibrary.Load(libraryPath, nullUid);
        if (err != KErrNone)
	        {
	        ERR_PRINTF2(_L("Preload resolver failed %d"), err);
	        return EFail;
	        }
		}

	// continue to do the actual install.
	TVerdict ret = CinstallStep::doTestStepL();

	if (iInstallSuccess)
		{
		WaitForEComReDiscoveryL();
		}

	return ret;
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
        err = Launcher::Uninstall(*iUi, iUid); 
        return err; 
        }
    else if (iType == EByPackage)
        {
        err = 0;
        
        // Go through list of packages from base package to get augmentations.
        CSisRegistryPackage* uninstallPackage=CSisRegistryPackage::NewLC(iUid, iPackageName, iVendorName);
        INFO_PRINTF3(_L("UnInstalling '%S', '%S'"), &iPackageName, &iVendorName);
        
        // err=Launcher::Uninstall(*iUi, *uninstallPackage);
         
		RSisRegistrySession registrySession;
        User::LeaveIfError(registrySession.Connect());
        CleanupClosePushL(registrySession);

        RSisRegistryEntry registryEntry;
    
        User::LeaveIfError(registryEntry.Open(registrySession, iUid));
        CleanupClosePushL(registryEntry);

        CSisRegistryPackage* package=registryEntry.PackageL();
        CleanupStack::PushL(package);
        
        if (*package == *uninstallPackage)
            {
            err=Launcher::Uninstall(*iUi, *package); 
            }
        else
            {
            // check augmenations
            RPointerArray<CSisRegistryPackage> augmentationPackages;
	        CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(augmentationPackages);

            registryEntry.AugmentationsL(augmentationPackages);
            
            for (TInt i=0; i < augmentationPackages.Count(); ++i)
                {
                if (*augmentationPackages[i] == *uninstallPackage)
                    {
                    err=User::LeaveIfError(Launcher::Uninstall(*iUi, *augmentationPackages[i])); 
                    break;
                    }
                }
                
            if (err != 0)
	        	{
	        	INFO_PRINTF1(HTML_RED);
	            ERR_PRINTF2(_L("Package Augmentation Not found for '%s' "), &iPackageName);
	            INFO_PRINTF1(HTML_RED_OFF);
	            SetTestStepResult(EFail);
	            return TestStepResult();
	        	}

            CleanupStack::PopAndDestroy(&augmentationPackages);
            }    
        
        CleanupStack::PopAndDestroy(3, &registrySession);
        CleanupStack::PopAndDestroy(uninstallPackage);
        }
    return err;
    }

/* ******************************************************************************
 *  End of copied code.
 * ******************************************************************************/

TVerdict CuninstallStep::doTestStepL()
    {
    INFO_PRINTF1(KUninstallStep);



    // Wait a fraction over a second. This is necessary because we may just
    // have been messing about with repository settings and if the install
    // happens really quickly we might not see a datestamp change (esp. on
    // hardware where the timestamp granularity is poor).
    User::After(1100000);
    User::LeaveIfError(DoUninstallL());

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

// CResolverUninstallStep class

/** constructor */
CResolverUninstallStep::CResolverUninstallStep(CuninstallStep::TUninstallType aType)
	: CuninstallStep(aType)
	{
	}

/** nothing to do in destructor */
CResolverUninstallStep::~CResolverUninstallStep()
	{
	}

/** runs CuninstallStep::doTestStepL and then
do a WaitForEComReDiscoveryL */
TVerdict CResolverUninstallStep::doTestStepL()
	{
	TVerdict ret = CuninstallStep::doTestStepL();
	if (ret == EPass)
		{
		WaitForEComReDiscoveryL();
		}
    return ret;
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
                TInt timeout=1e7;
                TInt wait=2.5e5;
                // check if the file (doesn't) exist. Give it some time if
                // we see a failure in case there's a race with the
                // (un)installer 
                
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

