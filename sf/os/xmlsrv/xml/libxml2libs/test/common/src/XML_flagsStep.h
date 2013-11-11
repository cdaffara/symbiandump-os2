/**
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
 @internalTechnology
*/
#if (!defined __XML_FLAGS_STEP_H__)
#define __XML_FLAGS_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_XML_FTmgrSuiteStepBase.h"
#include <featmgr/featmgr.h>
#include <featmgr/featurecontrol.h> 





class CXML_flagsStep : public CTe_XML_FTmgrSuiteStepBase
	{
public:
	CXML_flagsStep();
	void ConstructL();
	void ShowMenuL();
	~CXML_flagsStep();
	virtual TVerdict doTestStepL();


private:
	TBool iFeatMgrInitialized;
	};

_LIT(KXML_flagsStep, "XML_flagsStep");
_LIT(KTe_XML_flagsSuiteLibxml2UID, "flag1");
_LIT(KTe_XML_flagsSuiteLibxml2SAXParserUID, "flag2");
_LIT(KTe_XML_flagsSuiteLibxml2Libxml2DOMParserUID, "flag3");



#endif
