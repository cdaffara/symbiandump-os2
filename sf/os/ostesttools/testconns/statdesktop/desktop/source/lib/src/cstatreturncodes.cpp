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




#include "stdafx.h"
#include "CSTATReturnCodes.h"

static char *messages[] = 
{
	"General Failure.",															// 0
	"This version of STAT has expired, please contact Symbian Ltd.",
	"Unable to initialise time delay for the chosen platform.",
	"DLL initialisation failure.",
	"Interpreting STAT commands...",
	"Unable to convert commands to 8-bit.",
	"Unable to initialise Command Decoder.",
	"Unable to initialise Data Format Converter.",
	"Unable to initialise Directory Manager.",
	"Unable to initialise Return Codes.",
	"Unable to initialise COM library.",										// 10
	"Copying...",
	"Old images currently exist on the local machine.\r\n\r\nSelect YES to replace the images, NO to use the existing images, or CANCEL to stop.",
	"The script completed successfully",
	"Time delay set ok.",
	"ALMOST THERE...",
	"Error accessing registry.",
	"Error with logfile directory.",
	"Logfile directory found.",
	"Logfile name found, but it is not a directory!  Please remove/rename.",
	"Error in logfile registry key value - directory cannot be created.",		// 20
	"Cannot create directory.",
	"Logfile directory is :",
	"Error with log file registry entry value.",
	"Error with log file creation.",
	"Log file created ok.",
	"Script opened ok.",
	"Unable to open script.",
	"Error with cleanup.",
	"NO END COMMAND FOUND IN SCRIPT FILE",
	"End of script file.",														// 30
	"Missing <B> or <E> command in script file",
	"Please Connect first!",
	"Unable to allocate memory for connection.",
	"Connected.",
	"Invalid # command.",
	"New image directory...",
	"Standard image directory created.",
	"Cannot find trailing comment syntax.",
	"Comment...",
	"Invalid '/' found.",														// 40
	"Unable to initialise log file class.",
	"Pause...",
	"Device information received.",
	"Trying for device info...",
	"Cannot retrieve device info.",
	"Machine information received.",
	"Trying for machine info...",
	"Cannot retrieve machine info.",
	"Key data...",
	"Key data sent.",															// 50
	"Key data sending failure.",
	"System key data...",
	"System key data sent.",
	"System key data sending failure.",
	"Combination key data...",
	"Combination key data sent.",
	"Combination key data sending failure.",
	"Invalid script combination key data.",
	"Application startup...",
	"Application data sent.",													// 60
	"Application data sending failure.",
	"File opening...",
	"File data sent.",
	"File data sending failure.",
	"File transfer...",
	"File transfer ok.",
	"File transfer failure.",
	"A Symbian Connect session could not be instantiated.",
	"File retrieval (move)...",
	"File retrieval ok.",														// 70
	"File retrieval failure.",
	"Touch screen...",
	"Touch screen ok.",
	"Touch screen failure.",
	"Attempting to close application...",
	"Closed application ok.",
	"Could not close application.",
	"Starting EShell...",
	"Started EShell ok.",
	"Starting EShell failure.",													// 80
	"Stopping EShell...",
	"Stopped EShell ok.",
	"Stopping EShell failure.",
	"Attempting to take screenshot...",
	"Screenshot taken ok.",
	"Screenshot failure.",
	"Attempting to move screenshot to PC...",
	"Screenshot moved.",
	"Screenshot moving failure.",
	"Screenshot conversion failure.",											// 90
	"Screenshot converted ok.",
	"Unable to initialise Image Verifier.",
	"Unable to delete reference images.",
	"Reference image loaded ok.",
	"Reference image loading failure.",
	"No more reference images left for verification.",
	"New image loaded OK",
	"New image loading failure.",
	"Image verification passed.",
	"Image verification failed.",												// 100
	"Attempting image verification...",
	"Unable to create new image directory.",
	"No reference images!  Verification disabled.",
	"Invalid touch screen command.",
	"Key hold command...",
	"Key hold ok.",
	"Key hold failure.",
	"Invalid keyhold command.",
	"Keyhold command string is too long.",
	"Invalid image directory.",													// 110
	"File copy operation failed.",
	"An invalid platform was specified.",
	"An external COM file system interface could not be obtained.",
	"An external COM request interface could not be obtained.",
	"A Symbian Connect session could not be initialised.",
	"A COM request object could not be instantiated.",
	"Remote device could not be initialised.  Check STAT support DLLs exist.",
	"No reference images could be located",
	"Deleting file...",
	"File delete operation ok.",												// 120
	"File delete operation failed.",
	"Creating folder...",
	"Create folder operation ok.",
	"Create folder operation failed.",
	"Removing folder...",
	"Remove folder operation ok.",
	"Remove folder operation failed.",
	"The requested operation is not supported",
	"Script cancelled at user's request",
	"Out of memory",															// 130
	"Invalid command format",
	"Messaging initialisation failure",
	"The file format is invalid",
	"Could not open Symbian OS file to read",
	"Image folder (PC side) does not exist for conversion",
	"No images to convert",
	"The bitmap could not be loaded prior to conversion",
	"The converted bitmap could not be saved",
	"The file write operation failed",
	"Could not open bitmap file to write",										// 140
	"Unknown data compression type",
	"A leftover .mbm file could not be removed prior to bitmap conversion",
	"Converting bitmap image from:",
	"To:",
	"File retrieval (copy)...",
	"Invalid command syntax",
	"Reading bitmap image into memory...",
	"Screenshot read into memory successfully.",
	"Command could not be sent...Retrying...",
	"Retries exhausted.  Command failed.",										// 150
	"A connection could not be established",
	"Write to port failed",
	"Read from port failed",
	"No data read from port",
	"Maximum time limit exceeded on single command",
	"Maximum error limit exceeded on single command",
	"Command ID of this type was not expected",
	"Bad file name or file does not exist",
	"Incorrect number of bytes read or written",
	"Insufficient memory or connection type not supported",						// 160
	"Retrieving file to:",
	"Invalid command type",
	"Communication needs to resynchronise",
	"No data to save to file",
	"Windows Socket Server could not be started",
	"Socket could not be created",
	"Socket connection failed",
	"Socket could not be bound to this port",
	"Socket could not listen on this port",
	"Host name could not be obtained",											// 170
	"Socket send failed",
	"Socket receive failed",
	"Script task terminated at user's request",
	"Socket could not be closed",
	"Windows Server shutdown failed",
	"Using existing images",
	"No connection specified",
	NULL
};

CSTATReturnCodes::CSTATReturnCodes()
: iMaxCodes(0)
{
	// determine the maximum error code possible
	while(messages[iMaxCodes])
		iMaxCodes++;
}

const char*
CSTATReturnCodes::GetRetMsg(const DWORD codenum)
{
	static CString retval;
	if (codenum < iMaxCodes)
		retval = messages[(int)codenum];
	else
		retval = _T("No error text associated with this code");

	return retval.operator LPCTSTR( );
}
