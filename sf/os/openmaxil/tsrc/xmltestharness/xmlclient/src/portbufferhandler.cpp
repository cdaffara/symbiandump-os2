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


#include "portbufferhandler.h"
#include "log.h"  // currently needed for INFO_PRINTF etc macros

/**
 * Overflow handler to generate a warning message if a log line will not fit
 * in the descriptor.
 */
class TOverflowHandler : public TDes16Overflow
    {
public:
    void Overflow(TDes& aDes)
        {
        _LIT(KWarning, "[truncated]");
        if(aDes.Length() + KWarning().Length() > aDes.MaxLength())
            {
            aDes.SetLength(aDes.Length() - KWarning().Length());
            }
        aDes.Append(KWarning);
        }
    };

CPortBufferHandler::CPortBufferHandler()
	{
	}

CPortBufferHandler::~CPortBufferHandler()
	{
	}

void CPortBufferHandler::AddPortL(OMX_COMPONENTTYPE* aComp,
								TInt aPortNum,
								const TDesC& aFileName,
								OMX_BUFFERSUPPLIERTYPE aSupplier,
								OMX_DIRTYPE aPortDir,
								MOmxScriptTestLogger& aLogger,
								TInt aHeaderLength)
	{
		
	CComponentInfo* compInfo = NULL;
	TInt pos = iComponents.Find(*aComp, CComponentInfo::CompareComp);
	if (pos >= 0)
		{
		compInfo = iComponents[pos];
		}
	else
		{
		compInfo = new (ELeave) CComponentInfo(aComp);
		CleanupStack::PushL(compInfo);
		iComponents.AppendL(compInfo);
		CleanupStack::Pop(compInfo);
		}
		
			
	CPortInfo* portInfo = compInfo->AddPortL(aPortNum,aFileName,aSupplier,aPortDir,aLogger,aHeaderLength);
	}

void CPortBufferHandler::AllocateBuffersL(OMX_COMPONENTTYPE* aComp)
	{
	TInt pos = iComponents.Find(*aComp, CComponentInfo::CompareComp);
	if (pos >= 0)
		{
		iComponents[pos]->AllocateBuffersL();
		}
	}
	
void CPortBufferHandler::Start(OMX_COMPONENTTYPE* aComp, TInt aPortNum)
	{
	TInt pos = iComponents.Find(*aComp, CComponentInfo::CompareComp);
	if (pos >= 0)
		{
		iComponents[pos]->Start(aPortNum);
		}
	}
	
void CPortBufferHandler::Resume(OMX_COMPONENTTYPE* aComp, TInt aPortNum)
	{
	TInt pos = iComponents.Find(*aComp, CComponentInfo::CompareComp);
	if (pos >= 0)
		{
		iComponents[pos]->Resume(aPortNum);
		}
	}	

void CPortBufferHandler::BufferDone(OMX_COMPONENTTYPE* aComp, OMX_BUFFERHEADERTYPE* aBufHdr, TBool aSource)
	{
	TInt pos = iComponents.Find(*aComp, CComponentInfo::CompareComp);
	if (pos >= 0)
		{
		iComponents[pos]->BufferDone(aBufHdr, aSource);
		}
	}

void CPortBufferHandler::Stop(OMX_COMPONENTTYPE* aComp, TInt aPortNum)
	{
	TInt pos = iComponents.Find(*aComp, CComponentInfo::CompareComp);
	if (pos >= 0)
		{
		iComponents[pos]->Stop(aPortNum);
		}
	}
	

void CPortBufferHandler::Pause(OMX_COMPONENTTYPE* aComp, TInt aPortNum)
	{
	TInt pos = iComponents.Find(*aComp, CComponentInfo::CompareComp);
	if (pos >= 0)
		{
		iComponents[pos]->Pause(aPortNum);
		}
	}	

void CPortBufferHandler::FreeBuffers(OMX_COMPONENTTYPE* aComp)
	{
	TInt pos = iComponents.Find(*aComp, CComponentInfo::CompareComp);
	if (pos >= 0)
		{
		iComponents[pos]->FreeBuffers();
		}
	}

