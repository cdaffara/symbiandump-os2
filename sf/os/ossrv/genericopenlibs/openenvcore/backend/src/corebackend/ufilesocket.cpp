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
// conv.CPP
// Descrpition: This file implements the functions for local socket(AF_LOCAL, PF_LOCAL, AF_UNIX) 
// 
//

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <utf.h>
#include "fdesc.h"
#include "systemspecialfilercg.h"
#include "lposix.h"

// -----------------------------------------------------------------------------
// CFileSocketDesc::Socket
// Creates socket using RSocket::Open
// -----------------------------------------------------------------------------
//

TInt CFileSocketDesc::Socket(RSocketServ& aSs, int /*aFamily*/, int aStyle, int aProtocol)
	{
	if(aProtocol == 0)
		{
		switch (aStyle)
			{
			case SOCK_STREAM:
				aProtocol = IPPROTO_TCP;
				break;

			case SOCK_DGRAM:
				aProtocol = IPPROTO_UDP;
				break;

			default:
				aProtocol = KUndefinedProtocol;
			}
		}

	iFcntlFlag = 0;
	iStyle = aStyle;
	iProtocol = aProtocol; 
	TInt err = CreateLock();
	if (err)
		{
		return err;
		}

	err = iSocket.Open(aSs, KAfInet, aStyle, aProtocol);
	return err;

	}


// -----------------------------------------------------------------------------
// CFileSocketDesc::Bind
// Binds the socket to path provided in the  aAddr
// Creates a filesystem entry for the path, binds iSocket to a loopback address and 
// registers the address with ipc server.
// 
// -----------------------------------------------------------------------------

