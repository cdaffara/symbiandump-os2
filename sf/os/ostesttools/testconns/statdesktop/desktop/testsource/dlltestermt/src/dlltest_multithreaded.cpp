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
#include "dlltest_worker.h"

int main(int argc, char* argv[])
{
	dlltest_worker oWorker;
	if (oWorker.Prepare())
	{
		int count = 0;
		while (count = oWorker.WorkPending())
		{
			printf("Waiting for %d threads...\r\n", count);
			Sleep(3000);
		}

		oWorker.Finish();
	}

	return 0;
}