CPortBufferHandler::CComponentInfo::CComponentInfo(OMX_COMPONENTTYPE* aComp) :
 iComp(aComp),iState(OMX_StateLoaded)
	{
	}

CPortBufferHandler::CComponentInfo::~CComponentInfo()
	{
	iPorts.ResetAndDestroy();
	}

CPortBufferHandler::CPortInfo* CPortBufferHandler::CComponentInfo::AddPortL(TInt aPortNum, const TDesC& aFileName,OMX_BUFFERSUPPLIERTYPE aSupplier, OMX_DIRTYPE aPortDir, MOmxScriptTestLogger& aLogger, TInt aHeaderLength)
	{
	__ASSERT_ALWAYS(iPorts.Find(aPortNum, CPortInfo::ComparePortNum) == KErrNotFound, User::Invariant());

	CPortInfo* portInfo = CPortInfo::NewL(iComp, aPortNum, aFileName, aSupplier, aPortDir, aLogger, aHeaderLength);
	CleanupStack::PushL(portInfo);
	iPorts.AppendL(portInfo);
	CleanupStack::Pop(portInfo);

	return portInfo;
	}

void CPortBufferHandler::CComponentInfo::AllocateBuffersL()
	{
	TInt numPorts = iPorts.Count();
	
	for (TInt port = 0; port < numPorts; ++port)
		{
		iPorts[port]->AllocateBuffersL();
		}
	iState = OMX_StateIdle;	
	}

void CPortBufferHandler::CComponentInfo::Start(TInt aPortNum)
	{
	TInt numPorts = iPorts.Count();
	
	if(aPortNum == -1)
		{
		for (TInt port = 0; port < numPorts; ++port)
			{
			iPorts[port]->Start();
			}
		}
	else
		{
		TInt pos = iPorts.Find(aPortNum, CPortInfo::ComparePortNum);
		iPorts[pos]->Start();
		}		
	iState = OMX_StateExecuting;
	}

void CPortBufferHandler::CComponentInfo::Resume(TInt aPortNum)
	{
	if(aPortNum == -1)
		{ 
		TInt numPorts = iPorts.Count();
		
		for (TInt port = 0; port < numPorts; ++port)
			{
			iPorts[port]->Resume();
			}
		}
	else
		{
		TInt pos = iPorts.Find(aPortNum, CPortInfo::ComparePortNum);
		iPorts[pos]->Resume();
		}	
	iState = OMX_StateExecuting;
	}

void CPortBufferHandler::CComponentInfo::BufferDone(OMX_BUFFERHEADERTYPE* aBufHdr, TBool aSource)
	{
	TInt pos = KErrNotFound;

	if (aSource)
		{
		pos = iPorts.Find(static_cast<TInt>(aBufHdr->nOutputPortIndex), CPortInfo::ComparePortNum);
		}
	else
		{
		pos = iPorts.Find(static_cast<TInt>(aBufHdr->nInputPortIndex), CPortInfo::ComparePortNum);
		}

	__ASSERT_ALWAYS(pos != KErrNotFound, User::Invariant());

	iPorts[pos]->BufferDone(aBufHdr);
	}

void CPortBufferHandler::CComponentInfo::Stop(TInt aPortNum)
	{
	if(aPortNum == -1)
		{
		for (TInt port = 0; port < iPorts.Count(); ++port)
			{
			iPorts[port]->Stop();
			}
		}
	else
		{
		TInt pos = iPorts.Find(aPortNum, CPortInfo::ComparePortNum);
		iPorts[pos]->Stop();
		}		
	iState = OMX_StateIdle;
	}

void CPortBufferHandler::CComponentInfo::Pause(TInt aPortNum)
	{
	if(aPortNum == -1)
		{	
		for (TInt port = 0; port < iPorts.Count(); ++port)
			{
			iPorts[port]->Pause();
			}
		}
	else
		{
		TInt pos = iPorts.Find(aPortNum, CPortInfo::ComparePortNum);
		iPorts[pos]->Pause();	
		}
	iState = OMX_StatePause;
	}

