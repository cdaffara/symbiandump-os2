// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// ULogger Input Framework header file
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/

#ifndef CINPUTFRAMEWORK_H_
#define CINPUTFRAMEWORK_H_

#include <e32base.h>
#include "uloggerinputplugin.h"
#include "inputdata.h"
#include "uloggershared.h"
#include "uloggercommands.h"


namespace Ulogger
{

/**
*/
class MInputFrameworkObserver
	{
	public: 
		virtual ControlData* ProcessCommandL(TCommand aOpCode, RArray<TPtrC8> &aArguments) = 0; 
		virtual void DoPostProcessing(TCommand aCmd) = 0;
	};


/**
*/
class CInputFramework : public CActive, public MCommandImpl
	{
	friend class CUloggerServerTest4Step; //only for test purposes
	public:
		IMPORT_C static CInputFramework* NewL(MInputPlugin *aInputPlugin, const RPointerArray<TPluginConfiguration>& aInputSettings, MInputFrameworkObserver *aObserver);
		IMPORT_C static CInputFramework* NewLC(MInputPlugin *aInputPlugin, const RPointerArray<TPluginConfiguration>& aInputSettings, MInputFrameworkObserver *aObserver);
		IMPORT_C virtual ~CInputFramework();
		IMPORT_C TInt StartReading();
		IMPORT_C void StopReading();
		
	public: //from MCommandImpl
		IMPORT_C TInt DoCommandL(TCommand aCommand, const RArray<TPtrC>& aValues);
	
	protected: //from CActive
		void DoCancel();
		void RunL();

	private:
		CInputFramework(MInputPlugin *aInputPlugin, MInputFrameworkObserver *aObserver);
		void ConstructL(const RPointerArray<TPluginConfiguration>& aInputSettings);
		static void CleanupHBufCPtrArray(TAny* aPtr);
		static void CleanupHBufC8PtrArray(TAny* aPtr);
		static void CleanupControlData(TAny* aPtr);

	private://data
		CInputData* 			iInputData;
		RPointerArray<TPluginConfiguration> iInputSettings;
		MInputPlugin 			*iInputPlugin;
		RBuf8					iControlData;
		HBufC8*					iData;
		TPtr8					iDataPtr;
		TBool					iContinueReading;
		MInputFrameworkObserver* iObserver;
		CCommand*				iCommand;
	};

}

#endif /*CINPUTFRAMEWORK_H_*/
