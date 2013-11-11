/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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



#ifndef __TESTMSGQUEUE_H__
#define __TESTMSGQUEUE_H__

#include <test/testexecutestepbase.h>
#include <e32svr.h>
#include <sys/msg.h>
#include <errno.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ipc.h>

// MACROS
#define MAX_IPC_OBJ 1024

_LIT(KMessageQueueCreate,"MessageQueueCreate");
_LIT(KMessageQueueControl,"MessageQueueControl");
 
_LIT(KIntgTest1,"IntgTest1");
_LIT(KIntgTest2,"IntgTest2");
_LIT(KIntgTest3,"IntgTest3");
_LIT(KIntgTest4,"IntgTest4");
_LIT(KIntgTest5,"IntgTest5");
_LIT(KIntgTest6,"IntgTest6");
_LIT(KIntgTest7,"IntgTest7");
_LIT(KIntgTest8,"IntgTest8");
_LIT(KIntgTest9,"IntgTest9");
_LIT(KIntgTest10,"IntgTest10");
_LIT(KIntgTest11,"IntgTest11");
_LIT(KIntgTest12,"IntgTest12");
_LIT(KIntgTest13,"IntgTest13");
_LIT(KIntgTest14,"IntgTest14");
_LIT(KIntgTest15,"IntgTest15");
_LIT(KIntgTest16,"IntgTest16");
_LIT(KIntgTest17,"IntgTest17");
_LIT(KIntgTest6_1,"IntgTest6_1");

_LIT(Kmsggettest1,"msggettest1");
_LIT(Kmsggettest2,"msggettest2");
_LIT(Kmsggettest3,"msggettest3");
_LIT(Kmsggettest4,"msggettest4");
_LIT(Kmsggettest5,"msggettest5");
_LIT(Kmsggettest6,"msggettest6");
_LIT(KMsggetCreateKey,"MsggetCreateKey");
_LIT(Kmsgctltest1,"msgctltest1");
_LIT(Kmsgctltest2,"msgctltest2");
_LIT(Kmsgctltest3,"msgctltest3");
_LIT(Kmsgctltest4,"msgctltest4");
_LIT(Kmsgctltest5,"msgctltest5");
_LIT(Kmsgsndtest1,"msgsndtest1");
_LIT(Kmsgsndtest2,"msgsndtest2");
_LIT(Kmsgsndtest3,"msgsndtest3");
_LIT(Kmsgsndtest4,"msgsndtest4");
_LIT(Kmsgsndtest5,"msgsndtest5");
_LIT(Kmsgsndtest6,"msgsndtest6");
_LIT(Kmsgrcvtest1,"msgrcvtest1");
_LIT(Kmsgrcvtest2,"msgrcvtest2");
_LIT(Kmsgrcvtest3,"msgrcvtest3");
_LIT(Kmsgrcvtest4,"msgrcvtest4");
_LIT(Ksndrcvtest1,"sndrcvtest1");
_LIT(Ksndrcvtest2,"sndrcvtest2");
_LIT(Ksndrcvtest3,"sndrcvtest3");
_LIT(Ksndrcvtest4,"sndrcvtest4");
_LIT(Kthsndrcvtest1,"thsndrcvtest1");
	
class CTestMsgqueue : public CTestStep
	{
public:
	~CTestMsgqueue(); 
	CTestMsgqueue(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	
	TInt IntgTest1();
	TInt IntgTest2();
	TInt IntgTest3();
	TInt IntgTest4();
	TInt IntgTest5();
	TInt IntgTest6();
	TInt IntgTest7();
	TInt IntgTest8();
	TInt IntgTest9();
	TInt IntgTest10();
	TInt IntgTest11();
	TInt IntgTest12();
	TInt IntgTest13();
	TInt IntgTest14();
	TInt IntgTest15();
	TInt IntgTest16();
	TInt IntgTest17();
	TInt IntgTest6_1();

	TInt msggettest1();
	TInt msggettest2();
	TInt msggettest3();
	TInt msggettest4();
	TInt msggettest5();
	TInt msggettest6();
	TInt MsggetCreateKey();
	TInt msgctltest1();
	TInt msgctltest2();
	TInt msgctltest3();
	TInt msgctltest4();
	TInt msgctltest5();
	TInt msgsndtest1();
	TInt msgsndtest2();
	TInt msgsndtest3();
	TInt msgsndtest4();
	TInt msgsndtest5();
	TInt msgsndtest6();
	TInt msgrcvtest1();
	TInt msgrcvtest2();
	TInt msgrcvtest3();
	TInt msgrcvtest4();
	TInt sndrcvtest1();
	TInt sndrcvtest2();
	TInt sndrcvtest3();
	TInt sndrcvtest4();
	TInt thsndrcvtest1();

 protected:  // Functions from base classes
    
        static void* ThreadEntryFunctionRead(void* arg);
        static void* ThreadEntryFunctionDestroyQ(void* arg);
        static void* ThreadEntryFunctionWrite(void* arg);
private:
	/**
        * Message queue create
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt MessageQueueCreate(  );
        
        /**
        * Message queue close
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt MessageQueueClose(  );
        
        /**
        * Message queue control
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt MessageQueueControl(  );
        
        /**
        * Message queue send
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt MessageQueueSend(  );
        
        /**
        * Message queue receive
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt MessageQueueRecv(  );
        
        /**
        * Create Thread
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CreateThread(  );
        
                
        /**
        * Send Message to Queue
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt SendMessage(  );
        
        /**
        * Recv Message from Queue
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt RecvMessage(  );
        
        TInt MsgKey(  TInt& aKey );
        
        TInt MsgFlag(  TInt& aFlag );
        
        TInt MsgId(  TInt& aId );
        
        TInt MsgCmd(  TInt& aCmd );
        
        TInt MsgType(  TInt& aType );
        
        TInt MsgLen(  TInt& aLen );
        
        TInt MsgSegs(  TInt& aSegs );
        
        TInt ErrCode(  TInt& aErrCode );
        
        TInt CheckMessageQueueStats(struct msqid_ds& buf);
        
        void SetMessageQueueParms(struct msqid_ds* buf);
        
     public:     // Data
        TInt iErrno;
     private:    // Data
    	TInt iParamCnt;
        TInt iBlocked;
        TInt iThreadId;
        TInt *iNoOfMsqIds;
        TInt msgq_id;
	};
#endif 
