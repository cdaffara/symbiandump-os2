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


#ifndef __DISKWRITERPLUGIN_H__
#define __DISKWRITERPLUGIN_H__

//  Include Files
#include <w32std.h>
#include <piprofiler/WriterPluginInterface.h>
#include <piprofiler/ProfilerGenericClassesUsr.h>
#include <e32base.h>	// CBase
#include <e32std.h>	 // TBuf
#include <e32property.h>

_LIT(KFileOutShortName, "dsw");

// forward declarations
class CDiskWriterHandler;
class CProfilerSampleStream;

//  Class Definitions

class CDiskWriterPlugin : public CWriterPluginInterface
	{
public:
	// new function
	static CDiskWriterPlugin* NewL(const TUid aImplementationUid, TAny* aInitParams);
	~CDiskWriterPlugin();

	void	DoCancel();

	
public: // new functions
	
	TInt 	Start();

	void 	Stop();
    
	void 	GetValue( const TWriterPluginValueKeys aKey, TDes& aValue );
	
	void 	SetValue( const TWriterPluginValueKeys aKey, TDes& aValue ); 
    
	TUid 	Id() const;
		 
	void 	GetWriterVersion(TDes* aDes);
	
	TUint32 GetWriterType();

	void   HandleError(TInt aError);
	
	void   WriteData();
	
	void   SetStream(CProfilerSampleStream& aStream) { iStream = &aStream; }
	
private: // new functions
	CDiskWriterPlugin(const TUid aImplementationUid);
	void 	ConstructL();

	void 	GetValueL( const TWriterPluginValueKeys aKey, TDes& aValue );
	void 	SetValueL( const TWriterPluginValueKeys aKey, TDes& aValue );
    TBool   GetEnabled();
	
	TInt   DisplayNotifier(const TDesC& aLine1, const TDesC& aLine2, const TDesC& aButton1, const TDesC& aButton2);
public: 
    CProfilerSampleStream*          iStream;
private: // data

    TBuf<256>                       iFileName;
	TInt 							iWriterType;
	TInt							iWriterId;
	CDiskWriterHandler*				iWriterHandler;
	RProperty                       iErrorStatus;
	};

/*
 * 
 * Definition of class CDiskWriterHandler
 * 
 */
class CDiskWriterHandler : public CActive //CBase
	{
public:
 

	static CDiskWriterHandler* NewL(CDiskWriterPlugin* aWriter);
	~CDiskWriterHandler();

	void StartL();

	void Stop();
	void RunL();
	TInt RunError(TInt aError);
	
	void DoCancel();
	void WriteBufferToOutput(TBapBuf* aBuf);
	void Reset();
	   
    TInt TestFile(const TDesC& totalPrefix);
private:
	CDiskWriterHandler(CDiskWriterPlugin* aWriter); 
	
	void ConstructL();
    void WriteMemBufferToFile(TDesC8& aDes, TRequestStatus& aStatus);
	static TInt Write(TAny* aObject);
	void HandleFullBuffers();
	void DoWrite();
	
private:
    RFile                           iFile;
    RFs                             iFs;
    TBuf<256>                       iFileName;
	CDiskWriterPlugin* 				iWriter;
    TBapBuf*                        iBufferBeingWritten;
    TBool                           iStopping;
	};

#endif  // __DEBOUTWRITERPLUGIN_H__

