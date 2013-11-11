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
* tpropertystep.cpp
*
*/


#include <e32property.h>
#include "numberconversion.h"
#include "tupspropertystep.h"  
 
CPropertyStep::CPropertyStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KPropertyStep);
	}

CPropertyStep::~CPropertyStep()
/**
 * Destructor
 */
	{
 	__UHEAP_MARKEND;
	}

TVerdict CPropertyStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
 	__UHEAP_MARK;
 	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CPropertyStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

TVerdict CPropertyStep::doTestStepL()
	{

	INFO_PRINTF1(_L(">> CPropertyStep::doTestStepL()"));

	TInt index = 1;

	TName fPropertyName;
	fPropertyName.Format(_L("PropertyName_%d"),index);
	TName fPropertyCreate;
	fPropertyCreate.Format(_L("PropertyCreate_%d"),index);
	TName fPropertyKey;
	fPropertyKey.Format(_L("PropertyKey_%d"),index); 
	TName fPropertyType;
	fPropertyType.Format(_L("PropertyType_%d"),index);

	TPtrC propertyName;
	TBool propertyCreate;
	TInt propertyKey;
	TInt propertyType;

    GetStringFromConfig(ConfigSection(), fPropertyName, propertyName);
	GetBoolFromConfig(ConfigSection(),fPropertyCreate, propertyCreate);
  	GetIntFromConfig(ConfigSection(),fPropertyKey, propertyKey);
	GetIntFromConfig(ConfigSection(),fPropertyType, propertyType);

	// read all the property details in from the ini file
	while (GetStringFromConfig(ConfigSection(), fPropertyName, propertyName)
		&& GetBoolFromConfig(ConfigSection(),fPropertyCreate, propertyCreate)
  		&& GetIntFromConfig(ConfigSection(),fPropertyKey, propertyKey)
		&& GetIntFromConfig(ConfigSection(),fPropertyType, propertyType))
		{
	   	// INFO_PRINTF2(_L("Read test action  %d"),index);  // debug
	   	
	   	if (propertyCreate)
	   		{
			static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
	   		if (propertyType == static_cast<TInt>(RProperty::EInt))
	   			{
	   			 RProperty::Define(propertyKey, RProperty::EInt, KAllowAllPolicy, KAllowAllPolicy);
	   			}
	   		else
	   			{
	   		     RProperty::Define(propertyKey, RProperty::EByteArray, KAllowAllPolicy, KAllowAllPolicy);
	   			}
			}
	   	else
	   		{
	   		 RProperty::Delete(propertyKey);
	   		}
	    ++index;
		fPropertyName.Format(_L("PropertyName_%d"), index);
		fPropertyCreate.Format(_L("PropertyCreate_%d"), index);
		fPropertyKey.Format(_L("PropertyKey_%d"), index); 
		fPropertyType.Format(_L("PropertyType_%d"), index);

    GetStringFromConfig(ConfigSection(), fPropertyName, propertyName);
	GetBoolFromConfig(ConfigSection(),fPropertyCreate, propertyCreate);
  	GetIntFromConfig(ConfigSection(),fPropertyKey, propertyKey);
	GetIntFromConfig(ConfigSection(),fPropertyType, propertyType);

		}
	SetTestStepResult(EPass);
	return TestStepResult();
	}
