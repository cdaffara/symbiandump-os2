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
* Name        : loadeddlls.h
* Part of     : libdl library
* Header file for loadeddlls.cpp
* 
*
*/




#ifndef _LOADEDDLLS_H_
#define _LOADEDDLLS_H_

// INCLUDE FILES
#include <e32base.h>//class CBase
#include <f32image.h>//E32EmulExpSymInfoHdr, E32EpocExpSymInfoHdr


#if defined(__WINSCW__) || defined(__WINS__)
typedef E32EmulExpSymInfoHdr TE32ExpSymInfoHdr;
#else
typedef E32EpocExpSymInfoHdr TE32ExpSymInfoHdr;
#endif

//-----------------------------------------------------------------------
//Class name: TDllEntry
//Description: It represents the item of CLoadedDlls, contains the data item
//				like library object, reference count etc
//-----------------------------------------------------------------------
class TDllEntry 
	{
public:
	static	TBool CompareByHandle(const TDllEntry& aEntry1, const TDllEntry& aEntry2);
	static  TBool CompareByPath(const TDllEntry& aEntry1, const TDllEntry& aEntry2);
	TDllEntry() {}
	TDllEntry(const TInt aHandle) { iLibrary.SetHandle(aHandle); }
	TDllEntry(const RLibrary& aLibrary, const TInt aFlag = 0);

	RLibrary iLibrary;
	TE32ExpSymInfoHdr* iSymbolHeader;
	TInt iFlags;
	TInt iRefCount;
	};
	
//-----------------------------------------------------------------------
//Class name: CLoadedDlls
//Description: It represents the list of the opened dlls. It is the main 
//			 class to manipulate the data on the opened dlls.
//-----------------------------------------------------------------------
NONSHARABLE_CLASS(CLoadedDlls) : public CBase
	{
public:
	CLoadedDlls();
	~CLoadedDlls();
	TInt Find(const void* aHandle) const;
	TInt Find(const RLibrary& aLibrary) const;
 	TInt Add(const TDllEntry& aDllItem);
	void Remove(const TInt aIndex);
	TDllEntry& At(const TInt aIndex);
	inline TInt CreateLock(){	return iLock.CreateLocal();	}
	inline void Lock() { iLock.Wait();	}
	inline void UnLock() { iLock.Signal();	}
	inline TInt Count() const 	{ return iHandleList.Count();}	
private:
	RFastLock iLock;
	RArray<TDllEntry> iHandleList;
	};

#endif //_LOADEDDLLS_H_
