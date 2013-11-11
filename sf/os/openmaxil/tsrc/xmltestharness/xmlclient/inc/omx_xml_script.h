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


#ifndef OMX_XML_SCRIPT_H_
#define OMX_XML_SCRIPT_H_

#include <e32base.h>

/**
 * Bitmask flags for logging severity levels.
 */
enum TOmxScriptSeverity
	{
	EOmxScriptSevErr  = 0x0001,
	EOmxScriptSevWarn = 0x0002,
	EOmxScriptSevInfo = 0x0004,
	EOmxScriptSevVer  = 0x0007,
	EOmxScriptSevAll  = 0x0007
	};

/**
 * Interface to receive messages from ROmxScriptTest.
 */
NONSHARABLE_CLASS(MOmxScriptTestLogger)
	{
public:
	IMPORT_C virtual void Log(const TText8* aFile, TInt aLine, TOmxScriptSeverity aSeverity, const TDes& aMessage) = 0;
	};

NONSHARABLE_CLASS(COmxXmlScript) : public CBase
	{
public:
	IMPORT_C static COmxXmlScript* NewL(MOmxScriptTestLogger& aLogger);
	~COmxXmlScript();
	
	IMPORT_C void RunScriptL(const TDesC& aScriptFilename, const TDesC& aScriptSection);
	
private:
	COmxXmlScript(MOmxScriptTestLogger& aLogger);
	
private:
	MOmxScriptTestLogger& iLogger;
	};

#endif /*OMX_XML_SCRIPT_H_*/
