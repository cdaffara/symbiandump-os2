// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// 
//

/**
 @file UriUtils.h
 @publishedAll
 @released
*/

#ifndef __URIUTILS_H__
#define __URIUTILS_H__

// System includes
//
#include <e32base.h>
#include <authority8.h>
#include <uri8.h>
#include <uri16.h>


/**
  Comments :
  @publishedAll
  @released
*/
class UriUtils
	{
public:
/** 
	Enum defining a flag used to indicate host type.
	@publishedAll
	@released
	@since 9.1	
 */
	enum TUriHostType
		{
		/** 
			Indicates host is of type IPv6. Example: 1080:0:0:0:8:800:200C:417A 
		 */
		EIPv6Host,
		/**
			Indicates host is of type IPv4. Example: 192.168.202.18 
		 */
		EIPv4Host,
		/** 
			Indicates host is in text form. Example: www.mypage.com 
		 */
		ETextHost,
		};

	IMPORT_C static CUri8* ConvertToInternetFormL(const TUriC16& aUri);
	IMPORT_C static CUri16* ConvertToDisplayFormL(const TUriC8& aUri);

	IMPORT_C static CUri8* CreateUriL(const TDesC& aUri);
	IMPORT_C static CAuthority8* CreateAuthorityL(const TDesC& aAuthority) ;

	IMPORT_C static TBool HasInvalidChars(const TDesC8& aData);
	IMPORT_C static TBool HasInvalidChars(const TDesC16& aData);

	IMPORT_C static TUriHostType HostType(const TDesC8& aHost);
	IMPORT_C static TUriHostType HostType(const TDesC16& aHost);
	IMPORT_C static CUri8* NormaliseUriL(const TUriC8& aUri);
	};

#endif	// __URIUTILS_H__
