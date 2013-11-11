/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#include "sysreent.h"
#include <sys/socket.h>
#include <stdapis/netinet/in.h>
#include <es_sock.h>
#include <utf.h>

#include "sysif.h"
#include "fdesc.h"
#include "lposix.h"

#include "netdb_r.h"

extern "C" {

EXPORT_C int _socket_r (int *aErrno, int family, int style, int protocol)
	{
	return Backend()->socket(family, style, protocol, *aErrno);
	}


EXPORT_C int _recvfrom_r (int *aErrno, int fd, char *buf, size_t nbyte, int flags, struct sockaddr* from, size_t* fromsize)
	{
	return Backend()->recvfrom(fd, buf, nbyte, flags, from, (unsigned long*)fromsize, *aErrno);
	}

EXPORT_C int _sendto_r (int *aErrno, int fd, const char *buf, size_t nbyte, int flags, struct sockaddr* to, size_t tosize)
	{
	return Backend()->sendto(fd, buf, nbyte, flags, to, tosize, *aErrno);
	}


EXPORT_C int _shutdown_r (int *aErrno, int fd, int how)
	{
	return Backend()->shutdown(fd, how, *aErrno);
	}

EXPORT_C int _listen_r (int *aErrno, int fd, int n)
	{
	return Backend()->listen(fd, n, *aErrno);
	}

EXPORT_C int _accept_r (int *aErrno, int fd, struct sockaddr *addr, size_t *size)
	{
	int newfd = Backend()->accept(fd, addr, size, *aErrno);
	if (newfd>=0 && (addr != NULL) && (size != NULL))
		Backend()->sockname(newfd, addr, (unsigned long*)size, 1, *aErrno);	// getpeername
	return newfd;
	}

EXPORT_C int _bind_r (int *aErrno, int fd,const struct sockaddr *addr, size_t size)
	{
	return Backend()->bind(fd, addr, size, *aErrno);
	}


EXPORT_C int _connect_r (int *aErrno, int fd, const struct sockaddr *addr, size_t size)
	{
	return Backend()->connect(fd, addr, size, *aErrno);
	}

EXPORT_C int _getsockname_r (int *aErrno, int fd, struct sockaddr *addr, size_t* size)
	{
	if (size == 0)
		{
		*aErrno = EINVAL;
		return -1;
		}

	return Backend()->sockname(fd, addr, (unsigned long*)size, 0, *aErrno);
	}

EXPORT_C int _getpeername_r (int *aErrno, int fd, struct sockaddr *addr, size_t* size)
	{
	if (size == 0)
		{
		*aErrno = EINVAL;
		return -1;
		}
	return Backend()->sockname(fd, addr, (unsigned long*)size, 1, *aErrno);
	}

EXPORT_C int _getsockopt_r (int *aErrno, int fd, int level, int opt, void* buf, size_t* len) 
	{
	if (len == 0)
		{
		*aErrno = EFAULT;
		return -1;
		}
	return Backend()->getsockopt(fd, level, opt, buf, (unsigned long*)len, *aErrno);
	}

EXPORT_C int _setsockopt_r (int *aErrno, int fd, int level, int opt, void* buf, size_t len) 
	{
	return Backend()->setsockopt(fd, level, opt, buf, len, *aErrno);
	}

LOCAL_C inline TInt SockServConnect(RSocketServ& aSs)
	{
	TInt err = aSs.Connect(TUint(-1)); // allow arbit number of requests
	if (err == KErrNone)
		{
		err = aSs.ShareAuto();
		}
	return err;
	}

EXPORT_C int _gethostname_r (int *aErrno, char *name, size_t size)
	{
	TInt err = KErrNone;
	if(size <= 0)
        {
        return MapError(EINVAL, *aErrno);
        }
	RSocketServ& ss = Backend()->SockServSession();
	// connect to the Socket Server if necessary
	if (ss.Handle() == 0)
		{
		err = SockServConnect(ss);
		if (err != KErrNone)
			{
			return MapError(err, *aErrno);
			}
		}
	
	RHostResolver hr;
	/* Get the default RConnection instance and use it */
	RConnection& defConnection = Backend()->GetDefaultConnection();
	if(defConnection.SubSessionHandle() != 0)
	    {
	    err = hr.Open(ss, AF_INET, IPPROTO_UDP,defConnection);
	    }
	else
	    {
		err = ECONNABORTED;
	    }
	
	if (err == KErrNone)
		{
		TBuf<128> hostname;
		err = hr.GetHostName(hostname);
		if (err == KErrNone)
			{
				TPtr8 ret((TText8*)name, size-1);
				if(CnvUtfConverter::ConvertFromUnicodeToUtf8(ret,hostname) == 0)
				{
				name[ret.Size()] = '\0';
				}
		    	else
				{
				err = ENAMETOOLONG;
				}
				
			}

		hr.Close();
		}
	return MapError(err, *aErrno);
	}

EXPORT_C int _getprotobyname_r(int* aErrno, const char* name, TProtocolDesc* pProtoInf)
	{
	TInt err = KErrNone;
	RSocketServ& ss = Backend()->SockServSession();
	// connect to the Socket Server if necessary
	if (ss.Handle() == 0)
		{
		err = SockServConnect(ss);
		if (err != KErrNone)
			{
			return MapError(err, *aErrno);
			}
		}
		
	TBuf<128> buf;
	TPtrC8 bufPtr((TText8*)name);
	buf.Copy(bufPtr);
	
	err = ss.FindProtocol(buf, *pProtoInf);
	return MapError(err, *aErrno);
	}

EXPORT_C int _getprotobynumber_r(int *aErrno, int proto, TProtocolDesc* pProtoInf)
	{
	TInt err = KErrNone;
	RSocketServ& ss = Backend()->SockServSession();
	// connect to the Socket Server if necessary
	if (ss.Handle() == 0)
		{
		err = SockServConnect(ss);
		if (err != KErrNone)
			{
			return MapError(err, *aErrno);
			}
		}
		
	TUint count;
	err = ss.NumProtocols(count);
	if (err == KErrNone)
		{
		TInt idx = 1;
		for (; idx <= count; ++idx)
			{
			err = ss.GetProtocolInfo(idx, *pProtoInf);
			if (err != KErrNone || pProtoInf->iProtocol == proto)
				{
				break;
				}
			}
		
		if (idx > count)
			{
			// no protocol matched input
			return -1;
			}
		}
	return MapError(err, *aErrno);
	}


struct hostent_buf
	{
	struct hostent iHostent;
	struct sockaddr iAddr;
	char* iPtrs[2];
	char iName[1];	// and following bytes
	};

struct hostent* mapNameRecord(struct _reent* rp, TNameRecord& aRecord, int length, int format)
	{
	Backend()->Free(rp->_netdb);
	HBufC8 * name;
	TInt errNum;
	if(ConvertUnicodeToUtf8(aRecord.iName,name,errNum) == -1)
		{
		rp->_errno = EINVAL;
		return 0;
		}

	// Switch to the backend heap
	RHeap* oldHeap = User::SwitchHeap(Backend()->Heap());
	struct hostent_buf* hbp = (struct hostent_buf*)Backend()->Alloc(sizeof(struct hostent_buf)+name->Size()+1);
	// Revert to the default heap
	User::SwitchHeap(oldHeap);

	rp->_netdb = hbp;
	if (hbp == 0)
		{
		delete name;
		rp->_errno = ENOMEM;
		return 0;
		}
	hbp->iHostent.h_name = &hbp->iName[0];		// one name
	hbp->iHostent.h_aliases = &hbp->iPtrs[1];	// no aliases
	hbp->iHostent.h_addrtype = format;
	hbp->iHostent.h_length = length;
	hbp->iHostent.h_addr_list = &hbp->iPtrs[0];		// one address...
	hbp->iPtrs[0] = (char*)&hbp->iAddr.sa_data[0];	// ... which is iAddr

	TPtr8 tname((TText8*)&hbp->iName[0], name->Size()+1);
	tname.Copy(name->Des());
	tname.ZeroTerminate();
    delete name;
    
	unsigned long len = sizeof(hbp->iAddr);
	aRecord.iAddr.SetFamily(format);	// not set by GetByName(_L(""));
	static_cast<TUSockAddr*>(&aRecord.iAddr)->Get(&hbp->iAddr, &len);	
	return &hbp->iHostent;
	}

/*
Get the internet name of the host by address.
@return 
@param rp pointer
@param addr 
@param length 
@param format 
*/
EXPORT_C struct hostent* _gethostbyaddr_r (struct _reent* rp, const char* addr, int length, int format)
	{
	TInt err = KErrNone;
	struct hostent* retval = NULL;
	
	RSocketServ& ss = Backend()->SockServSession();
	// connect to the Socket Server if necessary
	if (ss.Handle() == 0)
		{
		err = SockServConnect(ss);
		if (err != KErrNone)
			{
			MapError(err, rp->_errno);
			return retval;
			}
		}
	
    if(format != AF_INET && format != AF_INET6)
        {
        format = AF_INET;
        }
	    
	if (err == KErrNone)
		{
		RHostResolver r;
		/* Get the default RConnection instance and use it */
		RConnection& defConnection = Backend()->GetDefaultConnection();
		if(defConnection.SubSessionHandle() != 0)
		    {
			err=r.Open(ss, format, IPPROTO_UDP, defConnection);
		    }
		else
		    {
            err = -ECONNABORTED; // needs to be negative for logic below to return NO_RECOVERY to caller
		    }
		
		if (err == KErrNone)
			{
			struct sockaddr buf;
			buf.sa_family = (unsigned short)format;
			memcpy(buf.sa_data,addr,length);
			TUSockAddr addr(&buf, length+4);
			TNameRecord record;
			TNameEntry entry(record);
		    TInetAddr inetAddr(addr);
		    
            if(inetAddr.IsLoopback())
                {
                if(format == AF_INET)
                    inetAddr.SetAddress(KInetAddrLoop);
                else if(format == AF_INET6)
                    inetAddr.SetAddress(KInet6AddrLoop);
                entry().iAddr = inetAddr;
                entry().iName.Copy(_L("localhost"));
                }
		            
            else
                {
                err = r.GetByAddress(addr, entry);
                while (err == KErrNone)
                    {
                    TInetAddr addr(entry().iAddr);
                    if(!addr.IsUnspecified())
                        {
                        if(format == addr.Family())
                            {
                            break;
                            }
                        else if(format == AF_INET && addr.Family() == AF_INET6 && addr.IsV4Mapped())
                            {
                            addr.ConvertToV4();
                            entry().iAddr = addr;
                            break;
                            }
                        }
                    err = r.Next(entry);
                    }
                
                if (err == -5121) // KErrDndAddrNotFound, equivalent to a KErrNotFound?
                    {
                    err = HOST_NOT_FOUND;
                    }
                }
		             if (err == KErrNone)
		                {
		                record = entry();
		                retval = mapNameRecord(rp, record, length, format);
		                }
			r.Close();
			}
		}
	if (err == -3004)	// NETDIAL busy
		err = TRY_AGAIN;
	if (err < -3000)	// i.e. a NETDIAL error
		err = NO_RECOVERY;
	MapError(err, rp->_errno);
	return retval;
	}

/*
Get the internet name of the host by name.
@return 
@param rp 
@param name name of the host
*/
EXPORT_C struct hostent* _gethostbyname_r (struct _reent* rp, const char* name)
	{
	TInt err = KErrNone;
	struct hostent* retval = NULL;
	RSocketServ& ss = Backend()->SockServSession();
	// connect to the Socket Server if necessary
	if (ss.Handle() == 0)
		{
		err = SockServConnect(ss);
		if (err != KErrNone)
			{
			MapError(err, rp->_errno);
			return retval;
			}
		}
	
    RHostResolver r;
    /* Get the default RConnection instance and use it */
    RConnection& defConnection = Backend()->GetDefaultConnection();
    if (defConnection.SubSessionHandle() != 0)
        {
        err = r.Open(ss, AF_INET, IPPROTO_UDP, defConnection);
        }
    else
        {
        err = -ECONNABORTED; // needs to be negative for logic below to return NO_RECOVERY to caller
        }
    
    if (err == KErrNone)
        {
        TNameRecord record;
        TNameEntry entry(record);
        
        TPtrC8 ptr(reinterpret_cast<const TUint8*> (name));
        THostName hostname;
        hostname.Copy(ptr);
        
        if (hostname.CompareF(_L("localhost")) == KErrNone)
            {
            TInetAddr addr;
            addr.SetAddress(KInetAddrLoop);
            entry().iAddr = addr;
            entry().iName.Copy(hostname);
            }
        else
            {
            err = r.GetByName(hostname, entry);
            while (err == KErrNone)
                {
                TInetAddr addr(entry().iAddr);
                if(!addr.IsUnspecified())
                    {
                    if (addr.Family() == KAfInet) 
                        {
                        break;
                        }
                    
                    if (addr.Family() == KAfInet6 && addr.IsV4Mapped())
                        {
                        addr.ConvertToV4(); 
                        entry().iAddr = addr;
                        break;
                        }
                    }
                err = r.Next(entry);
                }
            
            if (err == -5120) // KErrDndNameNotFound, equivalent to a KErrNotFound
                {
                err = HOST_NOT_FOUND;
                }
            }
        
        if (err == KErrNone)
            {
            record = entry();
            retval = mapNameRecord(rp, record, sizeof(struct in_addr), AF_INET);
            }
        
        r.Close();
        }

	if (err == -3004)	// NETDIAL busy
		err = TRY_AGAIN;
	if (err < -3000)	// i.e. a NETDIAL error
		err = NO_RECOVERY;
	MapError(err, rp->_errno);
	return retval;
	}

/*
 * Set the default IAP interface in the backend
 * @param	aErrno 	The errno to be set
 * @param	aIfReq	Pointer to the ifreq structure containing the interface name
 * @return 			0 on success, -1 on failure 
 */
EXPORT_C int _setdefaultif_r(int *aErrno, const struct ifreq* aIfReq)
	{
	TInt err = KErrNone;

	RSocketServ& ss = Backend()->SockServSession();
	// connect to the Socket Server if necessary
	//NOTE: The following is not really threadsafe. The checking for the 
	//server handle and connecting to the same should be atomic, and should
	//be done everywhere else too
	if (ss.Handle() == 0)
		{
		err = SockServConnect(ss);
		if (err != KErrNone)
			{
			MapError(err, *aErrno);
			return -1;
			}
		}
	
	err = Backend()->setdefaultif(aIfReq);
	if(err != KErrNone)
		{
		MapError(err, *aErrno);
		return -1;
		}
	
	return 0;
	}

EXPORT_C int _unsetdefaultif_r(int* /*aErrno*/)
    {
    return Backend()->unsetdefaultif(EFalse);
    }

/*
 * Helper function to create an addrinfo node and fill it.
 * @param	aRec		 	The name details to be used to fill the addrinfo node
 * @param	aHints			The hints to be used
 * @param	aNode			Pointer to the addrinfo structure pointer to be created
 * @return 					0 on success, EAI_MEMORY on error 
 */
static int CreateAddrInfoNode(TNameRecord& aRec, const struct addrinfo* aHints, struct addrinfo** aNode)
	{
	unsigned long addrLen = 0;
	TUint nameLen = aRec.iName.Length();
	
	//The following alloc function fills the chunk with zeros
	*aNode = (struct addrinfo*) Backend()->Alloc(sizeof(struct addrinfo));
	if(*aNode == NULL)
		return EAI_MEMORY;
	
	//Copy the canonical name
	if(nameLen != 0 || (aHints->ai_flags & AI_CANONNAME)) 
		{
		(*aNode)->ai_canonname = (char*) Backend()->Alloc(nameLen + 1);
		if((*aNode)->ai_canonname == NULL)
			{
			Backend()->Free(*aNode);
			*aNode = NULL;
			return EAI_MEMORY;
			}
		
		TPtr8 namePtr((TText8*) (*aNode)->ai_canonname, nameLen + 1); 
#ifdef _UNICODE
		CnvUtfConverter::ConvertFromUnicodeToUtf8(namePtr, aRec.iName); //wchar* string
#else	
		namePtr.Copy(aRec.iName); //char* string
#endif /* _UNICODE */
		namePtr.ZeroTerminate();
		}
	
	 TInetAddr inetAddr(aRec.iAddr);
	 (*aNode)->ai_family = inetAddr.Family();
	
	//Copy the address
	(*aNode)->ai_addr = (struct sockaddr*) Backend()->Alloc(sizeof(struct sockaddr));
	if((*aNode)->ai_addr == NULL) 
		{
		if((*aNode)->ai_canonname != NULL)
			Backend()->Free((*aNode)->ai_canonname);
		Backend()->Free(*aNode);
		*aNode = NULL;
		return EAI_MEMORY;
		}
		
	addrLen = sizeof((*aNode)->ai_addr);
	STATIC_CAST(TUSockAddr*, &aRec.iAddr)->Get((*aNode)->ai_addr, &addrLen);
	(*aNode)->ai_addrlen = addrLen;
	
	return 0;
	}

/*
 * Get the list of addresses (in the form of addrinfo structure) by 
 * resolving the given host name. This function returns only
 * the adresses and the canonical names
 * @param	aErrno		 	Pointer to errno
 * @param	aHostName		The host name to be resolved
 * @param	aHints			Pointer to the hints structure
 * @param	aRes			The pointer to store the result
 * @return 					0 on success, error code on failure
 */
EXPORT_C int _getaddrinfo_r(int* aErrno, const char* aHostName, 
							const struct addrinfo* aHints, struct addrinfo** aRes)
	{
	TInt err = KErrNone;
	
	//Check params
	if(!aHostName || !aHints || !aRes)
		return EAI_FAIL;
	
#ifdef _UNICODE
	TPtrC8 hNamePtr(REINTERPRET_CAST(const TUint8*, aHostName));
	TBuf<0x40> hostName;
	hostName.Copy(hNamePtr);
#else
	TPtrC8 hostName(REINTERPRET_CAST(const TUint8*, aHostName));
#endif /* _UNICODE */
	
	//Check for 'localhost'.
    TBuf<64> localHostName;
    _LIT(KNameBuf, "localhost");
    localHostName.Copy(KNameBuf);
	if(hostName.CompareF(localHostName) == 0 && aHints->ai_family == AF_INET)
		{
		TInetAddr localAddr;
		TNameRecord nameRec;
		localAddr.SetAddress(INET_ADDR(127, 0, 0, 1));
		nameRec.iAddr = localAddr;
		nameRec.iName.Copy(localHostName);
		
		return CreateAddrInfoNode(nameRec, aHints, aRes); 
		}
	
    // connect to the Socket Server if necessary
	RSocketServ& sockServ = Backend()->SockServSession();
	if (sockServ.Handle() == 0)
		{
		err = SockServConnect(sockServ);
		if (err != KErrNone)
			{
			MapError(err, *aErrno);
			return EAI_SYSTEM;
			}
		}
	
	//Open the host resolver
	RHostResolver resolver;
	/* Get the default RConnection instance and use it */
	RConnection& defConnection = Backend()->GetDefaultConnection();
	if (defConnection.SubSessionHandle() != 0)
	    {
		err = resolver.Open(sockServ, KAfInet, KProtocolInetUdp, defConnection);
	    }
	else
	    {
        err = ECONNABORTED;
	    }
	
	if (err != KErrNone)
		{
		MapError(err, *aErrno);
		return EAI_SYSTEM;
		}
	
	//Resolve by name
	TNameRecord nameRec;
	TNameEntry nameEntry(nameRec);

	err = resolver.GetByName(hostName, nameEntry);
	if( err != KErrNone)
		{
		resolver.Close();
		return EAI_FAIL; 
		}
	
	//Create a list of addrinfo nodes from the result
	*aRes = NULL;
	struct addrinfo** curr = aRes;
	do
        {
        nameRec = nameEntry();
        TInetAddr inetAddr(nameRec.iAddr);
        
        
        //Create the node if the address is valid, and the family matches that of hints
        if (!inetAddr.IsUnspecified())
            {
            if (inetAddr.Family() == KAfInet)
                {
                err = CreateAddrInfoNode(nameRec, aHints, curr);
                if (err != 0)
                    {
                    break;
                    }
                
                curr = &((*curr)->ai_next);
                
                inetAddr.ConvertToV4Mapped();
                nameRec.iAddr = inetAddr;
                
                err = CreateAddrInfoNode(nameRec, aHints, curr);
                if (err != 0)
                    {
                    break;
                    }
                
                (*curr)->ai_flags |= AI_V4MAPPED;
                curr = &((*curr)->ai_next);
                }
            else if (inetAddr.Family() == KAfInet6)
                {
                err = CreateAddrInfoNode(nameRec, aHints, curr);
                if (err != 0)
                    {
                    break;
                    }
                
                curr = &((*curr)->ai_next);
                if (inetAddr.IsV4Mapped())
                    {
                    inetAddr.ConvertToV4();
                    nameRec.iAddr = inetAddr;
                    
                    err = CreateAddrInfoNode(nameRec, aHints, curr);
                    if (err != 0)
                        {
                        break;
                        }
                    
                    (*curr)->ai_flags |= AI_V4CONVERTED;
                    curr = &((*curr)->ai_next);
                    }
                }
            }

        err = resolver.Next(nameEntry); //Get the next record
        if (err != KErrNone)
            {//No more records. Not an error, just stop iterating
            err = KErrNone;
            break;
            }
        } while(err == KErrNone);
	       
	
	//If no nodes are created even when the operation is succes, it's an error
	if(err == 0 && *aRes == NULL)
		err = -1;
	//If the operation failed and some nodes are created, free them
	if(err != 0 && *aRes != NULL)
		{		
		_freeaddrinfo_r(*aRes);
		*aRes = NULL;
		}
	if(err != 0 && err != EAI_MEMORY)
        err = EAI_FAIL;
	resolver.Close();

	return err;
	}

/*
 * Frees the addrinfo structure created in backend
 * @param	aInfo	The pointer to the addrinfo structure to be freed 
 */
EXPORT_C void _freeaddrinfo_r(struct addrinfo* aInfo)
	{
	if(aInfo == NULL)
		return;
	
	struct addrinfo *next;
	while(aInfo)
		{
		next = aInfo->ai_next;
		if(aInfo->ai_canonname != NULL)
			Backend()->Free(aInfo->ai_canonname);
		if(aInfo->ai_addr != NULL)
			Backend()->Free(aInfo->ai_addr);
		
		Backend()->Free(aInfo);
		aInfo = next;
		} 
	}

} //extern "C"
