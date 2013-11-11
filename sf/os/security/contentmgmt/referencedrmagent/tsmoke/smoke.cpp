/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <ecom/ecom.h>
#include <caf/content.h>
#include <caf/data.h>

#include "smoke.h"
#include <ecom/implementationinformation.h>

using namespace ContentAccess;

CTestConsole* CTestConsole::NewL(CConsoleBase* aCon)
{
  CTestConsole* self;
  self=new (ELeave) CTestConsole;
  self->iCon=aCon;
  self->iFile=NULL;
  return self;
}

CTestConsole::CTestConsole(void):CConsoleBase()
{}

CTestConsole::~CTestConsole(void)
{
  delete iCon;
  if (iFile)
    {
      iFile->Close();
    }
}

void CTestConsole::Write(const TDesC16& aString)
{
  iCon->Write(aString);
  if (iFile)
    {
      TUint8 space[200];
      TPtr8 ptr(space,200);
      ptr.Copy(aString);
      iFile->Write(ptr);
    }
}

void CTestConsole::SetLogFile(RFile* aFile)
{
  iFile=aFile;
}

RTest test(_L("CAF Build Test"));

void RunTest1(void)
{
  test.Start(_L(" @SYMTestCaseID:SEC-CAF-SMOKE-0001 CAF Test "));		 

  CTestConsole* con=NULL;
  TRAPD(ret, con=CTestConsole::NewL(test.Console()));
  RFs fs;
  
  fs.Connect();
  RFile* file;
  file=new (ELeave) RFile;
  
  file->Replace(fs,_L("c:\\DRMLog.txt"),EFileShareAny|EFileWrite);
  con->SetLogFile(file);
  test.SetConsole(con);
  
  const TUid KInterfaceUID = {0x101FC2CE};

  RImplInfoPtrArray aImplInfoArray;

  // do here
  test.Printf(_L("\nCAF Test Starting\n"));
  test.Getch();

  test.Printf(_L("Ready to get implementations of %08X\n"),KInterfaceUID);
  //test.Getch();

  __UHEAP_MARK; // start heap debug
  
  REComSession::ListImplementationsL(KInterfaceUID , aImplInfoArray);

  TUint count = aImplInfoArray.Count();
  CImplementationInformation *infoptr = aImplInfoArray[0];

  TBufC<30> temp = infoptr->DisplayName();
  test.Printf(_L("Found %d agent implementation(s).\n"),count);
  test.Printf(_L("Name: %S\n"),&temp);
  test.Getch();

  // Need to delete implementation array
  aImplInfoArray.ResetAndDestroy();

  /*
  TUid key;
  TAgentCreationParams params;
  params.iURIPtr    = &temp;
  params.iIntentPtr = &temp;

  // Note that we have no idea about the real type of CAgent...
    CAgent *agent = 
	reinterpret_cast<CAgent *>(REComSession::CreateImplementationL(KF32ImplementationUID, 
																   key,
																   (TAny*)&params));
  TUint major=0, minor=0;
  agent->Version(major, minor);
  test.Printf(_L("\nVersion (key %d) returned %d:%d\n"),key,major,minor);
  delete agent;
  REComSession::DestroyedImplementation(key);
  */

  _LIT(KDummyContent,"c:\\dummy.txt");
  test.Printf(_L("Hit enter to create content object (accessing %S)...\n"),&KDummyContent);
  test.Getch();

  CContent *myContent = 0; ret = 0;

  TRAP(ret, myContent = CContent::NewL(KDummyContent));
  if (ret)
	  {
	  test.Printf(_L("Content create failed (%d)..."),ret);
	  test.Getch();
	  }
  
  test.Printf(_L("Hit enter to create data object..."),ret);
  test.Getch();

  CData *myData = NULL;

  TRAP(ret, myData = myContent->OpenContentL(EPeek));

  if (ret)
	  {
	  test.Printf(_L("Data create failed (%d)..."),ret);
	  test.Getch();
	  }

  TInt mySize;
  myData->DataSizeL(mySize);
  test.Printf(_L("\nAgent reports data size as %d bytes\n"),mySize);
  test.Getch();

  test.Printf(_L("\nTest Ending\n"));
  test.Getch();
  __UHEAP_MARKEND; // end heap debug
  
  test.End();
  test.Close();
}


GLDEF_C TInt E32Main(void)
{
  CTrapCleanup* cleanup;
  cleanup = CTrapCleanup::New();
  // CActiveScheduler* s = new CActiveScheduler;
  // s->Install(s);
  
  TRAP_IGNORE(RunTest1());
  
  
  delete cleanup;
  return(KErrNone);
}
