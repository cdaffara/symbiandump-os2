// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Actually - it has nothing to do with the heap.
// It tests the case when we have a pointer to an interface, implemented
// by the dll and dll is upgraded, the pointer needs to point to the upgraded version
// 
//

#include <swi/launcher.h>
#include <e32test.h>
#include <f32file.h>
#include <bautils.h>
#include <ecom/ecom.h>
#include "EComUidCodes.h"
#include "HeapTestInterface.h"
#include "tui.h"




using namespace Swi;
        
LOCAL_D RTest test(_L("t_heaptest.exe"));
_LIT(KSisFilePath,"Z:\\HeapTestImpl\\HeapTestImpl.sis");
_LIT(KSisFilePath1,"Z:\\HeapTestImpl\\HeapTestImpl1.sis");

  /**
  @SYMTestCaseID          SYSLIB-ECOM-CT-0653
  @SYMTestCaseDesc			Tests the case when we have a pointer to an interface, implemented
							by the dll and dll is upgraded, the pointer needs to point to the upgraded version
  @SYMTestPriority 	    High
  @SYMTestActions  	    Query the interface,check the implementation UID,change its timestamp
                          replace with newer version(timestamp).
  @SYMTestExpectedResults The test must not fail.
  @SYMREQ                 REQ0000
 */


void DoTestsL()
	{

		TPtrC str(KSisFilePath);
		TPtrC str1(KSisFilePath1);
		TFileName sisfilename;
		sisfilename.Copy(str);
		TUI *Ui = new(ELeave) TUI;
		CleanupStack::PushL(Ui);

		CInstallPrefs* prefs = CInstallPrefs::NewLC();
			
		
		REComSession& session = REComSession::OpenL();
		TRequestStatus status;
		session.NotifyOnChange(status);
		
		RDebug::Printf("installing sis file base version:");
		//install HeapTestImpl.sis
		TInt err=Launcher::Install(*Ui, sisfilename,*prefs);
		test(err==KErrNone);
		User::WaitForRequest(status);
		

		//Create the interface
		CHeapTestInterface* intrf = CHeapTestInterface::NewL();
		intrf->DisplayMessage();
		RDebug::Printf("installed HeapTestImpl.sis- Base version");
		delete intrf;
		
		//This API is needed to install the upgraded plugin only on winscw
		REComSession::FinalClose() ;

		sisfilename.Copy(str1);
		RDebug::Printf("installing HeapTestImpl.sis- Upgraded  version");
		//install HeapTestImpl1.sis which is upgraded plugin
		err=Launcher::Install(*Ui, sisfilename,*prefs);
		test(err==KErrNone);  	
		
		CHeapTestInterface* intrf1 = CHeapTestInterface::NewL();
		intrf1->DisplayMessage();
		RDebug::Printf("installed HeapTestImpl.sis- Upgraded  version");
		delete intrf1;
	
		CleanupStack::PopAndDestroy(2);
		session.Close();
		REComSession::FinalClose() ;
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Title();
	test.Start(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0653 HeapTest. "));
	CTrapCleanup* cleanup = CTrapCleanup::New();

	TRAPD(err, ::DoTestsL());
	delete cleanup;
	test(err==KErrNone);
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return err;
	}

