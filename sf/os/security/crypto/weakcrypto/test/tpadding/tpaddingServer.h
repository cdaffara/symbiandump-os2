/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* PaddingServer.h
*
*/



#if (!defined __PADDING_SERVER_H__)
#define __PADDING_SERVER_H__
#include <test/testexecuteserverbase.h>

class CPaddingServer : public CTestServer
{
  public:
   static CPaddingServer* NewL();
   virtual CTestStep* CreateTestStep(const TDesC& aStepName);
   RFs& Fs(){return iFs;};
   
  private:
   RFs iFs;
};
#endif

