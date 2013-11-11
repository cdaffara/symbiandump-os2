// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Support for hostname & hostaddr related functions
// 
//

#include "SYSIF.H"
#include "FDESC.H"
#include <unistd.h>
#include <stdlib.h>
#include <sys/errno.h>
#include "LPOSIX.H"
#include <sys/socket.h>
#include <libc/netinet/in.h>
#include <netdb_r.h>
#include <string.h>
#include <in_sock.h>
#include <networking/dnd_err.h>

extern "C" {

/**
Get the internet name of this host. Actually this will always return a null 
string with TCPIP 030 and onwards because the "name" of a mobile host
isn't really very meaningful - in practice the IP address is chosen dynamically
once you start doing real networking, at which time the ISP can resolve the 
IP address into a name of some sort if you really want.
@return 
@param name
@param size
*/
EXPORT_C int gethostname (char *name, size_t size)
	{
	int* perrno=__errno();
	RSocketServ ss;
	TInt err=ss.Connect(1);
	if (err==KErrNone)
		{
		RHostResolver r;
		err=r.Open(ss, AF_INET, KProtocolInetUdp);
		if (err==KErrNone)
			{
			TBuf<128> hostname;
			err=r.GetHostName(hostname);
			if (err==KErrNone)
				{
				if (size>(size_t)hostname.Length())
					{
					TPtr8 retval((TText8*)name,size);
					retval.Copy(hostname);
					retval.PtrZ();
					}
				else
					err=ENAMETOOLONG;
				}
			r.Close();
			}
		ss.Close();
		}
	return MapError(err,*perrno);
	}

/**
Get the internet name of the host by address.
@return 
@param addr Address of the host
@param length 
@param format 
*/
EXPORT_C struct hostent* gethostbyaddr (const char* addr, int length, int format)
	{
	return _gethostbyaddr_r(_REENT,addr,length,format);
	}

/**
Get the internet name of the host by name.
@return 	
@param name Name of the host
*/
EXPORT_C struct hostent* gethostbyname (const char* name)
	{
	return _gethostbyname_r(_REENT,name);
	}

enum GetHostType {ByName = 1, ByAddr};

#define MAX_ADDR_OR_ALIAS_LIST	5

struct hostent_buf
	{
	struct hostent iHostent;
	char *iAliasesPtr[MAX_ADDR_OR_ALIAS_LIST+1];
	char *iAddrListPtr[MAX_ADDR_OR_ALIAS_LIST+1];
	THostName iHostAliases[MAX_ADDR_OR_ALIAS_LIST];
	struct sockaddr iHostAddress[MAX_ADDR_OR_ALIAS_LIST];
	};

struct hostent* mapNameRecord(struct _reent* rp, TFixedArray<TNameRecord, MAX_ADDR_OR_ALIAS_LIST>& aRecord, const TInt aCount, const GetHostType aHostType, const int aLength, const int aFormat)
	{
	free(rp->_netdb);
	struct hostent_buf* hbp = (struct hostent_buf*)calloc(1,sizeof(struct hostent_buf));
	rp->_netdb = hbp;
	if (hbp==0)
		{
		rp->_errno=ENOMEM;
		return 0;
		}

	// Set-up Aliases & Address list pointer
	hbp->iHostent.h_aliases = &hbp->iAliasesPtr[0];
	hbp->iHostent.h_addr_list = &hbp->iAddrListPtr[0];

	// Fill-up Host Name
	hbp->iHostent.h_name = (char*) &hbp->iHostAliases[0];
	TPtr8 name((TText8*)&hbp->iHostAliases[0], aRecord[0].iName.Length()+1);
	name.Copy(aRecord[0].iName);
	name.ZeroTerminate();

	hbp->iHostent.h_addrtype = aFormat;
	hbp->iHostent.h_length = aLength;

	// Fill-up Address
	hbp->iAddrListPtr[0] = (char*)&hbp->iHostAddress[0].sa_data[0];		// ... which is iAddr
	unsigned long len=sizeof(struct sockaddr);
	aRecord[0].iAddr.SetFamily(aFormat);	// not set by GetByName(_L(""));
	STATIC_CAST(TUSockAddr*,&aRecord[0].iAddr)->Get(&hbp->iHostAddress[0],&len);

	if (aCount > 1)
		{
		// If there are multiple host name or address
		TInt i;
		if (aHostType == ByName)
			{
			// there are multiple addresses for a given host
			hbp->iAliasesPtr[0] = NULL;
			for (i=1; i<aCount; i++)
				{
				hbp->iAddrListPtr[i] = (char*)&hbp->iHostAddress[i].sa_data[0];
				len=sizeof(struct sockaddr);
				aRecord[i].iAddr.SetFamily(aFormat);	// not set by GetByName(_L(""));
				STATIC_CAST(TUSockAddr*,&aRecord[i].iAddr)->Get(&hbp->iHostAddress[i],&len);
				}
			hbp->iAddrListPtr[i] = NULL;
			}
		else
			{
			// there are multiple host/aliases name for a given address
			hbp->iAddrListPtr[1] = NULL;
			for (i=1; i<aCount; i++)
				{
				hbp->iAliasesPtr[i-1] = (char*) &hbp->iHostAliases[i];
				name.Set((TText8*)&hbp->iHostAliases[i], 0, aRecord[i].iName.Length()+1);
				name.Copy(aRecord[i].iName);
				name.ZeroTerminate();
				}
			hbp->iAliasesPtr[i-1] = NULL;
			}
		}
	else
		{
		hbp->iAliasesPtr[0] = NULL;
		hbp->iAddrListPtr[1] = NULL;
		}
	return &hbp->iHostent;
	}

/**
Get the internet name of the host by address.
@return 
@param rp pointer
@param addr 
@param length 
@param format 
*/
EXPORT_C struct hostent* _gethostbyaddr_r (struct _reent* rp, const char* addr, int length, int format)
//
// For AF_INET, 
	{
	RSocketServ ss;
	TInt err=ss.Connect();
	struct hostent* retval=0;
	if (err==KErrNone)
		{
		RHostResolver r;
		err=r.Open(ss, AF_INET, KProtocolInetUdp);
		if (err==KErrNone)
			{
			struct sockaddr buf;
			buf.sa_family=(unsigned short)format;
			memcpy(buf.sa_data,addr,length);
			TUSockAddr addr(&buf,length+4);

			TFixedArray<TNameRecord, MAX_ADDR_OR_ALIAS_LIST> record;
			TNameEntry entry;
			TInt count = 0;
			err=r.GetByAddress(addr,entry);
			if (err==KErrNone)
				{
				record[count++]=entry();
				while ((r.Next(entry) == KErrNone) && (count < MAX_ADDR_OR_ALIAS_LIST))
					{
					record[count++] = entry();
					}
				retval = mapNameRecord(rp, record, count, ByAddr, length, format);
				}
			r.Close();
			}
		ss.Close();
		}
	if (err==-3004)	// NETDIAL busy
		err=TRY_AGAIN;

	// KErrDndAddrNotFound=-5121 should be excluded (Returned when no data found for GetByAddr)
	if ((err<-3000)	&& (err!=KErrDndAddrNotFound))	// i.e. a NETDIAL error
		err=NO_RECOVERY;
	MapError(err,rp->_errno);
	return retval;
	}

/**
Get the internet name of the host by name.
@return 
@param rp 
@param name name of the host
*/
EXPORT_C struct hostent* _gethostbyname_r (struct _reent* rp, const char* name)
	{
	RSocketServ ss;
	TInt err=ss.Connect();
	struct hostent* retval=0;
	if (err==KErrNone)
		{
		RHostResolver r;
		err=r.Open(ss, AF_INET, KProtocolInetUdp);
		if (err==KErrNone)
			{			
			TFixedArray<TNameRecord, MAX_ADDR_OR_ALIAS_LIST> record;
			TNameEntry entry;
#ifdef _UNICODE
			TPtrC8 ptr(REINTERPRET_CAST(const TUint8*,name));
			TBuf<0x40> hostname;
			hostname.Copy(ptr);
#else
			TPtrC8 hostname(REINTERPRET_CAST(const TUint8*,name));
#endif /* _UNICODE */

			TInt count = 0;
			err=r.GetByName(hostname,entry);
			if (err==KErrNone)
				{
				record[count++]=entry();
				while ((r.Next(entry) == KErrNone) && (count < MAX_ADDR_OR_ALIAS_LIST))
					{
					record[count++] = entry();
					}
				retval = mapNameRecord(rp, record, count, ByName, sizeof(struct in_addr), AF_INET);
				}
			r.Close();
			}
		ss.Close();
		}
	if (err==-3004)	// NETDIAL busy
		err=TRY_AGAIN;

	// KErrDndNameNotFound=-5120 should be excluded (Returned when no data found for GetByName)
	if ((err<-3000)	&& (err!=KErrDndNameNotFound)) // i.e. a NETDIAL error
		err=NO_RECOVERY;
	MapError(err,rp->_errno);
	return retval;
	}

} // extern "C"
