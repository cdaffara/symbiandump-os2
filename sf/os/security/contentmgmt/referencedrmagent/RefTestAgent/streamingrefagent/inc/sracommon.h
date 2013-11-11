/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Information shared between the Streaming Reference Agent client and server implementations.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SRACOMMON_H_
#define SRACOMMON_H_

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


_LIT(KSraName, "StreamingReferenceAgent");		///< Identifies Streaming Reference Agent server.

const TInt KSraVerMajor = 1;		///< Streaming Reference Agent version major component.
const TInt KSraVerMinor = 0;		///< Streaming Reference Agent version minor component.
const TInt KSraVerBuild = 0;		///< Streaming Reference Agent version build component.

inline TVersion SraVersion();
		
/**
	Executable which hosts the Streaming Reference Agent.  Required to launch the process.

	@see KUidStreamingTReferenceAgent
 */
	_LIT(KSraImageName, "sraserver.exe");
		
/**
	Hosting the executable's secure ID.  Required to launch the process.

	@see KStreamingReferenceAgentImageName
 */
const TUid KUidStreamingReferenceAgent = {0x10285A1C};
		
inline TUidType SraImageFullUid();
	 		
/**
	Delay in microseconds before the Streaming Reference Agent server is shut down, after
	the last remaining session has been closed.
 */
const TInt KSraShutdownPeriod = 2 * 1000 * 1000;
	
enum TSraFunction
/**
	Functions supported by the Streaming Reference Agent session.
 */
	{
	EGetAttribute=0,         	 ///< Gets an attribute.
	EGetStringAttributeSize, 	 ///< Gets the size of a string attribute.
	EGetStringAttributeData, 	 ///< Gets the data of a string attribute.
	EGetPostDeliveryRights,  	 ///< Gets the handle of a post-acqusition rights object file and the size of the mime type data.
	ESendKeyStream,    	 	  	 ///< Sends a key stream (an encrypted short-term key).
	ESetKeyStreamSink,   	 	 ///< Sends a key stream sink to the test agent server.
	ESetSdpKeyStream,			 ///< Sends an SDP key stream field object to the streaming reference agent server.
	ESetSdpDocument,			 ///< Sends an SDP document object to the streaming reference agent server.
	ESetIpSecAssoc			 	 ///< Sends an IPSec association to the streaming reference agent server.
	};
		
	
/** The file extension of the rights object files. */
_LIT(KRoFileExtension,".dr");
	
/**
	Streaming Reference Agent clients are panicked with this category when invalid
	input to the server is detected.
 */
_LIT(KSraClientPanicCat, "SRA-Client");

enum TSraClientPanic
/**
	Reasons why the SRA server might panic its clients.
 */
	{
	ESraClientBadDesc = 0,	///< Client provided a bad descriptor as an IPC argument.
	ESraNotSupported,		///< The service requested by the client is not supported.
	ESraOutofRange			///< The requested attribute is out of the defined attributes range.
	};

void PanicClient(const RMessagePtr2& aMessage, TSraClientPanic aReason);

enum TSraServerPanic
/**
	The server is panicked with these reasons when it detects an invalid state.
 */
	{
	EOutstandingSessions = 0,	///< Server object destroyed with outstanding sessions.
	};

void PanicServer(TSraServerPanic aPanic);
	
#include "sracommon.inl"

#endif /*SRACOMMON_H_*/
