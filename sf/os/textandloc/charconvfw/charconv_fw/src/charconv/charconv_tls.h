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


#if !defined(__CHARCONV_TLS_H__)
#define __CHARCONV_TLS_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif


/** 
Thread local storage used by CCnvCharacterSetConverter

@internalTechnology

*/
class TTlsData
	{
public:
	static void CharacterSetConverterIsBeingCreatedL();
	static void CharacterSetConverterIsBeingDestroyed();
	static void SetCurrentCharacterSetConverter(const CCnvCharacterSetConverter* aCharacterSetConverter);
	IMPORT_C static const CCnvCharacterSetConverter* CurrentCharacterSetConverter();
public:
	inline TTlsData() :iReferenceCount(1), iCurrentCharacterSetConverter(NULL) {}
private:
	TInt iReferenceCount;
	const CCnvCharacterSetConverter* iCurrentCharacterSetConverter;
	};
	

#endif

