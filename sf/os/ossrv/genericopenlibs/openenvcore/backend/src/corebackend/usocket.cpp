/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <sys/types.h>
#include <string.h>		// for memcpy
#include <fcntl.h>		// for struct stat
#include <sys/errno.h>		// for ENOTSOCK
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <netinet/in.h>		// for htons
#include <sys/sockio.h>
#include <bt_sock.h>     // Symbian BT constantss needed for cooperation with Symbian sockets
#include <arpa/inet.h>   
#include <es_sock.h>
#include <in_sock.h>
#include <net/if.h>
#include <commdb.h>
#include <cdbcols.h>
#include <commdbconnpref.h>
#include <e32base.h>
#include <e32err.h>
#include <utf.h>
#include <e32std.h>
#include <net/route.h>
#include <in_iface.h>
#include "fdesc.h"
#include "lposix.h"
#include "sysif.h"

// Support for struct sockaddr conversion
TUSockAddr::TUSockAddr (TAny* aBuf) 
: TSockAddr(), iError(0)
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
		iError = EFAULT;
		return;
		}
	}

EXPORT_C TUSockAddr::TUSockAddr (const TAny* aBuf, TUint aLen) 
: TSockAddr(), iError(0)
		{
		Set(aBuf,aLen);
		}

void TUSockAddr::Set (const TAny* aBuf, TUint aLen) 
//
// Construct an ESOCK TSockAddr from a struct sockaddr.
// We have to deal with the network byte ordering of AF_INET addresses
//
	{
	if (!aBuf)
		{
		iError = EFAULT;
		SetLength(0);
		return;
		}
	
	const TUint dataOffset = (TUint)&((struct sockaddr *)0)->sa_data[0];
	// aLen should be atleast have minimum size.
	if (aLen < dataOffset)
		{
		iError = EINVAL;
		SetLength(0);
		return;
		}

	const struct sockaddr* sp= (const struct sockaddr *)(aBuf);
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
	else if (sp->sa_family == AF_INET6)
		{
		SetFamily(sp->sa_family);
		SetPort(ByteOrder::Swap16(sp->sa_port));
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
		return;
		}	
	else if (sp->sa_family == KBTAddrFamily)
		{
		// Make a TBTSockAddr from a sockaddr_bt...
		TBTSockAddr tmpAddr;

		// Family is always Bluetooth:
		tmpAddr.SetFamily(KBTAddrFamily);

		// The port number has the correct byte order already, as the
		// POSIX Bluetooth APIs assume litle-endian format:
		tmpAddr.SetPort(sp->sa_port);

		// For the address we need to reverse the byte ordering, as
		// Symbian appears to index Bluetooth addresses in a
		// big-endian manner:

		const TInt KBTAddrLength = 6;  // Bytes in a Bluetooth address
		const TInt KBTAddrOffset = 4;  // Offset of address in sockaddr_bt

		TBTDevAddr devAddr;
		const TInt KMinLength = KBTAddrLength + KBTAddrOffset;

		if (aLen >= KMinLength)
			{
			for (TInt i=0; i<KBTAddrLength; ++i)
				{
				devAddr[i] = from[KMinLength - 1 - i];
				}
			}

		tmpAddr.SetBTAddr(devAddr);

		// Security settings:
		TBTServiceSecurity sec;
		sec.SetUid(TUid::Uid(0));
		sec.SetAuthentication(EFalse);
		sec.SetEncryption(EFalse);
		sec.SetAuthorisation(EFalse);
		sec.SetDenied(EFalse);
		tmpAddr.SetSecurity(sec);

		// Now we must copy the contents of the "tmpAddr" TBTSockAddr,
		// but unfortunately there isn't an assignment operator for us
		// to use. Still, we know it is a self-contained T class for
		// which a direct shallow copy using memcopy() will be ok:
		//
		memcpy(this, &tmpAddr, sizeof(TBTSockAddr));

		return;
		}
	else
		{
		// Not AF_INET or KBTAddrFamily:

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
	}

/*
 * Extract a struct sockaddr from a TSockAddr
 */
EXPORT_C void TUSockAddr::Get(TAny* addr, unsigned long* len)
	{
	if (addr==0)
		{
		iError = EFAULT;
		SetLength(0);
		return;
		}
	const TUint dataOffset = (TUint)&((struct sockaddr *)0)->sa_data[0];
	// aLen should be atleast have minimum size.
	if (*len < dataOffset)
		{
		iError = EINVAL;
		SetLength(0);
		return;
		};

	struct sockaddr* sp=(struct sockaddr*)addr;
	TUint16 port=(TUint16)Port();

	//The ipv4 address could be obtained either as
	//plain 32 bit address or as ipv4 mapped ipv6 address
	TInetAddr inetAddr(*this);
	TBool isV4Mapped = (Family()==AF_INET6) && (inetAddr.IsV4Mapped() || inetAddr.IsV4Compat());
	if (Family()==AF_INET || isV4Mapped)
		{
		sp->sa_family=AF_INET;
		sp->sa_port= ByteOrder::Swap16(port);
		TUint8* from=UserPtr();
		TUint32 fromaddr;
		//If the address is v4 mapped, we take the last 4 bytes from the 16 bytes long v6 structure. 
		if(isV4Mapped)
			fromaddr = (from[15]<<24)+(from[14]<<16)+(from[13]<<8)+from[12];
		else
			fromaddr = (from[0]<<24)+(from[1]<<16)+(from[2]<<8)+ from[3];
		*(TUint32*)sp->sa_data=fromaddr;
		*len=8;
		return;
		}
	else if (Family()==AF_INET6)
		{

		sp->sa_family=(TUint16)Family();
		sp->sa_port=ByteOrder::Swap16(port);
		TUint ulen=GetUserLen();
		if (ulen+4>(*len))
			ulen=(*len)-4;
		*len=ulen+4;
		memcpy(sp->sa_data,UserPtr(),ulen);
		sp->sa_len = ulen;
		return;
		}		
	else if (Family()==KBTAddrFamily)
		{
		sp->sa_family=KBTAddrFamily;
		sp->sa_port=port;

		const TInt KBTAddrLength = 6;
		if (GetUserLen() == KBTAddrLength)
			{
			for (TInt i=0; i<KBTAddrLength; ++i)
				{
				sp->sa_data[KBTAddrLength - 1 - i] = UserPtr()[i];
				}
			}
		return;
		}
	else
		{
		// Not AF_INET or KBTAddrFamily:
		sp->sa_family=(TUint16)Family();
		sp->sa_port=port;
		TUint ulen=GetUserLen();
		if (ulen+4>(*len))
			ulen=(*len)-4;
		*len=ulen+4;
		memcpy(sp->sa_data,UserPtr(),ulen);
		}
	}

// The Socket descriptor class
CSocketDesc::CSocketDesc()
:iIoctlBuf(0,0),
iSocketPtr(NULL),
iConnPref(),
iConnectionPtr(NULL),
iSubConnectionPtr(NULL),
iRConnectionIndex(-1),		
iSockServPtr(NULL)
		{

		};


TInt CSocketDesc::Socket(RSocketServ& aSs, int family, int style, int protocol)
	{
	if (protocol == 0)
		{
		if (family == AF_INET)
			{
			switch (style)
				{
				case SOCK_STREAM:
					protocol = IPPROTO_TCP;
					break;

				case SOCK_DGRAM:
					protocol = IPPROTO_UDP;
					break;
				}
			}
		else
			{
			protocol = KUndefinedProtocol;
			}
		}

	iFcntlFlag = 0;
	TInt err = CreateLock();
	if (err)
		{
		return err;
		}

	if (family == AF_INET6)
		{
		err = iSocket.Open(aSs, KAfInet, style, protocol);
		}
	else
		{
		err = iSocket.Open(aSs, family, style, protocol);
		}

	if (err == KErrNone)
		{
		iStyle = style;
		iSocket.Close(); // We will close now, to open later using connection preferences
		iAddrFamily = family;
		iProtocol = protocol; 
		iSockServPtr = &aSs; // Saving the const session pointer for later use
		}
	return err;
	}

TInt CSocketDesc::FinalClose()
	{
	CLocalSystemInterface* backend = Backend();
	backend->DefConnLock().Wait();
	RHeap* oheap = User::SwitchHeap(backend->Heap());	
	if (iSocketPtr != NULL)
		{
		iSocketPtr = NULL;	
		StopInterface(NULL); // Checks for RConnection as well as RSubConnection
	    backend->RemoveSocket(this);
		}

	CSockDescBase::FinalClose();
	User::SwitchHeap(oheap);
	backend->DefConnLock().Signal();
	return KErrNone;
	}


void CSocketDesc::Read (TDes8& aBuf, TRequestStatus& aStatus)
	{
	//Acquire the Lock before read and release it later
	iReadLock.Wait();
	if (iSocketPtr == NULL)
		{
		TInt ret = OpenUsingPreference();
		if (ret != KErrNone)	// Error in open
			{
			Complete(aStatus,ret);
			iReadLock.Signal();
			return;
			}
		}
	CSockDescBase::Read (aBuf,aStatus);
	iReadLock.Signal();
	}

TInt CSocketDesc::ReadCompletion(TDes8& aBuf, TInt aStatus)
	{
	return CSockDescBase::ReadCompletion(aBuf, aStatus);
	}

void CSocketDesc::ReadCancel()
	{
	if (iSocketPtr != NULL)
		{
		CSockDescBase::ReadCancel();
		}
	}

void CSocketDesc::Write (TDes8& aBuf, TRequestStatus& aStatus)
	{

    TInt err = maybe_reopen_socket();
    if (err != KErrNone)
        {
        Complete(aStatus, err);
        return;
        }
	
	iWriteLock.Wait();
	CSockDescBase::Write(aBuf, aStatus);
	iWriteLock.Signal();	
	}

void CSocketDesc::WriteCancel()
	{
	if (iSocketPtr != NULL)
		{
		CSockDescBase::WriteCancel();
		}
	}


TInt CSocketDesc::Bind(const struct sockaddr* aAddr, unsigned long aSize)
	{
	TInt ret;
	TUSockAddr addr(aAddr,aSize);

	if (addr.iError != 0)
		{
		return addr.iError;
		}
	
	ret = maybe_reopen_socket();
	if (ret != KErrNone)
	    return ret;
	ATOMICSOCKETOP(ret = iSocket.Bind(addr),return KErrBadHandle)
	return ret;
	}

TInt CSocketDesc::Listen(TUint qSize)
	{

    TInt ret;
	if (iStyle == SOCK_DGRAM) // Listen on UDP socket, crashing at RSocket::Listen().
		{
		return EOPNOTSUPP;
		}
	
	ret = maybe_reopen_socket();
	if (ret != KErrNone)
	    return ret;

	return CSockDescBase::Listen(qSize);
	}

TInt CSocketDesc::SockName(int anEnd, struct sockaddr* anAddr,unsigned long* aSize)
	{


	if (!anAddr)
        {
        return EFAULT;
        }

	TInt ret = maybe_reopen_socket();
	if (ret != KErrNone)
	    return ret;
	
   
    struct sockaddr temp;
    unsigned long len = sizeof( temp );
    
	TUSockAddr nSockAddr(&temp, len );

	ret = CSockDescBase::SockName(anEnd,nSockAddr);
	if(ret == KErrNone )
	    {
            nSockAddr.Get(&temp,&len );
            if( *aSize > len )
                *aSize = len;
            memcpy( anAddr,&temp,*aSize);
            
            }
	
	return ret;
	}

TInt CSocketDesc::GetSockOpt(TInt anOptionName, TInt anOptionLevel, TDes8& anOption)
	{
	TInt ret;
	if (anOption.Ptr() == NULL)
		{
		return EFAULT;
		}
	if (anOption.Length() == 0)
		{
		return EINVAL;
		}
	
	ret = maybe_reopen_socket();
	if (ret != KErrNone)
	    return ret;

	if (SO_TYPE == anOptionName && SOL_SOCKET == anOptionLevel)
		{
		TProtocolDesc protocolInfo;
		ATOMICSOCKETOP(ret = iSocket.Info(protocolInfo), ret = KErrBadHandle)
		if (KErrNone == ret )
			{
			// Copy the Socket Type to the buffer
			TInt size = (anOption.Length() < sizeof(protocolInfo.iSockType))? anOption.Length(): sizeof(protocolInfo.iSockType);
			Mem::Copy((unsigned char*)anOption.Ptr(), &protocolInfo.iSockType, size);
			anOption.SetLength(size);
			}
		return ret;
		}

	if(IPPROTO_IP == anOptionLevel && IP_MULTICAST_IF == anOptionName)
		{
		TUSockAddr addr;
		struct sockaddr_in sockAddress;
		sockAddress.sin_family = AF_INET;
		ATOMICSOCKETOP(sockAddress.sin_port = iSocket.LocalPort(),return KErrBadHandle)			
		ATOMICSOCKETOP(iSocket.LocalName(addr);,return KErrBadHandle)		
		TInt a = sizeof(sockAddress);
		addr.Get(&sockAddress,(unsigned long*)&a);  
		TInt size = (anOption.Length() < sizeof(sockAddress.sin_addr))? anOption.Length(): sizeof(sockAddress.sin_addr);
		Mem::Copy((unsigned char*)anOption.Ptr(), &(sockAddress.sin_addr), size); 
		anOption.SetLength(size);
		return KErrNone;
		}

	switch(anOptionLevel)
		{
		case IPPROTO_TCP:
			anOptionLevel=SOL_TCP;
			break;
		case IPPROTO_IP:
			anOptionLevel=SOL_IP;
			break;
		case SOL_SOCKET:
			{
			switch(anOptionName)
				{
				case SO_REUSEADDR: 
					anOptionLevel=SOL_IP;
					break;
				case SO_OOBINLINE:
				case SO_KEEPALIVE:
					anOptionLevel=SOL_TCP;
					break; 
				case SO_BROADCAST:
					*((TInt*)anOption.Ptr()) = 1;
					return KErrNone;
				}
			}
		} 

	switch(anOptionName)
		{
		case IP_MULTICAST_TTL:
			anOptionLevel=KSolInetIp;
			anOptionName=KSoIp6MulticastHops;
			break;
		case IP_MULTICAST_LOOP:
			anOptionLevel=KSolInetIp;
			anOptionName=KSoIp6MulticastLoop;
			break;
		}
	ATOMICSOCKETOP(ret = iSocket.GetOpt(anOptionName,anOptionLevel,anOption), return KErrBadHandle)
	return ret;
	}

TInt CSocketDesc::GetInterfaceIndex(TUint32 anAddr)
	{
	TPckgBuf<TSoInetIfQuery> ifq;
	TInt aIndex = -1;
	if (KInetAddrAny == anAddr)
		{
		aIndex = 0;
		return aIndex;
		}
	else 
		{
		TInt ret = KErrNone;
		ATOMICSOCKETOP(ret = iSocket.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl), ret = KErrBadHandle)
		if (ret != KErrNone)
			return KErrGeneral;

		TPckgBuf<TSoInetInterfaceInfo>iface;
		ATOMICSOCKETOP( ret = iSocket.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, iface), ret = KErrBadHandle )
		while(ret == KErrNone)
			{
			TSoInetInterfaceInfo &info = iface();
			TInt result;
			if(info.iState == EIfUp)
				{
				if (anAddr == info.iAddress.Address()) 
					{      	
					ifq().iName = info.iName;
					ATOMICSOCKETOP( result = iSocket.GetOpt(KSoInetIfQueryByName, KSolInetIfQuery, ifq), result = KErrBadHandle )
					if (result == KErrNone)
						aIndex = ifq().iIndex;

					}
				}
			ATOMICSOCKETOP( ret = iSocket.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, iface), ret = KErrBadHandle )
			}   	
		}
	return aIndex;
	}

