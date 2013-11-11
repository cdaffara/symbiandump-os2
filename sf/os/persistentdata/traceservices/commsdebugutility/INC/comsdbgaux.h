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
// Flogger (File and Serial logger) utils header
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef __COMSDBGAUX_H__
#define __COMSDBGAUX_H__

#include <e32base.h>
#include "comsdbgsvr.h"

class CFileLoggerServer;
class RFs;


class CIniFileWatcher : public CActive
	{
public:
	static CIniFileWatcher* NewL(RFs& aFs, CFileLoggerServer& aServer, const TDesC& aIniFile);
	//From CActive
	virtual void RunL();
	virtual void DoCancel();
	~CIniFileWatcher();
private:
	CIniFileWatcher(RFs& aFs, CFileLoggerServer& aServer, const TDesC& aIniFile);
	void Initialize();
   void NotifyChange();
private:
	RFs& iFs;
	CFileLoggerServer& iServer;
	TFullName iWatchedIniFile;
	};

class MLogArrayAccess;

class CTimeManager : public CBase, public MBeating
	{
public:
	static CTimeManager* NewL(MLogArrayAccess& aArrayAccess);
	//From MBeating
	virtual void Beat();
	virtual void Synchronize();
	~CTimeManager();
private:
	CTimeManager(MLogArrayAccess& aArrayAccess);
	void ConstructL();
	void SendTimeUpdate();
private:
	MLogArrayAccess& iArrayAccess;
	CHeartbeat* iHeartbeat;
	TTime iTime;
	};

class MIniFlushModeAndLogValidQuery
	{
public:
	virtual TBool LogValid(const TDesC8& aSubsystem, const TDesC8& aComponent) const = 0;
	virtual void FlushingOn(TBool& aFlushOn) const = 0;
	};

class MIniLoggingMediaQuery
	{
public:
	virtual void LogMediaSetting(TDes8& aString) const = 0;
	virtual void FlushingOn(TBool& aFlushOn) const = 0;
	virtual void LogPathSetting(TDes8& aString) const = 0;
	};



class CSubsystemSettings : public CBase
	{
public:
	CSubsystemSettings(const TDesC8& aName);
public:
	~CSubsystemSettings();
	void SetSubsystemName(const TDesC8& aName);
	TInt AddComponent(const TDesC8& aComponent);
	static TInt CompareCSubsystemSettings(const CSubsystemSettings& aFirst, const CSubsystemSettings& aSecond);
	static TInt CompareTComponent(const TNameTag& aFirst, const TNameTag& aSecond);
	void SetWildCarded(TBool aWildCarded);
	TBool IsWildCarded() const {return iSubsystemWildcarded;}
	TBool ElementExistsInArray(const TDesC8& aComponent) const ;
private:
	TBool iSubsystemWildcarded;
	TNameTag iSubsystemName;
	RArray<TNameTag> iComponentArray;
	};

class CIniLoggingPairs : public CBase
	{
public:
	static CIniLoggingPairs* NewL();
	void AddSettingL(const TDesC8& aSubsystem, const TDesC8& aComponent);
	~CIniLoggingPairs();
	TBool SettingValid(const TDesC8& aSubSystem, const TDesC8& aComponent) const;
private:
	CIniLoggingPairs()
		{ }
private:
	RPointerArray<CSubsystemSettings> iSubsystems;
	CSubsystemSettings* iProbeSubsystem;
	};

class CIniFileParser : public CBase, public MIniFlushModeAndLogValidQuery, public MIniLoggingMediaQuery
	{
public:
	static CIniFileParser* NewL(RFs& aFs);
	~CIniFileParser();
	TInt ParseIniFile(TDesC& aIniFile);
	//From MIniFlushModeAndLogValidQuery
	virtual TBool LogValid(const TDesC8& aSubsystem, const TDesC8& aComponent) const;
	//From MiniLoggingMediaQuery
	virtual void LogMediaSetting(TDes8& aString) const;
	virtual void LogPathSetting(TDes8& aString) const;
	// From MIniFlushModeAndLogValidQuery and MiniLoggingMediaQuery
	virtual void FlushingOn(TBool& aFlushOn) const;
	void FinaliseFlushSetting();
	TBool Win32DebugEnabled() const { return iWin32DebugEnabled; }
private:
	CIniFileParser(RFs& aFs);
	void DoParseIniFileL(TDesC& aIniFile);
	TInt GetNextTokenAndCheck(TLex8& lex, TPtr8& tempPtr);
private:
	RFs& iFs;
	TBuf8<KMaxMediaStringLength> iLoggingMediaString;
	TFlushStates iForceFlushState;
	TBool iWin32DebugEnabled;
	//iIniSettings == NULL <==> bad ini file, logging is off and client has been told.
	CIniLoggingPairs* iIniSettings;
	TFilePath iLoggingPathString;
	};

#endif //__COMSDBGAUX_H__

