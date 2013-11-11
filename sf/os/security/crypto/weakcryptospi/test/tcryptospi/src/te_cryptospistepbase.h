/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __TE_CRYPTOSPI_STEP_BASE__)
#define __TE_CRYPTOSPI_STEP_BASE__
#include <test/testexecutestepbase.h>

//********************CryptoSpi Specific Headers and Macros********************

#include "te_cryptospiconversion.h"
#include "te_cryptospidefs.h"
#include "traplog.h"

#include <cryptospi/cryptospidef.h>

#include <f32file.h> 

#define DES_AS_8_BIT(str) (TPtrC8(reinterpret_cast<const TText8*>(((str).Ptr())), (str).Size()))

//****************************************************************************

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_CryptoSpiStepBase : public CTestStep
	{
public:
	virtual ~CTe_CryptoSpiStepBase();
	CTe_CryptoSpiStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();	

protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	};

#endif
