// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of STDLIB serialports.
// 
//

#include "FDESC.H"
#include <sys/types.h>
#include <string.h>		// for memcpy
#include <sys/errno.h>		// for ENOTSOCK
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/serial.h>
#include <c32comm.h>


//define this to allow the code to be built for ER5U without most of the notifications
//leave commented for all notifications
//#define ER5U_NOTIFICATION_SUPPORT_ONLY
#ifdef _DEBUG
_LIT(KCSerialDescPanic, "CSerialDesc");
#endif

//
//
// -----> CSerialTimer (implementation)
//
//
CSerialTimer::CSerialTimer()
	: CTimer(CActive::EPriorityHigh)
	  // Construct standard-priority active object
	{};

CSerialTimer* CSerialTimer::NewLC(CFileDescBase* aFile)
	{
	CSerialTimer* self=new (ELeave) CSerialTimer;
	CleanupStack::PushL(self);
	self->ConstructL(aFile);
	return self;
	}

CSerialTimer* CSerialTimer::NewL(CFileDescBase* aFile)
	{
	CSerialTimer* self = NewLC(aFile);
	CleanupStack::Pop();
	return self;
	}

void CSerialTimer::ConstructL(CFileDescBase* aFile)
	{
	  // Base class second-phase construction.
	CTimer::ConstructL();
	iFile = aFile;
	  // Add active object to active scheduler
	CActiveScheduler::Add(this); 
	}


CSerialTimer::~CSerialTimer()
	{
	  // Make sure we're cancelled
	Cancel();
	}

void CSerialTimer::DoCancel()
	{
	  // Base class
	CTimer::DoCancel(); 
	}

void CSerialTimer::IssueRequest()
	{
	  // There should never be an outstanding request at this point.
	__ASSERT_DEBUG(!IsActive(),User::Panic(KCSerialDescPanic,1));

	  // Request
	CTimer::After(iFile->TimeoutValue()*1000);
	}

void CSerialTimer::RunL()
	{
		//the timer has gone off.
	iFile->ReadCancel();
	iFile->TimedMessage = NULL;
	iFile->ReadIsTimed = EFalse;
	iFile->ReadWasCancelled = ETrue;
	delete this;
	}



/****/
NONSHARABLE_CLASS(CNotifier) : public CActive
	{
	public:
		CNotifier();
		~CNotifier();
		void IssueRequest(TInt aRequest, TInt* aRequestParams=NULL);
		static CNotifier* NewLC(CSerialDesc* aPort);
		static CNotifier* NewL(CSerialDesc* aPort);
		void Construct(CSerialDesc* aPort);
		void Complete(TInt aVal);

	private:
		enum RequestTypes {None, DataAvailable, OutputEmpty, Break, Signals, WriteErrors};
		void DoCancel();
		void RunL();
		CSerialDesc * iSerialPort;
		enum RequestTypes iRequest;
		TUint iRequestData;
		TInt* iRequestParams;

} ;

CNotifier::CNotifier() : CActive(CActive::EPriorityStandard), iRequest(None), iRequestData(0), iRequestParams(NULL)
	{
	}


