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
// Implements flogger utility classes
// 
//

/**
 @file
 @internalComponent
*/

#include "comsdbgaux.h"
#include "comsdbgsvr.h"
#include "comsdbgmessages.h"

_LIT8(KSubsystemWildCard, "*");

_LIT8(KLogKeyword, "Log");
_LIT8(KMediaKeyword, "Media");
_LIT8(KWin32DbgPortKeyword, "Win32Debug");
_LIT8(KLogPathKeyword, "LogPath");
_LIT8(KCommentKeyword, "//");
_LIT8(KForceFlushKeyword, "Synchronous");
const TUint8 KCarriageReturn = 0x000D;
const TUint8 KLineFeed = 0x000A;



CIniFileWatcher* CIniFileWatcher::NewL(RFs& aFs, CFileLoggerServer& aServer, const TDesC& aIniFile)
	{
	CIniFileWatcher* self = new(ELeave) CIniFileWatcher(aFs, aServer, aIniFile);
	self->Initialize();
	return self;
	}

CIniFileWatcher::CIniFileWatcher(RFs& aFs, CFileLoggerServer& aServer, const TDesC& aIniFile)
: CActive(EPriorityStandard), iFs(aFs), iServer(aServer), iWatchedIniFile(aIniFile)
{}

void CIniFileWatcher::Initialize()
  	{
  	CActiveScheduler::Add(this);
  	NotifyChange();
  	}

void CIniFileWatcher::NotifyChange()
/* Listen for change on the CDU ini file. */
    {
    iFs.NotifyChange(ENotifyAll, iStatus, iWatchedIniFile);
  	SetActive();
  	}

void CIniFileWatcher::RunL()
	{
	iStatus=KRequestPending;
   NotifyChange();
	iServer.IniFileChanged(iWatchedIniFile);
	}

void CIniFileWatcher::DoCancel()
	{
	iFs.NotifyChangeCancel();
	}


CIniFileWatcher::~CIniFileWatcher()
	{
	Cancel();
	}

///////////////CIniFileParser//////////////////////////////////////////////

TInt CIniFileParser::ParseIniFile(TDesC& aIniFile)
/*
 * Returns: KErrNotFound if file not found or KErrPathNotFound if path not found.
 */
	{
	TRAPD(err, DoParseIniFileL(aIniFile));
	return err;
	}

TInt CIniFileParser::GetNextTokenAndCheck(TLex8& lex, TPtr8& tempPtr)
/*
Gets next token and ensures the token is simply not the EOF or a linefeed.
lex is the lexical string to get the next token from.
tempPtr points to the next token
Returns KErrGeneral if token is bad or if we've already read past the end.
*/
	{
	TUint8 ch;
	TInt len;

	if (lex.Eos())
		{
		return KErrGeneral;
		}
	
	tempPtr = lex.NextToken();

	len = tempPtr.Length();
	if (len == 0)
		{
		// lex has figured out what is left is just the EOF
		return KErrGeneral;
		}

	// this next part may be superfluous but we've had so much strife with
	// the parser thus far that for now we're leaving it in

	ch = tempPtr[0];
	if (ch == KCarriageReturn || ch == KLineFeed)
		{
		return KErrGeneral;
		}

	if (tempPtr.Length() < 2)
		{
		return KErrNone;
		}
	ch = tempPtr[1];
	if (ch == KCarriageReturn || ch == KLineFeed)
		{
		return KErrGeneral;
		}

	return KErrNone;
	}
	
