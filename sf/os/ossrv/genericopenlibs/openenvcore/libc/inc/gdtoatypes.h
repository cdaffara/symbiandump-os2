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
* Description:  Contains the structure and the macro declarations here, to be used 
* by various functions of gdtoa  
*/

#ifndef GDTOATYPES_H_
#define GDTOATYPES_H_

#define Kmax 15

#ifndef Omit_Private_Memory
#ifndef PRIVATE_MEM
#define PRIVATE_MEM 2304
#endif
#define PRIVATE_mem ((PRIVATE_MEM+sizeof(double)-1)/sizeof(double))
#endif

#ifndef ULong
typedef unsigned long ULong;
#endif

struct
Bigint {
	struct Bigint *next;
	int k, maxwds, sign, wds;
	ULong x[1];
};

typedef struct Bigint Bigint;

#endif /* GDTOATYPES_H_ */