void CNotifier::IssueRequest(TInt aRequest, TInt* aRequestParams)
	{
	iRequestParams = aRequestParams;
	
#ifndef ER5U_NOTIFICATION_SUPPORT_ONLY
	if (KNotifyDataAvailable == aRequest)
		{
		iRequest = DataAvailable;
		iSerialPort->NotifyDataAvailable(iStatus);
		}

	else if (KNotifyOutputEmpty == aRequest)
		{
		iRequest = OutputEmpty;
		iSerialPort->NotifyOutputEmpty(iStatus);
		}
	
	else if (KNotifyBreakInt == aRequest)
		{
		iRequest = Break;
		iSerialPort->NotifyBreak(iStatus);
		}
	
	else if (aRequest & (KNotifyCD|KNotifyCTS|KNotifyDSR|KNotifyRI))	//signals
		{
		TUint signalsRequested = 0;

		//build up the mask of signals to request
		if (aRequest & KNotifyCD) signalsRequested |=  KSignalDCD;
		if (aRequest & KNotifyCTS) signalsRequested |=  KSignalCTS;
		if (aRequest & KNotifyDSR) signalsRequested |=  KSignalDSR;
		if (aRequest & KNotifyRI) signalsRequested |=  KSignalRNG;
		iRequest = Signals;
		iSerialPort->NotifySignalChange(iStatus, iRequestData, signalsRequested);
		}
	
	else 
#endif	//ER5U_NOTIFICATION_SUPPORT_ONLY
		if (aRequest & (KNotifyFramingError|KNotifyOverrunError|KNotifyParityError))
		{
		iRequest = WriteErrors;
		iSerialPort->NotifyWriteErrors(iStatus, &iRequestData, aRequest);
		}
		
	SetActive();
	}

void CNotifier::RunL()
	{
	//now what!
	//cancel all the others
	//use iRequest to determine what we are doing here

	CNotifier** ppn = NULL;
	switch (iRequest)
		{
#ifndef ER5U_NOTIFICATION_SUPPORT_ONLY

		case DataAvailable:
			iSerialPort->iNotifyParamPtr[0] = KNotifyDataAvailable;
			ppn = &iSerialPort->iDataAvailableNotifier;
			break;

		case OutputEmpty:
			iSerialPort->iNotifyParamPtr[0] = KNotifyOutputEmpty;
			ppn = &iSerialPort->iOutputEmptyNotifier;
			break;

		case Break:
			iSerialPort->iNotifyParamPtr[0] = KNotifyBreakInt;
			ppn = &iSerialPort->iBreakNotifier;
			break;

		case Signals:
			{
			ppn = &iSerialPort->iSignalsNotifier;
			iSerialPort->iNotifyParamPtr[0] = 0;
			iSerialPort->iNotifyParamPtr[1] = 0;
			if (iRequestData & KDCDChanged)
				{
				iSerialPort->iNotifyParamPtr[0] |=  KNotifyCD;
				iSerialPort->iNotifyParamPtr[1] |=  (iRequestData & KSignalDCD);
				}

			if (iRequestData & KCTSChanged)
				{
				iSerialPort->iNotifyParamPtr[0] |=  KNotifyCTS;
				iSerialPort->iNotifyParamPtr[1] |=  (iRequestData & KSignalCTS);
				}

			if (iRequestData & KDSRChanged)
				{
				iSerialPort->iNotifyParamPtr[0] |=  KNotifyDSR;
				iSerialPort->iNotifyParamPtr[1] |=  (iRequestData & KSignalDSR);
				}

			if (iRequestData & KRNGChanged)
				{
				iSerialPort->iNotifyParamPtr[0] |=  KNotifyRI;
				iSerialPort->iNotifyParamPtr[1] |=  (iRequestData & KSignalRNG);
				}

			}
			break;
#endif //ER5U_NOTIFICATION_SUPPORT_ONLY
		case WriteErrors:
			{
			iSerialPort->iNotifyParamPtr[0] = iRequestData;
			iSerialPort->iNotifyParamPtr[1] = 0;
			ppn = &iSerialPort->iErrorsNotifier;
			}
			break;

		default:
			__ASSERT_DEBUG(EFalse, User::Panic(KCSerialDescPanic, 1));
			break;
		}

	//cancel all the others
	iSerialPort->CancelNotifiers(this);	//telling it who we are.

	//and complete the outstanding user request
	User::RequestComplete(iSerialPort->iNotifyStatus, iStatus.Int());
	delete this;
	if (ppn) *ppn = NULL;
	}