void CIniFileParser::DoParseIniFileL(TDesC& aIniFile)
/*
 * 
 * The force flush state is only updated if it is not already set to something other
 * than ENoValue. If force flush option is not found in ini file, force flush is set
 * to off.
 */
	{

	TInt fileLength;
	TInt ret = KErrNone;
	RFile iniFile;

	// Open file
	User::LeaveIfError(iniFile.Open(iFs, aIniFile, EFileShareAny));
		
	CleanupClosePushL(iniFile);
	
	// as we have been able to open the file, set the media to default.
	// If the ini file is parsed correctly, this then gets overwritten.
	// Otherwise the caller should pass thru a mesg to get the default enabled.
	if (iLoggingMediaString.Length() == 0)
		{
		iLoggingMediaString = KDefaultMedia;
		}
		
	

	User::LeaveIfError(iniFile.Size(fileLength));

	HBufC8* iniContents = HBufC8::NewLC(fileLength);
	TPtr8 hbufPtr = iniContents->Des();
	User::LeaveIfError(iniFile.Read(hbufPtr));
	TLex8 lex(*iniContents);

	//OK, file is open and ready for parsing. Make a tempory array and if there is a
	//problem in the ini file leave settings as they were, leave, and
	//the error will get picked up.

	delete iIniSettings;
	iIniSettings = NULL;
	CIniLoggingPairs* iniSettings = CIniLoggingPairs::NewL();
	CleanupStack::PushL(iniSettings);
	TNameTag tempTag;
	TNameTag tempTag2;
	TChar  tempChar;

	FOREVER
		{
		ret = GetNextTokenAndCheck(lex,hbufPtr);
		if (ret != KErrNone)
			{
			break;
			}
		if (hbufPtr.Find(KCommentKeyword)!=KErrNotFound)		//found a Comment
			{
			tempChar = lex.Get();
			while (!lex.Eos() && TUint(tempChar) != KCarriageReturn && TUint(tempChar) != KLineFeed)
				{
				tempChar = lex.Get();
				}
			}
		else if (hbufPtr.CompareF(KMediaKeyword)==0)		//MediaSetting
			{
			User::LeaveIfError(GetNextTokenAndCheck(lex,hbufPtr));
			if (hbufPtr.Length()>KMaxMediaStringLength)
				{
				User::Leave(KErrGeneral);
				}
			iLoggingMediaString = hbufPtr;
			}
		else if (hbufPtr.CompareF(KLogKeyword)==0)		//LOG
			{
			User::LeaveIfError(GetNextTokenAndCheck(lex,hbufPtr));

			if (hbufPtr.Length()>KMaxTagLength)
				{
				tempTag = hbufPtr.Left(KMaxTagLength);
				}
			else
				{
				tempTag = hbufPtr;
				}
			User::LeaveIfError(GetNextTokenAndCheck(lex,hbufPtr));
			if (hbufPtr.Length()>KMaxTagLength)
				{
				tempTag2 = hbufPtr.Left(KMaxTagLength);
				}
			else
				{
				tempTag2 = hbufPtr;
				}
			iniSettings->AddSettingL(tempTag, tempTag2);
			}
		else if (hbufPtr.CompareF(KForceFlushKeyword)==0)		//ForceFlush
			{
			if (iForceFlushState == ENoValue)
				{
				iForceFlushState = EFlushOn;
				}
			}
		else if (hbufPtr.CompareF(KLogPathKeyword) == 0)		//LogPath
			{
			User::LeaveIfError(GetNextTokenAndCheck(lex,hbufPtr));
			if (hbufPtr.Length()>KMaxName)
				{
				User::Leave(KErrOverflow);
				}
			iLoggingPathString.Copy(hbufPtr);
			}
		else if (hbufPtr.CompareF(KWin32DbgPortKeyword)==0)		//Win32DebugPort
			{
			iWin32DebugEnabled = ETrue;
			}
		else
			User::Leave(KErrBadName);

		if (lex.Eos())
			{
			break;
			}
		}
	iIniSettings = iniSettings;
	CleanupStack::Pop(iniSettings);
	CleanupStack::PopAndDestroy();	//iniContents
	CleanupStack::PopAndDestroy();	//iniFile

	if (iForceFlushState == ENoValue)
		{
		iForceFlushState = EFlushOff;
		}
	}

