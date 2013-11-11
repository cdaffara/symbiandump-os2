/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32test.h>
#include <ct.h>
#include <f32file.h>
#include "MTestInterface.h"
#include <ecom/ecom.h>

RTest test(_L("CT Framework Tests"));



TBool gLogging=ETrue;
TBool gSilent=EFalse;

const TInt gInterfaceA = 0x101f4e50;
const TInt gInterfaceB = 0x101f4e51;
const TInt gInterfaceC = 0x101f4e52;

const TInt gAttribute1 = 0x101f4e4a;
const TInt gAttribute2 = 0x101f4e4b;

const TInt gImplementation6 = 0x101f4e4c;
const TInt gImplementation5 = 0x101f4e4d;

class CTestConsole:public CConsoleBase

	{
	public:
		static CTestConsole* NewL(CConsoleBase* aCon);
		TInt Create(const TDesC16& aTitle,TSize aSize) {return iCon->Create(aTitle,aSize);};
		void Read(TRequestStatus& aStatus) {iCon->Read(aStatus);};
		void ReadCancel(void) {iCon->ReadCancel();};
		void Write(const TDesC16& aString);
		TPoint CursorPos(void) const {return iCon->CursorPos();};
		void SetCursorPosAbs(const TPoint& aPos) {iCon->SetCursorPosAbs(aPos);};
		void SetCursorPosRel(const TPoint& aPos) {iCon->SetCursorPosRel(aPos);};
		void SetCursorHeight(TInt aHeight) {iCon->SetCursorHeight(aHeight);};
		void SetTitle(const TDesC16& aTitle) {iCon->SetTitle(aTitle);};
		void ClearScreen(void) {iCon->ClearScreen();};
		void ClearToEndOfLine(void) {iCon->ClearToEndOfLine();};
		TSize ScreenSize(void) const {return iCon->ScreenSize();};
		TKeyCode KeyCode(void) const {return iCon->KeyCode();};
		TUint KeyModifiers(void) const {return iCon->KeyModifiers();};
		~CTestConsole(void);
		void SetLogFile(RFile* aFile);
	private:
		CTestConsole(void);
		CConsoleBase* iCon;
		RFile* iFile;
	};

CTestConsole* CTestConsole::NewL(CConsoleBase* aCon)

	{
	CTestConsole* self;
	self=new (ELeave) CTestConsole;
	self->iCon=aCon;
	self->iFile=NULL;
	return self;
	}

CTestConsole::CTestConsole(void):CConsoleBase()

	{
	}

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
	if (gSilent)
		return;
	iCon->Write(aString);
	if ((iFile)&&(gLogging))
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

template <class T> class TTestArray
	{
public:
	TTestArray(T* aArray, TInt aCount);
	TArray<T> Array();
private:
	static TInt Count(const CBase* aThat);
	static const TAny* Get(const CBase* aThat, TInt aIndex);

	T* iArray;
	TInt iCount;
	};

template <class T> TTestArray<T>::TTestArray(T* aArray, TInt aCount)
		: iArray(aArray), iCount(aCount)
	{	
	}

template <class T> TInt TTestArray<T>::Count(const CBase* aThat)
	{
	return reinterpret_cast<const TTestArray*>(aThat)->iCount;
	}

template <class T> const TAny* TTestArray<T>::Get(const CBase* aThat, TInt aIndex)
	{
	return &reinterpret_cast<const TTestArray*>(aThat)->iArray[aIndex];
	}

template <class T> TArray<T> TTestArray<T>::Array()
	{
	return TArray<T>(Count, Get, reinterpret_cast<CBase*>(this));
	}

/** A filter that includes nothing (as a random easy-to-write test filter */
class TNoTokenTypes : public MCTTokenTypeFilter
	{
 public:
	/** Always returns EFalse. */
	virtual TBool Accept(const CCTTokenTypeInfo& aTokenType) const;
	};

// Accept everything
TBool TNoTokenTypes::Accept(const CCTTokenTypeInfo&) const
	{
	return EFalse;
	}

class CStopScheduler : public CActive
	{
public:
	void DoCancel() {};
	void RunL();
	CStopScheduler();
	};

void CStopScheduler::RunL()
	{
	CActiveScheduler::Stop();
	};

CStopScheduler::CStopScheduler()
		: CActive(EPriorityIdle)
	{
	CActiveScheduler::Add(this);
	SetActive();
	TRequestStatus* r = &iStatus;
	User::RequestComplete(r, KErrNone);
	}

