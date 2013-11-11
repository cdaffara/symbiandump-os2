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
// ULogger output framework header
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/

#ifndef ULOGGEROUTFRWK_H
#define ULOGGEROUTFRWK_H

#include <e32base.h>
#include "uloggeroutputplugin.h"

namespace Ulogger
{
/*!COutputFramework class.
*/
NONSHARABLE_CLASS(COutputFramework) : public CBase
	{
public:
	/**Standard Symbian OS construction method.*/
	IMPORT_C static COutputFramework* NewL(MOutputPlugin& aOutputPlugin, const RPointerArray<TPluginConfiguration>& aOutputSettings);
	/**Destructor.*/
	IMPORT_C ~COutputFramework();
	/** This function send data to active output channel. */
	IMPORT_C TInt SendData(const TDesC8& aData);
	/** */
	IMPORT_C void ReleaseOutputResources();

private:
	COutputFramework(MOutputPlugin& aOutputPlugin);
	void ConstructL(const RPointerArray<TPluginConfiguration>& aOutputSettings);
	
	//data
	MOutputPlugin& iOutputPlugin; 
	RPointerArray<TPluginConfiguration> iOutputSettings;
	};

} // namespace
#endif // ULOGGER_H
