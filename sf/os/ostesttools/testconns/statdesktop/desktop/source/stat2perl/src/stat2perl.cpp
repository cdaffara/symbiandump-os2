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
#include <commandline.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#define IN_LINE_MAX 4096

///////////////////////////////////////////////
// AUTO-GENERATED TEXT - DO NOT MODIFY!!!
///////////////////////////////////////////////
#define S2P_VERSION "4.0"
///////////////////////////////////////////////

DWORD nFileLength = 0;
char *lpBuffer = NULL;
char szConnection[3] = {0};
char szPlatform[20] = {0};
char szInputName[MAX_PATH + 1] = {0};
char szOutputName[MAX_PATH + 1] = {0};

void cleanup(char *message, UINT errorcode = 0);
bool ReadFileIntoArray(const char *file);
bool ProcessFileContents(const char *file);
void ConvertBackslash(char *szCommand);
void WriteHeader(HANDLE outfile);
void WriteTrailer(HANDLE outfile);
void SkipOverComment(DWORD *counter);

int main(int argc, char* argv[])
{
	printf("\n------------------------------\n"
			"STAT2PERL Script Converter %s\n"
			"------------------------------\n"
			"Copyright Symbian Ltd 2002.\n"
			"---------------------------", S2P_VERSION);

	CommandLine oCmd;
	if (oCmd.GetNumberOfArgs() == 5)
	{
		strcpy(szConnection, oCmd.GetProgramArgument(1));
		strcpy(szPlatform, oCmd.GetProgramArgument(2));
		strcpy(szInputName, oCmd.GetProgramArgument(3));
		strcpy(szOutputName, oCmd.GetProgramArgument(4));

		if (ReadFileIntoArray(szInputName))
		{
			if (ProcessFileContents(szOutputName))
			{
				printf("\n\nFile converted successfully.\n");
				return 1;
			}
		}
		else
			printf("\nERROR: Could not read [%s] into memory.\n", szInputName);
	}
	else
		printf("\n\nERROR: Incorrect Number of arguments.\nUsage: stat2perl <connection> <platform> <input file> <output file>\n   Eg: stat2perl 3 COM1 file.txt file.pl\n");

	return -1;
}


bool ReadFileIntoArray(const char *file)
{
	HANDLE infile;
	DWORD bytes = 0;
	struct stat buf;

	if (stat(file, &buf) == 0)
	{
		nFileLength = buf.st_size;

		if ((infile = CreateFile(file,
							   GENERIC_READ,
							   0, 
							   NULL, 
							   OPEN_EXISTING,
							   FILE_ATTRIBUTE_NORMAL,
							   0))
				!= INVALID_HANDLE_VALUE)
		{
			lpBuffer = new char [nFileLength + 1];
			if (lpBuffer)
			{
				if (ReadFile(infile, lpBuffer, nFileLength, &bytes, NULL) && bytes == nFileLength)
				{
					CloseHandle(infile);
					return true;
				}
				else
					printf("\nERROR: Failed to read entire file into memory.");

			}
			else
				printf("\nERROR: Could not allocate memory to read file.");

			CloseHandle(infile);
		}
		else
			printf("\nERROR: Could not open [%s] to read.", file);
	}
	else
		printf("\nERROR: [%s] does not exist.", file);

	return false;
}