TInt TokenTypeInfoListTestsL()
	{
	test.Printf(_L("1.1,Testing getting all token types,"));
	RCPointerArray<CCTTokenTypeInfo> myTokenTypes;
	CleanupClosePushL(myTokenTypes);

	CCTTokenTypeInfo::ListL(myTokenTypes);

	if (myTokenTypes.Count() < 6)
		{
		CleanupStack::PopAndDestroy();
		test.Printf(_L("FAILED\r\n"));
		return (1);
		}

	myTokenTypes.ResetAndDestroy();
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("1.2,Testing user-supplied filter,"));
	TNoTokenTypes filter;
	CCTTokenTypeInfo::ListL(myTokenTypes, filter);

	if (myTokenTypes.Count() > 0)
		{
		CleanupStack::PopAndDestroy();
		test.Printf(_L("FAILED\r\n"));
		return (2);
		}

	myTokenTypes.ResetAndDestroy();
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("1.3,Testing finding filters matching 1 interface,"));
	RArray<TUid> a;
	TUid aa = {gInterfaceA};
	User::LeaveIfError(a.Append(aa));
	CleanupClosePushL(a);
	TCTFindTokenTypesByInterface findA(a.Array());
	CCTTokenTypeInfo::ListL(myTokenTypes, findA);
	if (myTokenTypes.Count() != 4)
		{
		CleanupStack::PopAndDestroy(2);
		test.Printf(_L("FAILED\r\n"));
		return (3);
		}
	TInt findAResults[] = {1,4,5,6};
	TInt flag[] = {1,1,1,1};
	for (TInt ii = 0; ii < 4; ii++)
		{
		TInt count = 0;
		TInt iii=0;
		TBuf<20> matchString;
		_LIT(format, "Test Token Type %d");
		matchString.Format(format, findAResults[iii]);
		TBuf<20> idisplay_name= myTokenTypes[ii]->Label();
        
        if (matchString.Compare(idisplay_name))
        	count++;
        else
        	{
        	if(flag[0]==1)
        		flag[0]=0;	
        	else
        		count++;
        	}
        matchString.Format(format, findAResults[iii+1]);
        if (matchString.Compare(idisplay_name))
        	count++;
        else
        	{
        	if(flag[1]==1)
        		flag[1]=0;	
        	else
        		count++;
        	}
        matchString.Format(format, findAResults[iii+2]);
        if (matchString.Compare(idisplay_name))
        	count++;
        else
        	{
        	if(flag[2]==1)
        		flag[2]=0;	
        	else
        		count++;
        	}
        matchString.Format(format, findAResults[iii+3]);
        if (matchString.Compare(idisplay_name))
        	count++;
        else
        	{
        	if(flag[3]==1)
        		flag[3]=0;	
        	else
        		count++;
        	}
        if(count==4)
        	{
        	CleanupStack::PopAndDestroy(2);
			test.Printf(_L("FAILED\r\n"));
			return (4);
        	}
       } 
			
	myTokenTypes.ResetAndDestroy();
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("1.5,Testing finding filters matching 2 interfaces,"));
	TUid aAndB[] = {{gInterfaceA}, {gInterfaceB}};
	TTestArray<TUid> aAndBArray(aAndB, 2);
	TCTFindTokenTypesByInterface findAAndB(aAndBArray.Array());
	CCTTokenTypeInfo::ListL(myTokenTypes, findAAndB);
	if (myTokenTypes.Count() != 2)
		{
		CleanupStack::PopAndDestroy(2);
		test.Printf(_L("FAILED\r\n"));
		return (5);
		}
	TInt findAAndBResults[] = {4,6};
	TInt flag1[] = {1,1};
	for (TInt jj = 0; jj < 2; jj++)
		{
		TInt count = 0;
		TInt jjj=0;
		TBuf<20> matchString;
		_LIT(format, "Test Token Type %d");
		matchString.Format(format, findAAndBResults[jjj]);
		TBuf<20> idisplay_name = myTokenTypes[jj]->Label();
		if (matchString.Compare(idisplay_name))
        	count++;
        else
        	{
        	if(flag1[0]==1)
        		flag1[0]=0;	
        	else
        		count++;
        	}
        matchString.Format(format, findAAndBResults[jjj+1]);
        if (matchString.Compare(idisplay_name))
        	count++;
        else
        	{
        	if(flag1[1]==1)
        		flag1[1]=0;	
        	else
        		count++;
        	}
        if(count==2)
        	{
        	CleanupStack::PopAndDestroy(2);
			test.Printf(_L("FAILED\r\n"));
			return (6);
        	}
        }
		
	// NOTE No ResetAndDestroy
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("1.6,Testing appending interface infos,"));
	CCTTokenTypeInfo::ListL(myTokenTypes, findA);
	if (myTokenTypes.Count() != 6)
		{
		CleanupStack::PopAndDestroy(2);
		test.Printf(_L("FAILED\r\n"));
		return (7);
		}
	TInt flag2[] = {1,1,1,1};
	for (TInt kk = 0; kk < 4; kk++)
		{
		TInt count = 0;
		TInt kkk=0;
		TBuf<20> matchString;
		_LIT(format, "Test Token Type %d");
		matchString.Format(format, findAResults[kkk]);
		TBuf<20> idisplay_name = myTokenTypes[kk+2]->Label();
		if (matchString.Compare(idisplay_name))
        	count++;
        else
        	{
        	if(flag2[0]==1)
        		flag2[0]=0;	
        	else
        		count++;
        	}
        matchString.Format(format, findAResults[kkk+1]);
        if (matchString.Compare(idisplay_name))
        	count++;
        else
        	{
        	if(flag2[1]==1)
        		flag2[1]=0;	
        	else
        		count++;
        	}
        matchString.Format(format, findAResults[kkk+2]);
        if (matchString.Compare(idisplay_name))
        	count++;
        else
        	{
        	if(flag2[2]==1)
        		flag2[2]=0;	
        	else
        		count++;
        	}
        matchString.Format(format, findAResults[kkk+3]);
        if (matchString.Compare(idisplay_name))
        	count++;
        else
        	{
        	if(flag2[3]==1)
        		flag2[3]=0;	
        	else
        		count++;
        	}
        if(count==4)
        {
        	CleanupStack::PopAndDestroy(2);
			test.Printf(_L("FAILED\r\n"));
			return (8);
        }
     } 
		
	test.Printf(_L("PASSED\r\n"));

	myTokenTypes.ResetAndDestroy();
	test.Printf(_L("1.7,Testing filtering by interface and attribute,"));
	TCTTokenTypeAttribute att = {{gAttribute1}, 1};
	RArray<TCTTokenTypeAttribute> attArray(sizeof(TCTTokenTypeAttribute), &att,
										   1);
	TCTFindTokenTypesByInterfaceAndAttribute findAWithAtt1(a.Array(),
														   attArray.Array());
	CCTTokenTypeInfo::ListL(myTokenTypes, findAWithAtt1);
	if (myTokenTypes.Count() != 1)
		{
		CleanupStack::PopAndDestroy(2);
		test.Printf(_L("FAILED\r\n"));
		return (9);
		}
	_LIT(KMatch, "Test Token Type 6");
	if (myTokenTypes[0]->Label() != KMatch)
		{
		CleanupStack::PopAndDestroy(2);
		test.Printf(_L("FAILED\r\n"));
		return (10);
		}
	test.Printf(_L("PASSED\r\n"));

	CleanupStack::PopAndDestroy(2);
	return KErrNone;
	};

