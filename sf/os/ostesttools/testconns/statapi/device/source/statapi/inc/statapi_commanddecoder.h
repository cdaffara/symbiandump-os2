/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#if !defined(__STATAPI_COMMANDDECODER_H__)
#define __STATAPI_COMMANDDECODER_H__

#include "filedump.h"

#ifndef LIGHT_MODE
	#include <apgcli.h>
	#include <eikappui.h>
#endif

class CStatApiCommandDecoder : public CBase
{
	public:
		CStatApiCommandDecoder();
		virtual ~CStatApiCommandDecoder();
		static CStatApiCommandDecoder* NewL(RFs *const aSession, MNotifyLogMessage *const aMsg);
		virtual TInt ExecuteCommand(TUint commandtype, MDataConsumer *const aDataConsumer, 
						MDataSupplier **aDataSupplier);
		virtual void ConstructL(RFs *const aSession, MNotifyLogMessage *const aMsg);
		TInt DeleteLastFile();
		void Reset() { iName = 1; }
		

	private:
		TInt ConvertKeycodeToScancode( TDesC& key, TInt& scancode, TInt& modifiers );
		TInt OpenIcon();
		TInt SendText(MDataConsumer *const aDataConsumer, int commandLength);
		TInt SendCombinationKeys();
		TInt SendSystemKeys();
		TInt SendKeyHold();
		TInt StartApplicationL();
		TInt StartProgramL(MDataSupplier **aDataSupplier);
		void StopApplicationL();
        void StopEShellL();
		TInt ReadTEFSharedDataL(MDataSupplier **aDataSupplier);
		TInt WriteTEFSharedDataL(MDataConsumer *const aDataConsumer, int commandLength);
		TInt OpenFileL();
		TInt DeleteFileL(TInt bDisplayMessage = TRUE);
		TInt CreateFolder();
		TInt RemoveFolder();
		TInt CheckLocation(MDataConsumer *const aDataConsumer);
		void ScreenCaptureL();
		TInt TransferFile(MDataConsumer *const aDataConsumer);
		TInt ReceiveFile(TUint commandtype, MDataConsumer *const aDataConsumer, MDataSupplier **aDataSupplier);
		TInt RenameFileL(MDataConsumer *const aDataConsumer);
		TInt DeviceInfo(MDataSupplier **aDataSupplier);
		void StartEShellL();
		TInt SaveLogFile(MDataSupplier **aDataSupplier);
		TInt GetDiskDriveInformationL(MDataSupplier **aDataSupplier);
		TInt GetDirectoryInformationL(MDataConsumer *const aDataConsumer, MDataSupplier **aDataSupplier);
		TInt GetData(TInt i, TDes8& info);
		void DoKeyEvent(TInt key);
		void AppendErrorCode(TInt ret, MDataSupplier **aDataSupplier);
		bool IsWindowServerAvailable(void) const;
		TInt StopProgramL(const TDesC& aHandleNumber);
		TInt ProgramStatusL(const TDesC& aHandleNumber, MDataSupplier **aDataSupplier);
		void RefreshStatus(void);
		void updatePathWithSysDrive(TBuf<1024>& pathToChange ) ; 
		void updatePathWithSysDrive(TBuf<KMaxPath>& pathToChange ) ; 
		TDriveName getSystemDrive();
		
#ifndef LIGHT_MODE
		RWsSession iWs;			//session object used for keypress
		RApaLsSession iApaS;	//session object for app name processing
		CWsScreenDevice* iDevice;	// Data retained by the object
	        CWsBitmap* screenBitmap;	// in order to optimise bitmap screen
#endif // ifndef LIGHT_MODE
		TInt iName;				// flag to see whether data is file name or contents
		TBuf<256> filename;		// holds the name of the transferred file
		TBuf<1024> command;		// holds our command data while we work with it - should not be used for raw data
		RFs* iFs;				//file server session
		MNotifyLogMessage *pMsg;			// logging

        TInt iUIDValue;				// capture.
        TBuf<3> iSystemDrive;
};

inline bool CStatApiCommandDecoder::IsWindowServerAvailable(void) const
{
#ifndef LIGHT_MODE
	return (0 != iWs.WsHandle( ));
#else // ifndef LIGHT_MODE
	return (EFalse);
#endif // ifndef LIGHT_MODE
}

#endif

