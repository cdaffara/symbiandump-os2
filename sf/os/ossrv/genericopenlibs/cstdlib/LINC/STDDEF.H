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

#ifndef _STDDEF_H_
#define _STDDEF_H_

/** 
There seems to be a lot of nonsense about _need_wchar_t etc., but for
STDLIB we have cut the Gordian knot and done the simple thing.
*/
#ifndef __wchar_t_defined
#ifndef __GCCXML__
typedef unsigned short int wchar_t;
#endif
#endif

#ifndef _PTRDIFF_T_DEFINED
#define _PTRDIFF_T_DEFINED
typedef long		ptrdiff_t;
#endif //_PTRDIFF_T_DEFINED

#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
typedef unsigned int	size_t;
#endif //_SIZE_T_DEFINED

/** 
Offset of member MEMBER in a struct of type TYPE.  
*/
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#endif /* _STDDEF_H_ */
