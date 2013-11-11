/*
* Copyright (c) 2001 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __STRINGTABLESUPPORT_H__
#define __STRINGTABLESUPPORT_H__

/**
@internalComponent
*/
#define _STLIT8(name,s) const static TStLitC8<sizeof(s)> name={sizeof(s)-1,s}

// A class that differs from TLitC only in that it does not have a
// dereference operator defined
//##ModelId=3B1E52AB0100
template <TInt S>
class TStLitC8
/**
@internalComponent
*/
	{
public:
	inline operator const TDesC8&() const;
	inline const TDesC8& operator()() const;
	inline operator const __TRefDesC8() const;
public:
	//##ModelId=3B1E52AB0120
	TUint iTypeLength;
	//##ModelId=3B1E52AB0117
	TText8 iBuf[__Align8(S)];
	};

template <TInt S>
inline const TDesC8& TStLitC8<S>::operator()() const
	{
	return *REINTERPRET_CAST(const TDesC8*,this);
	}

template <TInt S>
inline TStLitC8<S>::operator const TDesC8&() const
	{
	return *REINTERPRET_CAST(const TDesC8*,this);
	}

template <TInt S>
inline TStLitC8<S>::operator const __TRefDesC8() const
	{
	return *REINTERPRET_CAST(const TDesC8*,this);
	}

struct TStringTable;

#endif // __STRINGTABLESUPPORT_H__
