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
// Runs the example code provided in the design and how to docs with flogger.

#include <e32base.h>
#include <e32cons.h>
#include <comms-infras/commsdebugutility.h>

#ifdef _DEBUG
const TInt KFloggerFileFlushTime = 	1000000;	 //< Flogger flushes its file buffer every second
const TInt KFloggerIdleTimeWait = 1000000;	 //< A guess (!) at how long the system needs to be idle before flogger's lower-priority thread kicks in and writes the data.
const TInt KFloggerWriteTime = 500000;		///< Give flogger 1/2 second to write the data.
const TInt KTimeToLog = KFloggerFileFlushTime + KFloggerIdleTimeWait + KFloggerWriteTime;   //< 2.5-second delay used to guarantee the logger will have written to the log file before reading the message.
_LIT(KTitle1,"You have run the Comsdbg DesignExample.\n");
_LIT(KTitle2,"Make sure commsdbg.ini contains the line:\n");
_LIT(KTitle3,"LOG XComm *\n\n");
#endif
#ifndef __FLOG_ACTIVE
_LIT(KError1,"Error: You have not built this with flogger enabled.\n");
#endif
#ifdef _DEBUG
_LIT(KProgress1,"Running foo()...\n");
_LIT(KProgress2,"Running X::SetInt(5)...\n");
_LIT(KProgress3,"Running X::GetInt...\n");
_LIT(KProgressDone,"All Done. Check c:\\logs\\log.txt for results.");
#endif
_LIT(KWindowTtl,"DesignExample");
const TInt KConsWidth=50;
const TInt KConsHeight=15;
//***** Start of example code from Design Specification *****//
	
__FLOG_STMT(_LIT8(KSubsys,"XComm");)
__FLOG_STMT(_LIT8(KComponent,"Sprocket");)
__FLOG_STMT(_LIT8(KTempSubsys,"NewXComm");) // Temporary subsystem name
__FLOG_STMT(_LIT8(KTempComponent,"NewSprocket");) // Temporary component name
	
	
class X
	{
public:
	inline X();
	inline X(const X& aRhs);
	inline X& operator=(const X& aRhs);
	inline ~X();
	
	inline void ChangeTag();
	inline void RestoreTag();

	inline TUint GetInt();
	inline void SetInt(TUint aVal);
private:
	TUint iInt;
	__FLOG_DECLARATION_MEMBER;
	};

X::X() : iInt(0)
	{
	__FLOG_OPEN(KSubsys, KComponent);
	}

X::X(const X& aRhs) : iInt(aRhs.iInt)
	{
	__FLOG_OPEN(KSubsys, KComponent);
	}

X& X::operator=(const X& aRhs)
	{
	if (this != &aRhs)
	    {
	    iInt = aRhs.iInt;
    	__FLOG_OPEN(KSubsys, KComponent);
	    }
	return *this;
	}

void X::ChangeTag()
	{
	__FLOG_SET_TAGS(KTempSubsys, KTempComponent);
	__FLOG(_L8("Temporally changed the tags in the log file "));
	// The tags are changed
	}

void X::RestoreTag()
	{
	__FLOG_SET_TAGS(KSubsys, KComponent);
	__FLOG(_L8("Restored the original tags in the log file "));
	// The tags are restored 
	}


X::~X()
	{
	__FLOG_CLOSE;
	}

TUint X::GetInt()
	{
#ifdef __FLOG_ACTIVE
	_LIT(KHiThere,"hi there "); 
	_LIT8(KHiThere8,"hi there "); 
	_LIT8(KDumpPi," Pi = %d.%d%d%d%d%d%d%d%d"); 
	_LIT8(KGetInt," X::GetInt() - about to return the integer: %d ");
#endif

	//__FLOG("hi there");		//illegal - compile time error
	__FLOG(KHiThere);		//ok - but inefficient in a Unicode build
	__FLOG(KHiThere8);		//good!
	__FLOG_0(KHiThere8);		//exactly the same as the previous line
	__FLOG_9(KDumpPi, 3, 1, 4, 1, 5, 9, 2, 6, 5);

	__FLOG_1(KGetInt, iInt);
	return iInt;
	}

void X::SetInt(TUint aVal)
	{
	__FLOG_STMT(_LIT(KString, "X::SetInt - setting iInt to: ");)
	__FLOG(KString);

	__FLOG_STMT(_LIT8(KFormatForHex,"%2d");)
	__FLOG_STMT(const TInt KMaxHexString = 3;)
	__FLOG_STMT(TBuf8<KMaxHexString> buf;)
	__FLOG_STMT(buf.Format(KFormatForHex,aVal);)
	
	__FLOG_HEXDUMP (( buf ));

	iInt = aVal;
	}


__FLOG_STMT(_LIT8(KFooSubsys,"XComm");)
__FLOG_STMT(_LIT8(KFooComponent,"Foo");)
__FLOG_STMT(_LIT8(KDemoString,"Logging test");)

void foo()
	{
	__FLOG_DECLARATION_VARIABLE;
	__FLOG_OPEN(KFooSubsys, KFooComponent);
	__FLOG_VA (( KDemoString ));
	__FLOG_CLOSE;
	}

//***** End of example code from Design Specification *****//


void MainL()
{
	// Create a console for communication to user of progress	
	CConsoleBase *console=Console::NewL(KWindowTtl,TSize(KConsWidth,KConsHeight));
	__FLOG_STMT(console->Printf(KTitle1);)
	__FLOG_STMT(console->Printf(KTitle2);)
	__FLOG_STMT(console->Printf(KTitle3);)
	
#ifndef __FLOG_ACTIVE
	console->Printf(KError1);
#endif

	__FLOG_STMT(console->Printf(KProgress1));
	foo();
	
	X XTest;
	
	__FLOG_STMT(console->Printf(KProgress2);)
	XTest.SetInt(5);
	__FLOG_STMT(console->Printf(KProgress3);)
	XTest.GetInt();
	__FLOG_STMT(console->Printf(KProgressDone);)
	
	__FLOG_STMT(User::After(KTimeToLog);)
	__FLOG_STMT(User::After(KTimeToLog);)
}


GLDEF_C TInt E32Main()
	{
    TRAP_IGNORE(MainL());
    
	return KErrNone;
	}
