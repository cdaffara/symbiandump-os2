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




#ifndef CSTATReturnCodes_H
#define CSTATReturnCodes_H

#define GENERAL_FAILURE						0
#define EXPIRED_VERSION						1
#define TIME_DELAY_INIT_FAILURE				2
#define DLL_INIT_FAILURE					3
#define OPEN_SCRIPT_FILE_FAILURE			4
#define INTERPRET_COMMANDS_FAILURE			5
#define COMMANDDECODER_INIT_FAILURE			6
#define DATACONVERTER_INIT_FAILURE			7
#define DIRMANAGER_INIT_FAILURE				8
#define RETCODES_INIT_FAILURE				9
#define COMLIB_INIT_FAILURE					10
#define COPYING								11
#define REFDIR_FOUND						12
#define ITS_OK								13
#define TIME_DELAY_INIT_SUCCESS				14
#define ALMOST								15
#define ERROR_REGISTRY						16
#define LOG_DIR_FAILURE						17
#define LOG_DIR_FOUND						18
#define LOG_DIR_DUPLICATE_FOUND				19
#define LOG_DIR_CREATE_REG_FAILURE			20
#define LOG_DIR_CREATE_FAILURE				21
#define LOG_DIR_INFORM						22
#define LOG_DIR_REG_FAILURE					23
#define LOG_FILE_FAILURE					24
#define LOG_FILE_OK							25
#define OPEN_SCRIPT_FILE_OK					26
#define OPEN_SCRIPT_FILE_COMPLETE_FAILURE	27
#define CLEANUP_FAILURE						28
#define END_COMMAND_FAILURE					29
#define END_SCRIPT							30
#define BEGIN_COMMAND_FAILURE				31
#define CONNECT_FIRST						32
#define COM_MEM_FAILURE						33
#define CONNECTED							34
#define INVALID_HASH_COMMAND				35
#define NEW_IMAGE_DIRECTORY					36
#define CURRENT_IMAGE_DIRECTORY_OK			37
#define NO_END_TO_COMMENT_COMMAND			38
#define COMMENT_COMMAND						39
#define INVALID_COMMENT_COMMAND				40
#define LOGFILE_INIT_FAILURE				41
#define PAUSE_TIME							42
#define DEVINFO_RECEIVED					43
#define DEVINFO_TRYING						44
#define DEVINFO_FAILURE						45
#define MACHINEINFO_RECEIVED				46
#define MACHINEINFO_TRYING					47
#define MACHINEINFO_FAILURE					48
#define KEYDATA_INFORM						49
#define KEYDATA_OK							50
#define KEYDATA_FAILURE						51
#define SYSKEYDATA_INFORM					52
#define SYSKEYDATA_OK						53
#define SYSKEYDATA_FAILURE					54
#define COMBOKEYDATA_INFORM					55
#define COMBOKEYDATA_OK						56
#define COMBOKEYDATA_FAILURE				57
#define COMBOKEYDATA_INVALID				58
#define APPSTART_INFORM						59
#define APPSTART_OK							60
#define APPSTART_FAILURE					61
#define FILEOPEN_INFORM						62
#define FILEOPEN_OK							63
#define FILEOPEN_FAILURE					64
#define FILETRANSFER_INFORM					65
#define FILETRANSFER_OK						66
#define FILETRANSFER_FAILURE				67
#define E_ENGINEOBJECTFAILURE				68
#define FILERETRIEVE_INFORM					69
#define FILERETRIEVE_OK						70
#define FILERETRIEVE_FAILURE				71
#define TOUCHSCREEN_INFORM					72
#define TOUCHSCREEN_OK						73
#define TOUCHSCREEN_FAILURE					74
#define APPCLOSE_INFORM						75
#define APPCLOSE_OK							76
#define APPCLOSE_FAILURE					77
#define ESHELL_START_INFORM					78
#define ESHELL_START_OK						79
#define ESHELL_START_FAILURE				80
#define ESHELL_STOP_INFORM					81
#define ESHELL_STOP_OK						82
#define ESHELL_STOP_FAILURE					83
#define SCREENSHOT_INFORM					84 
#define SCREENSHOT_OK						85
#define SCREENSHOT_FAILURE					86
#define SCREENSHOT_MOVETOPC_INFORM			87
#define SCREENSHOT_MOVETOPC_OK				88
#define SCREENSHOT_MOVETOPC_FAILURE			89
#define SCREENSHOT_CONVERSION_FAILURE		90
#define SCREENSHOT_CONVERSION_OK			91
#define IMAGEVERIFY_INIT_FAILURE			92
#define DELETEIMAGES_FAILURE				93
#define REFIMAGELOAD_OK						94
#define REFIMAGELOAD_FAILURE				95
#define REFIMAGELOAD_FAILURE_LIMIT			96
#define NEWIMAGELOAD_OK						97
#define NEWIMAGELOAD_FAILURE				98
#define VERIFICATION_PASS					99
#define VERIFICATION_FAILURE				100
#define START_VERIFICATION					101
#define CREATE_DIR_FAILURE					102
#define NO_BITMAPS							103
#define TOUCHSCREEN_INVALID_COMMAND			104
#define KEYHOLD_INFORM						105
#define KEYHOLD_OK							106
#define KEYHOLD_FAILURE						107
#define KEYHOLD_INVALID_COMMAND				108
#define KEYHOLD_STRING_INVALID				109
#define ERR_INVALID_IMAGE_DIRECTORY			110
#define ERR_FILE_COPY_FAILED				111
#define E_BADPLATFORM						112
#define E_BADFILESYSINTERFACE				113
#define E_BADREQUESTINTERFACE				114
#define E_ENGINECONNECTFAILURE				115
#define E_REQUESTOBJECTFAILURE				116
#define E_REQUESTINITFAILURE				117
#define E_NOREFERENCEIMAGES					118
#define FILE_DELETE_INFORM					119
#define FILE_DELETE_OK						120
#define FILE_DELETE_FAILURE					121
#define CREATE_FOLDER_INFORM				122
#define CREATE_FOLDER_OK					123
#define CREATE_FOLDER_FAILURE				124
#define REMOVE_FOLDER_INFORM				125
#define REMOVE_FOLDER_OK					126
#define REMOVE_FOLDER_FAILURE				127
#define E_NOT_SUPPORTED						128
#define E_USERCANCEL						129
#define E_OUTOFMEM							130
#define INVALID_COMMAND_FORMAT				131
#define E_MESSAGE_INIT_FAILURE				132
#define E_BADFORMAT							133
#define E_FILE_OPEN_READ_FAILED				134
#define E_NOIMAGEFOLDER						135
#define E_NOIMAGESTOCONVERT					136
#define E_BITMAPLOADFAILED					137
#define E_BITMAPSAVEFAILED					138
#define E_BADWRITE							139
#define E_FILE_OPEN_WRITE_FAILED			140
#define E_BADCOMPRESSION					141
#define E_SCREENSHOT_LEFTOVERFILES			142
#define CONVERTINGIMAGE						143
#define TO									144
#define FILERETRIEVE_COPY					145
#define E_INVALIDCMDSYNTAX					146
#define READINGIMAGE						147
#define SCREENSHOT_INTOMEMORY_OK			148
#define E_COMMANDRETRY						149
#define E_COMMANDFAILED						150
#define E_CONNECTIONFAILED					151
#define E_WRITEFAILED						152
#define E_READFAILED						153
#define NO_DATA_AT_PORT						154
#define E_TOOMUCHTIME						155
#define E_TOOMANYERRORS						156
#define E_BADID								157
#define E_BADFILENAME						158
#define E_BADNUMBERBYTES					159
#define E_OUTOFMEMNOTSUPPORTED				160
#define RETRIEVETO							161
#define E_INVALIDMESSAGETYPE				162
#define E_RESYNCCOMMAND						163
#define E_NOFILEDATA						164
#define E_SOCKETSTARTUP						165
#define E_SOCKETCREATE						166
#define E_SOCKETCONNECT						167
#define E_SOCKETBIND						168
#define E_SOCKETLISTEN						169
#define E_SOCKETHOSTNAME					170
#define E_SOCKETSEND						171
#define E_SOCKETRECV						172
#define E_PROCESSTERMINATED					173
#define E_SOCKETCLOSE						174
#define E_SOCKETSHUTDOWN					175
#define USE_EXISTING_IMAGES					176
#define E_NOCONNECTION						177


class CSTATReturnCodes
{
	public:
		CSTATReturnCodes();
		const char* GetRetMsg(const DWORD codenum);

	private:
		DWORD iMaxCodes;
};

#endif