TInt CSocketDesc::SetSockOpt(TInt anOptionName, TInt anOptionLevel, TDesC8& anOption)
	{
	TInt ret;
	if (anOption.Ptr() == NULL)
		{
		return EFAULT;
		}
	if (anOption.Length() == 0)
		{
		return EINVAL;
		}
	
	ret = maybe_reopen_socket();
	if (ret != KErrNone)
	    return ret;

	switch(anOptionLevel)
		{
		case IPPROTO_TCP:
			anOptionLevel=SOL_TCP;
			break;
		case IPPROTO_IP:
			anOptionLevel=SOL_IP;
			break;
		case SOL_SOCKET:
			{
			switch(anOptionName)
				{
				case SO_REUSEADDR: 
					anOptionLevel=SOL_IP;
					break;
				case SO_OOBINLINE:
				case SO_KEEPALIVE:
					anOptionLevel=SOL_TCP;
					break; 
				}
			}

		}
	TPckgBuf<TIp6Mreq> req;
	struct ip_mreq *mreq;
	TInt aIndex = -1;
	TUint8 *from;
	TUint32 srcAddr;
	TUint32 maddr;
	TInetAddr multiAddr;
	TInt *Option = NULL;
	TInt ttlValue = 0;
	switch(anOptionName) 
		{
		case IP_ADD_MEMBERSHIP:  //KSoIp6JoinGroup
		case IP_DROP_MEMBERSHIP: //KSoIp6LeaveGroup

			mreq = (struct ip_mreq *)anOption.Ptr();
			from = (TUint8 *)anOption.Ptr(); 
			from+=4;
			// swaping the byte order
			srcAddr = ByteOrder::Swap32( mreq->imr_interface.s_addr); 
			//fetch the interface index
			aIndex = GetInterfaceIndex(srcAddr);

			// Set the multicast addr
			from = (TUint8 *)anOption.Ptr();
			maddr=(from[0]<<24)+(from[1]<<16)+(from[2]<<8)+from[3];
			multiAddr.SetAddress( maddr);
			multiAddr.ConvertToV4Mapped();
			if (multiAddr.IsMulticast())
			    {
                req().iAddr = multiAddr.Ip6Address();
                req().iInterface = aIndex;  
			    }
			ATOMICSOCKETOP( ret = iSocket.SetOpt(anOptionName, anOptionLevel, req), return KErrBadHandle )
			return ret;

		case IP_MULTICAST_TTL:
			anOptionLevel=KSolInetIp;
			anOptionName=KSoIp6MulticastHops;
			Option = (TInt*)anOption.Ptr();
			ttlValue = *Option;
			ATOMICSOCKETOP( ret = iSocket.SetOpt(anOptionName,anOptionLevel,ttlValue), return KErrBadHandle )
			return ret;
			
		case SO_BROADCAST: 
			//check if user is trying to disable broadcast
			Option = (TInt*)anOption.Ptr();
			return (*Option == 0 ? KErrNotSupported : KErrNone);

		case IP_MULTICAST_IF:
			{
			//No support for equivalent flag KsoIp6MulticastIf presently
			//call bind instead	    
			struct in_addr *inAddress = (struct in_addr*)anOption.Ptr();	    
			struct sockaddr_in sockAddress; 	 		    
			sockAddress.sin_family = AF_INET;
			ATOMICSOCKETOP(sockAddress.sin_port = iSocket.LocalPort();,return KErrBadHandle)         
			sockAddress.sin_addr.s_addr = inAddress->s_addr;
			TUSockAddr ifAddress(&sockAddress, sizeof(sockAddress));
			ATOMICSOCKETOP( ret = iSocket.Bind(ifAddress), return KErrBadHandle )
			return ret;
			}

		case IP_MULTICAST_LOOP:
			anOptionLevel=KSolInetIp;
			anOptionName=KSoIp6MulticastLoop;
			break;	    	



		}
	ATOMICSOCKETOP( ret = iSocket.SetOpt(anOptionName,anOptionLevel,anOption), return KErrBadHandle )
	return ret;
	}