bool ProcessFileContents(const char *file)
{
	HANDLE outfile;
	DWORD bytes = 0;

	// open our output file
	if ((outfile = CreateFile(file,
						   GENERIC_WRITE,
						   0, 
						   NULL, 
						   CREATE_ALWAYS,
						   FILE_ATTRIBUTE_NORMAL,
						   0))
			!= INVALID_HANDLE_VALUE)
	{
		bool bCommand = false;
		DWORD counter = 0;
		DWORD actualstart = 0;
		DWORD actualend = 0;

		WriteHeader(outfile);

		// find the beginning and end
		while(counter < nFileLength)
		{
			// check command format is correct
			if(lpBuffer[counter] == '<')
			{
				counter++;
				if(lpBuffer[counter] == 'B')
					actualstart = counter + 2;

				if(lpBuffer[counter] == 'E')
				{
					actualend = counter - 2;
					break;
				}
			}
			else if (lpBuffer[counter] == '/') // start of comment or command
			{	
				SkipOverComment(&counter);
			}

			counter++;
		}

		// only concerned with usable part of script
		counter = actualstart;
		nFileLength = actualend;

		// now parse it...
		while(counter < nFileLength)	//while not reached end of the file
		{
			// check command format is correct
			if(lpBuffer[counter] == '<')
			{
				bCommand = true;
			}
			else if (lpBuffer[counter] == '/') // start of comment or command
			{	
				SkipOverComment(&counter);
			}
			else if (bCommand) // start of comment or command
			{	
				char szCommand[2048] = {0};
				char szLine[IN_LINE_MAX + 1] = {0};
				char *ptr = NULL;
				int cmdLength = 0;

				// extract the command
				while (lpBuffer[counter + cmdLength] != '>')
					cmdLength++;
				strncpy(szLine, lpBuffer + counter, cmdLength);
				*(szLine + counter + cmdLength) = (char)0;

				switch(lpBuffer[counter]) 
				{
					case '#' :	// set image directory - format: '#image <location>
						if ((ptr = strchr(szLine, ' ')) && strncmp(szLine, "#image", 6) == 0)
						{
							(*ptr++) = (char)0;
							sprintf(szCommand, "set_screenshot_dir(\"%s\") or goto scripterror;", ptr);
						}
						break;
					case 'D' :		// retrieves information about connected device
					case 'd' :
						strcpy(szCommand, "get_device_info($hndl) or goto scripterror;");
						break;
					case 'K' :		//standard QWERTY Keypress command
					case 'k' :
						sprintf(szCommand, "basic_key_sequence($hndl, \"%s\") or goto scripterror;", szLine + 1);
						break;
					case 'L' :	//used to send non-standard key data over (ie - Menu button, Home button, PageDown button)
					case 'l' :
						sprintf(szCommand, "control_keystroke($hndl, \"%s\") or goto scripterror;", szLine + 1);
						break;
					case 'M' :	//used when combination keys are to be sent over
					case 'm' :
						sprintf(szCommand, "combination_keystroke($hndl, \"%s\") or goto scripterror;", szLine + 1);
						break;
					case 'H' :	// hold key down for a while
					case 'h' :
						if (ptr = strchr(szLine, ','))
						{
							(*ptr++) = (char)0;
							sprintf(szCommand, "hold_key($hndl, \"%s\", \"%s\") or goto scripterror;", szLine + 1, ptr);
						}
						break;
					case 'A' :	//Fire up application (second character is a number which opens up the app (e.g - <A1>))
					case 'a' :
						if (ptr = strchr(szLine, ','))
						{
							(*ptr++) = (char)0;
							sprintf(szCommand, "start_application($hndl, \"%s\", \"%s\") or goto scripterror;", szLine + 1, ptr);
						}
						else
							sprintf(szCommand, "start_application($hndl, \"%s\") or goto scripterror;", szLine + 1);
						break;
					case 'F' :	//Open file
					case 'f' :
						if (ptr = strchr(szLine, ','))
						{
							(*ptr++) = (char)0;
							sprintf(szCommand, "open_application($hndl, \"%s\", \"%s\") or goto scripterror;", szLine + 1, ptr);
						}
						break;
					case 'T' :	//used to transfer files from the PC to the EPOC device
					case 't' :
						if (ptr = strchr(szLine, ','))
						{
							(*ptr++) = (char)0;
							sprintf(szCommand, "copy_testfile_to_target($hndl, \"%s\", \"%s\") or goto scripterror;", szLine + 1, ptr);
						}
						break;
					case 'R' :	//used to retrieve files from the EPOC device
					case 'r' :
						if (ptr = strchr(szLine, ','))
						{
							(*ptr++) = (char)0;
							sprintf(szCommand, "copy_from_target($hndl, \"%s\", \"%s\") or goto scripterror;", szLine + 1, ptr);
						}
						break;
					case 'X' :
					case 'x' :
						if (ptr = strchr(szLine, ','))
						{
							(*ptr++) = (char)0;
							sprintf(szCommand, "move_from_target($hndl, \"%s\", \"%s\") or goto scripterror;", szLine + 1, ptr);
						}
						break;
					case 'S' :	// Used for screen capture 
					case 's' :
						if (cmdLength > 1)
							sprintf(szCommand, "get_screenshot($hndl, \"%s\") or goto scripterror;", szLine + 1);
						else
							strcpy(szCommand, "get_screenshot($hndl) or goto scripterror;");
						break;
					case 'I' :	//Icon coordinates
					case 'i' :
						if (ptr = strchr(szLine, ','))
						{
							(*ptr++) = (char)0;
							sprintf(szCommand, "touch_screen($hndl, %s, %s) or goto scripterror;", szLine + 1, ptr);
						}
						break;
					case 'C' :	//Close currently active application
					case 'c' :
						strcpy(szCommand, "close_application($hndl) or goto scripterror;");
						break;
					case '!' :	// Start ESHELL.EXE
						if (*(szLine + 1) != '!')
							strcpy(szCommand, "start_eshell($hndl) or goto scripterror;");
						else
							strcpy(szCommand, "stop_eshell($hndl) or goto scripterror;");
						break;
					case 'P' : //pause 
					case 'p' :
						sprintf(szCommand, "pause($hndl, %s) or goto scripterror;", szLine + 1);
						break;
					case 'U' :	// remove file
					case 'u' :
						sprintf(szCommand, "remove_file($hndl, \"%s\") or goto scripterror;", szLine + 1);
						break;
					case 'Y' :	// create folder
					case 'y' :
						sprintf(szCommand, "create_folder($hndl, \"%s\") or goto scripterror;", szLine + 1);
						break;
					case 'Z' :	// remove folder
					case 'z' :
						sprintf(szCommand, "remove_folder($hndl, \"%s\") or goto scripterror;", szLine + 1);
						break;
				} // switch

				// if command supported, write it to file
				if (*szCommand)
				{
					ConvertBackslash(szCommand);
					WriteFile(outfile, szCommand, strlen(szCommand), &bytes, NULL);
					WriteFile(outfile, "\r\n", 2, &bytes, NULL);
					(*szCommand) = (char)0;
				}

				counter += cmdLength;
				bCommand = false;
			}

			counter++;
		}

		WriteTrailer(outfile);
		CloseHandle(outfile);
	}
	else
	{
		printf("\nERROR: Could not open [%s] to write.", file);
		return false;
	}

	return true;
}

