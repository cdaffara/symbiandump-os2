/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include <test/testexecutelog.h>
#include <apmstd.h>
#include <f32file.h>
#include <e32debug.h>
#include <bautils.h>

#include "CleanupStep.h"
#include "cafserver.h"
#include "manager.h"
#include "dirstreamable.h"
#include "virtualpathptr.h"
#include "agent.h"
#include "attributeset.h"
#include "stringattributeset.h"
#include "contentIterator.h"

using namespace ContentAccess;

_LIT(KDelDirTCAF,"C:\\tcaf\\");
_LIT(KDelDirTCAFSub,"C:\\tcaf\\testdata\\");
_LIT(KDelDirTCAFSub2,"C:\\tcaf\\testdata\\TestAgentPrivateDir\\");
_LIT(KDelDirTCAFSub3,"C:\\tcaf\\tmtputils\\");

_LIT(KDelDirTSCAF,"C:\\tcaf\\tscaf\\");
_LIT(KDelDirTSCAFSub,"C:\\tcaf\\tscaf\\data\\");

_LIT(KDelDirRTA,"C:\\rta\\");
_LIT(KDelDirRTASub,"C:\\rta\\testdata\\");

/* 
 * This step deletes the test folders and files used in the CAF and RTA tests.
 */

CCAFTestCleanupStep::~CCAFTestCleanupStep()
	{
	}

CCAFTestCleanupStep::CCAFTestCleanupStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFTestCleanupStep);
	}

TVerdict CCAFTestCleanupStep::doTestStepL()
	{
	__UHEAP_MARK;
	
	SetTestStepResult(EPass);
	TInt err;  
	
	RFs fs;
	fs.Connect();
	CFileMan* fileMan = CFileMan::NewL(fs);
	CleanupStack::PushL(fileMan);

    // Make the files writeable 
	MakeFilesWritable(*fileMan, KDelDirTCAFSub2());
	MakeFilesWritable(*fileMan, KDelDirTCAFSub());
	MakeFilesWritable(*fileMan, KDelDirTCAFSub3());
	MakeFilesWritable(*fileMan, KDelDirTSCAFSub());
	MakeFilesWritable(*fileMan, KDelDirTSCAF());
	MakeFilesWritable(*fileMan, KDelDirTCAF());
    
    // Delete directory using CFileMan
    err = fileMan->RmDir(KDelDirTCAF);
    RDebug::Print(_L("CFileMan Delete file %S - err = %d\n"), &KDelDirTCAF, err);
    if((err!=KErrNone) && (err !=KErrPathNotFound))
   		SetTestStepResult(EFail);

    // Make the files writeable 
    MakeFilesWritable(*fileMan, KDelDirRTASub());
    MakeFilesWritable(*fileMan, KDelDirRTA());
    
    // Delete directory using CFileMan
   	err = fileMan->RmDir(KDelDirRTA); 
    RDebug::Print(_L("CFileMan Delete file %S - err = %d\n"), &KDelDirRTA, err); 
    if((err!=KErrNone) && (err !=KErrPathNotFound))
 	  	SetTestStepResult(EFail);
    
	CleanupStack::PopAndDestroy(fileMan); 
    fs.Close();
   			
	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CCAFTestCleanupStep::MakeFilesWritable(CFileMan& aFileMan, const TDesC& aDir)
	{
	TInt err = aFileMan.Attribs(aDir, 0, KEntryAttReadOnly, TTime(0), 0);
    if((err!=KErrNone) && (err !=KErrPathNotFound))
   		SetTestStepResult(EFail);
	}