void CSocketDesc::Sync (TRequestStatus& aStatus)
	{
	// Judging from the Solaris man pages, this does nothing.
	Complete(aStatus,KErrNone);
	}

void CSocketDesc::RecvFrom(TDes8& aDesc, TSockAddr& from, int flags, TRequestStatus& aStatus)
	{
    TInt err = maybe_reopen_socket();
    if (err != KErrNone)
        {
        Complete(aStatus, err);
        return;
        }

	iReadLock.Wait();
	CSockDescBase::RecvFrom(aDesc, from, flags, aStatus);
	iReadLock.Signal();
	}

void CSocketDesc::RecvFromCancel()
	{
	if (iSocketPtr != NULL)
		{
		CSockDescBase::RecvFromCancel();
		}
	}


void CSocketDesc::SendTo(TDes8& aDesc, const struct sockaddr* anAddr, unsigned long aAddrLen, int flags, TRequestStatus& aStatus)
	{
    TInt err = maybe_reopen_socket();
    if (err != KErrNone)
        {
        Complete(aStatus, err);
        return;
        }
    
	TUSockAddr toAddr(anAddr, aAddrLen);
	
	iWriteLock.Wait();  
	CSockDescBase::SendTo(aDesc, toAddr, flags, aStatus);
	iWriteLock.Signal();
	}

void CSocketDesc::SendToCancel()
	{
// Should we use atomic loads here?
	if (iSocketPtr != NULL)
		{
		CSockDescBase::SendToCancel();
		}
	}

void CSocketDesc::Shutdown(TUint aHow,TRequestStatus& aStatus)
	{

    if (__e32_atomic_load_acq32(&iSocketPtr) == NULL) // Not opened at all. Nothing to do.
		{
		Complete(aStatus,KErrNone);
		return;
		}

	CSockDescBase::Shutdown(aHow,aStatus);
	return;
	}

void CSocketDesc::Accept(CFileDescBase*& aNewSocket, TRequestStatus& aStatus, RSocketServ& aSs, TSockAddr * /*aAddr*/)
	{
    aNewSocket = NULL;
	TInt err = maybe_reopen_socket();
	if (err != KErrNone)
	    {
        Complete(aStatus, err);
        return;
	    }
	
	iReadLock.Wait();
	// what are the below coverity thingummichs?
	//coverity[alloc_fn]
	//coverity[assign]
	CSocketDesc *newSocket = new CSocketDesc;
	if (!newSocket)
	    {
        Complete(aStatus, KErrNoMemory);
        iReadLock.Signal();
        return;
	    }
	
	err = newSocket->CreateLock();
	if (err)
		{
        Complete(aStatus, KErrNoMemory);
        delete newSocket;
        iReadLock.Signal();
        //coverity[memory_leak]
        return;
		}

	err = newSocket->iSocket.Open(aSs);
	if (err)
		{
		Complete(aStatus, err);
		newSocket->FinalClose();  // this will Close locks
		delete newSocket;

		iReadLock.Signal();
		//coverity[memory_leak]
		return;
		}
	newSocket->iSocketPtr = &newSocket->iSocket;
	newSocket->iStyle = iStyle;
	err = KErrNone;
	ATOMICSOCKETOP( iSocket.Accept(newSocket->iSocket,aStatus), err = KErrBadHandle )
	if( err )
	    {
	    Complete(aStatus, err);
	    newSocket->FinalClose();  // this will Close locks
	    delete newSocket;
	    }
	else
	    {
	    aNewSocket = newSocket;
	    }
	iReadLock.Signal();
	}

void CSocketDesc::AcceptCancel()
	{
	if (iSocketPtr != NULL)
		{
		ATOMICSOCKETOP( iSocket.CancelAccept(), NOP )
		}
	}

void CSocketDesc::Connect(const struct sockaddr* aAddr,unsigned long size,TRequestStatus& aStatus)
	{
	TUSockAddr addr(aAddr,size);

	if (addr.iError != 0)
		{
		aStatus = addr.iError;
		return;
		}
	
	TInt err = maybe_reopen_socket();
	if (err != KErrNone)
	    {
        aStatus = err;
        return;
	    }
	
	iWriteLock.Wait();
	if( GetConnectionProgress() == EFalse )
	    {
		ATOMICSOCKETOP(iSocket.Connect(addr, aStatus), Complete(aStatus,KErrBadHandle))
	    User::WaitForRequest(aStatus);
	    if( aStatus.Int() == KErrWouldBlock )
	        SetConnectionProgress(ETrue);
	    }
	else
	    {
	    aStatus = EALREADY;
	    }
	iWriteLock.Signal();
	}

void CSocketDesc::ConnectCancel()
	{
	if (iSocketPtr != NULL)
		{
		ATOMICSOCKETOP(iSocket.CancelConnect(),NOP)
		}
	}

void CSocketDesc::Ioctl(int aCmd, void* aParam, TRequestStatus& aStatus)
	{
	TInt ret = KErrNone;
	int* param = reinterpret_cast<int*>(aParam);

	ret = maybe_reopen_socket();
	if (ret != KErrNone)
	    {
        Complete(aStatus, ret);
        return;
	    }

	switch ((unsigned)aCmd)
		{
		case FIONREAD:
		case E32IONREAD:
		    {
			ATOMICSOCKETOP( ret=iSocket.GetOpt(KSOReadBytesPending,KSOLSocket,*param), ret = KErrBadHandle )
		    }
			break;
		case E32IOSELECT:
			{
			iIoctlBuf.Set((TText8*)aParam,4,4);
			iIoctlLock.Wait();
			iIoctlFlag = ETrue;		
			ATOMICSOCKETOP(iSocket.Ioctl(KIOctlSelect,aStatus,&iIoctlBuf,KSOLSocket), Complete(aStatus,KErrBadHandle))
			}
			return;
		case SIOCGIFCONF:
			ret = GetInterfaceList(aParam);
			break;
		case SIOCGIFINDEX:
			ret = GetInterfaceIndexByName(aParam);
			break;
		case SIOCGIFACTIVECONF:
			ret = GetActiveInterfaceList(aParam);
			break;
		case SIOCSIFNAME:
			ret = SetInterfaceByName(aParam);
			break;
		case SIOCIFSTART:
			ret = StartInterface(aParam);
			if(KErrNone == ret)
				{
				iSocket.Close();
				iSocketPtr = NULL;
				}
			break;
		case SIOCIFACTIVESTART:
			ret = StartActiveInterface(aParam);
			if(KErrNone == ret)
				{
				iSocket.Close();
				iSocketPtr = NULL;
				}
			break;
		case SIOCIFSTOP:
			ret = StopInterface(aParam);
			break;	
		case SIOCATMARK:
			ATOMICSOCKETOP(ret=iSocket.GetOpt(KSoTcpRcvAtMark,KSolInetTcp,*param), ret = KErrBadHandle)
			break;	
		case SIOCGIFADDR:
			ret = GetIpAddress(aParam);
			break;	
		case SIOCGIFNUM:
			ret = GetInterafceNumber(aParam);
			break;	
		case SIOCADDRT:
			ret = RouteRequest(KSoInetAddRoute, aParam);
			break;	
		case SIOCDELRT:		
			ret = RouteRequest(KSoInetDeleteRoute, aParam);
			break;	
		case SIOCGIFHWADDR:
			ret = GetInterfaceHWAddress(aParam);
		    break;
		case SIOCGIFACTIVEIAP:
		    ret = GetActiveInterface( aParam);
		    break;
		
		default:
			ret=KErrNotSupported;
			break;
		}
	Complete(aStatus,ret);
	}

