// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : loadeddlls.cpp
// Part of     : libdl library
// Implements and maintains the list of opened dll.
// 
//

// INCLUDE FILES
#include "loadeddlls.h"
	
//-----------------------------------------------------------------------------
//Function Name : CLoadedDlls::CLoadedDlls()
//Description   : Constructor
//-----------------------------------------------------------------------------
CLoadedDlls::CLoadedDlls()
	{
	const TInt error = CreateLock();  
	//if error creating lock, panic.
	if ( KErrNone != error )
		{
		User::Panic(_L(""), error);
		}
	}
	
//-----------------------------------------------------------------------------
//Function Name : CLoadedDlls::~CLoadedDlls()
//Description   : Destructor
//-----------------------------------------------------------------------------
CLoadedDlls::~CLoadedDlls()
	{
	iLock.Close();
	iHandleList.Close();
	}

//-----------------------------------------------------------------------------
//Function Name : TInt CLoadedDlls::Add(const TDllEntry& aDllEntry)
//Description   : To add the dll to the list.
//Return Value  : KErrNone if added successfully else system wide error
//-----------------------------------------------------------------------------
TInt CLoadedDlls::Add(const TDllEntry& aDllEntry) 
	{
	return iHandleList.Append(aDllEntry);
	}

//-----------------------------------------------------------------------------
//Function Name : void CLoadedDlls::Remove(const TInt aIndex)
//Description   : To remove the dll from the list.
//-----------------------------------------------------------------------------
void CLoadedDlls::Remove(const TInt aIndex)
	{
	iHandleList.Remove(aIndex);
	iHandleList.GranularCompress();
	}
	
//-----------------------------------------------------------------------------
//Function Name : TDllEntry& CLoadedDlls::At(const TInt aIndex)
//Description   : Get DllEntry& at particular index. This function assumes that
//				  index is valid.
//Return Value  : TDllEntry& DllEntry corresponding to aIndex
//-----------------------------------------------------------------------------
TDllEntry& CLoadedDlls::At(const TInt aIndex)
	{
	return iHandleList[aIndex];
	}
	
//-----------------------------------------------------------------------------
//Function Name : TInt CLoadedDlls::Find(const void* aHandle) const
//Description   : Finds if dll with this handle exist or not
//Return Value  : Index in list if found else KErrNotFound
//-----------------------------------------------------------------------------
TInt CLoadedDlls::Find(const void* aHandle) const
	{
	TDllEntry tempDllEntry((TInt) aHandle);
	return iHandleList.Find(tempDllEntry, TIdentityRelation<TDllEntry>(TDllEntry::CompareByHandle));
	}

//-----------------------------------------------------------------------------
//Function Name : TInt CLoadedDlls::Find(const RLibrary& aLibrary) const
//Description   : Finds if dll with this library i.e. same path exist or not
//Return Value  : Index in list if found else KErrNotFound
//-----------------------------------------------------------------------------
TInt CLoadedDlls::Find(const RLibrary& aLibrary) const
	{
	TDllEntry tempDllEntry(aLibrary);
	return iHandleList.Find(tempDllEntry,TIdentityRelation<TDllEntry>(TDllEntry::CompareByPath));
	}

//-----------------------------------------------------------------------------
//Function Name : TBool TDllEntry::CompareByHandle(const TDllEntry& aEntry) 
//Description   : Comparator function, compares iLibrary.Handle() of two
//				  TDllEntry.
//Return Value  : ETrue if Handle of both TDllEntry's RLibrary's matches
//				  else EFalse.
//-----------------------------------------------------------------------------
TBool TDllEntry::CompareByHandle(const TDllEntry& aEntry1, const TDllEntry& aEntry2)
	{
	if (aEntry1.iLibrary.Handle() == aEntry2.iLibrary.Handle())
		{
		return ETrue;
		}
	return EFalse;
	}
	
//-----------------------------------------------------------------------------
//Function Name : TBool TDllEntry::CompareByPath(const TDllEntry& aEntry)
//Description   : Comparator function, compares path corresponding to two
//				  TDllEntry.
//Return Value  : ETrue if file name of both TDllEntry's RLibrary's matches
//				  else EFalse.
//-----------------------------------------------------------------------------
TBool TDllEntry::CompareByPath(const TDllEntry& aEntry1, const TDllEntry& aEntry2)
	{
	if (aEntry1.iLibrary.FileName().Compare(aEntry2.iLibrary.FileName()))
		{
		return EFalse;
		}
	return ETrue;
	}		
		
//-----------------------------------------------------------------------------
//Function Name : TDllEntry::TDllEntry(const RLibrary aLibrary, const TInt aFlag)
//Description   : constructor. to initialise library by RLibrary object
//-----------------------------------------------------------------------------
TDllEntry::TDllEntry(const RLibrary& aLibrary, const TInt aFlag) : iLibrary(aLibrary), iFlags(aFlag), iRefCount(1)
	{
	}