CIniFileParser* CIniFileParser::NewL(RFs& aFs)
	{
	return new(ELeave) CIniFileParser(aFs);
	}

CIniFileParser::~CIniFileParser()
	{
	delete iIniSettings;
	}

TBool CIniFileParser::LogValid(const TDesC8& aSubsystem, const TDesC8& aComponent) const
// If no memory, this operation will return EFalse so user will not get logs, nor a msg to say so.
	{
	if (iIniSettings)
		{
		TBool result = iIniSettings->SettingValid(aSubsystem, aComponent);
		return result;
		}
	else
		{
		return EFalse;
		}
	}

void CIniFileParser::LogMediaSetting(TDes8& aString) const
	{
	aString=iLoggingMediaString;
	}

void CIniFileParser::LogPathSetting(TDes8& aString) const
	{
	aString.Copy(iLoggingPathString);
	}

void CIniFileParser::FlushingOn(TBool& aFlushingOn) const
/*
 * Return whether flushing is on or off.
 */
	{
	if (iForceFlushState == EFlushOn)
		{
		aFlushingOn = ETrue;
		}
	else
		{
		aFlushingOn = EFalse;
		}
	}

void CIniFileParser::FinaliseFlushSetting()
/*
 * It is not safe to switch flush modes once we begin logging, so this is called
 * to let fileparser know that system is about to commence logging.
 * If the file parser has still not determined whether it should be logging or not,
 * then set flushing to off.
 */
	{
	if (iForceFlushState == ENoValue)
		{
		iForceFlushState = EFlushOff;
		}
	}

CIniFileParser::CIniFileParser(RFs& aFs)
: iFs(aFs)
	{
	iForceFlushState = ENoValue;
	}

/////////////////////////////////////////////

CSubsystemSettings::~CSubsystemSettings()
	{
	iComponentArray.Close();
	}

TInt CSubsystemSettings::AddComponent(const TDesC8& aComponent)
	{
	return iComponentArray.InsertInOrder(TNameTag(aComponent),
		TLinearOrder<TNameTag>(CompareTComponent));
	}

TBool CSubsystemSettings::ElementExistsInArray(const TDesC8& aComponent) const
	{
	return (iComponentArray.FindInOrder(aComponent, TLinearOrder<TNameTag>(CSubsystemSettings::CompareTComponent))!=KErrNotFound);
	}

CSubsystemSettings::CSubsystemSettings(const TDesC8& aName)
: iSubsystemName(aName)
	{}

void CSubsystemSettings::SetSubsystemName(const TDesC8& aName)
	{
	iSubsystemName = aName;
	}
TInt CSubsystemSettings::CompareCSubsystemSettings(const CSubsystemSettings& aFirst, const CSubsystemSettings& aSecond)
	{
	return aFirst.iSubsystemName.CompareF(aSecond.iSubsystemName);
	}

TInt CSubsystemSettings::CompareTComponent(const TNameTag& aFirst, const TNameTag& aSecond)
	{
	return aFirst.CompareF(aSecond);
	}

void CSubsystemSettings::SetWildCarded(TBool aWildCarded)
	{
	iSubsystemWildcarded = aWildCarded;
	}

CIniLoggingPairs* CIniLoggingPairs::NewL()
	{
	CIniLoggingPairs* self = new(ELeave) CIniLoggingPairs;
	if((self->iProbeSubsystem = new CSubsystemSettings(KNullDesC8)) == NULL)
		{
		delete self;
		User::Leave(KErrNoMemory);
		}
	return self;
	}
	
CIniLoggingPairs::~CIniLoggingPairs()
	{
	iSubsystems.ResetAndDestroy();
	delete iProbeSubsystem;
	}