TInt CSocketDesc::IoctlCompletion(int /*aCmd*/, void* /*aParam*/, TInt aStatus)
	{
	if(iIoctlFlag)
		{
		iIoctlLock.Signal();
		iIoctlFlag = EFalse;
		}
	return aStatus;
	}

void CSocketDesc::IoctlCancel()
	{
	if (iSocketPtr && iIoctlFlag)
		{
		ATOMICSOCKETOP(iSocket.CancelIoctl(), NOP)
		iIoctlLock.Signal();
		iIoctlFlag = EFalse;
		}
	}
// -----------------------------------------------------------------------------
// CSocketDesc::Fcntl
// Symbian Socket Specific Implementation for fcntl
// Implementation of fcntl for Socket will make use of  
// RSocket::SetOpt(KSONonBlockingIO/KSOBlockingIO, KSOLSocket)
// As of now fcntl supports F_SETFL and F_GETFL
// -----------------------------------------------------------------------------
//
TInt CSocketDesc::Fcntl(TUint anArg, TUint aCmd)
	{

    TInt err = maybe_reopen_socket();
    if (err != KErrNone)
        return err;

    return CSockDescBase::Fcntl(anArg, aCmd);
	}

TInt CSocketDesc :: GetIpAddress( void *aParam )
	{
	TInetAddr myAddr;
	ATOMICSOCKETOP( iSocket.LocalName(myAddr), return KErrBadHandle )
	TUint32 myIP = myAddr.Address();			
	if (myIP == 0)
		{
		return KErrGeneral;
		}

	ifreq *ifr = (ifreq *)aParam;
	((struct sockaddr_in *)&ifr->ifr_addr)->sin_addr.s_addr = myIP;
	return KErrNone;
	}

TInt CSocketDesc :: GetRemoteIpAddress( void *aParam )
	{
	TInetAddr remoteAddr;
	ATOMICSOCKETOP( iSocket.RemoteName(remoteAddr), return KErrBadHandle )
	TUint32 remoteIP = remoteAddr.Address();			

	if (remoteIP == 0)
		{
		return KErrGeneral;
		}
	ifreq *ifr = (ifreq *)aParam;
	((struct sockaddr_in *)&ifr->ifr_dstaddr)->sin_addr.s_addr = remoteIP;
	return KErrNone;
	}

TInt CSocketDesc :: GetInterafceNumber( void *aParam )
	{
	TInt count = 0;	
	TInt ret = KErrNone;
	TRAP(ret, AccessPointCountL(count));
	*(TInt *)aParam = count;
	return ret;	
	}

#ifdef __SYMBIAN_COMPILE_UNUSED__
TInt CSocketDesc :: GetInterafceParamInfo( void *aParam,TInt aType)
	{
	TInt ret = KErrNone;	
	TAccessPointRecord tempRecord;
	ifreq *ifr = (ifreq *)aParam;
	TRAP(ret,FindConnectionInfoL(tempRecord,ifr->ifr_name));
	if(ret != KErrNone)
		return ret;
	wchar_t *str = (wchar_t *)( tempRecord.iServiceType.PtrZ());	 
	TInt servFlag = 0;
	if(WcsCmp(str,L"LANService") == 0)
		{
		servFlag = 0;
		}
	else if(WcsCmp(str,L"OutgoingGPRS") == 0)
		{
		servFlag  = 1;	
		}
	if(aType == EACCESS_GETMETRIC)
		{
		ret = GetInterfaceDetails(aParam,servFlag,EACCESS_GETMETRIC);					
		}	
	else if(aType == EACCESS_GETMTU)
		{
		ret = GetInterfaceDetails(aParam,servFlag,EACCESS_GETMTU);					
		}
	else if(aType == EACCESS_GETNETMASK)
		{
		ret = GetInterfaceDetails(aParam,servFlag,EACCESS_GETNETMASK);					
		}
	else if(aType == EACCESS_GETBROADCAST)
		{
		ret = GetInterfaceDetails(aParam,servFlag,EACCESS_GETBROADCAST);					
		}
	else if(aType == EACCESS_GETPHYSADDR)
		{
		ret = GetInterfaceDetails(aParam,servFlag,EACCESS_GETPHYSADDR);					
		}
	else if(aType == EACCESS_GETFLAGS)
		{
		ret = GetInterfaceDetails(aParam,servFlag,EACCESS_GETFLAGS);					
		}
	return ret;	
	}

TInt CSocketDesc :: SetInterafceParamInfo( void *aParam,TInt aType)
	{
	TInt ret = KErrNone;	
	TAccessPointRecord tempRecord;
	ifreq *ifr = (ifreq *)aParam;
	TRAP(ret,FindConnectionInfoL(tempRecord,ifr->ifr_name));
	if(ret != KErrNone)
		return ret;
	wchar_t *str = (wchar_t *)(tempRecord.iServiceType.PtrZ());	 
	TInt servFlag = 0;
	if(WcsCmp(str,L"LANService") == 0)
		{
		servFlag = 0;
		}
	else if(WcsCmp(str,L"OutgoingGPRS") == 0)
		{
		servFlag  = 1;	
		}
	if(aType == EACCESS_SETMETRIC)
		{
		ret = SetInterfaceDetails(aParam,servFlag,EACCESS_SETMETRIC);					
		}	
	else if(aType == EACCESS_SETMTU)
		{
		ret = SetInterfaceDetails(aParam,servFlag,EACCESS_SETMTU);					
		}		
	else if(aType == EACCESS_SETFLAGS)
		{
		ret = SetInterfaceDetails(aParam,servFlag,EACCESS_SETFLAGS);					
		}
	else if(aType == EACCESS_SETPHYSADDR)
		{
		ret = SetInterfaceDetails(aParam,servFlag,EACCESS_SETPHYSADDR);					
		}	
	else if(aType == EACCESS_SETNETMASK)
		{
		ret = SetInterfaceDetails(aParam,servFlag,EACCESS_SETNETMASK);					
		}	
	else if(aType == EACCESS_SETBROADCAST)
		{
		ret = SetInterfaceDetails(aParam,servFlag,EACCESS_SETBROADCAST);					
		}		
	return ret;	

	}

void CSocketDesc::FindConnectionInfoL(TAccessPointRecord &aRecord,char *ptr)
	{
	//TAccessPointRecord tempRecord;
	CCommsDatabase* iapDatabase;
	CCommsDbTableView* view;
	TInt res;	
	TPtrC8 tptr((TText8*)ptr);
	TBuf<KCommsDbSvrMaxColumnNameLength> temp;
	res = CnvUtfConverter::ConvertToUnicodeFromUtf8(temp, tptr);
	if(res < KErrNone)
		{
		User::Leave(res);
		}
	else if(res > KErrNone)
		{
		User::Leave(KErrOverflow);
		}
	// This function can leave on insufficient memory
	OpenIapTableLC(&iapDatabase, &view);	
	res = view->GotoFirstRecord();
	while (res == KErrNone)
		{    	
		// On error continue with the next record	
		TRAP( res,ReadRecordFromIapTableL(view, aRecord));
		if (res == KErrNone)
			{   
			if(WcsCmp ((wchar_t*)aRecord.iName.PtrZ(),(wchar_t*)temp.PtrZ()) == 0)
				break; 	
			}    	
		res = view->GotoNextRecord();
		}
	CleanupStack::PopAndDestroy();// view
	CleanupStack::PopAndDestroy(iapDatabase);	
	return;
	}

