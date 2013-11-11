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
// Implementation of STDLIB sockets.
// 
//

#include "FDESC.H"
#include <sys/types.h>
#include <string.h>		// for memcpy
#include <fcntl.h>		// for struct stat
#include <sys/errno.h>		// for ENOTSOCK
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <libc/netinet/in.h>		// for htons

EXPORT_C unsigned short htons(unsigned short hs)
	{ return ByteOrder::Swap16(hs); }
EXPORT_C unsigned long  htonl(unsigned long hl)
	{ return ByteOrder::Swap32(hl); }

// Support for struct sockaddr conversion

TUSockAddr::TUSockAddr (TAny* aBuf) 
	: TSockAddr()
	{
	Prepare(aBuf);
	}

void TUSockAddr::Prepare (TAny* aBuf) 
//
// Prepare a TUSockAddr to receive an address (used in RecvFrom)
// Setting the length to 0 indicates that we don't really want this address anyway.
//
	{
	if (aBuf==0)
		{
		SetLength(0);
		return;
		}
	}

TUSockAddr::TUSockAddr (TAny* aBuf, TUint aLen) 
	: TSockAddr()
	{
	Set(aBuf,aLen);
	}

void TUSockAddr::Set (TAny* aBuf, TUint aLen) 
//
// Construct an ESOCK TSockAddr from a struct sockaddr.
// We have to deal with the network byte ordering of AF_INET addresses
//
	{
	if (aBuf==0)
		{
		SetLength(0);
		return;
		}
	struct sockaddr* sp=(struct sockaddr*)aBuf;
	TUint8 *from=(TUint8*)aBuf;
	if (sp->sa_family==AF_INET)
		{
		// byte-swap sockaddr_in back into host ordering
		TUint port=(from[2]<<8)+from[3];
		from+=4;
		TUint32 addr=(from[0]<<24)+(from[1]<<16)+(from[2]<<8)+from[3];
		from+=4;
		SetFamily(AF_INET);
		SetPort(port);
		*(TUint32*)UserPtr()=addr;
		SetUserLen(4);
		return;
		}
	// expand the family and port
	SetFamily(sp->sa_family);
	SetPort(sp->sa_port);
	from+=4;
	// copy the rest of the data as given
	TUint8 *to=UserPtr();
	if (aLen<4)
		aLen=4;
	aLen-=4;	// skip fmaily and port
	if (aLen>24)
		aLen=24;
	memcpy(to,from,aLen);
	SetUserLen(aLen);
	}

void TUSockAddr::Get(TAny* addr, unsigned long* len)
//
// Extract a struct sockaddr from a TSockAddr
//
	{
	if (addr==0)
		return;
	struct sockaddr* sp=(struct sockaddr*)addr;
	TUint16 port=(TUint16)Port();
	if (Family()==AF_INET)
		{
		sp->sa_family=AF_INET;
		sp->sa_port=htons(port);
		TUint8* from=UserPtr();
		TUint32 fromaddr=(from[0]<<24)+(from[1]<<16)+(from[2]<<8)+from[3];
		*(TUint32*)sp->sa_data=fromaddr;
		*len=8;
		return;
		}
	else
		{
		sp->sa_family=(TUint16)Family();
		sp->sa_port=port;
		}
	TUint ulen=GetUserLen();
	if (ulen+4>(*len))
		ulen=(*len)-4;
	*len=ulen+4;
	memcpy(sp->sa_data,UserPtr(),ulen);
	}

// The Socket descriptor class

TInt CSocketDesc::isStream() const	// inline, but private
	{ 
	return iStyle==SOCK_STREAM; 
	}

TInt CSocketDesc::Socket(RSocketServ& aSs, int family, int style, int protocol)
	{
	if (protocol==0)
		protocol=KUndefinedProtocol;
	TInt err=iSocket.Open(aSs,family,style,protocol);
	if (err==KErrNone)
		iStyle=style;
	return err;
	}

TInt CSocketDesc::FinalClose()
	{
	iSocket.Close();
	return KErrNone;
	}

TInt CSocketDesc::LSeek (int&, int)
	{
	return ESPIPE;	// can't seek on a socket
	}

TInt CSocketDesc::FStat (struct stat *st)
	{
	// I am a socket about which little is known
	st->st_mode = S_IFSOCK;
	st->st_blksize=0;
	return KErrNone;
	}

void CSocketDesc::Read (TDes8& aBuf, TRequestStatus& aStatus)
	{
	iSocket.Recv(aBuf, 0, aStatus, iLength);	// needs a completion which returns the length
	}
void CSocketDesc::ReadCancel()
	{
	iSocket.CancelRecv();
	}

void CSocketDesc::Write (TDes8& aBuf, TRequestStatus& aStatus)
	{
	iSocket.Write(aBuf, aStatus);
	}
void CSocketDesc::WriteCancel()
	{
	iSocket.CancelWrite();
	}


TInt CSocketDesc::Bind(TSockAddr& anAddr)
	{
	return iSocket.Bind(anAddr);
	}

