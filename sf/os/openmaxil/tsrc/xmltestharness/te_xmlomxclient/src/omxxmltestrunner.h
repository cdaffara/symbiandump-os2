/*
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


#ifndef OMXXMLTESTRUNNER_H
#define OMXXMLTESTRUNNER_H

#include <e32base.h>
#include <multimedia/omx_xml_script.h>

class CTestExecuteLogger;

class COmxXmlTestRunner : public CBase, public MOmxScriptTestLogger
	{
public:
	static COmxXmlTestRunner* NewL(CTestExecuteLogger& aLogger);
	~COmxXmlTestRunner();
	
	void SetFilenameL(const TDesC& aFilename);
	void RunTestL(const TDesC& aSectionName);
	
	// from MOmxScriptTestLogger
	void Log(const TText8* aFile, TInt aLine, TOmxScriptSeverity aSeverity, const TDes& aMessage);
	
private:
	COmxXmlTestRunner(CTestExecuteLogger& aLogger);
	void ConstructL();
	
	CTestExecuteLogger& iTestExecuteLogger;
	HBufC* iFilename;
	COmxXmlScript* iScript;
	};

#endif //OMXXMLTESTRUNNER_H
