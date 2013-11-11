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


#include "omx_xml_script.h"
#include "omxscripttest.h"
#include "threadedlogger.h"

EXPORT_C COmxXmlScript* COmxXmlScript::NewL(MOmxScriptTestLogger& aLogger)
	{
	return new(ELeave) COmxXmlScript(aLogger);
	}

COmxXmlScript::COmxXmlScript(MOmxScriptTestLogger& aLogger):
iLogger(aLogger)
	{
	}

COmxXmlScript::~COmxXmlScript()
	{
	// do nothing
	}

EXPORT_C void COmxXmlScript::RunScriptL(const TDesC& aScriptFilename, const TDesC& aScriptSection)
	{
	CThreadedLogger* threadedLogger = CThreadedLogger::NewLC(iLogger);
	ROmxScriptTest impl(*threadedLogger);
	CleanupClosePushL(impl);
	impl.RunTestL(aScriptFilename, aScriptSection);
	CleanupStack::PopAndDestroy(2, threadedLogger);
	}
