/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
 @publishedAll
 @released
*/

#ifndef	_MACHTYPES_H_
#define	_MACHTYPES_H_

#define	_CLOCK_T_   signed int	/* clock() - EPOC32 TInt */
#define	_TIME_T_    signed int	/* time()  - EPOC32 TInt */

#define CLOCKS_PER_SEC 1		/* machine dependent */
#define CLK_TCK CLOCKS_PER_SEC

#endif	/* _MACHTYPES_H_ */


