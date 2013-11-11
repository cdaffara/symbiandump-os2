// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ULogger File plugin header file
//



/**
 @file te_tracecontroltestplugin.h
 @internalTechnology
*/

#ifndef TE_FILEPLUGIN_H
#define TE_FILEPLUGIN_H

#include "uloggeroutputplugin.h" // Ulogger::MOutputPlugin
#include <f32file.h> // RFs
using namespace Ulogger; //CA:added so can use old ulogger api


/**
Settings keys for file plugin.
*/
_LIT(KTestOutputPath, "output_path");
_LIT(KTestLogDefaultFileName, "C:\\logs\\testlog.txt");
_LIT(KErrorCode, "ErrorCode");


class CTestFileWriter : public CPlugin, public MOutputPlugin
	{
public:
	 static CTestFileWriter* NewL();
	 ~CTestFileWriter();
	TInt ConfigureOutputPlugin(const RPointerArray<TPluginConfiguration>& aConfigs);
	TInt Write(const TDesC8 &aText);
	void CloseOutputPlugin();	

	//from CPublicBase
	TAny* GetInterfaceL(CPlugin::TPluginInterface aInterfaceId);

private:
	CTestFileWriter();
	void ConstructL();
	TInt WriteToFile(const TDesC8& aText);
	TInt OpenLog();
	void DoClearLog();
	TBool DoOwnProcessing();
private:	
	RFs iFs;	
    RFile iLog;
	TFileName iFileName;    
	TBool iIsOpen;
	RMutex* iMutex;
	TInt iErrorCode;
	TInt iCountDown;
	};


#endif // TE_FILEPLUGIN_H