TInt CSocketDesc :: SetInterfaceDetails( void *aParam ,TInt aFlag, TInt aType )
	{
	ifreq *ifr = (ifreq *)aParam;
	TPckgBuf<TSoInetIfQuery> ifq;
	TBuf8 <25> ipBuf8;
	TName aBuf;			
	TInt ret = KErrNone;
	ATOMICSOCKETOP( ret = iSocket.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl), ret = KErrBadHandle )
	if (ret != KErrNone)
		{
		return KErrGeneral;
		}    	
	TPckgBuf<TSoInet6InterfaceInfo> info;
	TSoInet6InterfaceInfo &in = info();	
	ATOMICSOCKETOP( ret = iSocket.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, info), ret = KErrBadHandle )
	while(ret == KErrNone)
		{			
		if(info().iName != _L("") && info().iName != _L("loop6") && info().iName != _L("loop4"))
			{   			
			TDes16& aName = info().iName;							
			if( ((aFlag == 0 ) && ( aName.FindC(_L("WLAN")) != KErrNotFound )) ||
					((aFlag == 1) && (aName.FindC(_L("Generic")) != KErrNotFound )) )
				{
				switch(aType)
					{				
					case  EACCESS_SETMETRIC:
						if(info().iState == EIfUp)										 
							{
							info().iSpeedMetric =  ifr->ifr_metric;
							}
						goto setout;

					case  EACCESS_SETMTU:										 
						if(info().iState == EIfUp)
							{
							info().iMtu = ifr->ifr_mtu ;		
							}		
						goto setout;

					case  EACCESS_SETNETMASK :																																																					
						// Presently netmask address is NULL											
						if((info().iState == EIfUp) && (ifr->ifr_addr.sa_data !=NULL))
							{																																																		
							/*
							CharToTBuf8(ifr->ifr_addr.sa_data,ipBuf8);											
											if (CnvUtfConverter::ConvertToUnicodeFromUtf8( aBuf,ipBuf8 ) == KErrNone)
											{													
												ret = info().iNetMask.Input(aBuf);												
											}																							
							info().iNetMask.SetAddress(INET_ADDR(255,255,255,0));	
							*/
							return KErrNotSupported;
							}
						break;	
					case  EACCESS_SETBROADCAST :										
						if((info().iState == EIfUp) && (ifr->ifr_broadaddr.sa_data !=NULL))
							{																																																		
							/*CharToTBuf8(ifr->ifr_broadaddr.sa_data,ipBuf8);											
											if (CnvUtfConverter::ConvertToUnicodeFromUtf8( aBuf,ipBuf8 ) == KErrNone)
											{													
												ret = info().iBrdAddr.Input(aBuf);												
											} 
							*/												
							return KErrNotSupported;
							}

						break;
					case  EACCESS_SETPHYSADDR :										
						// Currently no imeplentation is given as KIfHasHardwareAddr is always 
						// set to 0 for wlan and GPRS
						if(info().iFeatures&KIfHasHardwareAddr)
							{
							return KErrNotSupported;
							}
						break;									
					case  EACCESS_SETFLAGS :										
						info().iFeatures = 0;
						// Interface UP
						if((ifr->ifr_flags & IFF_UP) && (ifr->ifr_flags & IFF_DRV_RUNNING)) 
							{
							info().iState = EIfUp;																						
							}																						
						else
							{
							info().iState = EIfDown;																																	
							}																																	

						// Loopback										
						if(ifr->ifr_flags & IFF_LOOPBACK)
							{
							info().iFeatures |= KIfIsLoopback;																				            													           
							}																				            													           

						// point to point support
						if(ifr->ifr_flags &  IFF_POINTOPOINT)
							{
							info().iFeatures |= KIfIsPointToPoint; 							                
							} 							                

						// Broadcast
						if(ifr->ifr_flags & IFF_BROADCAST)
							{
							info().iFeatures |=KIfCanBroadcast; 
							} 


						// Multicast
						if(ifr->ifr_flagshigh & IFF_MULTICAST)
							{
							info().iFeatures = KIfCanMulticast;							            	      
							}
						//these flags details are available in symbian but not used by lib layer.
						/* if(info().iFeatures&KIfCanSetMTU)			            	
							               if(info().iFeatures&KIfHasHardwareAddr)    
							               if(info().iFeatures&KIfCanSetHardwareAddr) */
						goto setout;							            	

					default:
						break;					 
					}
				}

			}
		ATOMICSOCKETOP( ret = iSocket.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, info), ret = KErrBadHandle )
		}
	setout:	
	TPckgBuf<TSoInet6InterfaceInfo> changeToNew(info());
	ATOMICSOCKETOP(ret = iSocket.SetOpt(KSoInetConfigInterface, KSolInetIfCtrl,changeToNew), return KErrBadHandle )
	return ret;
	}
#endif // __SYMBIAN_COMPILE_UNUSED__

TInt CSocketDesc::GetInterfaceDetails( void *aParam ,TInt aFlag, TInt aType )
	{
	TPckgBuf<TSoInetIfQuery> ifq;

	TInt ret = KErrNone; 
	ATOMICSOCKETOP( ret = iSocket.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl), ret = KErrBadHandle )
	if (ret != KErrNone)
		{
		return KErrGeneral;
		}
	
	ifreq *ifr = (ifreq *)aParam;
    *(ifr->ifr_addr.sa_data) = '\0';

	TPckgBuf<TSoInetInterfaceInfo> info;
	ATOMICSOCKETOP( ret = iSocket.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, info), ret = KErrBadHandle )
	while( ret == KErrNone)
		{

		if(info().iName != _L("") && info().iName != _L("loop6") && info().iName != _L("loop4"))
			{   
			TDes16& aName = info().iName;
			TName aBuf;
			TBuf8<KMaxName> ipAddr;
			if( ((aFlag == 0 ) && ( aName.FindC(_L("WLAN")) != KErrNotFound )) ||
					((aFlag == 1) && (aName.FindC(_L("Generic")) != KErrNotFound )) )
				{
				switch(aType)
					{

					case  EACTIVE_GETIP :
						if((info().iState == EIfUp) && (info().iAddress.Address() != NULL))
							{
							if(!((info().iAddress.IsLinkLocal()) || (info().iAddress.IsSiteLocal())))
								{
								info().iAddress.Output(aBuf);  
								if (CnvUtfConverter::ConvertFromUnicodeToUtf8( ipAddr, aBuf ) == KErrNone)
									{			
									StrlCopy(ifr->ifr_addr.sa_data,(const char *) ipAddr.PtrZ(),ipAddr.Length()+1);														
									}  
								}
							}
						break;
					case  EACCESS_GETMETRIC:
						ifr->ifr_metric = 0;
						if (info().iState == EIfUp)
							{
							ifr->ifr_metric = info().iSpeedMetric;
							}
						break;
					case  EACCESS_GETMTU:
						ifr->ifr_mtu = 0;
						if (info().iState == EIfUp)
							{
							ifr->ifr_mtu = info().iMtu;
							}
						break;	
					case  EACCESS_GETNETMASK :
						*(ifr->ifr_addr.sa_data) = '\0';
						// Presently netmask address is NULL
						if((info().iState == EIfUp) && (info().iNetMask.Address() != NULL))
							{
							//anAddr = info().iNetMask.Address();	
							info().iNetMask.Output(aBuf);  
							if (CnvUtfConverter::ConvertFromUnicodeToUtf8( ipAddr, aBuf ) == KErrNone)
								{			
								StrlCopy(ifr->ifr_addr.sa_data,(const char *) ipAddr.PtrZ(),ipAddr.Length()+1);												
								}  											
							}
						break;	
					case  EACCESS_GETBROADCAST :
						*(ifr->ifr_broadaddr.sa_data) = '\0';
						// Presently Breaodcast address is NULL
						if((info().iState == EIfUp) && (info().iBrdAddr.Address() != NULL))
							{

							//anAddr = info().iBrdAddr.Address();	
							info().iBrdAddr.Output(aBuf);  
							if (CnvUtfConverter::ConvertFromUnicodeToUtf8( ipAddr, aBuf ) == KErrNone)
								{			
								StrlCopy(ifr->ifr_broadaddr.sa_data,(const char *) ipAddr.PtrZ(),ipAddr.Length()+1);												
								}											
							}
						break;
					case  EACCESS_GETPHYSADDR :
						ifr->ifr_phys = 0;
						// Currently no imeplentation is given as KIfHasHardwareAddr is always 
						// set to 0 for wlan and GPRS
						if(info().iFeatures&KIfHasHardwareAddr)
							{
							//nada.
							}
						break;									
					case  EACCESS_GETFLAGS :
						ifr->ifr_flags = 0;
						ifr->ifr_flagshigh=0;
						// Interface UP
						if(info().iState == EIfUp)
							{
							ifr->ifr_flags |= IFF_UP; 
							ifr->ifr_flags |= IFF_DRV_RUNNING;
							}
						// Loopback
						if(info().iFeatures&KIfIsLoopback)
							{
							ifr->ifr_flags |= IFF_LOOPBACK;
							}										            													           

						// point to point support
						if(info().iFeatures&KIfIsPointToPoint) 
							{
							ifr->ifr_flags |= IFF_POINTOPOINT;	
							}
						
						// Broadcast
						if(info().iFeatures&KIfCanBroadcast)
							{
							ifr->ifr_flags |= IFF_BROADCAST;
							}      

						// Multicast
						if(info().iFeatures&KIfCanMulticast)
							{
							ifr->ifr_flagshigh |= ((IFF_MULTICAST & 0xff00) >> 8);
							}
						//these flags details are available in symbian but not used by lib layer.
						/* if(info().iFeatures&KIfCanSetMTU)			            	
							               if(info().iFeatures&KIfHasHardwareAddr)    
							               if(info().iFeatures&KIfCanSetHardwareAddr) */

						break;																 				 
				 
					}
				}

			}
		ATOMICSOCKETOP( ret = iSocket.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, info), ret = KErrBadHandle )
		}

	return KErrNone;	
	}

