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

#include "tui.h"

TBool TUI::DisplayTextL(const CAppInfo&, TFileTextOption, const TDesC&)
	{
	return ETrue;
	}

void TUI::DisplayErrorL(const CAppInfo&, TErrorDialog, const TDesC&)
	{
	}

TBool TUI::DisplayDependencyBreakL(const CAppInfo&, const RPointerArray<TDesC>&)
	{
	return ETrue;
	}

TBool TUI::DisplayApplicationsInUseL(const CAppInfo&, const RPointerArray<TDesC>&)
	{
	return ETrue;
	}

TBool TUI::DisplayQuestionL(const CAppInfo&, TQuestionDialog, const TDesC&)
	{
	return ETrue;
	}

TBool TUI::DisplayUninstallL(const CAppInfo&)
	{
	return ETrue;
	}

TBool TUI::DisplayInstallL(const CAppInfo&, const CApaMaskedBitmap*, const RPointerArray<CCertificateInfo>&)
	{
	return ETrue;
	}

TBool TUI::DisplayGrantCapabilitiesL(const CAppInfo&, const TCapabilitySet&)
	{
	return ETrue;
	}

TInt TUI::DisplayLanguageL(const CAppInfo&, const RArray<TLanguage>&)
	{
	return 0;
	}

TInt TUI::DisplayDriveL(const CAppInfo&, TInt64, const RArray<TChar>&, const RArray<TInt64>&)
	{
	return 0;
	}

TBool TUI::DisplayUpgradeL(const CAppInfo&, const CAppInfo&)
	{
	return ETrue;
	}

TBool TUI::DisplayOptionsL(const CAppInfo&, const RPointerArray<TDesC>&, RArray<TBool>&)
	{
	return ETrue;
	}

TBool TUI::HandleInstallEventL(const CAppInfo&, TInstallEvent, TInt, const TDesC&)
	{
	return ETrue;
	}

void TUI::HandleCancellableInstallEventL(const CAppInfo&, TInstallCancellableEvent, MCancelHandler&, TInt, const TDesC&)
	{
	}

TBool TUI::DisplaySecurityWarningL(const CAppInfo&, TSignatureValidationResult, RPointerArray<CPKIXValidationResultBase>&, RPointerArray<CCertificateInfo>&, TBool)
	{
	return ETrue;
	}

TBool TUI::DisplayOcspResultL(const CAppInfo&, TRevocationDialogMessage, RPointerArray<TOCSPOutcome>&, RPointerArray<CCertificateInfo>&, TBool)
	{
	return ETrue;
	}

void TUI::DisplayCannotOverwriteFileL(const CAppInfo&, const CAppInfo&, const TDesC&)
	{
	}

TBool TUI::DisplayMissingDependencyL( const CAppInfo&, const TDesC&, TVersion, TVersion, TVersion)
	{
	return ETrue;
	}