TInt TokenTypeInfoTestsL()
	{
	test.Printf(_L("2.1,Testing token type for tests,"));
	RCPointerArray<CCTTokenTypeInfo> myTokenTypes;
	CleanupClosePushL(myTokenTypes);
	TUid aABndC[] = {{gInterfaceA}, {gInterfaceB}, {gInterfaceC}};
	TTestArray<TUid> aABndCArray(aABndC, 3);
	TCTFindTokenTypesByInterface findABAndC(aABndCArray.Array());
	CCTTokenTypeInfo::ListL(myTokenTypes, findABAndC);
	if (myTokenTypes.Count() != 1)
		{
		CleanupStack::PopAndDestroy();
		test.Printf(_L("FAILED\r\n"));
		return (1);
		}
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("2.2,Test UID function,"));
	CCTTokenTypeInfo* info = myTokenTypes[0];	
	if (info->Type().iUid != gImplementation6)
		{
		CleanupStack::PopAndDestroy();
		test.Printf(_L("FAILED\r\n"));
		return (1);
		}
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("2.3,Test getting an array of interfaces,"));
	RArray<TUid> interfaces = info->Interfaces();
	if (interfaces.Count() != 3)
		{
		CleanupStack::PopAndDestroy();
		test.Printf(_L("FAILED\r\n"));
		return (4);
		}
	TUid refInterfaces[] = {{gInterfaceA}, {gInterfaceB}, {gInterfaceC}};
	for (TInt ii = 0; ii < 3; ii++)
		{
		if (interfaces[ii] != refInterfaces[ii])
			{
			CleanupStack::PopAndDestroy();
			test.Printf(_L("FAILED\r\n"));
			return (5);
			}
		}
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("2.4,Test getting an array of attributes,"));
	RArray<TCTTokenTypeAttribute> attributes = info->Attributes();
	if (attributes.Count() != 2)
		{
		CleanupStack::PopAndDestroy();
		test.Printf(_L("FAILED\r\n"));
		return (8);
		}
	TCTTokenTypeAttribute refAttributes[] =
		{{{gAttribute2}, 2}, {{gAttribute1}, 1}};
	for (TInt jj = 0; jj < 2; jj++)
		{
		if (attributes[jj].iUID != refAttributes[jj].iUID ||
			attributes[jj].iVal != refAttributes[jj].iVal)
			{
			CleanupStack::PopAndDestroy();
			test.Printf(_L("FAILED\r\n"));
			return (9);
			}
		}
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("2.5,Test getting the label,"));
	_LIT(KLabel, "Test Token Type 6");
	if (info->Label() != KLabel)
		{
		CleanupStack::PopAndDestroy();
		test.Printf(_L("FAILED\r\n"));
		return (10);
		}
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("2.6,Test getting the type of the token type,"));
	TUid impl6 = {gImplementation6};
	if (info->Type() != impl6)
		{
		CleanupStack::PopAndDestroy();
		test.Printf(_L("FAILED\r\n"));
		return (11);
		}
	test.Printf(_L("PASSED\r\n"));

	CleanupStack::PopAndDestroy();
	return KErrNone;
	}