TInt CSocketDesc::Poll(TPollMode aMode,TBool& aReadyStatus,TInt& aErrno)
	{
	TInt ret = maybe_reopen_socket();
    if (ret != KErrNone)
        {
        return ret;
        }

    TInt status;
    ATOMICSOCKETOP(ret = iSocket.GetOpt(KSOSelectPoll, KSOLSocket, status), ret = KErrBadHandle)
    if (ret == KErrNone)
		{
    	aReadyStatus = status & aMode;	
		}
    
    return MapError(ret, aErrno);
	}

/* The synchronous - non-blocking Poll */
TInt CSocketDesc::Poll(TUint aEvents)
	{

    TInt err = maybe_reopen_socket();
	if (err != KErrNone)
	    return err;
		
	return CSockDescBase::Poll(aEvents);
	}


/* Cancel an outstanding notification request */
void CSocketDesc::CancelNotify()
	{
	CSockDescBase::CancelNotify();
	}

TAccessPointRecord::TAccessPointRecord()
:iId(0), 
iName(0),
iDialogPref(ECommDbDialogPrefDoNotPrompt),
iDirection(ECommDbConnectionDirectionUnknown), 
iService(0),
iServiceType(0), // Text value
iBearer(0),
iBearerType(0),	// Text value
iNetwork(0),
iNetworkWeighting(0),
iLocation(0)
		{
		}

inline TInt CSocketDesc::GetInterfaceList(void *aParam)
	{
	return GetInterface(aParam, EACCESS_POINT);
	}

TInt CSocketDesc::GetInterfaceIndexByName(void *aParam)
	{
	ifreq *ifrQuery = (ifreq*)aParam;	
	ifconf ifc;

	// Know how many interfaces
	ifc.ifc_len = 0;
	ifc.ifc_buf = NULL;
	TInt ret = GetInterface(&ifc, EACCESS_POINT);
	if (ret != KErrNone)
		{
		return ret;
		}

	// Allocate the memory to get the detail interface information
	ifc.ifc_buf = (caddr_t)Backend()->Alloc( ifc.ifc_len);
	if (ifc.ifc_buf == NULL)
		{
		return KErrNoMemory;
		}
	ret = GetInterface(&ifc, EACCESS_POINT);
	if (ret != KErrNone)
		{
		Backend()->Free(ifc.ifc_buf);
		return ret;
		}
	ifreq *ifr = ifc.ifc_req;
	TInt ifCount = ifc.ifc_len / sizeof(ifreq);
	TInt i = 0;

	// We wouldn't need this if we were using a StrNcmp below
	ifrQuery->ifr_name[IFNAMSIZ-1] = 0; // don't assume NULL terminated input
	// Search for the interface name
	for (; i < ifCount; i++, ifr++)
		{
		if ( StrCmp(ifrQuery->ifr_name, ifr->ifr_name) == 0)
			{
			ifrQuery->ifr_index = ifr->ifr_index;
			break;
			}
		}

	if (i == ifCount)
		{
		ret = KErrNotFound;
		}
	Backend()->Free(ifc.ifc_buf);
	return ret;
	}

inline TInt CSocketDesc::GetActiveInterfaceList(void *aParam)
	{
	return GetInterface(aParam, EACTIVE_CONNECTION);
	}

TInt CSocketDesc::GetInterface(void *aParam, TInt aType)
	{
	TInt ret = KErrNone;
	ifconf *ifc = (ifconf*)aParam;	
	TInt length = (ifc->ifc_len) / sizeof(ifreq);
	TInt count = 0;
	// If the length is zero, fill the number of interface information
	if ( length <= 0 || ifc->ifc_buf == NULL )
		{
		if (aType == EACCESS_POINT)
			{
			TRAP(ret, AccessPointCountL(count));
			}
		else if (aType == EACTIVE_CONNECTION)
			{
			ret = ActiveConnectionCount(count);
			}
		else
			{
			ret = KErrArgument;
			}

		if (ret == KErrNone)
		    {
            ifc->ifc_len = sizeof(ifreq) * count;
		    }
		return ret;
		}

	// length is not 0, read 'length' number of records
	CArrayFixFlat<TAccessPointRecord> *apArray;
	wchar_t *str;
	TInt apIndex;
	TInt result = KErrNone;
	ifreq *ifr = ifc->ifc_req;
	TInt ActiveSet = 0;

	// Fixed array of 'length'
	apArray = new CArrayFixFlat<TAccessPointRecord>(length);
	if (apArray == NULL)
		{
		return KErrNoMemory;
		}
	if (aType == EACCESS_POINT)
		{
		// On return length would contain the number of access points.
		TRAP(result, AccessPointListL(apArray, length));
		}
	else if (aType == EACTIVE_CONNECTION)
		{
		TRAP(ret, ret=ActiveConnectionListL(apArray, length));
		if( ret == KErrNone )
			{
			TRAP(ret,FindConnectionDetailsL(apArray, length));
			ActiveSet = 1;  	
			}
		}
	if (ret != KErrNone)
		{
		delete apArray;
		return ret;
		}
	for (apIndex = 0; apIndex < length; apIndex++,ifr++)
		{
		TAccessPointRecord& ref = (*apArray)[apIndex];
		ifr->ifr_name[IFNAMSIZ-1] = 0; // don't assume NULL terminated input
		TPtr8 ptr((TText8*)ifr->ifr_name, IFNAMSIZ);

		ret = CnvUtfConverter::ConvertFromUnicodeToUtf8(ptr, ref.iName);
		if(ret == KErrNone)
			{
			if(ptr.Length() < ptr.MaxLength())
				{
				ptr.ZeroTerminate();
				} 
			else
				{
				return KErrOverflow;
				} 
			}
		else if (ret > KErrNone)
			{
			return KErrOverflow;
			} 
		else
			{
			return ret;
			}

		// Copy theinterface index
		ifr->ifr_index = ref.iId;
		if(ActiveSet == 1)
			{
			str = (wchar_t *)(ref.iServiceType.PtrZ() ); 
			TInt servFlag = 0;
			if(WcsCmp(str,L"LANService") == 0)
				{
				servFlag = 0;
				}
			else if(WcsCmp(str,L"OutgoingGPRS") == 0)
				{
				servFlag  = 1;	
				}
			GetInterfaceDetails(ifr,servFlag,EACTIVE_GETIP);
			}
		}
	ifc->ifc_len = sizeof(ifreq) * apIndex;
	delete apArray;	
	return KErrNone;
	}

TInt CSocketDesc::SetInterfaceByName(void *aParam)
	{
	ifreq *ifr = (ifreq *)aParam;
	if (!ifr)
		{
		return KErrArgument;
		}

	ifr->ifr_name[IFNAMSIZ-1] = 0; // don't assume NULL terminated input
	TPtrC8 ptr((TText8*)ifr->ifr_name);
	TInt ret = CnvUtfConverter::ConvertToUnicodeFromUtf8(iConnPref.iName,ptr);
	if(ret > KErrNone )
		{
		return KErrOverflow;
		}
	return ret; 
	}

inline TInt CSocketDesc::StartInterface(void *aParam)
	{
	return StartConnection(aParam);	
	}

inline TInt CSocketDesc::StartActiveInterface(void *aParam)
	{
	return StartSubConnection(aParam);	
	}

TInt CSocketDesc::StopInterface(void *)
	{
	if (iConnectionPtr != NULL)
		{
		StopConnection();
		return KErrNone;
		}
	else if (iSubConnectionPtr != NULL)
		{
		StopSubConnection();
		return KErrNone;
		}	
	return KErrNotFound;
	}

