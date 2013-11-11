// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// Centrep SWI test step declaration
// 
//

#ifndef __CHECK_PLUGIN_H__
#define __CHECK_PLUGIN_H__

#include <test/testexecutestepbase.h>

#include <e32base.h>
#include "common.h"
#include "TE_EComSWITestSuiteStepBase.h"
#include <ecom/ecom.h>
#include "CEComSwiPluginInterfaceDefiniton.h"


const TInt KBufSize=64;


class CCheckListImplementation : public CTe_EComSWITestSuiteStepBase
	{
public:	
		enum TOperationType {EUseListImplUid};

    	CCheckListImplementation(TOperationType aOperationType);
    	~CCheckListImplementation();
		
		virtual TVerdict doTestStepPreambleL();
		virtual TVerdict doTestStepPostambleL();
		virtual TVerdict doTestStepL();
	
private:
		TOperationType		iOperationType;
		
		RImplInfoPtrArray	iImplyArrayPtr;
		TUid 				iInterfaceUid;
		
		// Plugin Implementation Properties
		TUid 				iImplUid;
		TInt				iImplCount;
		TInt				iVersion;
		TDriveUnit			iDrive;
		TUid				iVendorID;
	};


class CCheckCreateImplementation : public CTe_EComSWITestSuiteStepBase
	{
public:	
	enum TOperationType {EUseImplUidAndDtorID};

    CCheckCreateImplementation(TOperationType aOperationType);
    ~CCheckCreateImplementation();
		
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	
private:
	TOperationType		iOperationType;
	TUid 				iImplUid;

	TUid				iDtorIDKey;
	
//	TBuf<KBufSize>		iClassImplPluginName;
	TInt 				iInt;
	};

// Test step to issue a custom resolver list request.
// The custom resolver used will be cached by ECOM
class CUseCustomResolver : public CTe_EComSWITestSuiteStepBase
	{
public:	
    CUseCustomResolver();
    ~CUseCustomResolver();
	virtual TVerdict doTestStepL();
	};


//INI File varables
_LIT(KInterfaceUID, "InterfaceUID");
_LIT(KImplUID, "ImplUID");
_LIT(KExpectedCount, "Count");
_LIT(KExpectedVersion, "Version");
_LIT(KExpectedDrive, "DriveLetter");
_LIT(KExpectedVendorID, "VendorID");
_LIT(KExpectedErrorCheckPlugin, "ExpectedError");

//_LIT(KPluginClassName, "PluginClass");
_LIT(KExpectedPluginInt, "PluginInt");


//Test Steps for ListImplementationL
_LIT(KListImplUidStep, "CheckListImplUidStep");

//Test Steps for CreatImplementationL
_LIT(KCreateImpl_ImpId_DtorId_Step, "CheckCreateImpl_ImpId_DtorId_Step");

//Test Steps for custom resolver cache
_LIT(KUseCustomResolverStep, "UseCustomResolverStep");


//Plugin Names
//_LIT(KPlugin1, "CEComSwiPluginInterfaceImplementationOne");



#endif //  __CHECK_PLUGIN_H__