void CIniLoggingPairs::AddSettingL(const TDesC8& aSubsystem, const TDesC8& aComponent)
	{
	TBool subsystemIsNewInList(EFalse);
	CSubsystemSettings* subsystemSetting;
	iProbeSubsystem->SetSubsystemName(aSubsystem);
	TInt subsysPos = iSubsystems.FindInOrder(iProbeSubsystem, TLinearOrder<CSubsystemSettings>(CSubsystemSettings::CompareCSubsystemSettings));
	if (subsysPos == KErrNotFound)
		{
		subsystemSetting = new(ELeave) CSubsystemSettings(aSubsystem);
		CleanupStack::PushL(subsystemSetting);
		iSubsystems.InsertInOrderL(subsystemSetting, TLinearOrder<CSubsystemSettings>(CSubsystemSettings::CompareCSubsystemSettings));
		subsystemIsNewInList = ETrue;
		CleanupStack::Pop(subsystemSetting);
		}
	else 
		{
		ASSERT(subsysPos >= KErrNone);
		subsystemSetting = iSubsystems[subsysPos];
		}
	//subsystemSetting is now owned in the array, so we don't need to worry about any leaves from here on.
	if (aComponent.CompareF(KSubsystemWildCard)==0)
		{
		subsystemSetting->SetWildCarded(ETrue);
		return;
		}
	TInt err;
	if ((err = subsystemSetting->AddComponent(aComponent))!=KErrNone)
		{
		if (subsystemIsNewInList)
			{
			delete subsystemSetting;
			iSubsystems.Remove(subsysPos);
			User::Leave(err);
			}
		}
	}

TBool CIniLoggingPairs::SettingValid(const TDesC8& aSubsystem, const TDesC8& aComponent) const
	{
	iProbeSubsystem->SetSubsystemName(aSubsystem);
	TBool settingValid;
	TInt positionInArray;
	if (iSubsystems.FindInOrder(iProbeSubsystem, positionInArray, TLinearOrder<CSubsystemSettings>(CSubsystemSettings::CompareCSubsystemSettings))==KErrNotFound)
		{
		settingValid = EFalse;
		}
	else if (iSubsystems[positionInArray]->IsWildCarded())
		{
		settingValid = ETrue;
		}
	else
		{
		settingValid = iSubsystems[positionInArray]->ElementExistsInArray(aComponent);
		}
	return settingValid;
	}

/////////////////////////////////////////////

CTimeManager* CTimeManager::NewL(MLogArrayAccess& aArrayAccess)
	{
	CTimeManager* self = new(ELeave) CTimeManager(aArrayAccess);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CTimeManager::Beat()
/*
 * Called by kernel every second if we're synchronised.
 * Updates the variable recording the current time, and sends it to the log queue.
 */
	{
	iTime+=TTimeIntervalSeconds(1);
	SendTimeUpdate();
	}

void CTimeManager::Synchronize()
/*
 * Called by kernel when it finds synchronisation is lost (a heartbeat was missed - maybe
 * device has been off for a while).
 * Updates the variable recording the current time, and sends it to the log queue.
 */
	{
	iTime.HomeTime();
	SendTimeUpdate();
	}

CTimeManager::CTimeManager(MLogArrayAccess& aArrayAccess)
: iArrayAccess(aArrayAccess)
{}

void CTimeManager::SendTimeUpdate()
/*
 * append to queue a time update message
 */
	{
	CTimeUpdateMessage* timeMessage = new CTimeUpdateMessage(iTime);
	if (timeMessage)
		{
		if (iArrayAccess.AppendAndGiveOwnership(timeMessage)!=KErrNone)
			{
			delete timeMessage;	//We failed to get a time update in. Ahh well. 
			}
		}
	}

void CTimeManager::ConstructL()
/*
 * Start heartbeat active object to trigger every second, passing in this class
 * for kernel to callback to.
 */
	{
	iTime.HomeTime();
	iHeartbeat = CHeartbeat::NewL(EPriorityHigh);
	iHeartbeat->Start(ETwelveOClock, this); // 12 1/12th intervals
	SendTimeUpdate();
	}

CTimeManager::~CTimeManager()
	{
	delete iHeartbeat;
	}

