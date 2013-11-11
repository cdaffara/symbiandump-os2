// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : abort_test.cpp
// 
//


#include <stdio.h>
#include <stdlib.h>

int main()
	{
	FILE * pFile;;
	pFile = fopen(NULL, "r");
	if (pFile == NULL)
		{
		printf("file opening error\n");
		abort();
		}
	return 0;
	}
	
	
	



