/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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


// This file defines the API for DebOutWriterPlugin.dll

#ifndef __DEBOUTWRITERPLUGIN_H__
#define __DEBOUTWRITERPLUGIN_H__

//  Include Files
#include <w32std.h>
#include <piprofiler/WriterPluginInterface.h>
#include <piprofiler/ProfilerGenericClassesUsr.h>
#include <e32base.h>	// CBase
#include <e32std.h>	 // TBuf
#include <e32property.h>

#ifdef OST_TRACE_COMPILER_IN_USE
// trace core ldd activation/deactivation interface
#include <TcDriverIf.h>

// trace core ldd global definitions
_LIT( KTcLdd, "tcldd.ldd" );
GLDEF_D RTcDriver tcldd;

#endif

_LIT(KDebOutShortName, "dow");

// forward declarations
class CDebOutWriterHandler;

//  Class Definitions

class CDebOutWriterPlugin : public CWriterPluginInterface
	{
public:
	// new function
	static CDebOutWriterPlugin* NewL(const TUid aImplementationUid, TAny* /*aInitParams*/);
	~CDebOutWriterPlugin();

	void	DoCancel();
	static void 	PrintDescriptorAsBase64(TDesC8& aDes, TRequestStatus* aStatus, TUint32 sampleTime, TBool aEmptying);

public: // new functions

	TInt 	Start();

	void 	Stop();
    
	void 	GetValue( const TWriterPluginValueKeys aKey, TDes& aValue );
	
	void 	SetValue( const TWriterPluginValueKeys aKey, TDes& aValue ); 
    
	TUid 	Id() const;
		 
	void 	GetWriterVersion(TDes* aDes);
	
	TUint32 GetWriterType();
	
	void    SetStream(CProfilerSampleStream& aStream) { iStream = &aStream; }
	
	void   HandleError(TInt aError);
	   
	void   WriteData();
private: // new functions
	CDebOutWriterPlugin(const TUid aImplementationUid);
	void 	ConstructL();
	
	void 	GetValueL( const TWriterPluginValueKeys aKey, TDes& aValue );
	void 	SetValueL( const TWriterPluginValueKeys aKey, TDes& aValue );
	TBool	GetEnabled();
public:
    CProfilerSampleStream*          iStream;
private: // data
	TBapBuf*						iBufferBeingWritten;
	
	TInt 							iWriterType;
	TInt							iWriterId;
	CDebOutWriterHandler*			iWriterHandler;
	RProperty                       iErrorStatus;

	};

/*
 * 
 * Definition of class CDebOutWriterHandler
 * 
 */
class CDebOutWriterHandler : public CActive
	{
public:
 

	static CDebOutWriterHandler* NewL(CDebOutWriterPlugin* aWriter);
	~CDebOutWriterHandler();
    void DoCancel();
	void StartL();

	void Stop();
    
    void Reset();
private:
	CDebOutWriterHandler(CDebOutWriterPlugin* aWriter); 
	
	void ConstructL();
    void RunL();
    
	void WriteBufferToOutput(TBapBuf* aBuf);
	void PrintBufferToOutput(TBapBuf* aBuffer, TRequestStatus& aStatus);
	void HandleFullBuffers();
private:
	CDebOutWriterPlugin* 			iWriter;
    RFile                           iFile;
    RFs                             iFs;
    TBuf<256>                       iFileName;
//  CPeriodic*                      iTimer;
    TBapBuf*                        iBufferBeingWritten;
    TBool                           iStopping;
	};

#endif  // __DEBOUTWRITERPLUGIN_H__

