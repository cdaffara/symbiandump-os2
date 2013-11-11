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
// Defines the configuration for the format of the log data. 
// 
//

/**
 @test
*/

#ifndef __DATAFORMAT_H__
#define __DATAFORMAT_H__

#include <e32std.h>

/**
	@internalAll
	@enum TLoggingStyle
	Comments : Indicates the logging style which should be used by CDataLogger
 */
enum TLoggingStyle {EText, EHtml, ECustom};

/**
	@internalAll
	@struct TLogFormat
	Configures the format of the DataLogger's
	documentatry output. (see the comments 
	for an example usage.
 */
typedef struct
	{
	const TDesC* iDocumentStart;	// <HTML><HEAD><TITLE>
	const TDesC* iContentStart;		// </TITLE><BODY>
	const TDesC* iCommentStart;		// <P>
	const TDesC* iCommentEnd;		// </P>
	const TDesC* iDocumentEnd;		// </BODY></HTML>
	} TLogFormat;

/**
	@internalAll

 */

class MLogOutput
	{
public:
/**
	@fn				OpenL() = 0
	Intended Usage	: Sets up the logging facility provided by the implementation of this
					interface.  If logging cannot be established for any reason then
					this function should leave.
	@leave  		Implementation dependant.
	@since			7.0
	@pre 			None
	@post			Nothing is left on the CleanupStack
 */
	
	virtual void OpenL() = 0;

/**
	@fn				Write(const TDesC& aOutput) = 0
	Intended Usage	: Logs the specified descriptor.
	@since			7.0
	@param			aOutput The descriptor to stream to the logging facility
	@pre 			OpenL has been called to set up logging
	@post			aOutput has been logged
 */
	
	virtual void Write(const TDesC& aOutput) = 0;

/**
	@fn				Close() = 0
	Intended Usage	: Called when all logging has finished to close down any log
					connections.
	@since			7.0
	@pre 			None
	@post			Logging facility is closed down and this object can be deleted.
 */
	
	virtual void Close() = 0;
	};

/**
	@internalAll
	@struct TLoggingInfo
	Specifies the logging configuration.
 */
typedef struct
	{
	/** The title to be used in the output logs */
	const TDesC*	iTitle;
	/** If logging messages should also be printed using RDebug */
	TBool			iUseRDebug;
	/** The output style of the logging (plain text, html etc) */
	TLoggingStyle	iStyle;
	/** If iStyle is set to ECustom then this will be used for outputting log comments */
	TLogFormat*		iLogFormat;
	/** The logging mechanism.  NULL will default to RFileLogger */
	MLogOutput*		iLogOutput;
	/** The reporting mechanism.  NULL will default to RFileLogger */
	MLogOutput*		iReportOutput;
	} TLoggingInfo;

#endif