void CNotifier::DoCancel()
	{
	switch (iRequest)
		{
#ifndef ER5U_NOTIFICATION_SUPPORT_ONLY

		case DataAvailable:
			iSerialPort->NotifyDataAvailableCancel();
			break;

		case OutputEmpty:
			iSerialPort->NotifyOutputEmptyCancel();
			break;

		case Break:
			iSerialPort->NotifyBreakCancel();
			break;

		case Signals:
			iSerialPort->NotifySignalChangeCancel();
			break;
#endif //ER5U_NOTIFICATION_SUPPORT_ONLY
		case WriteErrors:
			iSerialPort->NotifyWriteErrorsCancel();
			break;

		default:
			break;
		}


	}


void CNotifier::Complete(TInt aVal)
	{
	TRequestStatus* ps = &iStatus;
	User::RequestComplete(ps, aVal);
	}


CNotifier* CNotifier::NewLC(CSerialDesc* aPort)
	{
	CNotifier* self=new (ELeave) CNotifier;
	CleanupStack::PushL(self);
	self->Construct(aPort);
	return self;
	}

CNotifier* CNotifier::NewL(CSerialDesc* aPort)
	{
	CNotifier* self = NewLC(aPort);
	CleanupStack::Pop();
	return self;
	}

void CNotifier::Construct(CSerialDesc* aPort)
	{
	iSerialPort = aPort;
	CActiveScheduler::Add(this) ;  // add to active scheduler
	}

CNotifier::~CNotifier()
	{
	Cancel();
	}


/****/



// The Serial descriptor class

TInt CSerialDesc::Open(RCommServ& aSession, const wchar_t* name, int mode, int /*perms*/)
	{
	//the name will be a wide version of COM?: or IRCOM?: where ? is a number 1 to 9.
	//this has already been checked in the call to CFileDescBase open.

	
	
	TInt err = KErrArgument;
	if (L'C' == name[0])
		{
		//serial port
		//load the comms module we require
		err = aSession.LoadCommModule(_L("ECUART"));
		if (KErrAlreadyExists != err && KErrNone != err)	//problem
			return err;
		//convert the name into an epoc port name
		//eg COMM::0
		TBuf<7> epocName(_L("COMM::0"));
		epocName[6] = (TText)(name[3] - 1);

		//try opening as a dte or a dce
		err = iCommPort.Open(aSession, epocName, (enum TCommAccess)mode, ECommRoleDTE);
		if (err) 			
			err = iCommPort.Open(aSession, epocName, (enum TCommAccess)mode, ECommRoleDCE);
		}
	else
		{
		//IR port
		err = aSession.LoadCommModule(_L("IrCOMM"));
		if (KErrAlreadyExists != err && KErrNone != err)	//problem
			return err;
		//convert the name into an epoc port name
		//eg COMM::0
		TBuf<9> epocName(_L("IrCOMM::0"));
		epocName[8] = (TText)(name[5] - 1);

		//try opening as a dte or a dce
		err = iCommPort.Open(aSession, epocName, (enum TCommAccess)mode, ECommRoleDTE);
		if (err) 			
			err = iCommPort.Open(aSession, epocName, (enum TCommAccess)mode, ECommRoleDCE);
		}
	return err;
	}

void CSerialDesc::UserClose()
	{
	IoctlCancel();
	}

TInt CSerialDesc::FinalClose()
	{
	iCommPort.Close();
	return 0;
	}




TBool CSerialDesc::TimedRead()
	{
		//if we have a timeout without a threshold we need an external timer
		return (-1 != iReadTimeout && -1 == iReadThreshold);
	}


void CSerialDesc::Read(TDes8& aBuf, TRequestStatus& aStatus)
	{
	//do a read..
	//4 different ones
	if (-1 == iReadThreshold)
		{
		iCommPort.ReadOneOrMore(aStatus, aBuf);
		}
	else
		{
		TInt len = (iReadThreshold < aBuf.MaxLength() ? iReadThreshold : aBuf.MaxLength());
		if (-1 == iReadTimeout)
			{
			//read threshold with no timeout
			iCommPort.Read(aStatus, aBuf, len);
			}
		else
			{
			//read threshold and timeout
			TTimeIntervalMicroSeconds32 timeout(iReadTimeout*1000);
			iCommPort.Read(aStatus, timeout, aBuf, len);
			}
		}

	}


