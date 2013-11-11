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


/**
 @file 
 @internalTechnology
*/

#ifndef DATETIMEFATALTESTSTEP_H
#define DATETIMEFATALTESTSTEP_H

#include "validateteststep.h"

_LIT(KDateTimeFatalTestStep, "DateTimeFatalValidateTest");  
_LIT(KDateTime, "datetime"); //YYYYMMDD:HHMMSS.MMMMMM

_LIT(KSecondConfigSection, "_secondstage");

class CDateTimeFatalValidateStep : public CValidateCertChainStep
	{
public:
	CDateTimeFatalValidateStep();
	virtual TVerdict doTestStepPreambleL();
	
	virtual void PerformTestL();
	
private:
	TTime iDateTime;
	};	

	

#endif // DATETIMEFATALTESTSTEP_H