TInt CFileSocketDesc::Bind(const struct sockaddr* aAddr, unsigned long aSize)
	{
	struct SSpecialFileMagicHeader enBuf;
	int err;
	const wchar_t* wspath = NULL;	
	if(aSize == NULL)
		{
		return EINVAL;
		}
		
	sockaddr_un* addr = (sockaddr_un*)aAddr;
	if((err=ValidateAddress(addr,&aSize)) != KErrNone)
		{
		return err;
		}
	if (iPath.Length() != 0)
		{
		//Already bound condition
		return EINVAL;
		}

	err = _EncodeSystemSplFileMagicHeader(&enBuf, EFileTypeSocket);
	if (err != 0)
		{
		return err;
		}

	TPtrC8 sunpath((TUint8 *)addr->sun_path);
	TFileName fname16;
	err = CnvUtfConverter::ConvertToUnicodeFromUtf8(fname16, sunpath);
	if(err != KErrNone)
		{
		return err;
		}

	wspath = (const wchar_t*)(fname16.PtrZ());
	
	TInt errorNum;
	err = _CreateSysSplFile(wspath, (char*)&enBuf, sizeof(struct SSpecialFileMagicHeader), errorNum, Backend()->FileSession());
	if (err != KErrNone)
		{
		if (errorNum == EEXIST)
			{
			err = EADDRINUSE;
			}
		return err;
		}
		
	TFileName fullpathName;
	err=GetFullFile(fullpathName,(const TText16*)wspath,Backend()->FileSession());
	if(err != KErrNone)
		{
		return err;
		}	
	
	err = CnvUtfConverter::ConvertFromUnicodeToUtf8(iPath, fullpathName);
	if(err != KErrNone)
		{
		return err;
		}
		
	TInetAddr inetAddr(KInetAddrLoop,0);
	err = iSocket.Bind(inetAddr);
	if(err != KErrNone)
		{
		iPath.Zero();
		return err;
		}
	TUint portNum = iSocket.LocalPort();
	if(KErrNone == err)
		{
		err = Backend()->iIpcS.RegisterSockAddrWithIPCSvr(iPath,portNum);
		if(KErrNone != err)
			{
			iPath.Zero();
			return err;
			}
		}

	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CFileSocketDesc::Accept
// Accepts connections from clients
// -----------------------------------------------------------------------------
//

void CFileSocketDesc::Accept(CFileDescBase*& aNewSocket, TRequestStatus& aStatus, RSocketServ& aSs, TSockAddr *)
	{
	TAutoFastLock lock(iReadLock);
	if (iStyle == SOCK_DGRAM) // Listen on UDP socket, crashing at RSocket::Listen().
		{
		Complete(aStatus,KErrNotSupported); 	
		return;
		}

	//coverity[alloc_fn]
	//coverity[assign]
	CFileSocketDesc *newSocket = new CFileSocketDesc;
	TInt err=KErrNone;
	if (newSocket!=0)
		{
		err = newSocket->CreateLock();
		if (err)
			{
			Complete(aStatus, KErrNoMemory);
			delete newSocket;
			aNewSocket = NULL;
			//coverity[memory_leak]
			return;
			}

		err=newSocket->iSocket.Open(aSs);
		}
	if (newSocket ==0 || err!=KErrNone)
		{
		Complete(aStatus,KErrNoMemory);
		delete newSocket;
		aNewSocket = NULL;
		//coverity[memory_leak]
		return;
		}

	newSocket->iStyle = iStyle;
	newSocket->iProtocol = iProtocol;
	iSocket.Accept(newSocket->iSocket,aStatus);
	aNewSocket = newSocket;
	}

// -----------------------------------------------------------------------------
// CFileSocketDesc::AcceptCancel
// Cancels the accept request
// -----------------------------------------------------------------------------
//
void CFileSocketDesc::AcceptCancel()
	{
	iSocket.CancelAccept();
	}

// -----------------------------------------------------------------------------
// CFileSocketDesc::Connect
// Gets the local socket port number from the IPCs and connects to that port
// -----------------------------------------------------------------------------
//
void CFileSocketDesc::Connect(const struct sockaddr* addr, unsigned long size,TRequestStatus& aStatus)
	{
	TUint portNo,ret;

	if((ret = GetLocalSockPortByPath((sockaddr_un*)addr,size,portNo)) != KErrNone)
		{
		aStatus = ret;
		return;
		}
	TInetAddr sAddr(KInetAddrLoop,portNo);
	iSocket.Connect(sAddr,aStatus);
	User::WaitForRequest(aStatus);
	}

// -----------------------------------------------------------------------------
// CFileSocketDesc::ConnectCancel
// Cancels the connect request
// -----------------------------------------------------------------------------
//
void CFileSocketDesc::ConnectCancel()
	{
	iSocket.CancelConnect();
	}

// -----------------------------------------------------------------------------
// CFileSocketDesc::Listen
// Sets up the listen buffer
// -----------------------------------------------------------------------------
//
TInt CFileSocketDesc::Listen(TUint qSize)
	{
	if (iStyle == SOCK_DGRAM) // Listen on UDP socket, crashing at RSocket::Listen().
		{
		return EOPNOTSUPP;
		}
	return CSockDescBase::Listen(qSize);
	}

// -----------------------------------------------------------------------------
// CFileSocketDesc::Write
//
// -----------------------------------------------------------------------------

void CFileSocketDesc::Write (TDes8& aBuf, TRequestStatus& aStatus)
	{
	//Acquire the Lock before write and release it later
	TAutoFastLock lock(iWriteLock);	
	CSockDescBase::Write(aBuf,aStatus);
	return;
	}

// -----------------------------------------------------------------------------
// CFileSocketDesc::Read
//
// -----------------------------------------------------------------------------

void CFileSocketDesc::Read (TDes8& aBuf, TRequestStatus& aStatus)
	{
	//Acquire the Lock before read and release it later
	TAutoFastLock lock(iReadLock);
	CSockDescBase::Read(aBuf,aStatus);
	return;
	}

// -----------------------------------------------------------------------------
// CFileSocketDesc::RecvFrom
//
// -----------------------------------------------------------------------------

void CFileSocketDesc::RecvFrom(TDes8& aDesc, TSockAddr& AFrom, int aFlags, TRequestStatus& aStatus)
	{
	TAutoFastLock lock(iReadLock);
	CSockDescBase::RecvFrom(aDesc, AFrom, aFlags, aStatus);
	return;
	}


// -----------------------------------------------------------------------------
// CFileSocketDesc::SendTo
//
// -----------------------------------------------------------------------------

void CFileSocketDesc::SendTo(TDes8& aDesc, const struct sockaddr* aToAddr, unsigned long aAddrLen,int aFlags, TRequestStatus& aStatus)
	{
	TAutoFastLock lock(iWriteLock);	

	if(iStyle == SOCK_DGRAM)
		{
		TUint portNo;
		TInt ret;
		if((ret = GetLocalSockPortByPath((sockaddr_un*)aToAddr,aAddrLen,portNo)) != KErrNone)
			{
			Complete(aStatus,ret);
			return;
			}
		TInetAddr sAddr(KInetAddrLoop,portNo);
		CSockDescBase::SendTo(aDesc, sAddr, aFlags, aStatus);
		return;
		}

	TUSockAddr addr(NULL,0);
	CSockDescBase::SendTo(aDesc, addr, aFlags, aStatus);
	return;
	}


// -----------------------------------------------------------------------------
// CFileSocketDesc::Ioctl
// Not supported as of now. 
// -----------------------------------------------------------------------------

void CFileSocketDesc::Ioctl(int /*aCmd*/, void* /*aParam*/, TRequestStatus& aStatus)
	{
	Complete(aStatus, KErrNotSupported);
	return;
	}

// -----------------------------------------------------------------------------
// CFileSocketDesc::FinalClose
//
// -----------------------------------------------------------------------------

TInt CFileSocketDesc::FinalClose()
	{
	RemoveLocalSockAddr();
	RHeap* oheap = User::SwitchHeap(Backend()->Heap());
	CSockDescBase::FinalClose();
	User::SwitchHeap(oheap);
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CFileSocketDesc::GetSockOpt
// Get the socket option
// -----------------------------------------------------------------------------
TInt CFileSocketDesc::GetSockOpt(TInt anOptionName,TInt anOptionLevel,TDes8& anOption)
	{
	if (anOption.Ptr() == NULL)
		{
		return EFAULT;
		}	
	if (anOption.Length() == 0)
		{
		return EINVAL;
		}
	if(anOptionLevel != SOL_SOCKET)
		{
		return KErrNotSupported;
		}
	switch(anOptionName)
		{
		case SO_TYPE:
			TInt size;
			size = (anOption.MaxLength() < sizeof(iStyle))? anOption.MaxLength(): sizeof(iStyle);
			Mem::Copy((unsigned char*)anOption.Ptr(), &iStyle, size);
			anOption.SetLength(size);
			return KErrNone;
		case SO_KEEPALIVE:
			anOptionLevel=SOL_TCP;
			break;
		case SO_SNDBUF:
		case SO_RCVBUF:
			break;
		default: 
			return KErrNotSupported;
		}

	return iSocket.GetOpt(anOptionName,anOptionLevel,anOption);

	}

// -----------------------------------------------------------------------------
// CFileSocketDesc::SetSockOpt
// Set the socket option
// -----------------------------------------------------------------------------

TInt CFileSocketDesc::SetSockOpt(TInt anOptionName,TInt anOptionLevel,TDesC8& anOption)
	{
	if (anOption.Ptr() == NULL)
		{
		return EFAULT;
		}
	if (anOption.Length() == 0)
		{
		return EINVAL;
		}
	if(anOptionLevel != SOL_SOCKET)
		{
		return KErrNotSupported;
		}
	switch(anOptionName)
		{
		case SO_KEEPALIVE:
			if(iStyle == SOCK_STREAM)
				{
				anOptionLevel=SOL_TCP;
				}
			else
				{	
				return KErrNotSupported;
				}
			break;
		case SO_SNDBUF:
		case SO_RCVBUF:
			break;
		default: 
			return KErrNotSupported;
		}

	return iSocket.SetOpt(anOptionName,anOptionLevel,anOption);

	}

// ---------------------------------------------------------------------------------
// CFileSocketDesc::SockName
// Fetches the local socket address.
// ---------------------------------------------------------------------------------
//
TInt CFileSocketDesc::SockName(int anEnd, struct sockaddr* aAddr,unsigned long* aAddrLen)
	{
	TInt ret = KErrNone;
	TUSockAddr lSockAddr;
	if(anEnd == 0)
		{
		TInt ret;
		if((ret = ValidateAddress((sockaddr_un*)aAddr,aAddrLen)) != KErrNone)
			{
			if ( ( ret == EINVAL ) && (aAddrLen != 0))
                    {
                    // since the length passed is less than the first field but greater than 0 i.e sun_family we dont need to fill  any field into aAddr structure passed.
                        return KErrNone;
                    }
                return ret;
			}
		struct sockaddr_un* addr = (struct sockaddr_un*)aAddr;
		TInt buflen = *aAddrLen - (sizeof(addr->sun_family) + sizeof(addr->sun_len));
		addr->sun_family = AF_UNIX;
		*aAddrLen = sizeof(addr->sun_family);

		if(iPath.Length() && buflen > 1)
			{
			TPtr8 sockPath((TText8*)(addr->sun_path),buflen);
			sockPath.Copy(iPath.Left(buflen-1));
			sockPath.ZeroTerminate();
			*aAddrLen = sizeof(addr->sun_family) + sockPath.Length() + 1; //+1 for NULL character
			}
		else if (buflen == 1)
			{
			addr->sun_path[0] = '\0';
			}
		return KErrNone;
		}
	else
		{
		
		ret = CSockDescBase::SockName(anEnd,lSockAddr);
		if( ret == EFAULT )
		    return ret;
		
		// If the length passed is greater than or equal to 0 but less than the first field i.e sun_family  , return kErrnone and dont  fill  any field into aAddr structure passed.
		if( (aAddrLen != 0) && (*aAddrLen < sizeof(((struct sockaddr_un*)aAddr)->sun_family)) )
            {
            return KErrNone;
            }
		    
		if(ret == KErrNone)
			{
			ret = GetLocalSockAddrByPort((sockaddr_un*)aAddr,aAddrLen,lSockAddr.Port());
			}
		}
	return ret;
	}

// ---------------------------------------------------------------------------------
// CFileSocketDesc::ValidateAddress
// Utility function to check the validity of the socket address.
// ---------------------------------------------------------------------------------
//
TInt CFileSocketDesc::ValidateAddress(const struct sockaddr_un* aAddr,unsigned long* aAddrLen)
	{
	if (aAddr == NULL)
		{
		return EFAULT;
		}
	if (aAddrLen == 0)
		{
		return EINVAL;
		}
	if( *aAddrLen <  sizeof(aAddr->sun_family) )
	    {
	    return EINVAL;
	    }

	return KErrNone;
	}

// ---------------------------------------------------------------------------------
// CFileSocketDesc::GetLocalSockAddrByPort
// Gets the file socket address associated with the port by querying the IPC server.
// ---------------------------------------------------------------------------------
//
TInt CFileSocketDesc::GetLocalSockAddrByPort(struct sockaddr_un* aAddr,unsigned long* aAddrLen,TUint aPortNum)
	{
	TInt ret;
	if((ret = ValidateAddress(aAddr,aAddrLen)) != KErrNone)
		{
            return ret;
		}
	

	TInt buflen = *aAddrLen - (sizeof(aAddr->sun_family) + sizeof(aAddr->sun_len));
	TInt err;

	aAddr->sun_family = AF_UNIX;
	*aAddrLen = sizeof(aAddr->sun_family);
	if(buflen > 1)
		{
		TPtr8 sockPath((TText8*)aAddr->sun_path,buflen-1);
		if((err=Backend()->iIpcS.GetLocalSockAddrByPort(sockPath,aPortNum)) != KErrNone)
			{
			sockPath.ZeroTerminate();
			*aAddrLen = 0;
			return err;
			}

		aAddr->sun_path[sockPath.Length()] = '\0';
		*aAddrLen = sizeof(aAddr->sun_family) + sockPath.Length() + 1; //+1 for NULL character
		return KErrNone;
		}
	else if (buflen == 1)
		{
		aAddr->sun_path[0] = '\0';
		}

	return KErrNone;

	}

// -----------------------------------------------------------------------------
// CFileSocketDesc::GetLocalSockPortByPath
// Queries the IPC server to get the port associated with the path
// -----------------------------------------------------------------------------

TInt CFileSocketDesc::GetLocalSockPortByPath(const struct sockaddr_un* aAddr,unsigned long aAddrLen,TUint& aPortNum)
	{
	TInt err;
	if((err = ValidateAddress(aAddr,&aAddrLen)) != KErrNone)
		{
		return err;
		}

	TInt buflen = aAddrLen - (sizeof(aAddr->sun_family) + sizeof(aAddr->sun_len));

	if(buflen <= 1)
		{
		return EINVAL;
		}
		
	TBuf8<KMaxFileName> fullname;	
	err = GetFullFileUtf8(fullname, aAddr->sun_path, Backend()->FileSession());
		
	return Backend()->iIpcS.GetLocalSockPortByPath(fullname, aPortNum);
	}

// -----------------------------------------------------------------------------
// CFileSocketDesc::RemoveLocalSockAddr
// Removes the local socket address from the IPCServer
// -----------------------------------------------------------------------------

TInt CFileSocketDesc::RemoveLocalSockAddr()
	{
	if(iPath.Length())
		{
		return Backend()->iIpcS.RemLocalSockAddr(iPath);
		}
	return KErrNone;
	}

