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



#include <iostream>
using namespace std;

int main(void)
{
	bool flag = true;
	cout<<"Running WSD conformance testcase for standard library.\n";
	while(1)
	{
		if(flag)
			cout<<"\nPress 'e' or 'E' and Enter to exit: ";
		flag = !flag;
		int c = getchar();
		if (c == 'E' || c == 'e')
		{
			break;
		}
	}
	return 0;
}

// end of file

