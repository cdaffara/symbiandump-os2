/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "glibbackend.h"
#include <e32std.h>
#include <string.h>
#include <stdlib.h>

extern "C" EXPORT_C char *getProgPath(char *progName)
{
	TText16 iName16[KMaxFileName];
	char *progPath;
	int pathLength;
	RProcess iProcess;
	char replacePath[KMaxFileName];
	
	/* replace / with // */
	strcpy(replacePath,progName);
	char *temp = strchr(replacePath,'/');
	while (temp)
	{
		*temp = '\\';
		temp = strchr(replacePath,'/');
	} // end while
	
	//convert narrow chars to wide chars
	mbstowcs((wchar_t *)iName16, replacePath, strlen(replacePath)+1);
	
	//create TDesC to pass to RProcess.Create()
	TBuf16<KMaxFileName> iCmdName16(iName16);
	_LIT(KArg,"");
	
	// Check if Create if successful. If not successful
	// the executable by that name does not exits. Hence,
	// return NULL. 
	if(iProcess.Create(iCmdName16,KArg))
		progPath = NULL;
	else
	{
		// If successful, convert from TDesC to narrow chars and 
		// return.
		TFileName fileName = iProcess.FileName();
		iProcess.Kill(0);
		iProcess.Close();
		
		pathLength = fileName.Length();
		
		progPath = (char *)malloc(pathLength + 1);
		
		if(progPath == NULL)
			return NULL;
		
		for(int i=0;i < pathLength;i++)
		{
			progPath[i] = fileName[i];
		} // end for 
		
		progPath[pathLength] = '\0';
	
	} // end if
	return progPath;
}