void CSerialDesc::Write (TDes8& aBuf, TRequestStatus& aStatus)
	{
	iCommPort.Write(aStatus, aBuf);
	}


void CSerialDesc::Ioctl(int aCmd, void* aParam, TRequestStatus& aStatus)
	{
	TInt ret=KErrNone;
	if (aParam)
		{
		
		aCmd &= ~0x4000;	//mask off the queue bit!

		switch (aCmd)
			{
			case COMMIOCTL_SETSIGNALS:
				{
				int* param =REINTERPRET_CAST(int*,aParam);
				TUint setMask = (TUint)param[0];
				TUint clearMask = (TUint)param[1];
				iCommPort.SetSignals(setMask, clearMask);
				}
				break;

			case COMMIOCTL_GETSIGNALS:
				{
				int* param =REINTERPRET_CAST(int*,aParam);
				TUint signals = iCommPort.Signals();
				*param = (int)signals;
				}
				break;

			case COMMIOCTL_SETCONFIG:	
				{
				SerialConfig * param =REINTERPRET_CAST(SerialConfig *,aParam);
				TCommConfig cfg;

				TCommConfigV01& cfg01 =cfg();
				
				cfg01.iRate = (enum TBps)param->iRate; 
				cfg01.iDataBits = (enum TDataBits)param->iDataBits;
				cfg01.iStopBits = (enum TStopBits)param->iStopBits; 
				cfg01.iParity = (enum TParity)param->iParity; 
				cfg01.iHandshake = param->iHandshake;
				cfg01.iParityError = param->iParityError;
				cfg01.iFifo = param->iFifo;
				cfg01.iSpecialRate = param->iSpecialRate;
				cfg01.iTerminatorCount = param->iTerminatorCount;
				cfg01.iXonChar = param->iXonChar; 
				cfg01.iXoffChar = param->iXoffChar; 
				cfg01.iParityErrorChar = param->iParityErrorChar; 
				cfg01.iSIREnable = (enum TSir)param->iSIREnable; 
				cfg01.iSIRSettings = param->iSIRSettings;

				for (int i =0; i < ConfigMaxTerminators; i++)
					cfg01.iTerminator[i] = param->iTerminator[i];

				iCommPort.SetConfig(cfg);
				}
				break;

			case COMMIOCTL_GETCONFIG:	
				{
				SerialConfig * param =REINTERPRET_CAST(SerialConfig *,aParam);
				TCommConfig cfg;
				iCommPort.Config(cfg);
				TCommConfigV01& cfg01 =cfg();
				
				param->iRate = (enum Bps)cfg01.iRate;
				param->iDataBits = (enum DataBits)cfg01.iDataBits;
				param->iStopBits = (enum StopBits)cfg01.iStopBits;
				param->iParity = (enum Parity)cfg01.iParity;
				param->iHandshake = cfg01.iHandshake;
				param->iParityError = cfg01.iParityError;
				param->iFifo = cfg01.iFifo;
				param->iSpecialRate = cfg01.iSpecialRate;
				param->iTerminatorCount = cfg01.iTerminatorCount;
				for (int i =0; i < ConfigMaxTerminators; i++)
					param->iTerminator[i] = cfg01.iTerminator[i];
				param->iXonChar = cfg01.iXonChar;
				param->iXoffChar = cfg01.iXoffChar;
				param->iParityErrorChar = cfg01.iParityErrorChar;
				param->iSIREnable = (enum Sir)cfg01.iSIREnable;
				param->iSIRSettings = cfg01.iSIRSettings;
				}
				break;

			case COMMIOCTL_BREAK:	
				{
				int* param =REINTERPRET_CAST(int*,aParam);
				TTimeIntervalMicroSeconds32 time(*param);
				iCommPort.Break(aStatus, time);
				return;
				}

			case COMMIOCTL_SETREADTIMEOUT:
				{
				int* param =REINTERPRET_CAST(int*,aParam);
				iReadTimeout = *param;
				}
				break;

			case COMMIOCTL_GETREADTIMEOUT:
				{
				int* param =REINTERPRET_CAST(int*,aParam);
				*param = iReadTimeout;
				}
				break;

			case COMMIOCTL_SETREADTHRESHOLD:
				{
				int* param =REINTERPRET_CAST(int*,aParam);
				iReadThreshold = *param;
				}
				break;

			case COMMIOCTL_GETREADTHRESHOLD:
				{
				int* param =REINTERPRET_CAST(int*,aParam);
				*param = iReadThreshold;
				}
				break;

			case COMMIOCTL_SETBUFFERLENGTH:
				{
				int* param =REINTERPRET_CAST(int*,aParam);
				iCommPort.SetReceiveBufferLength(TInt(*param));
				}
				break;

			case COMMIOCTL_GETBUFFERLENGTH:
				{
				int* param =REINTERPRET_CAST(int*,aParam);
				*param = iCommPort.ReceiveBufferLength();
				}
				break;

			case COMMIOCTL_NOTIFYSUPPORTED:
				{
				int* param =REINTERPRET_CAST(int*,aParam);
				*param = NotifiesSupported();
				}
				break;

			case REAL_COMMIOCTL_NOTIFY:
				{
				int* param =REINTERPRET_CAST(int*,aParam);
				//if they are supported
				if (RequestedNotifiesSupported(*param))
					{
					//see if we need real notifications or we are to fake them
					//always use aStatus for the final thing
					TBool wantDataAvailable = *param & KNotifyDataAvailable;
					TBool wantOutputEmpty = *param & KNotifyOutputEmpty;
					TBool wantBreakInt = *param & KNotifyBreakInt;
					TBool wantSignals = *param & (KNotifyCD|KNotifyCTS|KNotifyDSR|KNotifyRI);
					TBool wantErrors = *param & (KNotifyFramingError|KNotifyOverrunError|KNotifyParityError);

					iDataAvailableNotifier = NULL;
					iOutputEmptyNotifier = NULL;
					iBreakNotifier = NULL;
					iSignalsNotifier = NULL;
					iErrorsNotifier = NULL;

					TRAPD(tRes,
						{
						if (wantDataAvailable) iDataAvailableNotifier = CNotifier::NewL(this);
						if (wantOutputEmpty) iOutputEmptyNotifier = CNotifier::NewL(this);
						if (wantBreakInt) iBreakNotifier = CNotifier::NewL(this);
						if (wantSignals) iSignalsNotifier = CNotifier::NewL(this);
						if (wantErrors) iErrorsNotifier = CNotifier::NewL(this);
						});
					
					if (KErrNone == tRes)
						{
						//smashing, no failure, request those events
						if (wantDataAvailable) iDataAvailableNotifier->IssueRequest(KNotifyDataAvailable);
						if (wantOutputEmpty) iOutputEmptyNotifier->IssueRequest(KNotifyOutputEmpty);
						if (wantBreakInt) iBreakNotifier->IssueRequest(KNotifyBreakInt);
						if (wantSignals) iSignalsNotifier->IssueRequest(*param & (KNotifyCD|KNotifyCTS|KNotifyDSR|KNotifyRI));
						if (wantErrors) iErrorsNotifier->IssueRequest(*param & (KNotifyFramingError|KNotifyOverrunError|KNotifyParityError));

						iRequestedSignals = *param;
						iNotifyParamPtr = REINTERPRET_CAST(unsigned int*,aParam);
						iNotifyStatus = &aStatus;
						aStatus = KRequestPending;
						return;			//on an async call here
						}
					else
						{
						//deal with the problem
						//we're going to have to tidy up, delete things etc
						delete iDataAvailableNotifier;
						delete iOutputEmptyNotifier;
						delete iBreakNotifier;
						delete iSignalsNotifier;
						delete iErrorsNotifier;
						iDataAvailableNotifier = NULL;
						iOutputEmptyNotifier = NULL;
						iBreakNotifier = NULL;
						iSignalsNotifier = NULL;
						iErrorsNotifier = NULL;
						ret = tRes;
						}

					}
				else
					{
					ret = KErrNotSupported;
					*param &=~NotifiesSupported();
					}
				}
				break;

				
			default:
				ret=KErrNotSupported;
				break;
			}
		}
		else
			ret = KErrArgument;
 
	Complete(aStatus,ret);
	}


