// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ECom SWI test step declaration
// 
//

/**
 @file
*/

#ifndef __INSTALL_STEP_H__
#define __INSTALL_STEP_H__

#include <test/testexecutestepbase.h>
#include "tui.h"

#include <e32base.h>
#include "TE_EComSWITestSuiteStepBase.h"

namespace Swi
        {
        class CInstallPrefs;
        }

class Cinstallers :  public CTe_EComSWITestSuiteStepBase
    {
protected:
        Cinstallers();
        ~Cinstallers();
        void CheckFilesExistL();
        void CheckFilesNonExistL();
        void CheckFilesL(const TDesC&, const TDesC&, TBool);
		void WaitForEComReDiscoveryL();
    };


/**
 * Installation test step class
 * @test
 * @internalComponent
 */
class CinstallStep : public Cinstallers
        {
public:
        enum TInstallType { EUseFileHandle, EUseMemory, EUseFileName, EUseCAF, EUseOpenFileName, ECheckExitValue};
        CinstallStep(TInstallType aInstallType, TBool aDoCancelTest = EFalse);
        ~CinstallStep();
        virtual TVerdict doTestStepPreambleL();
        virtual TVerdict doTestStepPostambleL();
        virtual TVerdict doTestStepL();

private:
        TInt DoInstallL(Swi::CInstallPrefs& aInstallPrefs);

protected:
        TFileName iSisFileName; // name of the file to install
        TUI* iUi; // Minimal install framework
        TInstallType iInstallType; // use file handle to install
        TBool iDoCancelTest;
        TPtrC iExpectedHash;
        TBool iInstallSuccess; // indicate software installed
        };

/** Custom resolver installation test step class
@test
@internalComponent
*/
class CResolverInstallStep : public CinstallStep // codescanner::missingcclass
	{
public:
	CResolverInstallStep(CinstallStep::TInstallType aInstallType);
	~CResolverInstallStep();
	virtual TVerdict doTestStepL();

private:
	RLibrary iLibrary;
	};

/**
 * Uninstallation test step class
 * @test
 * @internalComponent
 */
class CuninstallStep : public Cinstallers
        {
public:
        enum TUninstallType { EByUid, EByPackage };
        CuninstallStep(TUninstallType aType, TBool aDoCancelTest = EFalse);
        ~CuninstallStep();
        virtual TVerdict doTestStepPreambleL();
        virtual TVerdict doTestStepPostambleL();
        virtual TVerdict doTestStepL();
private:
        TInt DoUninstallL();

private:
        TUid iUid;
        TPtrC iVendorName;

        TUI* iUi; // Minimal install framework
        TPtrC iPackageName;
        TUninstallType iType;
        TBool iDoCancelTest;
        };

/** Custom resolver uninstallation test step class
 * @test
 * @internalComponent
 */
class CResolverUninstallStep : public CuninstallStep // codescanner::missingcclass
	{
public:
	CResolverUninstallStep(CuninstallStep::TUninstallType aType);
	~CResolverUninstallStep();
	virtual TVerdict doTestStepL();
	};

_LIT(KMMCNeeeded, "MMCNeeded");
_LIT(KMMCHWDSys, "e:\\sys\\bin");
_LIT(KMMCHWDPlg, "e:\\resource\\plugins\\");

_LIT(KMMCEmulSys, "x:\\sys\\bin");
_LIT(KMMCEmulPlg, "x:\\resource\\plugins\\");


_LIT(KNumNotifies, "NumNotifies");
_LIT(KNotifyPrefix, "Notify");
_LIT(KAffectedFile, "AffectedFile");

_LIT(KInstallStep, "InstallStep");
_LIT(KResolverInstallStep, "ResolverInstallStep");
_LIT(KResolverUninstallStep, "ResolverUninstallStep");
_LIT(KExpectedError, "ExpectedError");
_LIT(KInstallFHStep, "InstallFHStep"); // install using file handles
_LIT(KInstallMemStep, "InstallMemStep"); // install from memory
_LIT(KInstallCAFStep, "InstallCAFStep"); // install from CAF
_LIT(KInstallOpenFileStep, "InstallOpenFileStep"); // install with already opened file
_LIT(KCheckedInstallStep, "CheckedInstallStep");

// Cancel testing 
_LIT(KInstallCancelStep, "InstallStepCancel");
_LIT(KInstallFHCancelStep, "InstallFHStepCancel"); // install using file handles
_LIT(KInstallMemCancelStep, "InstallMemStepCancel"); // install from memory
_LIT(KInstallCAFCancelStep, "InstallCAFStepCancel"); // install from CAF

_LIT(KUninstallStep, "UninstallStep");
_LIT(KUninstallStepPackageName, "UninstallStepPackageName");

_LIT(KUninstallAugStep, "UninstallAugStep"); // uninstall of augmentation
_LIT(KTrustStatus, "TrustStatus");
_LIT(KPirCheck, "PostInstallRevocationCheck");

_LIT(KUninstallCancelStep, "UninstallStepCancel");
_LIT(KUninstallAugCancelStep, "UninstallAugStepCancel"); // uninstall of augmentation
_LIT(KNumExpectedSizes, "numExpectedSizes"); // This specifies that the test section
// has values for installation size(s).


#endif //  __INSTALL_STEP_H__
