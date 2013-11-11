/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include "datetimefatalteststep.h"

	
CDateTimeFatalValidateStep::CDateTimeFatalValidateStep()
	{
	SetTestStepName(KDateTimeFatalTestStep);
	}

TVerdict CDateTimeFatalValidateStep::doTestStepPreambleL()
	{
	CValidateCertChainStep::doTestStepPreambleL();
	
	TPtrC dateAndTime;
	GetStringFromConfig(ConfigSection(), KDateTime, dateAndTime);
	
	iDateTime = TTime(dateAndTime);
	return EPass;
	}

void CDateTimeFatalValidateStep::PerformTestL()
	{
	CPkixCertStepBase::PerformTestL();
	
	iCertChain->SetSupportedCriticalExtensionsL(iProcessedOids);
	
	ValidateL(iDateTime, ConfigSection());
		
	iCertChain->SetValidityPeriodCheckFatal(EFalse);
	
	HBufC* secondConfigSection = HBufC::NewLC(ConfigSection().Length() + KSecondConfigSection().Length());
	TPtr ptr = secondConfigSection->Des();
	
	ptr.Append(ConfigSection());
	ptr.Append(KSecondConfigSection);
	
	ValidateL(iDateTime, ptr);
	
	CleanupStack::PopAndDestroy(secondConfigSection);
		
	iCertChain->SetValidityPeriodCheckFatal(ETrue);	
	
	ValidateL(iDateTime, ConfigSection());	
	
	CleanupStack::PopAndDestroy(iCertChain);
	}




	
