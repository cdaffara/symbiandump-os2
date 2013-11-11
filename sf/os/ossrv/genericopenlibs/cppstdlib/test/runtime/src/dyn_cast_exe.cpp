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
// Name        : dyn_cast_exe.cpp
// Part of     : standard c++ tests.
// 
//

#include "dlla.h"
#include "test_decls.h"
#include <stdio.h>
int main()
{
	printf("\n\nDynamic cast test...\n\n");
	DynamicCastTest *aPtr = DynamicCastTest ::Instance();
	printf("Ptr to object across dll boundary 0x%x\n",aPtr);
	

	if(dynamic_cast< DynamicCastTest_DerivedClass*>(aPtr)){
		printf("dynamic_cast successful\n");
		CPP_TESTS_ASSERT_ALLWAYS(1);
		return 0;
	}

	CPP_TESTS_ASSERT_ALLWAYS(0);
	return 1; // -> Even though the type of aPtr is DynamicCastTest*, dynamic_cast fails
}