TInt TestTokenTypeL(MCTTokenType* aTokenType, TInt aNum)
	{
	test.Printf(_L("3.4.1,Getting token info,"));
	RCPointerArray<HBufC> tokenInfo;
	CleanupClosePushL(tokenInfo);
	TRequestStatus status = KRequestPending;
	aTokenType->List(tokenInfo, status);
	// Cancel call is pointelss as it does nothing in the test
	// plugin. But it won't do any harm to check it doesn't explode.
	aTokenType->CancelList();
	User::WaitForRequest(status);
	if (status.Int() != KErrNone || tokenInfo.Count() != 1)
		{
		CleanupStack::PopAndDestroy();
		test.Printf(_L("FAILED\r\n"));
		return (100);
		}
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("3.4.1,Checking token type name,"));
	TBuf<20> buf;
	_LIT(KLabelFormat, "Test Token Type %d");
	buf.Format(KLabelFormat, aNum);
	if (aTokenType->Label().Compare(buf))
		{
		CleanupStack::PopAndDestroy();
		test.Printf(_L("FAILED\r\n"));
		return (105);
		}
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("3.4.2,Checking token type Type UID,"));
	TUid uid = {gImplementation5 + 5 - aNum};
	if (aTokenType->Type() != uid)
		{
		CleanupStack::PopAndDestroy();
		test.Printf(_L("FAILED\r\n"));
		return (105);
		}
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("3.4.3,Checking token name,"));
	_LIT(KFormat, "Test Token %d");
	buf.Format(KFormat, aNum);
	if (buf.Compare(*tokenInfo[0]))
		{
		CleanupStack::PopAndDestroy();
		test.Printf(_L("FAILED\r\n"));
		return (101);
		}
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("3.4.4,Opening token,"));
	status = KRequestPending;
	MCTToken* token;
	aTokenType->OpenToken(*tokenInfo[0], token, status);
	// Cancel call will fail as token isn't opened asynchronous. Check
	// that the reference counting still works.
	aTokenType->CancelOpenToken();
	User::WaitForRequest(status);
	if (status.Int() != KErrNone)
		{
		CleanupStack::PopAndDestroy();
		test.Printf(_L("FAILED\r\n"));
		return (102);
		}
	token->Release();
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("3.4.5,Opening token by handle,"));
	status = KRequestPending;
	TCTTokenHandle handle(aTokenType->Type(), 1);
	aTokenType->OpenToken(handle, token, status);
	User::WaitForRequest(status);
	if (status.Int() != KErrNone)
		{
		CleanupStack::PopAndDestroy();
		test.Printf(_L("FAILED\r\n"));
		return (102);
		}
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("3.4.6,Checking token's TokenType() function,"));
	if (&token->TokenType() != aTokenType)
		{
		CleanupStack::PopAndDestroy();
		test.Printf(_L("FAILED\r\n"));
		return (102);
		}
	test.Printf(_L("PASSED\r\n"));

	_LIT(KVersion, "The Ultimate Version");
	_LIT(KSerial, "Serial No. 1");
	_LIT(KManufacturer, "ACME Corporation");

	test.Printf(_L("3.4.6.1,Checking token's Information() function,"));
	if (token->Information(MCTToken::EVersion) != KVersion ||
		token->Information(MCTToken::ESerialNo) != KSerial ||
		token->Information(MCTToken::EManufacturer)  != KManufacturer)
		{
		CleanupStack::PopAndDestroy();
		test.Printf(_L("FAILED\r\n"));
		return (102);
		}
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("3.4.7,Registering for removal notification,"));
	// This is another test to check that an API doesn't crash. We
	// call the base NotifyOnRemoval/CancelNotify functions and check
	// they do nothing! There's no point in creating derived versions
	// that do something as that would be testing the test plugin, not
	// the framework.
	TRequestStatus removalStatus;
	token->NotifyOnRemoval(removalStatus);
	token->CancelNotify();
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("3.4.9,Testing cancellation of async interface opening,"));
	MTestInterface* interface;
	MCTTokenInterface* temp;
	TUid intB = {gInterfaceB};
	token->GetInterface(intB, temp, status);
	token->CancelGetInterface();
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("3.4.10,Opening an interface,"));
	status = KRequestPending;
	TUid intC = {gInterfaceC};
	token->GetInterface(intC, temp, status);
	interface = static_cast<MTestInterface*>(temp);
	// Cancel call is pointelss as it does nothing in the test
	// plugin. But it won't do any harm to check it doesn't explode.
	token->CancelGetInterface();
	CleanupReleasePushL(*interface);
	token->Release();
	User::WaitForRequest(status);
	if (status.Int() != KErrNone)
		{
		CleanupStack::PopAndDestroy(2);
		test.Printf(_L("FAILED\r\n"));
		return (103);
		}
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("3.4.11,Checking interface name,"));
	if (buf.Compare(interface->Label()))
		{
		CleanupStack::PopAndDestroy(2);
		test.Printf(_L("FAILED\r\n"));
		return (104);
		}
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("3.4.12,Getting an object,"));
	MTestObject* object = interface->ObjectL();
	object->Release();
	test.Printf(_L("PASSED\r\n"));

	CleanupStack::PopAndDestroy(2);
	return KErrNone;
	}

