// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// file descriptor transfer session - client interface
// 
//

#include "fdtransfer_cli.h"
#include "fdtransfermsg.h"
#include "sysif.h"
#include "fdesc.h"
// -------------------------------------------------------------------------------
// RFileDesTransferSession::Connect
// -------------------------------------------------------------------------------
TInt RFileDesTransferSession::Connect(TDesC& aServerName)
    {
	return CreateSession(aServerName, TVersion(0, 0, 0), 1);
    }

//--------------------------------------------------------------------------------
//This passes the pipe to server
//--------------------------------------------------------------------------------

TInt RFileDesTransferSession::TransferPipesToServer()
	{
	const CFileTable& ftable = Backend()->FileTable();
	int count = ftable.GetFileCount();
	CFileDescBase* fdesc = NULL;
	for (TInt i = count-1; i >=3 ; --i)
			{
				if (ftable.At(i, fdesc) == KErrNone)
				{
					TUint attr = fdesc->Attributes();
					/* Named RPipes cannot be passed to another process by IPC
				   Hence the check for KFifoFd 	 */   
					if ((attr & KPipeFd) && !(attr & KFifoFd) && 
					!(attr & KCloseonExec) && !(attr & KSpawnCloseInChild))
					{
					   	SendReceive(ETransferPipe, TIpcArgs(i,((CPipeDescBase*)fdesc)->Handle(),fdesc->FcntlFlag()));
					}
				}
				
			}
	return KErrNone;
	}

//--------------------------------------------------------------------------------
//This passes the files to server
//--------------------------------------------------------------------------------
TInt RFileDesTransferSession::TransferFilesToServer()
	{
	TBuf<256> fname;
	const CFileTable& ftable = Backend()->FileTable();
	CFileDescBase* fdesc = NULL;
	int count = ftable.GetFileCount();
	TInt j;
	for (int i = count-1; i >=3;  --i)
		{
			if (ftable.At(i, fdesc) == KErrNone)
			{
			j=fdesc->Type();
			if((j==CFileDescBase::EFileDesc)||(j==CFileDescBase::EFileTempDesc))
				{
					CFileDesc* fp = (CFileDesc*)(fdesc);
					if (fp)
					{
						TUint attr = fp->Attributes();
						if (!(attr & KCloseonExec) && !(attr & KSpawnCloseInChild))
						{
							fp->FileHandle().FullName(fname);
						    TParsePtr pars(fname);
						    if (pars.Path().FindC(_L("\\PRIVATE\\")) != 0)
							{
							
							(Backend()->iFs).ShareProtected();
							fp->DoSync();
#if defined(SYMBIAN_OE_LARGE_FILE_SUPPORT) && !defined(SYMBIAN_OE_NO_LFS)
							//The integer width is bigger for 64 bit sizes
							TBuf<140> params;
#else		
							TBuf<60> params;
#endif /* SYMBIAN_OE_LARGE_FILE_SUPPORT && !SYMBIAN_OE_NO_LFS */
				        	params.AppendNum(fp->Offset());
					        params.Append(' ');
					        params.AppendNum(fp->Extent());
				         	params.Append(' ');
				        	params.AppendNum(fp->FcntlFlag());
				         	params.Append(' ');	
				        	params.AppendNum(fp->Attributes());
				        	params.Append(' ');
				        	params.AppendNum(fp->Pos());
				        	params.Append(' ');
				        	params.AppendNum(fp->Ext());
				        	params.Append(' ');
				        	params.AppendNum(fp->Size());
				        	TIpcArgs args(i,&params);
				        	if((fp->FileHandle()).TransferToServer(args, 2,3) == KErrNone)  
				        	    SendReceive(ETransferFile, args); 
				        	(Backend()->iFs).ShareAuto();
							}
						}
					}
				}
			}
		}  //for 
	return KErrNone;
	}
