/*
 ============================================================================
 Name		: T_LangUtilsServer.h
 Author	  : Yanchun Li
 Copyright   : Your copyright notice
 Description : Exe header file
 ============================================================================
 */

#ifndef __T_LANGUTILSSERVER_H__
#define __T_LANGUTILSSERVER_H__

//  Include Files

#include <test/testexecuteserverbase.h>

class CT_LangUtilsServer : public CTestServer
    {
public:
    static CT_LangUtilsServer* NewL();
//from  CTestServer
    virtual CTestStep* CreateTestStep(const TDesC& aStepName);
    };

#endif  // __T_LANGUTILSSERVER_H__