TInt CSerialDesc::IoctlCompletion(int /*aCmd*/, void* /*aParam*/, TInt aStatus)
	{
	return aStatus;
	}


void CSerialDesc::ReadCancel()
	{
	iCommPort.ReadCancel();
	}


void CSerialDesc::IoctlCancel()
	{
	//stop the ioctl if in progress
	CancelNotifiers(NULL);

	if (iNotifyStatus)
		{
		iNotifyParamPtr[0] = 0;
		Complete(*iNotifyStatus, -3);
		}

	}

TInt CSerialDesc::ReadCompletion (TDes8& /*aBuf*/, TInt aStatus)
	{
	//The read has completed.  
	//See if we need to signal 'cos it completed with an error and someone is waiting 
	//on a notification.  In which case we need to complete the request with the correct results.
	
	if ((aStatus < 0) && (iRequestedSignals&(KNotifyFramingError|KNotifyOverrunError|KNotifyParityError)))	//we have a signal outstanding we can deal with here
		{
		switch (aStatus)
			{
			case KErrCommsFrame:	//comms framing error
				if (iRequestedSignals&KNotifyFramingError)
					Notify(KNotifyFramingError);
				break;

			case KErrCommsOverrun:	//comms overrrun error
 				if (iRequestedSignals&KNotifyOverrunError)
					Notify(KNotifyOverrunError);
				break;

			case KErrCommsParity:	//comms parity error
 				if (iRequestedSignals&KNotifyParityError)
					Notify(KNotifyParityError);
				break;
			
			default:
				//an error we don't signal
				break;

			}
		}
	
	return aStatus;
	}

