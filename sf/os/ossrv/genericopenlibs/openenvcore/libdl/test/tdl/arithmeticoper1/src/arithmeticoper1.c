/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : arithmeticoper1.c
* 
*
*/



extern int mult(int, int);
extern int subt(int x, int y);

/*few global symbols*/
int add(int x, int y)
	{
	return ((x+y) - subt(x,y) + subt(x,y));
	}

int square(int x)
	{
	return mult(x,x);
	}