TInt CSocketDesc::OpenUsingPreference()
	{
	TInt ret = 0;	
    // Update when supporting INET6
    TInt addrfamily = (iAddrFamily == AF_INET6 ? KAfInet : iAddrFamily);	
	if (iConnectionPtr != NULL) // iConnection initialized
		{
		ret = iSocket.Open(*iSockServPtr,addrfamily,iStyle,iProtocol,iConnection);
		}
	else if (iSubConnectionPtr != NULL) // iSubConnection initialized
		{
		ret = iSocket.Open(*iSockServPtr,addrfamily,iStyle,iProtocol,iSubConnection);
		}
	else
	    {
	    RConnection& defConnection = Backend()->GetDefaultConnection();
	    //Now check if the default connection is intialized. This is given lesser 
	     //priority than the socket specific preferences checked above.
	     if(defConnection.SubSessionHandle() != 0)
	         {
	         ret = iSocket.Open(*iSockServPtr,addrfamily,iStyle,iProtocol,defConnection);	         
	         if (!ret)
	             {
	             Backend()->AddSocket(this);
	             }
	         }
	     else // No connection preference is set
	         {
	         ret = ECONNABORTED;
            }
        }
    
    if (ret == KErrNone)
        {
        __e32_atomic_store_rel32(&iSocketPtr, (unsigned long)&iSocket);
        }
    
    iConnectInProgress = EFalse;

    return ret;
    }

void CSocketDesc::TempClose()
    {
    TUint32 ret = __e32_atomic_ior_ord32((void *)&iCount,0x8000);
    if( ret >= 0x8000 )
        {
        // This indicates a TempClose has already been done from one of the threads
        return;
        }
    // loop and yeild till no more references are held to the iSocket
    while( iCount != 0x8000 )
        {
        // Yeild for 1 ms
        User::After(1000);
        }
    if (iSocket.SubSessionHandle() != 0)
        {
	    iSocketPtr = NULL;
        iSocket.CancelAll();    
        TRequestStatus status;
        iSocket.Shutdown(RSocket::EImmediate, status);
        User::WaitForRequest(status);
        iSocket.Close();
        }
    }

void CSocketDesc::AccessPointListL(CArrayFixFlat<TAccessPointRecord> *&aRecordPtr,
		TInt &aCount)
	{
	TAccessPointRecord tempRecord;
	CCommsDatabase* iapDatabase;
	CCommsDbTableView* view;

	// This function can leave on insufficient memory
	OpenIapTableLC(&iapDatabase, &view);
	TInt count = 0;
	TInt ret = view->GotoFirstRecord();
    while (ret == KErrNone && count < aCount)
		{
    	TRAP(ret, ReadRecordFromIapTableL(view, tempRecord));
    	if (ret == KErrNone)
			{
			aRecordPtr->AppendL(tempRecord);
			count++;
			}	
    	ret = view->GotoNextRecord();
		}
    
	CleanupStack::PopAndDestroy();// view
	CleanupStack::PopAndDestroy(iapDatabase);
	aCount = aRecordPtr->Count();
	}

void CSocketDesc::FindConnectionDetailsL(CArrayFixFlat<TAccessPointRecord> *&aRecordPtr,
		TInt &aCount)
	{
	TAccessPointRecord tempRecord;
	CCommsDatabase* iapDatabase;
	CCommsDbTableView* view;

	// This function can leave on insufficient memory
	OpenIapTableLC(&iapDatabase, &view);
	TInt count = 0;
	TInt ret = view->GotoFirstRecord();
    while (ret == KErrNone && count < aCount)
		{
    	TRAP(ret, ReadRecordFromIapTableL(view, tempRecord));
    	if (ret == KErrNone)
			{    		
			if((*aRecordPtr)[count].iId == tempRecord.iId )    			
				{    			    		
				(*aRecordPtr)[count].iServiceType = tempRecord.iServiceType;
				ret = view->GotoFirstRecord();
				count++;    		
				continue;
				}    		
    		ret = view->GotoNextRecord();
			}
		}
    
	CleanupStack::PopAndDestroy();// view
	CleanupStack::PopAndDestroy(iapDatabase);
	aCount = aRecordPtr->Count();
		}

void CSocketDesc::AccessPointCountL(TInt &aCount)
	{
	CCommsDatabase* iapDatabase;
	CCommsDbTableView* view;

	OpenIapTableLC(&iapDatabase, &view);
	aCount = 0;
	TInt ret = view->GotoFirstRecord();
	while(ret == KErrNone)
		{
		aCount++;
		ret = view->GotoNextRecord();
		}
	CleanupStack::PopAndDestroy();// view
	CleanupStack::PopAndDestroy(iapDatabase);
	}

void CSocketDesc::ReadRecordFromIapTableL(CCommsDbTableView* aView, TAccessPointRecord &aRecord)
	{
	aView->ReadUintL(TPtrC(COMMDB_ID), aRecord.iId);
	aView->ReadTextL(TPtrC(COMMDB_NAME), aRecord.iName); 
	aRecord.iDialogPref = ECommDbDialogPrefDoNotPrompt;
	aView->ReadUintL(TPtrC(IAP_SERVICE), aRecord.iService);
	aView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), aRecord.iServiceType);
	aView->ReadUintL(TPtrC(IAP_BEARER), aRecord.iBearer);
	/*
	 * IAP_BEARER_TYPE is not a column of IAP table even though its 
	 * listed in header. So not trying to read it.
	 */
	aView->ReadUintL(TPtrC(IAP_NETWORK), aRecord.iNetwork);
	aView->ReadUintL(TPtrC(IAP_NETWORK_WEIGHTING), aRecord.iNetworkWeighting);
	aView->ReadUintL(TPtrC(IAP_LOCATION), aRecord.iLocation);
	}

void CSocketDesc::OpenIapTableLC(CCommsDatabase **aIapDatabase, CCommsDbTableView **aView)
	{
	// Get IAP names and ids from the database
	*aIapDatabase = CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(*aIapDatabase);

	(*aIapDatabase)->ShowHiddenRecords();

	// This is LC code, that means code has pushed, but not popped
	(*aView) = (*aIapDatabase)->OpenTableLC(TPtrC(IAP));
	}

inline TInt CSocketDesc::ActiveConnectionCount(TInt &aCount)
	{
	aCount = ((CFileTable*)iFids)->RConnectionCount();
	return KErrNone;
	}

TInt CSocketDesc::ActiveConnectionListL(CArrayFixFlat<TAccessPointRecord> *aRecordPtr, TInt &aLength)
	{
	TInt rcIndex;
	RConnection *rc;
	TAccessPointRecord tempRecord;

	for (rcIndex = 0;rcIndex < aLength; rcIndex++)
		{
		if  ( ((CFileTable*)iFids)->RConnectionAt(rcIndex, rc) != KErrNone )
			{
			break;
			}
		// Read index and name of the connection
		if  ( GetRConnectionDetails(rc, tempRecord) != KErrNone )
			{
			break;
			}
		aRecordPtr->AppendL(tempRecord);
		}
	aLength = aRecordPtr->Count();
	return KErrNone;
	}

TInt CSocketDesc::GetRConnectionDetails(RConnection *aRc, TAccessPointRecord &aApr)
	{
	_LIT(KName, "IAP\\Name");
	_LIT(KId, "IAP\\Id");

	TInt ret = aRc->GetDesSetting(KName, aApr.iName);
	if (ret == KErrNone)
		{
		ret = aRc->GetIntSetting(KId, aApr.iId);
		}
	
		return ret;
		}

TInt CSocketDesc::StartConnection(void * /* ptr */)
	{
	if (iConnectionPtr != NULL)
		{
		return KErrAlreadyExists;
		}
	TCommDbConnPref connPref;
	TInt ret = GetConnectionPreference(connPref);
	if (ret != KErrNone)
		{
		return ret;
		}
	ret = iConnection.Open(*iSockServPtr);
	if (ret != KErrNone)
		{
		return ret;
		}	
	ret = iConnection.Start(connPref);
	if (ret != KErrNone)
		{
		iConnection.Close();
		return ret;
		}
	iConnectionPtr = &iConnection;
	ret = ((CFileTable*)iFids)->AddRConnectionPtr(iConnectionPtr, iRConnectionIndex);
	if (ret != KErrNone)
		{
		iConnection.Close();
		iConnectionPtr = NULL;
		}
	return ret;
	}

TInt CSocketDesc::GetConnectionPreference(TCommDbConnPref &aCommPref)
	{
	if (iConnPref.iName.Length() == 0)
		{
		return KErrArgument;
		}
	
	TAccessPointRecord apr;	
	TInt ret;

	// Connection preference is set
		TRAP(ret, GetIapDetailsByNameL(iConnPref.iName, apr));

	if (ret == KErrNone)
		{
		aCommPref.SetIapId(apr.iId);
		aCommPref.SetNetId(apr.iNetwork);
		aCommPref.SetBearerSet(apr.iBearer);		
		aCommPref.SetDialogPreference((TCommDbDialogPref)apr.iDialogPref);
		aCommPref.SetDirection((TCommDbConnectionDirection)apr.iDirection);
		}
	return ret;	
	}