TBool CSerialDesc::RequestedNotifiesSupported(TInt aRequested)
	{
	//return true if these notifies are OK.  0 if any of them are illegal

	TInt mask = ~(NotifiesSupported());
	return !(aRequested&mask);
	}

TInt CSerialDesc::NotifiesSupported()
	{
	//return which notifies are supported.
	//looks like the driver/server is going to have to be interrogated here

	//start with the ones we can fake
	TInt supported = KNotifyFramingError|KNotifyOverrunError|KNotifyParityError;
	
#ifndef ER5U_NOTIFICATION_SUPPORT_ONLY
	//get the supported ones from C32
	TCommCaps2 devCap;
	TCommCapsV02& deviceCapabilities = devCap();
	deviceCapabilities.iNotificationCaps = 0;
	iCommPort.Caps(devCap);


	//signals
	if (deviceCapabilities.iNotificationCaps & KNotifySignalsChangeSupported)
		supported |= (KNotifyCD|KNotifyCTS|KNotifyDSR|KNotifyRI);

	//break interrupt
	if (deviceCapabilities.iNotificationCaps & KNotifyBreakSupported)
		supported |= KNotifyBreakInt;


	//Data Available
	if (deviceCapabilities.iNotificationCaps & KNotifyDataAvailableSupported)
		supported |= KNotifyDataAvailable;

	//Output Empty
	if (deviceCapabilities.iNotificationCaps & KNotifyOutputEmptySupported)
		supported |= KNotifyOutputEmpty;

#endif  //ER5U_NOTIFICATION_SUPPORT_ONLY

	return supported;
	}