void CPortBufferHandler::CComponentInfo::FreeBuffers()
	{
	for (TInt port = 0; port < iPorts.Count(); ++port)
		{
		iPorts[port]->FreeBuffers();
		}
	iState = OMX_StateLoaded;	
	}

TBool CPortBufferHandler::CComponentInfo::CompareComp(const OMX_COMPONENTTYPE* aComp, const CComponentInfo& aComponentInfo)
	{
	return (aComp == aComponentInfo.iComp);
	}


CPortBufferHandler::CPortInfo::CPortInfo( OMX_COMPONENTTYPE* aComp,
		   TInt aPortNum,
		   OMX_BUFFERSUPPLIERTYPE aSupplier,
		   OMX_DIRTYPE aPortDir,
		   MOmxScriptTestLogger& aLogger,
		   TInt aHeaderLength):
	 iComp(aComp),
	 iPortNum(aPortNum),
	 iSupplier(aSupplier),
	 iPortDir(aPortDir),
	 iPosInputData(0),
	 iFileHeaderLength(0),
	 iFileFillingLength(0),
	 iLogger(aLogger),
	 iEOSReached(FALSE),
	 iTimeStamp(0)
	{
    if(aHeaderLength > -1)
        {
        iFileHeaderLength = aHeaderLength;
        }
	}

CPortBufferHandler::CPortInfo* CPortBufferHandler::CPortInfo::NewL( OMX_COMPONENTTYPE* aComp,
											TInt aPortNum,
											const TDesC& aFileName,
											OMX_BUFFERSUPPLIERTYPE aSupplier,
											OMX_DIRTYPE aPortDir,
											MOmxScriptTestLogger& aLogger,
											TInt aHeaderLength)
	{
	CPortInfo* self = new (ELeave) CPortInfo(aComp,aPortNum,aSupplier,aPortDir,aLogger,aHeaderLength);
	CleanupStack::PushL(self);
	
	self->iFileName.Copy(KPortDirectoryPath);
	self->iFileName.Append(aFileName);
	
	User::LeaveIfError(self->iFs.Connect());
	if(self->iPortDir == OMX_DirInput)
		{
		User::LeaveIfError(self->iFileObject.Open(self->iFs, self->iFileName, EFileShareExclusive|EFileRead));
		}
	else if(self->iPortDir == OMX_DirOutput)
		{
		User::LeaveIfError(self->iFileObject.Replace(self->iFs, self->iFileName, EFileWrite));
		}
	else
		{
		User::LeaveIfError(KErrNotFound);	
		}	
		
	CleanupStack::Pop(self);
	return self;
	}

CPortBufferHandler::CPortInfo::~CPortInfo()
	{
	DeleteAllBuffers();
	iFileObject.Close();
	iFs.Close();
	}