TInt TokenTypeTestsL()
	{
	test.Printf(_L("3.1,Getting token types for tests,"));
	RFs fs;
	RCPointerArray<CCTTokenTypeInfo> myTokenTypes;
	CleanupClosePushL(myTokenTypes);
	TUid aABndC[] = {{gInterfaceA}, {gInterfaceB}, {gInterfaceC}};
	TTestArray<TUid> aABandCArray(aABndC, 3);
	TCTFindTokenTypesByInterface findABAndC(aABandCArray.Array());
	CCTTokenTypeInfo::ListL(myTokenTypes, findABAndC);
	if (myTokenTypes.Count() != 1)
		{
		CleanupStack::PopAndDestroy();
		test.Printf(_L("FAILED\r\n"));
		return (1);
		}
	test.Printf(_L("PASSED\r\n"));

	test.Printf(_L("3.2,Loading token type 6,"));
	MCTTokenType* token6 = MCTTokenType::NewL(*myTokenTypes[0], fs);
	if (!token6)
		{
		CleanupStack::PopAndDestroy();
		test.Printf(_L("FAILED\r\n"));
		return (1);
		}
	CleanupReleasePushL(*token6);
	test.Printf(_L("PASSED\r\n"));

	TInt ret = TestTokenTypeL(token6, 6);
	if (ret)
		{
		CleanupStack::PopAndDestroy(2);
		return ret;
		}

	test.Printf(_L("3.3,Loading token type 5,"));
	TUid UID5 = {gImplementation5};
	MCTTokenType* token5 = MCTTokenType::NewL(UID5, fs);
	if (!token5)
		{
		CleanupStack::PopAndDestroy(2);
		test.Printf(_L("FAILED\r\n"));
		return (2);
		}
	CleanupReleasePushL(*token5);
	test.Printf(_L("PASSED\r\n"));

	ret = TestTokenTypeL(token5, 5);
	CleanupStack::PopAndDestroy(3);

	return ret;
	}

