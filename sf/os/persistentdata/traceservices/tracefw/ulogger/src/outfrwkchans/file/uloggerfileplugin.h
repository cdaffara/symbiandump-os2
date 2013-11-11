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
//

/**
 @file
 @internalTechnology
 @prototype
*/

#ifndef ULOGGERFILEPLUGIN_H
#define ULOGGERFILEPLUGIN_H

#include "uloggeroutputplugin.h" // Ulogger::MOutputPlugin
#include <f32file.h> // RFs


namespace Ulogger
{

/**
Settings keys for file plugin.
*/
_LIT(KOutputPath, "output_path");
_LIT(KLogDefaultFileName, "C:\\logs\\ulogger.log");



class CFileWriter : public CPlugin, public MOutputPlugin
	{
public:
	 static CFileWriter* NewL();
	 ~CFileWriter();

	//from MOutputPlugin
	TInt ConfigureOutputPlugin(const RPointerArray<TPluginConfiguration>& aSettings);
	TInt Write(const TDesC8& aText);
	void CloseOutputPlugin();	
	
	//from CPublicBase
	TAny* GetInterfaceL(TPluginInterface aInterfaceId);
	
private:
	CFileWriter();
	void ConstructL();
	TInt WriteToFile(const TDesC8& aText);
	TInt OpenLog();

private:
    RFs iFs;	
    RFile iLog;
	TFileName iFileName;    
	TBool iIsOpen;
	RMutex* iMutex;
	};

} // namespace

#endif // ULOGGERFILEPLUGIN_H
