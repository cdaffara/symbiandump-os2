/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "testsecurityinstuihandler.h"

/**
 * Creates a new Instance of the CTestSecurityInstUIHandler and calls the
 * appropriate ConsturctL
 * @param None
 * @return CTestSecurityInstUIHandler* 
 * 
 */
CTestSecurityInstUIHandler* CTestSecurityInstUIHandler::NewL()
	{
	CTestSecurityInstUIHandler*	self=new(ELeave) CTestSecurityInstUIHandler();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
 * Constructor for CTestSecurityInstUIHandler
 */	
CTestSecurityInstUIHandler::CTestSecurityInstUIHandler()
:	iDispInstall(TRUE),
	iDispGrantCapabilities(TRUE),
	iDispUnInstall(TRUE),
	iDispLanguage(0),
	iDispDialect(0),
	iDispDrive('C'),
	iDispUpgrade(TRUE),
	iDispDependencyBreak(TRUE),
	iDispQuestion(TRUE),
	iDispSecurityWarning(TRUE),
	iDispRevocationQuery(TRUE),
	iDispApplicationInUse(TRUE),
	iDispTextDialog(TRUE),
	iDispOptions(0),
	iAppUID(0),
	iErrCode(KErrNothing)
	{
	}

/**
 * Second stage constructor for CTestSecurityInstUIHandler
 */	
void CTestSecurityInstUIHandler::ConstructL()
	{
	}

/**
 * Destructor for CTestSecurityInstUIHandler
 */	
CTestSecurityInstUIHandler::~CTestSecurityInstUIHandler()
	{
	iDispInstall = FALSE;
	iDispGrantCapabilities = FALSE;
	iDispUnInstall = FALSE;
	iDispLanguage = 0;
	iDispDialect = 0;
	iDispDrive = 'C';
	iDispUpgrade = FALSE;
	iDispDependencyBreak = FALSE;
	iDispQuestion = FALSE;
	iDispSecurityWarning = FALSE;
	iDispRevocationQuery = FALSE;
	iDispApplicationInUse = FALSE;
	iDispTextDialog = FALSE;
	iDispOptions = 0;
	iAppUID = 0;
		
	iErrCode = KErrNothing;
	}

/**
 * Displays a dialog at the beginning of an installation with details about the SISX package
 * being installed. This dialog must be shown in TrustedUI mode.
 *
 * @param aAppInfo		Information about the application.
 * @param aLogo			The logo data in the SISX file. If no logo is present it will be NULL.
 * @param aCertificates An array of end entity certificates, one for each chain in the SISX file.
 *						If the SISX file is unsigned this array will have zero length.
 *
 * @note 				Trusted UI
 *
 * @return				ETrue if the user pressed the Yes button to continue the install,
 *						EFalse if the user pressed the No button, aborting the installation.
 */
TBool CTestSecurityInstUIHandler::DisplayInstallL(const CAppInfo& /*aAppInfo*/, 
			const CApaMaskedBitmap* /*aLogo*/, 
			const RPointerArray< CCertificateInfo >& /*aCertificates*/)
	{
	return iDispInstall;
	}

/**
 * Displays a dialog requesting that the user grants these capabilities to the SISX package 
 * being installed. This dialog must be shown in TrustedUI mode.
 *
 * @param aAppInfo	 		Information about the application.
 * @param aCapabilitySet 	The capabilities requested by the SISX package.
 *
 * @note 					Trusted UI
 *
 * @return					ETrue to continue the installation, or EFalse to abort the installation.
 */
TBool CTestSecurityInstUIHandler::DisplayGrantCapabilitiesL(const CAppInfo& /*aAppInfo*/, 
			const TCapabilitySet& /*aCapabilitySet*/)
	{
	return iDispGrantCapabilities;
	}

/**
 * Requests preferred language and dialect information from the UI.
 *
 * @param aAppInfo		Information about the application.
 * @param aLanguages	Array of languages supported by the SIS file. 
 *
 * @return				Must return index in the passed array that corresponds to the user's selected 
 *						preferred language. If out-of-bounds value is returned, the engine will abort 
 *						installation.
 */
TInt CTestSecurityInstUIHandler::DisplayLanguageL(const CAppInfo& /*aAppInfo*/, 
	      const RArray<TLanguage>& aLanguages)
	{
	TInt languageCode = iDispLanguage;
	TInt index;
	
	// Return the index value in the array which matches the language code 
	// obtained from the ini file.
	for (index=0; index<aLanguages.Count(); index++)
		{
		if (aLanguages[index] == (TLanguage)languageCode)
			{
			return index;
			}
		}
	// Language Code did not match - error
	iErrCode = KErrLanguage;
	
	return languageCode;
	}
	
/**
 * Requests the drive to install the package on, from the UI.
 *
 * @param aAppInfo	Information about the application.
 * @param aSize		Total size of the files that will be installed on the configurable drive.
 * @param aDriveLetters	Array of drive letters on which the package can be installed.
 * @param aDriveSpaces	Array of space free (in bytes) on each of the drives in the aDriveLetters array. 
 *
 * @return		An index in the drive array. If the index is out of range the engine will abort
 *                      the installation.
 */
TInt CTestSecurityInstUIHandler::DisplayDriveL(const CAppInfo& /*aAppInfo*/, 
		   TInt64 /*aSize*/,
		   const RArray<TChar>& aDriveLetters,
		   const RArray<TInt64>& /*aDriveSpaces*/)
	{
	TInt index;
	TInt driveIndex=0;
	for (index=0; index < aDriveLetters.Count(); index++)
		{
		if (aDriveLetters[index] == iDispDrive)
			{
			driveIndex = index;
			break;
			}
		}
		
	if (index == aDriveLetters.Count()) // Out of range error 
		{
		iErrCode = KErrDrive;
		driveIndex = index;
		}
	return driveIndex;
	}
	
/**
 * Displays a dialog asking whether we want to upgrade an application present on the device.
 *
 * @param aAppInfo	        Information about the application being installed.
 * @param aExistingAppInfo	Information about the existing application.
 *
 * @return		ETrue if the user pressed the Yes button,
 *			EFalse if the user pressed the No button.
 */
TBool CTestSecurityInstUIHandler::DisplayUpgradeL(const CAppInfo& /*aAppInfo*/, 
		      const CAppInfo& /*aExistingAppInfo*/)
	{
	return iDispUpgrade;
	}
	
/**
 * Displays a dialog box asking which options the user wishes to install.
 *
 * @param aAppInfo	Information about the application.
 * @param aOptions	An array of option names.
 * @param aSelections	An array of booleans, corresponding to the aOptions array.
 *
 * @return		ETrue on success, EFalse on error.
 */
TBool CTestSecurityInstUIHandler::DisplayOptionsL(const CAppInfo& /*aAppInfo*/,
		      const RPointerArray<TDesC>& aOptions,
		      RArray<TBool>& aSelections)
	{
	
	TInt index;
	TBool setFlag = EFalse;
	
	for (index=0; index<aOptions.Count();index++)
		{
		if (index == iDispOptions)
			{
			aSelections[index] = ETrue;
			setFlag = ETrue;
			}
		else
			{
			aSelections[index] = EFalse;
			}
		}

	if (setFlag == (TBool)ETrue)
		{
		return ETrue;
		}
	else
		{
		iErrCode = KErrOptions;
		return EFalse;
		}
	}
	
/**
 * Informs the UI of events occurring during installation.  
 *
 * @param aEvent	The event which has occurred.
 * @param aValue	The value associated with the event if applicable.
 * @param aDes		A descriptor associated with the event if applicable.
 * @return TBool	Always returns ETrue to indicate the Install Event is always handled
 */
TBool CTestSecurityInstUIHandler::HandleInstallEventL(const CAppInfo& /*aAppInfo*/,
			 TInstallEvent aEvent, 
			 TInt /*aValue=0*/, 
			 const TDesC& /*aDes=KNullDesC*/)
	{
	switch (aEvent)
		{
		case EEventSetProgressBarFinalValue:
			break;
		case EEventUpdateProgressBar:
			break;
		case EEventAbortedInstall:
			iErrCode = KErrAbortInstall;
			break;
		case EEventCompletedInstall:
			break;
		case EEventCompletedUnInstall:
			break;
		case EEventAbortedUnInstall:
			iErrCode = KErrAbortUnInstall;
			break;
		case EEventCompletedNoMessage:
			break;
		case EEventLeave:
			iErrCode = KErrLeave;
			break;
		case EEventOcspCheckEnd:
			break;
		default:
			break;
		}
	return ETrue;
	}
	
/**
 * Informs the UI of cancellable events occurring during installation.  
 *
 * @param aEvent	 		The event which has occurred.
 * @param aCancelHandler 	This mix-in is used to inform Software Install that the user has
 *			 				pressed the cancel button on the dialog.
 * @param aValue	 		The value associated with the event if applicable.
 * @param aDes           	A descriptor associated with the event if applicable.
 */
void CTestSecurityInstUIHandler::HandleCancellableInstallEventL(const CAppInfo& /*aAppInfo*/,
			TInstallCancellableEvent aEvent, 
		    MCancelHandler& /*aCancelHandler*/,
		    TInt /*aValue=0*/, 
		    const TDesC& /*aDes=KNullDesC*/)
	{
	switch (aEvent)
		{
		case EEventRemovingFiles:
			break;
		case EEventShuttingDownApps:
			break;
		case EEventCopyingFiles:
			break;
		case EEventOcspCheckStart:
			break;
		default:
			break;
		}
	}

/**
 * Displays a dialog indicating that the package is unsigned, or that signature validation has failed.
 *
 * @param aAppInfo              Information about the application.
 * @param aSigValidationResult	The result of signature validation.
 * @param aPkixResult           Results of a validation of the PKIX  certificate chain.
 *                              Note that is can be NULL, in this case no validation results are available.
 * @param aCertificates         An array containing information about the certificates in the chain.
 * @param aInstallAnyway		ETrue if installation is allowed to continue.
 *
 * @return						ETrue to continue the installation, EFalse to abort.
 */
TBool CTestSecurityInstUIHandler::DisplaySecurityWarningL(const CAppInfo& /*aAppInfo*/,
						Swi::TSignatureValidationResult /*aSigValidationResult*/,
						RPointerArray<CPKIXValidationResultBase>& /*aPkixResults*/,
						RPointerArray<CCertificateInfo>& /*aCertificates*/,
						TBool /*aInstallAnyway*/)
	{
	return ETrue;
	}

/**
 * Displays a dialog indicating that there was an error carrying out online certificate status checking.
 *
 * @param aMessage		The error which occurred during online certificate status checking
 * @param aOutcome		The outcome of the OCSP check. If no outcome can be given, it will be NULL.
 * @param aWarningOnly	ETrue if the installation is allowed to continue after a warning, EFalse otherwise.
 *
 * @return				ETrue on success, EFalse on error.
 */
TBool CTestSecurityInstUIHandler::DisplayOcspResultL(const CAppInfo& /*aAppInfo*/,
						TRevocationDialogMessage /*aMessage*/, 
						RPointerArray<TOCSPOutcome>& /*aOutcomes*/, 
						RPointerArray<CCertificateInfo>& /*aCertificates*/,
						TBool aWarningOnly)
	{
	return aWarningOnly;
	}
	
/**
 * Used to inform the user the installation they are attempting is trying to overwrite
 * a file that does not belong to it. Following this message installation is aborted.
 *
 * @param aAppInfo	        Information about the application.
 * @param aInstalledAppInfo	Information about the application that owns the file in question.
 * @param aFileName			The filename of the file in question
 */
void CTestSecurityInstUIHandler::DisplayCannotOverwriteFileL(const CAppInfo& /*aAppInfo*/,
				 const CAppInfo& /*aInstalledAppInfo*/,
				 const TDesC& /*aFileName*/)
	{
	iErrCode = KErrCannotOverwriteFile;
	}
	
/**
 * Displays a dialog at the start of the uninstallation with information about the application
 * to be uninstalled. The user may cancel the uninstallation at this stage. This dialog must 
 * be shown in TrustedUI mode.
 *	
 * @param aAppInfo	Information about the application.
 *
 * @note Trusted UI
 *
 * @return			ETrue to continue the uninstallation, or EFalse to abort the uninstallation.
 */
TBool CTestSecurityInstUIHandler::DisplayUninstallL(const CAppInfo& /*aAppInfo*/)
	{
	return iDispUnInstall;
	}

/**
 * Displays a dialog containing text. This dialog has only a Continue button if aOption is 
 * EInstFileTextOptionContinue, or Yes and No buttons otherwise.
 *
 * @param aAppInfo	Information about the application.
 * @param aOption	The type of dialog box to display.
 * @param aText		The text to display
 *
 * @return		ETrue if the user wants to go forward, EFalse otherwise.
 */
TBool CTestSecurityInstUIHandler::DisplayTextL(const CAppInfo& /*aAppInfo*/, 
			    TFileTextOption /*aOption*/,
			    const TDesC& /*aText*/)
	{
	return iDispTextDialog;
	}
	
/**
 * Displays a dialog showing an error message. 
 *
 * @param aAppInfo	    Information about the application.
 * @param aType		    The error message to display.
 * @param aParam	    An error specific parameter.
 */
void CTestSecurityInstUIHandler::DisplayErrorL(const CAppInfo& /*aAppInfo*/,
			   TErrorDialog /*aType*/,
			   const TDesC& /*aParam*/)
	{
	iErrCode = KErrError;
	}

/**
 * Displays a dialog indicating that a component which is being uninstalled is a dependency
 * of another component installed.
 *
 * @param aAppInfo		Information about the application.
 * @param aComponents	An array of components which depend on components of the application
 *
 * @return				ETrue to continue the uninstallation, EFalse to abort.
 */
TBool CTestSecurityInstUIHandler::DisplayDependencyBreakL(const CAppInfo& /*aAppInfo*/,
		      const RPointerArray<TDesC>& /*aComponents*/)
	{
	return iDispDependencyBreak;
	}
	
/**
 * Informs user that the applications are in use during an uninstallation.
 * Return ETrue if the user wishes to continue the removal process and 
 * close the applications anyway
 *
 * @param aAppNames		An array containing the name of the application in use.
 *
 * @return				ETrue if the user wishes to continue with removal.
 */
TBool CTestSecurityInstUIHandler::DisplayApplicationsInUseL(const CAppInfo& /*aAppInfo*/, 
			const RPointerArray<TDesC>& /*aAppNames*/)
	{
	return iDispApplicationInUse;
	}
	
/**
 * Displays a generic question dialog with a Yes/No answer.
 *
 * @param aAppInfo	Information about the application.
 * @param aQuestion The question to ask.
 * @param aDes		A descriptor associated with the question if applicable. 
 *
 * @return			ETrue if the user pressed the Yes button,
 *					EFalse if the user pressed the No button.
 */
TBool CTestSecurityInstUIHandler::DisplayQuestionL(const CAppInfo& /*aAppInfo*/, 
			TQuestionDialog aQuestion, const TDesC& /*aDes=KNullDesC*/)
	{
	
	switch(aQuestion)
		{
		case EQuestionIncompatible:
			break;
		default:
			break;
		}
		
	return iDispQuestion;
	}

TBool CTestSecurityInstUIHandler::DisplayMissingDependencyL(const CAppInfo& /*aAppInfo*/,
	const TDesC& /*aDependencyName*/,TVersion /*aWantedVersionFrom*/,
	TVersion /*aWantedVersionTo*/,TVersion /*aInstalledVersion*/)
	{
	return ETrue;
	}
	