void CPortBufferHandler::CPortInfo::AllocateBuffersL()
	{
	OMX_BUFFERHEADERTYPE* bufHdr = NULL;

	if (iComp)
		{
		
		OMX_PARAM_PORTDEFINITIONTYPE portDef;
        portDef.nSize = sizeof(portDef);
        portDef.nVersion = KOmxLILVersion;
        portDef.nPortIndex = iPortNum;
        OMX_ERRORTYPE error = iComp->GetParameter(iComp, OMX_IndexParamPortDefinition, &portDef);
        if(error != OMX_ErrorNone)
            {
            User::Leave(error);
            }
        
        iNumBufs = portDef.nBufferCountActual;
        TInt bufSize = portDef.nBufferSize;
        
        if(iFileHeaderLength <=0)
            {
            iFileHeaderLength = bufSize;
            }
        
        //fix for video partial tunnelling
        //mpeg4decoder needs timestamp info in each frame.
        //Buffer size approximately adjusted to framesize and timestamps added for each buffer
        //this change is done for all video components(needed only for mpeg4decoder)
        
        if(portDef.eDomain == OMX_PortDomainVideo)
            {
			iFileFillingLength = 1600; //bufSize; 
            }
        else
            {
            iFileFillingLength = bufSize;
            }
		
		if (iSupplier == OMX_BufferSupplyInput)
			{
			for (TInt buf = 0; buf < iNumBufs; ++buf)
				{
				iComp->AllocateBuffer(iComp, &bufHdr, iPortNum, NULL, bufSize);
				bufHdr->pAppPrivate = (OMX_PTR)EBufferFree;
				iBufferHeaders.AppendL(bufHdr);
				}
			}
		else
			{
			OMX_U8* newBuffer = NULL;			
			
			for (TInt buf = 0; buf < iNumBufs; ++buf)
				{
				newBuffer = new (ELeave) OMX_U8[bufSize];
				CleanupStack::PushL(newBuffer);
				iBuffers.AppendL(newBuffer);
				CleanupStack::Pop(newBuffer);
				
				OMX_ERRORTYPE err = iComp->UseBuffer(iComp, &bufHdr, iPortNum, NULL, bufSize, newBuffer);
				if(err != OMX_ErrorNone)
					{
					//error encountered in allocating.
					}
				bufHdr->pAppPrivate = (OMX_PTR)EBufferFree;				
				iBufferHeaders.AppendL(bufHdr);
				}
			}
		}
	}

void CPortBufferHandler::CPortInfo::BufferDone(OMX_BUFFERHEADERTYPE* aBufHdr)
	{
	for (TInt buf = 0; buf < iNumBufs; ++buf)
		{
		if (iBufferHeaders[buf] == aBufHdr)
			{
			aBufHdr->pAppPrivate = (OMX_PTR)EBufferFree;
			if(iPortDir == OMX_DirInput)
				{
				TPtr8 data(aBufHdr->pBuffer,aBufHdr->nAllocLen);
				Mem::FillZ(aBufHdr->pBuffer,aBufHdr->nAllocLen);
				// Read data to this buffer.
				TInt err = iFileObject.Read(iPosInputData,data,iFileFillingLength);
				iPosInputData += iFileFillingLength;
				if(err != KErrNone)
					{
					return; //error return.
					}
                
				aBufHdr->nTimeStamp = iTimeStamp* 1000;
                iTimeStamp += 100;
                if(!iEOSReached)
					{
					aBufHdr->nFilledLen = data.Size();
					aBufHdr->nOffset = 0;
					aBufHdr->nInputPortIndex = iPortNum;
					aBufHdr->nOutputPortIndex = OMX_ErrorMax;
					
					//Send buffer to port and mark that it is under use.
					aBufHdr->pAppPrivate = (OMX_PTR)EBufferUnderUse;
					if((data.Size() < aBufHdr->nAllocLen)&&(data.Size() < iFileFillingLength))
						{
						//INFO_PRINTF1(_L("EOS detected on Input Port"));
						aBufHdr->nFlags |= OMX_BUFFERFLAG_EOS;
						iEOSReached = ETrue;
						}
					iComp->EmptyThisBuffer(iComp, aBufHdr);
					}
				}
			else
				{
				//create a pointer into buffer
				// using ptr , write buffer data to File.
				TPtr8 data( aBufHdr->pBuffer+aBufHdr->nOffset, aBufHdr->nFilledLen, aBufHdr->nAllocLen );
				TInt err = iFileObject.Write(data);
				if(err != KErrNone)
					{
					return; //error return.
					}
				err = iFileObject.Flush();	
				if(err != KErrNone)
					{
					return; //error return.
					}
				aBufHdr->pAppPrivate = (OMX_PTR)EBufferFree;
				aBufHdr->nFilledLen = 0;
				aBufHdr->nOffset = 0;
				aBufHdr->nOutputPortIndex = iPortNum;
				aBufHdr->nInputPortIndex = OMX_ErrorMax;
				if(aBufHdr->nFlags & OMX_BUFFERFLAG_EOS)
					{
					// TODO Logic for EOS
					return;
					}
				else
					{
					iComp->FillThisBuffer(iComp, aBufHdr);		
					}
				
				}
			}
		}
	}