TInt MemoryTestL(TInt (*aFnToTest)())
	{
	gLogging = EFalse;
	for (TInt ii = 1; ; ii++)
		{
		if (ii % 10)
			test.Printf(_L("."));
		else
			test.Printf(_L("*"));
		if (!(ii % 50))
			test.Printf(_L("\r\n"));
		gSilent = ETrue;
		__UHEAP_MARK;
		__UHEAP_FAILNEXT(ii);
		TRAPD(err,aFnToTest());
		__UHEAP_RESET;
		REComSession::FinalClose();
		__UHEAP_MARKEND;
		User::Heap().Check();
		gSilent = EFalse;
		if (err != KErrNoMemory)
			{
			test.Printf(_L("\r\n"));
			gLogging = ETrue;
			return err;
			}
		}
	}

void TestsL(void)
	{
	CActiveScheduler* as = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(as);

	TInt errors = 0;
	TInt ret;
	__UHEAP_MARK;
	ret = TokenTypeInfoListTestsL();
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	if (ret)
		{
		test.Printf(_L("1.9,ERROR %d in Info List test,FAILED\r\n"),ret);
		errors++;
		}
	else
		{
		test.Printf(_L("1.9,Info List test,PASSED\r\n"),ret);
		}
	__UHEAP_MARK;
	ret = TokenTypeInfoTestsL();
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	if (ret)
		{
		test.Printf(_L("2.9,ERROR %d in Info test,FAILED\r\n"),ret);
		errors++;
		}
	else
		{
		test.Printf(_L("2.9,Info test,PASSED\r\n"),ret);
		}

	__UHEAP_MARK;
	ret = TokenTypeTestsL();
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	if (ret)
		{
		test.Printf(_L("3.9,ERROR %d in token test,FAILED\r\n"),ret);
		errors++;
		}
	else
		{
		test.Printf(_L("3.9,token test,PASSED\r\n"),ret);
		}

	ret = MemoryTestL(TokenTypeInfoListTestsL);
	if (ret)
		{
		test.Printf(_L("4.9,ERROR %d in Info List memory test,FAILED\r\n"),ret);
		errors++;
		}
	else
		{
		test.Printf(_L("4.9,Info List memory test,PASSED\r\n"),ret);
		}
	ret = MemoryTestL(TokenTypeTestsL);
	if (ret)
		{
		test.Printf(_L("5.9,ERROR %d in Token Type memory test,FAILED\r\n"),ret);
		errors++;
		}
	else
		{
		test.Printf(_L("5.9,Token Type memory test,PASSED\r\n"),ret);
		}

	test.Printf(_L("%d tests failed out of 44 hardcoded\r\n"), errors);


	if (errors)
		{
		test.Printf(_L("%d TESTS FAILED\r\n"),errors);
		}
	else
		{
		test.Printf(_L("ALL TESTS PASSED\r\n"));
		}

	delete as;
	}

GLDEF_C TInt E32Main(void)

	{
	CTrapCleanup* cleanup;
	cleanup=CTrapCleanup::New();
	test.Start(_L(" @SYMTestCaseID:SEC-CRYPTOTOKENS-CTFRAMEWORK-0001 Starting token framework tests\r\n"));		 
	CTestConsole* con=NULL;
	TRAPD(ret,con=CTestConsole::NewL(test.Console()));
	RFs fs;
	if (gLogging)
		{
		User::LeaveIfError(fs.Connect());
		RFile* file;
		file=new (ELeave) RFile;
		TDriveUnit sysDrive (fs.GetSystemDrive());
		TDriveName driveName(sysDrive.Name());
		TBuf<64> logFile (driveName);
		logFile.Append(_L("\\T_CTFrameworkLog.txt"));
		User::LeaveIfError(file->Replace(fs, logFile, EFileShareAny|EFileWrite));
		con->SetLogFile(file);
		}
	test.SetConsole(con);
	TRAP(ret,TestsL());
	if (ret)
		{
		test.Printf(_L("Unexpected leave\r\n"));
		}
	gLogging=EFalse;
	test.Close();
	delete cleanup;
	return(KErrNone);
	}
