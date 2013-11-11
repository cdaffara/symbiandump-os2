// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Information shared between the Streaming Test Agent client and server implementations.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef STACOMMON_H_
#define STACOMMON_H_

#include <e32base.h>
#include <e32std.h>
#include <e32cmn.h>
#include <e32ver.h>
#include <e32uid.h>
#include <s32mem.h>
#include <stringpool.h>

#include "sdpdocument.h"
#include "sdpmediafield.h"
#include "sdpattributefield.h"
#include "sdpcodecstringpool.h"
#include "sdpfmtattributefield.h"
#include "sdpcodecstringconstants.h"

#include <caf/streaming/streamcaftypes.h>
#include <caf/streaming/keyassociation.h>
#include <caf/streaming/keystreamsink.h>
#include <caf/caferr.h>

namespace StreamAccess
	{
	_LIT(KStaName, "StreamingTestAgent");		///< Identifies Streaming Test Agent server.

	const TInt KStaVerMajor = 1;		///< Streaming Test Agent version major component.
	const TInt KStaVerMinor = 0;		///< Streaming Test Agent version minor component.
	const TInt KStaVerBuild = 0;		///< Streaming Test Agent version build component.

	inline TVersion StaVersion();
		
	/**
		Executable which hosts the Streaming Test Agent.  Required to launch the process.

		@see KUidStreamingTestAgent
	*/
	_LIT(KStaImageName, "staserver.exe");
		
	/**
		Hosting the executable's secure ID.  Required to launch the process.

		@see KStreamingTestAgentImageName
	 */
	const TUid KUidStreamingTestAgent = {0x1028588B};
		
	inline TUidType StaImageFullUid();
	 		
	/**
		Delay in microseconds before the Streaming Test Agent server is shut down, after
		the last remaining session has been closed.
	 */
	const TInt KStaShutdownPeriod = 2 * 1000 * 1000;
	
	enum TStaFunction
	/**
		Functions supported by the Streaming Test Agent session.
	 */
		{
			EGetAttribute=0,         	 ///< Gets an attribute.
			EGetStringAttributeSize, 	 ///< Gets the size of a string attribute.
			EGetStringAttributeData, 	 ///< Gets the data of a string attribute.
			EGetPostDeliveryRights,  	 ///< Gets the handle of a post-acqusition rights object file and the size of the mime type data.
			ESendKeyStream,    	 	  	 ///< Sends a key stream (an encrypted short-term key).
			ESetKeyStreamSink,   	 	 ///< Sends a key stream sink to the test agent server.
			ESetSdpKeyStream,			 ///< Sends an SDP key stream field object to the streaming test agent server.
			ESetSdpDocument,			 ///< Sends an SDP document object to the streaming test agent server.
			ESetIpSecAssoc			 	 ///< Sends an IPSec association to the streaming test agent server.
		};
	
	}//End of the namespace StreamAccess
	
	/** The file extension of the rights object files. */
	_LIT(KRoFileExtension,".dr");
	
	/**
		Test Stream Agent clients are panicked with this category when invalid
		input to the server is detected.
	 */
	_LIT(KStaClientPanicCat, "STA-Client");
	
#include "stacommon.inl"

#endif /*STACOMMON_H_*/