void CPortBufferHandler::CPortInfo::Start()
	{
	for (TInt buf = 0; buf < iNumBufs; ++buf)
		{
		OMX_BUFFERHEADERTYPE *bufferHdr = iBufferHeaders[buf];
		
		if (bufferHdr->pAppPrivate == (OMX_PTR)EBufferFree)
			{
			if(iPortDir == OMX_DirInput)
				{
				TPtr8 data(bufferHdr->pBuffer,bufferHdr->nAllocLen);
				// Read data to this buffer.
				TInt err =0;
				if(buf == 0)
					{
					err = iFileObject.Read(iPosInputData,data,iFileHeaderLength);
					iPosInputData += iFileHeaderLength;
					bufferHdr->nFlags |= OMX_BUFFERFLAG_STARTTIME;
					}
				else
					{
					err = iFileObject.Read(iPosInputData,data,iFileFillingLength);
					iPosInputData += iFileFillingLength;	
					}				
				if(err != KErrNone)
					{
					return; //error return.
					}
              
				bufferHdr->nTimeStamp = iTimeStamp * 1000;
                iTimeStamp += 100;
				if(data.Size())
					{
					bufferHdr->nFilledLen = data.Size();
					bufferHdr->nOffset = 0;
					
					//Send buffer to port and mark that it is under use.
					bufferHdr->pAppPrivate = (OMX_PTR)EBufferUnderUse;
					//CHeck for EOF
					if(buf > 0)
						{
						if(data.Size() < iFileFillingLength)
							{
							bufferHdr->nFlags |= OMX_BUFFERFLAG_EOS;
							}	
						}
						
					if(buf == 0)
						{
						if(data.Size() < iFileHeaderLength)
							{
							ASSERT(0);
							return;
							}	
						}	
					
					iComp->EmptyThisBuffer(iComp, bufferHdr);
					}
				}
			else
				{
				bufferHdr->pAppPrivate = (OMX_PTR)EBufferUnderUse;
				bufferHdr->nFilledLen = 0;
				bufferHdr->nOffset = 0;
				iComp->FillThisBuffer(iComp, bufferHdr);	
				}
			}
			
		}
	}

void CPortBufferHandler::CPortInfo::Stop()
	{
//Todo: implement any stop functionality required.
	}

void CPortBufferHandler::CPortInfo::Resume()
	{
//Todo: implement any resume functionality required.
	}
	

void CPortBufferHandler::CPortInfo::Pause()
	{
//Todo: implement any resume functionality required.
	}	

void CPortBufferHandler::CPortInfo::FreeBuffers()
	{
	for (TInt buf = 0; buf < iNumBufs; ++buf)
		{
		iComp->FreeBuffer(iComp, iPortNum, iBufferHeaders[buf]);
		}

	DeleteAllBuffers();
	}

void CPortBufferHandler::CPortInfo::DeleteAllBuffers()
	{
	iBufferHeaders.Reset();

	if (iComp && iSupplier != OMX_BufferSupplyInput)
		{
		iBuffers.ResetAndDestroy();
		}
	else
		{
		iBuffers.Reset();
		}
	}

TBool CPortBufferHandler::CPortInfo::ComparePortNum(const TInt* aPortNum, const CPortInfo& aPortInfo)
	{
	//return (aComp == aPortInfo.iComp);
	return (*aPortNum == aPortInfo.iPortNum);
	}
	
OMX_DIRTYPE& CPortBufferHandler::CPortInfo::PortDir()
	{
	return iPortDir;
	}
	
void CPortBufferHandler::CPortInfo::LogExtra(const TText8* aFile, TInt aLine, TOmxScriptSeverity aSeverity,
            TRefByValue<const TDesC16> aFmt,...)
    {
    VA_LIST aList;
    VA_START(aList, aFmt);

    TOverflowHandler overflow;
    TBuf<255> msg;
    msg.AppendFormatList(aFmt, aList, &overflow);
    
    iLogger.Log(aFile, aLine, aSeverity, msg);
    
    VA_END(aList);
    }
