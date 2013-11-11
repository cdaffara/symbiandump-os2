/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tbatterycharging.h"
#include "musbdevicenotify.h"

#include <e32svr.h>
#include <e32base.h>
#include <e32std.h>
#include <f32file.h>
#include <barsc.h>
#include <barsread.h>
#include <bautils.h>
#include "UsbUtils.h"


#include"tbatterychargingdefinitions.h"


LOCAL_C void ConsoleMainL();

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	
	TRAPD(error, ConsoleMainL() );
	
	__ASSERT_ALWAYS(!error,User::Panic(_L("UsbChargingTest"), error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return KErrNone;
	}

void ConsoleMainL()
	{
    CActiveScheduler* myScheduler = new(ELeave) CActiveScheduler;
    CleanupStack::PushL(myScheduler);
    CActiveScheduler::Install(myScheduler);	
    
	CDummyUsbDevice* device = CDummyUsbDevice::NewL();
	CleanupStack::PushL(device);
	device->DoTestsL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(2, myScheduler);
	}

CDummyUsbDevice* CDummyUsbDevice::NewL()
/**
 * Constructs a CDummyUsbDevice object.
 *
 * @return	A new CDummyUsbDevice object
 */
	{
	CDummyUsbDevice* r = new (ELeave) CDummyUsbDevice();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDummyUsbDevice::~CDummyUsbDevice()
/**
 * Destructor.
 */
	{
	// Cancel any outstanding asynchronous operation.
	Cancel();
	iTimer.Close();
	//delete iRepository;
	iExtensionPlugins.ResetAndDestroy();

	if(iEcom)
		iEcom->Close();
	REComSession::FinalClose();

	// Free any memory allocated to the list of observers. Note that
	// we don't want to call ResetAndDestroy, because we don't own
	// the observers themselves.
	iObservers.Reset();
	delete iText;
	iTest.Close();
	
	iProperty.Close();
	iPropertyWriteToRepositoryAck.Close();
	iPropertyReadChargingCurrentAck.Close();

	}

CDummyUsbDevice::CDummyUsbDevice()
	: CActive(EPriorityStandard), iTest(_L("Usb Charging Plugin Test")), iPtr(0,200), iLine(0,200)
/**
 * Constructor.
 */
	{
	CActiveScheduler::Add(this);
	}

void CDummyUsbDevice::ConstructL()
/**
 * Performs 2nd phase construction of the USB device.
 */
	{
	iEcom = &(REComSession::OpenL());

	InstantiateExtensionPluginsL();

	if (iExtensionPlugins.Count() != 1)
		{
		User::Panic(KUsbChargingTestPanic, EUsbChargingTestPanicIncorrectPlugin);
		}

	iPlugin = iExtensionPlugins[0];

	iDummyLdd.Initialise();
	User::LeaveIfError(iTimer.CreateLocal());
	
	DefinePropertyL(KBattChargWriteRepositoryUid, KBattChargWriteRepositoryKey,RProperty::EInt);
	DefinePropertyL(KBattChargReadPropertyCurrentUid,KBattChargReadCurrentChargingKey,RProperty::EInt);
	
	User::LeaveIfError(iPropertyWriteToRepositoryAck.Attach(TUid::Uid(KBattChargWriteRepositoryUid), KBattChargWriteRepositoryAckKey));
	User::LeaveIfError(iPropertyReadChargingCurrentAck.Attach(TUid::Uid(KBattChargReadPropertyCurrentUid), KBattChargReadCurrentChargingAckKey));
	
	User::LeaveIfError(StartPropertyBatteryCharging());

	}
	
void CDummyUsbDevice::DefinePropertyL(const TInt32 aCategory, TUint aKey,RProperty::TType eType)
	{
	
	_LIT_SECURITY_POLICY_PASS(KAlwaysPass);

	TInt err = iProperty.Define(TUid::Uid(aCategory),
									aKey,
									eType,
									KAlwaysPass,
									KAlwaysPass
									);
	if ( err != KErrAlreadyExists )
		{
		User::LeaveIfError(err);
		}
	}


	
void CDummyUsbDevice::InstantiateExtensionPluginsL()
	{
	const TUid KUidExtensionPluginInterface = TUid::Uid(KUsbmanExtensionPluginInterfaceUid);
	RImplInfoPtrArray implementations;
	const TEComResolverParams noResolverParams;
	REComSession::ListImplementationsL(KUidExtensionPluginInterface, noResolverParams, KRomOnlyResolverUid, implementations);
	CleanupResetAndDestroyPushL(implementations);

	for (TInt i=0; i<implementations.Count(); i++)
		{
		const TUid KTestPluginUid = {0x1020DEA8};
		if (implementations[i]->ImplementationUid() == KTestPluginUid)
			{
			CUsbmanExtensionPlugin* plugin = CUsbmanExtensionPlugin::NewL(implementations[i]->ImplementationUid(), *this);
			CleanupStack::PushL(plugin);
			// there will most likely be two plugins - the standard one, and the test one (which
			// is an extension of the standard one, to include the interface 
			// MUsbBatteryChargingTestPluginInterface2. So check, and only keep the test one:
			MUsbBatteryChargingTestPluginInterface2* pluginIf
				= reinterpret_cast<MUsbBatteryChargingTestPluginInterface2*>(
					plugin->GetInterface(KUidUsbBatteryChargingTestPluginInterface2));
			
			if (pluginIf)
				{
				iExtensionPlugins.AppendL(plugin); // transfer ownership to iExtensionPlugins
				CleanupStack::Pop(plugin);
				}
			else
				{ // destroy it - it's the standard plugin.
				CleanupStack::PopAndDestroy(plugin);
				iObservers.Remove(iObservers.Count() - 1);
				}
			}
		}
	CleanupStack::PopAndDestroy(&implementations);
	}

void CDummyUsbDevice::RegisterObserverL(MUsbDeviceNotify& aObserver)
/**
 * Register an observer of the device.
 * Presently, the device supports watching state.
 *
 * @param	aObserver	New Observer of the device
 */
	{
	User::LeaveIfError(iObservers.Append(&aObserver));
	}


void CDummyUsbDevice::DeRegisterObserver(MUsbDeviceNotify& aObserver)
/**
 * De-registers an existing device observer.
 *
 * @param	aObserver	The existing device observer to be de-registered
 */
	{
	TInt index = iObservers.Find(&aObserver);

	if (index >= 0)
		iObservers.Remove(index);
	}

void CDummyUsbDevice::RunL()
	{
	DoCheck();
	if (GetNextLine() < 0)
		{
		iTest.End();
		iTest.Getch();
		CActiveScheduler::Stop();
		}
	else
		{
		iLineNumber++;
		iTest.Next(_L(""));
		InterpretLine();
		DoCommand();
		DoAsyncOp();	
		}
	}

void CDummyUsbDevice::DoCancel()
	{
	iTimer.Cancel();
	}

TInt CDummyUsbDevice::RunError(TInt /*aError*/)
	{

	return KErrNone;
	}

RDevUsbcClient& CDummyUsbDevice::MuepoDoDevUsbcClient()
/**
 * Inherited from MUsbmanExtensionPluginObserver - Function used by plugins to
 * retrieve our handle to the LDD
 *
 * @return The LDD handle
 */
	{
	return iDummyLdd;
	}

void CDummyUsbDevice::MuepoDoRegisterStateObserverL(MUsbDeviceNotify& aObserver)
/**
 * Inherited from MUsbmanExtensionPluginObserver - Function used by plugins to
 * register themselves for notifications of device/service state changes.
 *
 * @param aObserver New Observer of the device
 */
	{
	RegisterObserverL(aObserver);
	}

void CDummyUsbDevice::UpdatePluginInfo()
	{
	// we can assume our plugin does support this interface....
	reinterpret_cast<MUsbBatteryChargingTestPluginInterface2*>(iPlugin->GetInterface(KUidUsbBatteryChargingTestPluginInterface2))->GetPluginInfo(iInfo);
	}

void CDummyUsbDevice::DoTestsL()
	{
	iTest.SetLogged(ETrue);
	iTest.Title();
	OpenFileL();
	TInt length = GetNextLine();
	if (length <= 0)
		{
		User::Panic(KUsbChargingTestPanic, EUsbChargingTestPanicBadInputData);
		}
	iLineNumber = 1;
	iTest.Start(_L("test"));
	InterpretLine();
	DoCommand();
	DoAsyncOp();
	}

void CDummyUsbDevice::InterpretLine()
	{
	TLex8 lex(iLine);
	lex.SkipCharacters();
	iCommand = GetCommand(iLine.Left(lex.Offset()));
	lex.SkipSpace();
	lex.Val(iCommandValue);
	lex.SkipSpace();
	TInt pos = lex.Offset();
	lex.SkipCharacters();
	iAsyncOp = GetAsyncOp(iLine.Mid(pos,lex.Offset()-pos));
	lex.SkipSpace();
	lex.Val(iAsyncOpValue);
	lex.SkipSpace();
	pos = lex.Offset();
	lex.SkipCharacters();
	iCheck = GetCheck(iLine.Mid(pos,lex.Offset()-pos));
	lex.SkipSpace();
	lex.Val(iCheckValue);
	}

void CDummyUsbDevice::OpenFileL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	TFindFile ff(fs);
	User::LeaveIfError(ff.FindByDir(_L("\\system\\data\\t_charging.txt"),KNullDesC));
	
	RFile file;
	TInt size;
	User::LeaveIfError(file.Open(fs,ff.File(),EFileStreamText|EFileRead|EFileShareReadersOnly));
	CleanupClosePushL(file);
	
	User::LeaveIfError(file.Size(size));

	iText = REINTERPRET_CAST(TText8*, User::AllocL(size));
	iPtr.Set(iText, size/sizeof(TText8), size/sizeof(TText8));
	TPtr8 dest(REINTERPRET_CAST(TUint8*,iText), 0, size);
	User::LeaveIfError(file.Read(dest)); 

	CleanupStack::PopAndDestroy(); // file
	CleanupStack::PopAndDestroy(); // fs
	}

TInt CDummyUsbDevice::GetNextLine()
	{
	TInt newLineOffset = (iPtr.Mid(iFileOffset)).Locate(13);
	if (newLineOffset < 0)
		{
		return newLineOffset;
		}
	if (newLineOffset == 0)
		{
		iFileOffset += 2;
		return GetNextLine();
		}
	iLine.Set(iPtr.MidTPtr(iFileOffset, newLineOffset));
	iFileOffset += (newLineOffset + 2);
	if (iLine.Find(_L8("//")) == 0) // i.e. line begins with "//"
		{
		return GetNextLine();
		}
	if (iLine.Find(_L8("**")) == 0) // line begins with **, so display it
		{
		TBuf<100> buf; // max length 100 for test messages
		buf.Copy(iLine);
		iTest.Printf(_L("\n%S\n\n"),&buf);
		return GetNextLine();
		};
	return newLineOffset;
	}

TInt CDummyUsbDevice::GetCommand(const TDesC8& aDes)
	{
	if (aDes.MatchF(_L8("none")) != KErrNotFound)
		return EUsbChargingTestCommandNone;
	if (aDes.MatchF(_L8("devicestate")) != KErrNotFound)
		return EUsbChargingTestCommandDeviceState;
	if (aDes.MatchF(_L8("usersetting")) != KErrNotFound)
		return EUsbChargingTestCommandUserSetting;
	User::Panic(KUsbChargingTestPanic, EUsbChargingTestPanicBadInputData);
	return -1;
	}

TInt CDummyUsbDevice::GetAsyncOp(const TDesC8& aDes)
	{
	if (aDes.MatchF(_L8("none")) != KErrNotFound)
		return EUsbChargingTestAsyncOpNone;
	if (aDes.MatchF(_L8("delay")) != KErrNotFound)
		return EUsbChargingTestAsyncOpDelay;
	User::Panic(KUsbChargingTestPanic, EUsbChargingTestPanicBadInputData);
	return -1;
	}

TInt CDummyUsbDevice::GetCheck(const TDesC8& aDes)
	{
	if (aDes.MatchF(_L8("none")) != KErrNotFound)
		return EUsbChargingTestCheckNone;
	if (aDes.MatchF(_L8("pluginstate")) != KErrNotFound)
		return EUsbChargingTestCheckPluginState;
	if (aDes.MatchF(_L8("milliamps")) != KErrNotFound)
		return EUsbChargingTestCheckMilliAmps;
	if (aDes.MatchF(_L8("charging")) != KErrNotFound)
		return EUsbChargingTestCheckCharging;
	User::Panic(KUsbChargingTestPanic, EUsbChargingTestPanicBadInputData);
	return -1;
	}

void CDummyUsbDevice::DoCommand()
	{
	switch (iCommand)
		{
		case EUsbChargingTestCommandNone:
			{
			// do nothing	
			}
			break;
		case EUsbChargingTestCommandDeviceState:
			{
			iObservers[0]->UsbDeviceStateChange(KErrNone, iDeviceState, TUsbDeviceState(iCommandValue));
			iDeviceState = TUsbDeviceState(iCommandValue);
			}
			break;
		case EUsbChargingTestCommandUserSetting:
			{
			TInt err = WriteToRepositoryProperty(iCommandValue);
			
			iTest(err == KErrNone);
			}
			break;
		default:
			User::Panic(KUsbChargingTestPanic, EUsbChargingTestPanicBadCommand);
		}
	}

void CDummyUsbDevice::DoAsyncOp()
	{
	switch (iAsyncOp)
		{
		case EUsbChargingTestAsyncOpNone:
			{
			SetActive();
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			}
			break;
		case EUsbChargingTestAsyncOpDelay:
			{
			iTimer.After(iStatus, TTimeIntervalMicroSeconds32(iAsyncOpValue));
			SetActive();
			}
			break;
		default:
			User::Panic(KUsbChargingTestPanic, EUsbChargingTestPanicBadAsyncOp);
		}
	}

void CDummyUsbDevice::DoCheck()
	{
	UpdatePluginInfo();
	switch (iCheck)
		{
		case EUsbChargingTestCheckNone:
			{
			// do nothing
			}
			break;
		case EUsbChargingTestCheckPluginState:
			{
			iTest(iInfo.iPluginState == iCheckValue);
			}
			break;
		case EUsbChargingTestCheckMilliAmps:
			{
			iTest(iInfo.iRequestedCurrentValue == iCheckValue);
			}
			break;
		case EUsbChargingTestCheckCharging:
			{
			TInt current;			
			TInt err = GetChargingCurrentFromProperty(current);
			
			iTest(err == KErrNone);
			iTest(current == iCheckValue);
			}
			break;
		default:
			User::Panic(KUsbChargingTestPanic, EUsbChargingTestPanicBadCheck);
		}
	}
	
TInt CDummyUsbDevice::GetChargingCurrentFromProperty(TInt &aCurrent)
	{
	
	TRequestStatus stat;
	iPropertyReadChargingCurrentAck.Subscribe(stat);
	
	TInt err = iProperty.Set(TUid::Uid(KBattChargReadPropertyCurrentUid),
								KBattChargReadCurrentChargingKey,
								1);
	ASSERT(!err);	
	User::WaitForRequest(stat);
	
	TBufC8<50> value;
	TPtr8 myPtr(value.Des());
	TInt error = iPropertyReadChargingCurrentAck.Get(myPtr);
	ASSERT(!error);
	
	TDataFromPropBattChargToTBatteryCharging received;
	TPckg<TDataFromPropBattChargToTBatteryCharging>tmp(received);
	tmp.Copy(value);
	aCurrent = received.iCurrent;
	return received.iError;
	
	}

TInt CDummyUsbDevice::WriteToRepositoryProperty(TInt iCommandValue)
	{
	TRequestStatus stat;
	iPropertyWriteToRepositoryAck.Subscribe(stat);
		
	TInt err = iProperty.Set(TUid::Uid(KBattChargWriteRepositoryUid),
									KBattChargWriteRepositoryKey,
									iCommandValue);
	ASSERT(!err);	
	User::WaitForRequest(stat);
	TInt value;
	err = iPropertyWriteToRepositoryAck.Get(value);
	ASSERT(!err);
	return value;
	}
	
TInt CDummyUsbDevice::StartPropertyBatteryCharging()
	{
	const TUidType serverUid(KNullUid, KNullUid, KBatteryChargingTUid);
	RProcess server;
	TInt err = server.Create(KBattChargingImg, KNullDesC, serverUid);
	if ( err != KErrNone )
		{
		return err;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	
	if ( stat != KRequestPending )
		{		
		server.Kill(0); 	// abort startup
		}
	else
		{		
		server.Resume();	// logon OK - start the server
		}
	
	User::WaitForRequest(stat); 	// wait for start or death
	
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	
	err = (server.ExitType() == EExitPanic) ? KErrServerTerminated : stat.Int();
	
	server.Close();	
	
	return err;
	
	}
	

