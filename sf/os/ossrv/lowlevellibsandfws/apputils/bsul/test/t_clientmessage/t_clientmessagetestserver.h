// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test server code for validating CClientMessage Framework
// 
//

#if !defined(T_CLIENTMESSAGETESTSERVERH)
#define T_CLIENTMESSAGETESTSERVERH

#include <e32std.h>
#include <e32base.h>
#include <bsul/bsul.h>


/* These types are used in testing to pass as TPckg parameter type data
 */
enum TCMTestEnum
	{
	ETestVal0,
	ETestVal1,
	ETestVal2,
	ETestVal3
	};

struct CMTestStruct1
	{
	TInt iInt;
	TBuf<32> iDes;
	};

struct CMTestStruct2
	{
	TInt iCount;
	CMTestStruct1* iStruct;
	TInt iEnum;
	TBuf<32> iDes;
	};

enum TTestServerMessages
	{
	ETestMessage0 = 10,
	ETestMessage1,
	ETestMessage2,
	ETestMessage3,
	ETestMessage4,
	ETestMessage5,
	ETestMessage6,
	ETestMessage7,
	ETestMessage8,
	ETestMessage9,
	ETestMessage10,
	ETestMessage11,
	ETestMessage12,
	ETestMessage13,
	ETestMessage14,
	ETestMessage15,
	ETestMessage16,
	ETestMessage17,
	ETestMessage18,
	ETestMessage19,
	ETestMessage20,
	ETestMessage21,
	ETestMessage22,
	ETestMessage23,
	ETestMessage24,
	ETestMessage25,
	ETestMessage26,
	ETestMessage27,
	ETestMessage28,
	ETestMessage29,
	ETestMessage30,
	ETestMessage31,
	ETestMessage32
	
	};

enum TParameterTestFunctions
	{
	EGetIntL,
	EGetDes8L,
	EGetDes16L,
	EGetDesLengthL,
	EGetDesMaxLengthL,
	ERead8L,
	ERead16L,
	EWrite8L,
	EWrite16L
	};

/* Server class. Very basic server implementation
 */
class CClientMessageTestServer : public CServer2
	{
	
public:
	static CServer2* NewLC();
	
protected:
	virtual TInt RunError(TInt aError);
	
private:
	CClientMessageTestServer(TInt aPriority);
	void ConstructL();
	CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;
	
	};

/* Server Session class. Basic implementation of session and associated test functions
 */
class CClientMessageTestSession : public CSession2
	{
	
public:
	CClientMessageTestSession(){};
	virtual void CreateL(const CServer2& aServer);
	
	
private:
	void TestFunction0L();
	void TestFunction1L();
	void TestFunction4L();
	void TestFunction5L();
	void TestFunction6L();
	void TestFunction7L();
	void TestFunction9L();
	void TestFunction10L();
	void TestFunction11L();
	void TestFunction14L();
	void TestFunction15L();
	void TestFunction16L();
	void TestFunction17L();
	void TestFunction18L();
	void TestFunction19L();
	void TestFunction31L();
	
private:
	~CClientMessageTestSession();
	void ServiceL(const RMessage2& aMessage);
	void DoServiceL();
	BSUL::CClientMessage* iClientMessage;
	
	};


/* Server class. Very basic server implementation
 */
class CClientMessageTestServer2 : public CServer2
	{
	
public:
	static CServer2* NewLC();
	
protected:
	virtual TInt RunError(TInt aError);
	
private:
	CClientMessageTestServer2(TInt aPriority);
	void ConstructL();
	CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;
	
	};

/* Server Session class. Basic implementation of session and associated test functions
 */
class CClientMessageTestSession2 : public CSession2
	{
	
public:
	CClientMessageTestSession2(){};
	virtual void CreateL(const CServer2& aServer);
	
private:
	void TestFunction0L();
	void TestFunction1L();
	void TestFunction2L();
	
private:
	~CClientMessageTestSession2();
	void ServiceL(const RMessage2& aMessage);
	void DoServiceL();
	BSUL::CClientMessage* iClientMessage;
	};





#endif
