/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <e32twin.h>
#include <c32comm.h>
#include <d32comm.h>
#include <usbman.h>

TCommNotificationPckg TheSerialConfigBuf;
TCommNotificationPckg TheUsbConfigBuf;

TCommNotificationV01 &TheSerialConfig=TheSerialConfigBuf();
TCommNotificationV01 &TheUsbConfig=TheUsbConfigBuf();
TInt TheLastError=KErrNone;

static const TInt KMaxBufSize = 4096;
const TInt KMaxDumpLength=0x100;

enum TTermPanic
	{
	EStraySignal,
	ELoadPhysicalDeviceErr,
	ELoadLogicalDeviceErr,
	EOpenErr,
	EConnectFsErr,
	ECaptureFileOpen,
	EOpenUploadFile,
	};

enum TRxMode
	{
	ENormal=0,
	ELoopBack=1,
	ECountChars=2,
	ERxOff=3,
	ECapture=128,
	};

NONSHARABLE_STRUCT(SSettings)
	{
	TBool iNotFinished;
	TBool iLocalEcho;
	TInt iAddLF;
	TBool iDump;
	TInt iDumpRepeat;
	TBuf8<KMaxDumpLength> iDumpData;
	TRxMode iRxMode;
	TInt iCharCount;
	TInt iMaxInOne;
	TInt iInfraRed;
	TBool iWaitAfterWrite;
	// Fifo
	// Brk
	};

LOCAL_D SSettings TheSettings;
LOCAL_D RUsb TheUsbServer;
LOCAL_D RCommServ TheCommServer;
LOCAL_D RComm TheUsbPort;
LOCAL_D RComm TheSerialPort;
LOCAL_D RConsole TheWindow;


#define USBCSY_NAME _L("ECACM")
#define USBPORT_NAME _L("ACM::0")
#define SERIALCSY_NAME _L("ECUART")
#define SERIALPORT_NAME _L("COMM::0")		// second comm port

#define SERIALPDD_NAME _L("EUART1")
#define SERIALLDD_NAME _L("ECOMM")
#define USBLDD_NAME _L("EUSBC")

class CDuplex;
//Base class for simplex transfer activity
class MTransferNotifier
	{
public:
	virtual void NotifyRead(TInt aErr,TInt aSize, TDes8& aBuf)=0;
	virtual void NotifyWrite(TInt aErr)=0;
	};

class CSimplex: public CActive
	{
public:
	void SetTotalAmount(TUint aTotalAmount){iTotalAmount = aTotalAmount;}
	virtual ~CSimplex(){Cancel();}
protected:
	CSimplex(CDuplex& aDuplex):
		CActive(CActive::EPriorityStandard),iDuplex(aDuplex){}
	
protected:
	TBuf8<KMaxBufSize> iBuffer;
	CDuplex& iDuplex;
	TInt iTotalAmount;
	};

class CSimplexRead: public CSimplex
	{
public:
	static CSimplexRead* NewL(CDuplex& aDuplex);
	virtual ~CSimplexRead(){}
	void SetPort(RComm* aPort){iCommPort = aPort;}
	void StartL();
protected:
	CSimplexRead(CDuplex& aDuplex):
		CSimplex(aDuplex){}
	virtual void DoCancel(void);
	void ConstructL();
	virtual void RunL();
private:
	TBuf8<KMaxBufSize> iBuffer;
	TBool iPortType;
	RComm* iCommPort;
	};

class CSimplexWrite: public CSimplex
	{
public:
	static CSimplexWrite* NewL(CDuplex& aDuplex);
	virtual ~CSimplexWrite(){}
	void SetPort(RComm* aPort){iCommPort = aPort;}
	void StartL(TDes8& abuf);
protected:
	CSimplexWrite(CDuplex& aDuplex):
		CSimplex(aDuplex){}
	virtual void DoCancel(void);
	void ConstructL();
	virtual void RunL();
private:
	TBool iPortType;
	RComm* iCommPort;
	};

class CDuplex : public CBase, MTransferNotifier
	{
public:
	// Construction
	static CDuplex* NewL();
	// Destruction
	void StartL();
	void Cancel();
	void NotifyRead(TInt aErr,TInt aSize, TDes8& aBuf);
	void NotifyWrite(TInt aErr);
	void SetTxPort(RComm* aPort){iSimplexWrite->SetPort(aPort);}
	void SetRxPort(RComm* aPort){iSimplexRead->SetPort(aPort);}
	void SetTotalAmount(TInt aTotalAmount){iSimplexWrite->SetTotalAmount(aTotalAmount);
											iSimplexRead->SetTotalAmount(aTotalAmount);}
	virtual ~CDuplex();
protected:
	// Construction
	CDuplex(){};
	void ConstructL();
private:
	CSimplexRead* iSimplexRead;
	CSimplexWrite* iSimplexWrite;
	};

class CConfigChangeNotifier: public CActive
	{
public:
	static CConfigChangeNotifier* NewL();
	~CConfigChangeNotifier(){};

	// Issue request
	void StartL();
private:
	CConfigChangeNotifier();
	virtual void DoCancel();
	virtual void RunL();
	};

class CSignalChangeNotifier: public CActive
	{
public:
	static CSignalChangeNotifier* NewL(TBool aIsusbPort);
	~CSignalChangeNotifier(){};

	// Issue request
	void StartL();
private:
	CSignalChangeNotifier(TBool aIsusbPort);
	virtual void DoCancel();
	virtual void RunL();
	const TBool iIsUsbPort;
	TUint iInSignals;
	};

class CFControlChangeNotifier: public CActive
	{
public:
	static CFControlChangeNotifier* NewL();
	~CFControlChangeNotifier(){};

	// Issue request
	void StartL();
private:
	CFControlChangeNotifier();
	virtual void DoCancel();
	virtual void RunL();
	TFlowControl iFlowControl;
	};


class CActiveConsole : public CActive
//-----------------------------------
	{
public:
	// Construction
	static CActiveConsole* NewLC();
	static CActiveConsole* NewL();
	void ConstructL();

	// Destruction
	~CActiveConsole();

	// Issue request
	void WaitForKeyPress();

private:
	// Construction
	CActiveConsole();

	// Cancel request.
	// Defined as pure virtual by CActive;
	// implementation provided by this class.
	virtual void DoCancel();

	// Service completed request.
	// Defined as pure virtual by CActive;
	// implementation provided by this class,
	virtual void RunL();

	void Start();
	void ProcessKeyPressL();

private:
	TBool iIsRunning;
	TConsoleKey iKeypress;
	TRequestStatus iBreakRequest;
	// Data members defined by this class
	CDuplex*	iUsbToSerial;
	CDuplex*	iSerialToUsb;
	CSignalChangeNotifier*	iUsbSignalChangeNotifier;
	CSignalChangeNotifier*	iSerialSignalChangeNotifier;
	CFControlChangeNotifier* 		iUsbFControlNotifier;
	CConfigChangeNotifier*	iUsbConfigChangeNotifier;
	};


