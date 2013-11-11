/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


/**
 @file 
 @publishedAll
 @released
*/
 
#ifndef __MCTCERTAPPS_H__
#define __MCTCERTAPPS_H__

#include <e32base.h>
#include <f32file.h>
#include <s32file.h>
#include <ct/mcttokeninterface.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mctcertappinterface.h>
#endif



/**
 * Information about a certificate application.
 *
 * Certificate applications are used to represent classes of certificate use,
 * for example Software Install, TLS, WTLS, WMLScript, SignText, etc.
 *
 * CA Certificates are associated with zero or more applications, and this
 * information is used to determine which certificates are used when building
 * and validating certificate chains.
 *
 * An application is identified by a UID.  A TCertificateAppInfo contains this
 * and the name of the application.
 * 
 */
class TCertificateAppInfo
	{
public:
	IMPORT_C TCertificateAppInfo(const TUid& aUid, const TName& aName);
	IMPORT_C TCertificateAppInfo& operator = (const TCertificateAppInfo& aClient);
	IMPORT_C TCertificateAppInfo();
	IMPORT_C const TUid& Id() const;
	IMPORT_C const TName& Name() const;
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);

private:
	TUid iUid;                    
	TName iName;
	};


#endif // __MCTCERTAPPS_H__
