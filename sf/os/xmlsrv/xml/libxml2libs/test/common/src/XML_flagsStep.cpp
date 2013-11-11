// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file
 @internalTechnology
*/
#include "XML_flagsStep.h"
#include "Te_XML_FTmgrSuiteDefs.h"

CXML_flagsStep::~CXML_flagsStep()
/**
 * Destructor
 */
	{
    if ( iFeatMgrInitialized )
          {
             FeatureManager::UnInitializeLib();  
          }

	}

CXML_flagsStep::CXML_flagsStep()
/**
 * Constructor
 */
	{

	TRAPD(err,ConstructL()); 
	if(err==KErrNone)
	{
		SetTestStepName(KXML_flagsStep);
	}
		
	}

void CXML_flagsStep::ConstructL()
	{

    FeatureManager::InitializeLibL();
    iFeatMgrInitialized = ETrue;
	 
	}


void CXML_flagsStep::ShowMenuL()
	{
	 TInt Libxml2UID, Libxml2SAXParserUID, Libxml2DOMParserUID;
	 
     if( !GetIntFromConfig(ConfigSection(),KTe_XML_flagsSuiteLibxml2UID, Libxml2UID) ||
			!GetIntFromConfig(ConfigSection(),KTe_XML_flagsSuiteLibxml2SAXParserUID,Libxml2SAXParserUID) ||
			!GetIntFromConfig(ConfigSection(),KTe_XML_flagsSuiteLibxml2Libxml2DOMParserUID,Libxml2DOMParserUID )
			)
			{
			User::Leave(KErrNotFound);
			}
		else
			{
			INFO_PRINTF4(_L("The flag1 is %d, The flag2 is %d, The flag3 is %d"), Libxml2UID, Libxml2SAXParserUID, Libxml2DOMParserUID); // Block end
			}
	 
	 
	 
    if ( FeatureManager::FeatureSupported(Libxml2UID) == KFeatureSupported && FeatureManager::FeatureSupported(Libxml2SAXParserUID) == KFeatureSupported
            && FeatureManager::FeatureSupported(Libxml2DOMParserUID) == KFeatureSupported)
        {
          SetTestStepResult(EPass);            
          INFO_PRINTF1(_L("\n libxml2, DOM , SAX present."));
        }

  
    else if ( FeatureManager::FeatureSupported(Libxml2UID) == KFeatureUnsupported && FeatureManager::FeatureSupported(Libxml2SAXParserUID) == KFeatureUnsupported
            && FeatureManager::FeatureSupported(Libxml2DOMParserUID) == KFeatureUnsupported)
        {
          SetTestStepResult(EPass);
          INFO_PRINTF1(_L("\n libxml2, DOM , SAX not present."));
        }
    
    else if ( FeatureManager::FeatureSupported(Libxml2UID) == KFeatureSupported && FeatureManager::FeatureSupported(Libxml2SAXParserUID) == KFeatureUnsupported
            && FeatureManager::FeatureSupported(Libxml2DOMParserUID) == KFeatureUnsupported)
        {
          SetTestStepResult(EPass);    
          INFO_PRINTF1(_L("\n libxml2 present, DOM and SAX not present. "));
        }
    
    else if ( FeatureManager::FeatureSupported(Libxml2UID) == KFeatureSupported && FeatureManager::FeatureSupported(Libxml2SAXParserUID) == KFeatureUnsupported
            && FeatureManager::FeatureSupported(Libxml2DOMParserUID) == KFeatureSupported)
        {
          SetTestStepResult(EPass);        
          INFO_PRINTF1(_L("\n libxml2 and Dom present, SAX not present."));
        }
    
    else if ( FeatureManager::FeatureSupported(Libxml2UID) == KFeatureSupported && FeatureManager::FeatureSupported(Libxml2SAXParserUID) == KFeatureSupported
            && FeatureManager::FeatureSupported(Libxml2DOMParserUID) == KFeatureUnsupported)
        {          
          SetTestStepResult(EPass);        
          INFO_PRINTF1(_L("\n libxml2 and SAX present, DOM not present ..."));          
        }


	}



TVerdict CXML_flagsStep::doTestStepL()
	{
	  INFO_PRINTF1(_L("XML Variation Support test case(feature manager based) -- BASESRVCS-XML-CT-4927, PREQ2051"));
	  SetTestStepResult(EFail);
	  ShowMenuL();
	  return TestStepResult();
	}