void CSocketDesc::GetIapDetailsByNameL(
		TBuf<KCommsDbSvrMaxColumnNameLength> aIapName, 
		TAccessPointRecord &aRecord)
	{
	TAccessPointRecord tempRecord;
	CCommsDatabase* iapDatabase;
	CCommsDbTableView* view;
	TInt ret = KErrNotFound;

	OpenIapTableLC(&iapDatabase, &view);
	ret = view->GotoFirstRecord();
	while (ret == KErrNone)
		{
		TRAP(ret, ReadRecordFromIapTableL(view, tempRecord));
		if (ret == KErrNone)
			{
			if (aIapName == tempRecord.iName)
				{
				aRecord = tempRecord;
				ret = KErrNone;
				break;
				}
			}    	
		ret = view->GotoNextRecord();
		}
	CleanupStack::PopAndDestroy();// view
	CleanupStack::PopAndDestroy(iapDatabase);

	if (ret != KErrNone)
		{
		User::Leave(KErrNotFound);
		}
	}

void CSocketDesc::StopConnection()
	{
	iConnection.Close();
	iConnectionPtr = NULL;
	((CFileTable*)iFids)->RemoveRConnectionPtrAt(iRConnectionIndex);
	iRConnectionIndex = -1;
	}

void CSocketDesc::StopSubConnection()
	{
	iSubConnection.Close();
	iSubConnectionPtr = NULL;
	}

TInt CSocketDesc::StartSubConnection(void *)
	{
	if (iSubConnectionPtr != NULL)
		{
		return KErrAlreadyExists;
		}

	TInt ret = KErrArgument;
	TInt rcIndex = 0;
	RConnection *rc;
	TAccessPointRecord record;	
	do
		{
		if (((CFileTable*)iFids)->RConnectionAt(rcIndex, rc) != KErrNone)
			{
			break;
			}

		// Read index and name of the connection
		GetRConnectionDetails(rc, record);
		if (record.iName == iConnPref.iName)
			{
			ret = KErrNone; // RConnection found
			break;
			}
		rcIndex++;	
		}
	while (ETrue); //Loop over all active connections

	if (ret == KErrNone)
		{
		ret = iSubConnection.Open(*iSockServPtr, RSubConnection::EAttachToDefault, *rc);
		if (ret == KErrNone)
			{
			iSubConnectionPtr = &iSubConnection;
			}
		}

	return ret;
	}

inline void CSocketDesc::SetFids(void *aFids)
	{
	iFids = aFids;
	}

//Add an entry in the interface routing table	
TInt CSocketDesc::RouteRequest(TInt aReq, void *aParam)
	{
	struct rtentry* rt=reinterpret_cast<struct rtentry*>(aParam);

	if (!rt)
		{
		return KErrArgument;	
		}

	TInt ret;
	TPckgBuf<TSoInetRouteInfo>iroute;	
	iroute().iType=ERtNormal;
	iroute().iState=ERtReady; 
	iroute().iMetric=rt->rt_metric;
	if (rt->rt_dev)
		{
		TPckgBuf<TSoInetInterfaceInfo> iface;		
		TFileName name;
		name.Copy(TPtrC8((TText8*)rt->rt_dev));

		if((ret=GetInterfaceByName(name, iface)) != KErrNone)
			return ret;

		Copy(iroute().iIfAddr, iface().iAddress);
		ConvertRtEntry(iroute(), rt);

		//add the entry
		ATOMICSOCKETOP( ret = iSocket.SetOpt(aReq, KSolInetRtCtrl, iroute),return KErrBadHandle )
		return ret;
		}

	return KErrUnknown;				
	}


TInt CSocketDesc::Copy(TInetAddr& aDest, TInetAddr& aSrc)
	{
	if (aSrc.IsV4Mapped() || aSrc.IsV4Compat())
		{
		aSrc.ConvertToV4();
		}

	aDest.SetAddress(aSrc.Address());
	aDest.SetFamily(aSrc.Family());
	aDest.SetPort(aSrc.Port());

	return KErrNone;
	}

TInt CSocketDesc::ConvertSockAddr(TInetAddr& aInetAddr, struct sockaddr_in *aSockAddr)
	{
	if (aSockAddr == NULL)
		{
		return KErrArgument;
		}

		//convert the address and port from network to host byte order...
		aInetAddr.SetAddress(ByteOrder::Swap32(aSockAddr->sin_addr.s_addr));
		aInetAddr.SetFamily(aSockAddr->sin_family);
		aInetAddr.SetPort(ByteOrder::Swap16(aSockAddr->sin_port));
	return KErrNone;	
	}

TInt CSocketDesc::ConvertRtEntry(TSoInetRouteInfo& aRouteInfo, struct rtentry *aRouteEntry)
	{
	if(aRouteEntry == NULL)	
		{
		return KErrArgument;
		}

		ConvertSockAddr(aRouteInfo.iDstAddr, (struct sockaddr_in *)&aRouteEntry->rt_dst);
		ConvertSockAddr(aRouteInfo.iNetMask, (struct sockaddr_in *)&aRouteEntry->rt_genmask);
		ConvertSockAddr(aRouteInfo.iGateway, (struct sockaddr_in *)&aRouteEntry->rt_gateway);			
	return KErrNone;	
	}

TInt CSocketDesc::GetInterfaceByName(const TDesC& aIfName, TPckgBuf<TSoInetInterfaceInfo>& aIface)
	{
	TInt ret = KErrNone;
	ATOMICSOCKETOP(ret = iSocket.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl), ret = KErrBadHandle)
	if (ret != KErrNone)
		{
		return ret;
		}
	TPckgBuf<TSoInetInterfaceInfo> iface;
	ATOMICSOCKETOP(ret = iSocket.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, iface), ret = KErrBadHandle)
	while(ret == KErrNone)
		{
		if (!iface().iAddress.IsUnspecified() && iface().iName.CompareF(aIfName) == 0)
			{
			aIface = iface;
			return ret;    	        
			}
		ATOMICSOCKETOP( ret = iSocket.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, iface), ret = KErrBadHandle )
		}   	
	return KErrUnknown;	
	}

TInt CSocketDesc::GetInterfaceHWAddress(void *aParam)
	{
	ifreq *ifr = reinterpret_cast<ifreq *>(aParam);

	if (!ifr)
		{
		return KErrArgument;	
		}

	if (ifr->ifr_name[0] != '\0')
		{
        TPckgBuf<TSoInetInterfaceInfo> iface;
        TFileName name;
        ifr->ifr_name[IFNAMSIZ-1] = 0; // don't assume NULL terminated input
        name.Copy(TPtrC8((TText8*)ifr->ifr_name));
    
        TInt ret = GetInterfaceByName(name, iface); 		
        if (ret != KErrNone)
            return ret;
    
        if (iface().iHwAddr.Length() > sizeof(SSockAddr))		
            {
            Mem::Copy(&(ifr->ifr_hwaddr.sa_data[0]),&(iface().iHwAddr[sizeof(SSockAddr)]), 6);
            ifr->ifr_hwaddr.sa_family = (TUint16)iface().iHwAddr.Family();
            ifr->ifr_hwaddr.sa_port = ByteOrder::Swap16(iface().iHwAddr.Port());				
            return ret;
            }
		}

	return KErrUnknown;
	}

//This function retrieves the Currently active IAP ID using the RSocket Query.
//First get the interface index of the active connection using KSoInterfaceIndex
//Then with the help of the interface index, fetch the TSoInetIfQuery, under which iZone
//will contain the snap id. User is supposed to call if_indextoname() api to get the IAP name
//from the ID retured with this Ioctl implementation.
TInt CSocketDesc::GetActiveInterface( void *aParam)
    {
    TInt ifindex = -1;
    TInt ret = KErrNone;
    ATOMICSOCKETOP(ret = iSocket.GetOpt(KSoInterfaceIndex, KSolInetIp , ifindex), ret = KErrBadHandle)
    if(ret!=KErrNone)
        {
        return ret;
        }   
    TPckgBuf<TSoInetIfQuery> opt;
    opt().iIndex = ifindex;
    ATOMICSOCKETOP(ret = iSocket.GetOpt(KSoInetIfQueryByIndex, KSolInetIfQuery, opt), ret = KErrBadHandle)
    if(ret!=KErrNone)
        {
        return ret;
        }
    ifreq *ifr = (ifreq * )aParam;
    ifr->ifr_index  = opt().iZone[1]; //IAP_ID
    return KErrNone;
    }
