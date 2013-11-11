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

#ifndef __SWI_CENTREP_INSTALLER__
#define __SWI_CENTREP_INSTALLER__

#include <swi/msisuihandlers.h>

using namespace Swi;

class TUI : public Swi::MUiHandler
	{
public:
	// MCommonDialogs
	TBool DisplayTextL(const CAppInfo& aAppInfo, TFileTextOption aOption, const TDesC& aText);
	void DisplayErrorL(const CAppInfo& aAppInfo, TErrorDialog aType, const TDesC& aParam);
	TBool DisplayDependencyBreakL(const CAppInfo& aAppInfo, const RPointerArray<TDesC>& aComponents);
	TBool DisplayApplicationsInUseL(const CAppInfo& aAppInfo, const RPointerArray<TDesC>& aAppNames);
	TBool DisplayQuestionL(const CAppInfo& aAppInfo, TQuestionDialog aQuestion, const TDesC& aDes);
	// MUninstallerUiHandler
	TBool DisplayUninstallL(const CAppInfo& aAppInfo);
	// MInstallerUiHandler
	TBool DisplayInstallL(const CAppInfo& aAppInfo, const CApaMaskedBitmap* aLogo, const RPointerArray<CCertificateInfo>& aCertificates);
	TBool DisplayGrantCapabilitiesL(const CAppInfo& aAppInfo, const TCapabilitySet& aCapabilitySet);
	TInt DisplayLanguageL(const CAppInfo& aAppInfo, const RArray<TLanguage>& aLanguages);
	TInt DisplayDriveL(const CAppInfo& aAppInfo, TInt64 aSize, const RArray<TChar>& aDriveLetters, const RArray<TInt64>& aDriveSpaces);
	TBool DisplayUpgradeL(const CAppInfo& aAppInfo, const CAppInfo& aExistingAppInfo);
	TBool DisplayOptionsL(const CAppInfo& aAppInfo, const RPointerArray<TDesC>& aOptions, RArray<TBool>& aSelections);
	TBool HandleInstallEventL(const CAppInfo& aAppInfo, TInstallEvent aEvent, TInt aValue, const TDesC& aDes);
	void HandleCancellableInstallEventL(const CAppInfo& aAppInfo, TInstallCancellableEvent aEvent, MCancelHandler& aCancelHandler, TInt aValue, const TDesC& aDes);
	TBool DisplaySecurityWarningL(const CAppInfo& aAppInfo, TSignatureValidationResult aSigValidationResult, RPointerArray<CPKIXValidationResultBase>& aPkixResults, RPointerArray<CCertificateInfo>& aCertificates, TBool aInstallAnyway);
	TBool DisplayOcspResultL(const CAppInfo& aAppInfo, TRevocationDialogMessage aMessage, RPointerArray<TOCSPOutcome>& aOutcomes, RPointerArray<CCertificateInfo>& aCertificates, TBool aWarningOnly);
	void DisplayCannotOverwriteFileL(const CAppInfo& aAppInfo, const CAppInfo& aInstalledAppInfo, const TDesC& aFileName);
	TBool DisplayMissingDependencyL( const CAppInfo& aAppInfo, const TDesC& aDependencyName, TVersion aWantedVersionFrom, TVersion aWantedVersionTo, TVersion aInstalledVersion);
	};

#endif	/* __SWI_CENTREP_INSTALLER__ */