TInt CSocketDesc::Listen(TUint qSize)
	{
	return iSocket.Listen(qSize);
	}

TInt CSocketDesc::SockName(int anEnd, TSockAddr& anAddr)
	{
	const TUint KBadFamily = 0xFF000000;
	anAddr.SetFamily(KBadFamily);
	if (anEnd==0)
		iSocket.LocalName(anAddr);
	else
		iSocket.RemoteName(anAddr);
	if (anAddr.Family()==KBadFamily)
		return KErrGeneral; // assume that the call failed, but there is no way to find out why
	return KErrNone;
	}

TInt CSocketDesc::GetSockOpt(TUint anOptionName, TUint anOptionLevel, TDes8& anOption)
	{
	return iSocket.GetOpt(anOptionName,anOptionLevel,anOption);
	}

TInt CSocketDesc::SetSockOpt(TUint anOptionName, TUint anOptionLevel, TDesC8& anOption)
	{
	return iSocket.SetOpt(anOptionName,anOptionLevel,anOption);
	}

void CSocketDesc::Sync (TRequestStatus& aStatus)
	{
	// Judging from the Solaris man pages, this does nothing.
	Complete(aStatus,KErrNone);
	}

void CSocketDesc::RecvFrom(TDes8& aDesc, TSockAddr& from, int flags, TRequestStatus& aStatus)
	{
	if (isStream())
		{
		// recvfrom on a stream ignores the from address - get the peername
		if (from.Length())
			SockName(1,from);
		iSocket.RecvOneOrMore(aDesc,flags,aStatus,iLength);
		}
	else
		iSocket.RecvFrom(aDesc,from,flags,aStatus,iLength);
	}

void CSocketDesc::RecvFromCancel()
	{
	iSocket.CancelRecv();
	}

TInt CSocketDesc::RecvFromCompletion(TInt& aLength, TInt aStatus)
        {
        aLength=iLength();      // ignored if there's an error, 0 at EOF
        if (aStatus == KErrEof || aStatus == KErrDisconnected)
                return 0;       // indicates graceful close at the other end
        return aStatus;
        }


void CSocketDesc::SendTo(TDes8& aDesc, TSockAddr& to, int flags, TRequestStatus& aStatus)
	{
	if (to.Length()==0)
		iSocket.Send(aDesc,flags,aStatus);
	else
		{
		if (isStream())
			Complete(aStatus,KErrNotSupported);	// can't sendto a stream
		else
			iSocket.SendTo(aDesc,to,flags,aStatus);
		}
	}

void CSocketDesc::SendToCancel()
	{
	iSocket.CancelSend();
	}

void CSocketDesc::Shutdown(TUint aHow,TRequestStatus& aStatus)
	{
	aHow=(aHow+1)&0x3;	// 0=>EStopInput, 1=>EStopOutput, 2=>EStopInOut
	iSocket.Shutdown(*REINTERPRET_CAST(RSocket::TShutdown*,&aHow),aStatus);
	}

void CSocketDesc::Accept(CSocketDesc*& aNewSocket, TRequestStatus& aStatus, RSocketServ& aSs)
	{
	aNewSocket= new CSocketDesc;
	TInt err=KErrNone;
	if (aNewSocket!=0)
		err=aNewSocket->iSocket.Open(aSs);
	if (aNewSocket==0 || err!=KErrNone)
		{
		Complete(aStatus,KErrNoMemory);
		delete aNewSocket;
		return;
		}
	aNewSocket->iStyle=SOCK_STREAM;	/* necessarily */
	iSocket.Accept(aNewSocket->iSocket,aStatus);
	}

void CSocketDesc::AcceptCancel()
	{
	iSocket.CancelAccept();
	}

void CSocketDesc::Connect(TSockAddr& anAddr,TRequestStatus& aStatus)
	{
	iSocket.Connect(anAddr,aStatus);
	}

void CSocketDesc::ConnectCancel()
	{
	iSocket.CancelConnect();
	}

void CSocketDesc::Ioctl(int aCmd, void* aParam, TRequestStatus& aStatus)
	{
	TInt ret=KErrNone;
	int* param=REINTERPRET_CAST(int*,aParam);
	switch (aCmd)
		{
	case E32IONREAD:
		ret=iSocket.GetOpt(KSOReadBytesPending,KSOLSocket,*param);
		break;
	case E32IOSELECT:
		{
		iIoctlBuf.Set((TText8*)aParam,4,4);
		iSocket.Ioctl(KIOctlSelect,aStatus,&iIoctlBuf,KSOLSocket);
		}
		return;
	default:
		ret=KErrNotSupported;
		break;
		}
	Complete(aStatus,ret);
	}

TInt CSocketDesc::IoctlCompletion(int /*aCmd*/, void* /*aParam*/, TInt aStatus)
	{
	return aStatus;
	}

void CSocketDesc::IoctlCancel()
	{
	iSocket.CancelIoctl();
	}