void SkipOverComment(DWORD *counter)
{
	int iInComment = 0;

	// beginning of comment
	if (lpBuffer[(*counter) + 1] == '*')
	{
		iInComment++;
		(*counter)+=2;
		while(iInComment && (*counter) < nFileLength)	//while not reached end of the file
		{
			if (lpBuffer[(*counter)] == '/')
			{
				// beginning of comment - support nested comments
				if (lpBuffer[(*counter) + 1] == '*')
				{
					iInComment++;
					(*counter)++;
				}

				// end of comment
				if (lpBuffer[(*counter) - 1] == '*')
				{
					iInComment--;
				}
			}

			(*counter)++;
		}
	}
}

void ConvertBackslash(char *szCommand)
{
	char szCopy[IN_LINE_MAX + 1] = {0};
	int iSrc = 0, iDest = 0;

	while(*(szCommand + iSrc))
	{
		// need to substitute the special chars
		if (*(szCommand + iSrc) == '\r')
		{
			*(szCopy + iDest) = '\\';
			iDest++;
			*(szCopy + iDest) = 'r';
			iDest++;
		}
		else if (*(szCommand + iSrc) == '\n')
		{
			*(szCopy + iDest) = '\\';
			iDest++;
			*(szCopy + iDest) = 'n';
			iDest++;
		}
		else if (*(szCommand + iSrc) == '\t')
		{
			*(szCopy + iDest) = '\\';
			iDest++;
			*(szCopy + iDest) = 't';
			iDest++;
		}
		else if (*(szCommand + iSrc) == '@')
		{
			*(szCopy + iDest) = '\\';
			iDest++;
			*(szCopy + iDest) = '@';
			iDest++;
		}
		else
		{
			// append a backslash
			if (*(szCommand + iSrc) == '\\')
			{
				*(szCopy + iDest) = '\\';
				iDest++;
			}

			// now copy the char
			*(szCopy + iDest) = *(szCommand + iSrc);
			iDest++;
		}

		// don't go too far
		if (iDest >= IN_LINE_MAX)
		{
			*(szCopy + iDest) = (char)0;
			break;
		}
		iSrc++;
	}

	strcpy(szCommand, szCopy);
}

