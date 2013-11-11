// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//


#ifndef PORTBUFFERHANDLER_H
#define PORTBUFFERHANDLER_H

#include <e32base.h>
#include <f32file.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>
#include <openmax/il/khronos/v1_x/OMX_Types.h>

#include "omx_xml_script.h"

#if defined(__WINSCW__)
_LIT(KPortDirectoryPath, "c:\\mm\\omxlogicalil\\");
#else
_LIT(KPortDirectoryPath, "e:\\unittest\\mm\\omxlogicalil\\");
#endif

const OMX_VERSIONTYPE KOmxLILVersion = { 1, 1, 1, 0 };

NONSHARABLE_CLASS(CPortBufferHandler) : public CBase
	{
public:
	void AddPortL( OMX_COMPONENTTYPE* aComp,
			TInt aPortNum,
			const TDesC& aFileName,
			OMX_BUFFERSUPPLIERTYPE aSupplier,
			OMX_DIRTYPE aPortDir,
			MOmxScriptTestLogger& aLogger,
			TInt aHeaderLength);

	void AllocateBuffersL(OMX_COMPONENTTYPE* aComp);
	void BufferDone(OMX_COMPONENTTYPE* aComp, OMX_BUFFERHEADERTYPE* aBufHdr, TBool aSource);
	void FreeBuffers(OMX_COMPONENTTYPE* aComp);
	void Start(OMX_COMPONENTTYPE* aComp, TInt aPortNum);
	void Resume(OMX_COMPONENTTYPE* aComp, TInt aPortNum);
	void Pause(OMX_COMPONENTTYPE* aComp, TInt aPortNum);
	void Stop(OMX_COMPONENTTYPE* aComp, TInt aPortNum);
		
	CPortBufferHandler();
	~CPortBufferHandler();
	
private:
	NONSHARABLE_CLASS(CPortInfo) : public CBase
		{
	public:
		static CPortInfo* NewL( OMX_COMPONENTTYPE* aComp,
				TInt aPortNum,
				const TDesC& aFileName,
				OMX_BUFFERSUPPLIERTYPE aSupplier,
				OMX_DIRTYPE aPortDir,
				MOmxScriptTestLogger& aLogger,
				TInt aHeaderLength);
		
		CPortInfo( OMX_COMPONENTTYPE* aComp,
				   TInt aPortNum,
				   OMX_BUFFERSUPPLIERTYPE aSupplier,
				   OMX_DIRTYPE aPortDir,
				   MOmxScriptTestLogger& aLogger,
				   TInt aHeaderLength);
		~CPortInfo();
		void AllocateBuffersL();
		void BufferDone(OMX_BUFFERHEADERTYPE* aBufHdr);
		void FreeBuffers();
		void DeleteAllBuffers();
		static TBool ComparePortNum(const TInt* aPortNum, const CPortInfo& aPortInfo);
		void Start();
		void Stop();
		void Resume();
		void Pause();
		OMX_DIRTYPE& PortDir();
		void LogExtra(const TText8* aFile, TInt aLine, TOmxScriptSeverity aSeverity,
		            TRefByValue<const TDesC16> aFmt,...);

		enum 
			{
			EBufferFree,
			EBufferUnderUse
			};

	private:
		TBuf<KMaxFileName> iFileName;		
		RFs iFs;		
		RFile iFileObject;
		OMX_COMPONENTTYPE* iComp;
		TInt iPortNum;
		TInt iNumBufs;
		OMX_BUFFERSUPPLIERTYPE iSupplier;
		RPointerArray<OMX_U8> iBuffers;
		RPointerArray<OMX_BUFFERHEADERTYPE> iBufferHeaders;
		OMX_DIRTYPE iPortDir;
		TInt iPosInputData;
		TInt iFileHeaderLength;
		TInt iFileFillingLength;
		MOmxScriptTestLogger& iLogger;
	    TBool iEOSReached;
	    TUint32 iTimeStamp;
		};
		
	NONSHARABLE_CLASS(CComponentInfo) : public CBase
		{
	public:
		CComponentInfo(OMX_COMPONENTTYPE* aComp);
		~CComponentInfo();
		CPortInfo* AddPortL(TInt aPortNum, const TDesC& aFileName,OMX_BUFFERSUPPLIERTYPE aSupplier, OMX_DIRTYPE aPortDir, MOmxScriptTestLogger& aLogger, TInt aHeaderLength);
		void AllocateBuffersL();
		void FillBuffers();
		void BufferDone(OMX_BUFFERHEADERTYPE* aBufHdr, TBool aSource);
		void HoldBuffers();
		void FreeBuffers();
		static TBool CompareComp(const OMX_COMPONENTTYPE* aSource, const CComponentInfo& aComponentInfo);
		
		void Start(TInt aPortNum);
		void Stop(TInt aPortNum);
		void Resume(TInt aPortNum);
		void Pause(TInt aPortNum);

	private:
		OMX_COMPONENTTYPE* iComp;
		RPointerArray<CPortInfo> iPorts;
		OMX_STATETYPE iState;
		};	
		
	
	RPointerArray<CComponentInfo> iComponents;
	};

#endif // PORTBUFFERHANDLER_H