void CSerialDesc::Notify(TInt aVal)
	{
	if (iErrorsNotifier)
		{
//		iNotifyParamPtr[0] = aVal;
		*iRequestDataPtr = aVal;
		iErrorsNotifier->Complete(0);
		}
	}


#ifndef ER5U_NOTIFICATION_SUPPORT_ONLY
void CSerialDesc::NotifyDataAvailable(TRequestStatus& aStatus)
	{
	iCommPort.NotifyDataAvailable(aStatus);
	}

void CSerialDesc::NotifyDataAvailableCancel()
	{
	iCommPort.NotifyDataAvailableCancel();
	}

void CSerialDesc::NotifyOutputEmpty(TRequestStatus& aStatus)
	{
	iCommPort.NotifyOutputEmpty(aStatus);
	}

void CSerialDesc::NotifyOutputEmptyCancel()
	{
	iCommPort.NotifyOutputEmptyCancel();
	}

void CSerialDesc::NotifyBreak(TRequestStatus& aStatus)
	{
	iCommPort.NotifyBreak(aStatus);
	}

void CSerialDesc::NotifyBreakCancel()
	{
	iCommPort.NotifyBreakCancel();
	}

void CSerialDesc::NotifySignalChange(TRequestStatus& aStatus, TUint& aRequestData, TUint aSignalsMask)
	{
	iCommPort.NotifySignalChange(aStatus, aRequestData, aSignalsMask);
	}

void CSerialDesc::NotifySignalChangeCancel()
	{
	iCommPort.NotifySignalChangeCancel();
	}
#endif  //ER5U_NOTIFICATION_SUPPORT_ONLY

void CSerialDesc::NotifyWriteErrors(TRequestStatus& aStatus, TUint* aRequestData, TUint aSignalsMask)
	{
	iRequestedSignals = aSignalsMask;
	iRequestDataPtr = aRequestData;
//	iNotifyParamPtr = aRequestData;
	aStatus = KRequestPending;
	}

void CSerialDesc::NotifyWriteErrorsCancel()
	{
	iErrorsNotifier->Complete(KErrCancel);
	}

TUint CSerialDesc::Signals()
	{
	return iCommPort.Signals();
	}


void CSerialDesc::CancelNotifiers(const CNotifier* aCompletedNotifier)
	{
#ifndef ER5U_NOTIFICATION_SUPPORT_ONLY
	if (iDataAvailableNotifier && (aCompletedNotifier != iDataAvailableNotifier))
		{
		iDataAvailableNotifier->Cancel();
		delete iDataAvailableNotifier;
		iDataAvailableNotifier = NULL;
		}

	if (iOutputEmptyNotifier && (aCompletedNotifier != iOutputEmptyNotifier))
		{
		iOutputEmptyNotifier->Cancel();
		delete iOutputEmptyNotifier;
		iOutputEmptyNotifier = NULL;
		}
	
	if (iBreakNotifier && (aCompletedNotifier != iBreakNotifier))
		{
		iBreakNotifier->Cancel();
		delete iBreakNotifier;
		iBreakNotifier = NULL;
		}


	if (iSignalsNotifier && (aCompletedNotifier != iSignalsNotifier))
		{
		iSignalsNotifier->Cancel();
		delete iSignalsNotifier;
		iSignalsNotifier = NULL;
		}
#endif //ER5U_NOTIFICATION_SUPPORT_ONLY
	if (iErrorsNotifier && (aCompletedNotifier != iErrorsNotifier))
		{
		iErrorsNotifier->Cancel();
		delete iErrorsNotifier;
		iErrorsNotifier = NULL;
		}
	}