void WriteHeader(HANDLE outfile)
{
	DWORD bytes = 0;
	char szLine[2048] = {0};

	WriteFile(outfile, "#########################################################################\r\n", 75, &bytes, NULL);
	WriteFile(outfile, "# STAT2PERL CONVERTED SCRIPT\r\n", 30, &bytes, NULL);
	WriteFile(outfile, "#\r\n", 3, &bytes, NULL);
	WriteFile(outfile, "# For STAT Perl Interface ", 26, &bytes, NULL);
	WriteFile(outfile, S2P_VERSION, strlen(S2P_VERSION), &bytes, NULL);
	WriteFile(outfile, " or higher\r\n#\r\n", 15, &bytes, NULL);
	WriteFile(outfile, "# Auto-Generated from script:\r\n", 31, &bytes, NULL);
	sprintf(szLine, "# %s\r\n#\r\n", szInputName);
	WriteFile(outfile, szLine, strlen(szLine), &bytes, NULL);
	WriteFile(outfile, "#########################################################################\r\n", 75, &bytes, NULL);
	WriteFile(outfile, "\r\n", 2, &bytes, NULL);
	WriteFile(outfile, "use Symbian::StatAPI30;  # must include this library\r\n\r\n", 56, &bytes, NULL);
	WriteFile(outfile, "my $result = 1;          # test result\r\n", 40, &bytes, NULL);
	WriteFile(outfile, "my $hndl;                # handle to connection\r\n\r\n", 51, &bytes, NULL);
	WriteFile(outfile, "# set everything up\r\n", 21, &bytes, NULL);
	sprintf(szLine, "my @arglist = (\"STAT.DLL\", %c, \"%s\");\r\n", szConnection[0], _strupr(szPlatform));
	WriteFile(outfile, szLine, strlen(szLine), &bytes, NULL);
	WriteFile(outfile, "initialise(@arglist) or goto scripterror;\r\n\r\n", 45, &bytes, NULL);
	WriteFile(outfile, "# connect to the board\r\n", 24, &bytes, NULL);
	WriteFile(outfile, "$hndl = connect_to_target();\r\n", 30, &bytes, NULL);
	WriteFile(outfile, "$hndl or goto scripterror;\r\n\r\n", 30, &bytes, NULL);
	WriteFile(outfile, "# send commands\r\n", 17, &bytes, NULL);
}


void WriteTrailer(HANDLE outfile)
{
	DWORD bytes = 0;

	WriteFile(outfile, "\r\n\r\n#########################################################################\r\n", 79, &bytes, NULL);
	WriteFile(outfile, "# test succeeded\r\n", 18, &bytes, NULL);
	WriteFile(outfile, "goto endscript;\r\n\r\n\r\n", 21, &bytes, NULL);
	WriteFile(outfile, "#########################################################################\r\n", 75, &bytes, NULL);
	WriteFile(outfile, "# fail the test\r\n", 17, &bytes, NULL);
	WriteFile(outfile, "scripterror:\r\n", 14, &bytes, NULL);
	WriteFile(outfile, "print \"\\nScript aborted due to error:\\n\" . get_last_error($hndl) . \"\\n\";\r\n", 74, &bytes, NULL);
	WriteFile(outfile, "$result = 0;\r\n\r\n\r\n", 18, &bytes, NULL);
	WriteFile(outfile, "#########################################################################\r\n", 75, &bytes, NULL);
	WriteFile(outfile, "# finish\r\n", 10, &bytes, NULL);
	WriteFile(outfile, "endscript:\r\n", 12, &bytes, NULL);
	WriteFile(outfile, "$hndl and disconnect_from_target($hndl) and print \"Disconnected OK\\n\";\r\n\r\n\r\n", 76, &bytes, NULL);
	WriteFile(outfile, "#########################################################################\r\n", 75, &bytes, NULL);
	WriteFile(outfile, "# return value (zero=fail, non-zero=success)\r\n", 46, &bytes, NULL);
	WriteFile(outfile, "exit($result);\r\n", 16, &bytes, NULL);
	WriteFile(outfile, "\r\n", 2, &bytes, NULL);
}
