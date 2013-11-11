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

  
 #include <f32file.h>
 #include <e32des16.h>
 #include "bus-helper.h"


char getSystemConfDriveLetter()
{
	 RFs fs;
	 User::LeaveIfError(fs.Connect());
	 TFindFile findFile(fs);
	 
	 char *cpath = NULL;
	 char drive = ' ';

	 if (findFile.FindByDir(_L("system.conf"),_L("\\data\\dbus\\")) == KErrNone   )
	 {

		TBuf16<256> buf16(findFile.File());

		cpath = (char *)(buf16.PtrZ());
		drive =	cpath[0];
	 }


	return drive;
}
