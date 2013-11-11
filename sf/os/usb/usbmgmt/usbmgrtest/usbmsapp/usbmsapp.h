/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <e32property.h>		// RProperty
#include <usbman.h>

#include "usbmsshared.h"		// TUsbMsBytesTransferred

/**
    Class describing a file system mounted on the drive.
    It has a limitation: full list of possible FS extensions is not supported, only the primary one.
*/
NONSHARABLE_CLASS(CFileSystemDescriptor) : public CBase
    {
 public:
    ~CFileSystemDescriptor();    
    static CFileSystemDescriptor* NewL(const TDesC& aFsName, const TDesC& aPrimaryExtName, TBool aDrvSynch);

    TPtrC FsName() const            {return iFsName;}
    TPtrC PrimaryExtName() const    {return iPrimaryExtName;}
    TBool DriveIsSynch() const      {return iDriveSynch;}


 private:
    CFileSystemDescriptor() {}
 
 private:   
    RBuf    iFsName;        ///< file system name
    RBuf    iPrimaryExtName;///< name of the primary extension if present. Empty otherwise
    TBool   iDriveSynch;    ///< ETrue if the drive is synchronous
    };


    
/**
A set of static objects that hold the latest properties published by Mass Storage,
and a set of corresponding static functions that process the publish events. 
The functions are passed by pointer to, and invoked by, CPropertyWatch instances.
*/
NONSHARABLE_CLASS(PropertyHandlers) {
public:
	/** The prototype for all public property handler functions */
	typedef void(*THandler)(RProperty&);

public:
	static void Read(RProperty& aProperty);
	static void Written(RProperty& aProperty);
	static void DriveStatus(RProperty& aProperty);
private:
	static void Transferred(RProperty& aProperty, TUsbMsBytesTransferred& aReadOrWritten);

public:
	static TBuf8<16> allDrivesStatus;
	static TUsbMsBytesTransferred iKBytesRead;
	static TUsbMsBytesTransferred iKBytesWritten;
	};

/**
An active object that subscribes to a specified Mass Storage property and
calls a provided handler each time the property is published.
*/
NONSHARABLE_CLASS(CPropertyWatch) : public CActive
	{
public:
	static CPropertyWatch* NewLC(TUsbMsDriveState_Subkey aSubkey, PropertyHandlers::THandler aHandler);
private:
	CPropertyWatch(PropertyHandlers::THandler aHandler);
	void ConstructL(TUsbMsDriveState_Subkey aSubkey);
	~CPropertyWatch();
	void RunL();
	void DoCancel();
	
	RProperty iProperty;
	PropertyHandlers::THandler iHandler;
	};

/**
An active object that handles changes to the KUsbMsDriveState properties.
*/
NONSHARABLE_CLASS(CUsbWatch) : public CActive
	{
public:
	static CUsbWatch* NewLC(RUsb& aUsb);
private:
	CUsbWatch(RUsb& aUsb);
	void ConstructL();
	~CUsbWatch();
	void RunL();
	void DoCancel();

private:
	RUsb& iUsb;
	TUsbDeviceState iUsbDeviceState;
	TBool iIsConfigured;
	};

/**
An active object for handling user menu selections.
*/
NONSHARABLE_CLASS(CMessageKeyProcessor) : public CActive
	{
public:
	static CMessageKeyProcessor* NewLC(CConsoleBase* aConsole);
	static CMessageKeyProcessor* NewL(CConsoleBase* aConsole);
	~CMessageKeyProcessor();

public:
	  // Issue request
	void RequestCharacter();
	  // Cancel request.
	  // Defined as pure virtual by CActive;
	  // implementation provided by this class.
	void DoCancel();
	  // Service completed request.
	  // Defined as pure virtual by CActive;
	  // implementation provided by this class,
	void RunL();
	  // Called from RunL() to handle the completed request
	void ProcessKeyPress(TChar aChar); 

private:
	CMessageKeyProcessor(CConsoleBase* aConsole);
	void ConstructL();

private:
	CConsoleBase* iConsole; // A console for reading from
	};



