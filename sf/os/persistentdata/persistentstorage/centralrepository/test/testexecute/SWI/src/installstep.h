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
// Centrep SWI test step declaration
// 
//

/**
 @file
*/

#ifndef __INSTALL_STEP_H__
#define __INSTALL_STEP_H__

#include <test/testexecutestepbase.h>
#include "tui.h"

#include "e32base.h"
#include "Te_centrep_SWISuiteStepBase.h"

namespace Swi
        {
        class CInstallPrefs;
        }

class Cinstallers :  public CTe_centrep_SWISuiteStepBase
    {
protected:
        Cinstallers();
        ~Cinstallers();
        void CheckFilesExistL();
        void CheckFilesNonExistL();
        void CheckFilesL(const TDesC&, const TDesC&, TBool);
        TBool SetupNotifyRequestsL(CRepository *, TInt, TRequestStatus *);
        TBool WaitOnNotifyRequests(TRequestStatus *, TInt );
        TInt GetFileTimeStamp(const TDesC& , TTime& );
        void SetCheckTimeStampsL(RArray<TTime> &atstamps);
    };

#define TEFSWICENTREPMAXNOTIFYREQS (10)

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
private:
        TFileName iSisFileName; // name of the file to install
        TUI* iUi; // Minimal install framework
        TInstallType iInstallType; // use file handle to install
        TBool iDoCancelTest;
        TPtrC iExpectedHash;
        TPtrC Outcomes;// outcomes as received from ini file
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

_LIT(KNumNotifies, "NumNotifies");
_LIT(KNotifyPrefix, "Notify");
_LIT(KAffectedFile, "AffectedFile");

_LIT(KInstallStep, "InstallStep");
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
_LIT(KUninstallAugStep, "UninstallAugStep"); // uninstall of augmentation
_LIT(KTrustStatus, "TrustStatus");
_LIT(KPirCheck, "PostInstallRevocationCheck");

_LIT(KUninstallCancelStep, "UninstallStepCancel");
_LIT(KUninstallAugCancelStep, "UninstallAugStepCancel"); // uninstall of augmentation
_LIT(KNumExpectedSizes, "numExpectedSizes"); // This specifies that the test section
// has values for installation size(s).


#endif //  __INSTALL_STEP_H__
