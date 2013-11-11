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
// Ost Performance Tests ULogger session controller
//



/**
 @file te_tracecontrolcmds.h
 @internalTechnology
 @prototype
*/

#ifndef TE_UPTULOGGERCMDS_H
#define TE_UPTULOGGERCMDS_H

#ifndef __KERNEL_MODE__
#include <e32base.h>
#include <f32file.h>
#endif
#include <uloggerclient.h>
//C.A. previously:using namespace UTF;
using namespace Ulogger; //CA:added so can use old ulogger api
/**
This is a user-side utility class to start and configure ulogger for the various upt tests

@internalTechnology
@prototype
 */

class CUptULogger: public CBase
	{
public:
	CUptULogger();
	~CUptULogger();
	
	TInt StartFileL(RULogger& aLogger, const TBool& aGroupIdFiltering, const TBool& aComponentIdFiltering, const TBool& aGroupIdFilterMatch, const TBool& aComponentIdFilterMatch);
	TInt StartGeneralL(RULogger& aLogger, const TPtrC8& aPluginName, const TPluginConfiguration& aPluginConfiguration, const TBool& aGroupIdFiltering, const TBool& aComponentIdFiltering, const TBool& aGroupIdFilterMatch, const TBool& aComponentIdFilterMatch);
	TInt GetDataL(TUint8*& aData, TUint& aSize);
	TInt StopAndDeleteFile(RULogger& aLogger);
	TInt Stop(RULogger& aLogger);
	TInt DeleteFile();
	void SetFilterL(CArrayFixFlat<TUint8>* aFilters);

	TInt Test(); 
private:	

	};

#endif


