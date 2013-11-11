/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : SdpCodecStringPool.h
* Part of       : Local SDP Codec
* Interface     : SDK API, Local SDP Codec API
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSDPCODECSTRINGPOOL_H
#define CSDPCODECSTRINGPOOL_H

//  INCLUDES
#include <e32base.h>
#include "_sdpdefs.h"
#include <stringpool.h>


// CLASS DECLARATION
/**
 *	This class defines string pool for SdpCodec.
 *  Only one CSdpCodecStringPool object must be created in an application.
 *  If more than one CSdpCodecStringPool objects are used by an
 *  application with CSdpDocument objects, panic occurs. 
 *
 *  @lib localsdpcodec.lib
 */
class CSdpCodecStringPool : public CBase
	{
public: // Constructors and destructor
 		
	/**
	*	StringPool is created for SdpCodec.
	*
    *	@return The new instance.
	*/
	static CSdpCodecStringPool* NewLC();

	/**
	*	Deletes the resources held by the instance.
	*/
	~CSdpCodecStringPool();

public: 
	
	/**
	* Gets the string pool used by SDP
	*
	* @return a handle to a string pool
	*/
	RStringPool StringPool();

	/**
	* Gets the SDP's string pool table
	*
	* @return the string pool table
	*/
	static const TStringTable& StringTable();

private:

	CSdpCodecStringPool();
	void ConstructL();

private: // Data
		
	RStringPool iPool;
	};

#endif
