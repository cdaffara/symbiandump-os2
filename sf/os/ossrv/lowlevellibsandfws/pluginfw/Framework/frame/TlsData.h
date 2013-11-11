// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __TLSDATA_H__
#define __TLSDATA_H__

/** 
@internalComponent
@file
Comments : definition of the thread local storage data structure,
			and the global access pointer.
*/
#include <ecom/ecom.h>

//Forward declarations
class CLoadManager;
class CGlobalData;

/**
CGlobalData class controls the life time of a single CEComSession instance and
single CLoadManager instance. It works in a close cooperation with REComSession class.
The life time of the CGlobalData singleton instance will be controlled  by the following
static and non-static methods of REComSession:
1) REComSession::OpenL() (static method).
   CGlobalData singleton instance, so the CEComSession instance and CLoadManager instance, 
   will be created during the first successfull call of REComSession::OpenL(). 
   Each next REComSession::OpenL() call will not create new CGlobalData instance, 
   it will just increment the reference count of the already created singleton.
2) REComSession::Close() (non-static method).
   It will not destroy the created CGlobalData singleton, it will just decrement its 
   reference count.
3) REcomSession::FinalClose() (static method).
   It will check the reference count of CGlobalData singleton and if it is less or equal to
   0, then CGlobalData singleton, so the CEComSession instance and CLoadManager instance, 
   will be destroyed.
@internalComponent
*/
NONSHARABLE_CLASS(CGlobalData) : public CBase
	{
public:	
	static CGlobalData* NewL();
	static CGlobalData* Instance();
	virtual ~CGlobalData();
	
	inline TInt IncRefCnt();
	inline TInt DecRefCnt();
	inline TInt RefCnt() const;
	
	inline REComSession& EComSession();
	inline CLoadManager& LoadManager();
	
private:	
	CGlobalData();
	void ConstructL();
	
private:	
	/** The overall count of successfull REComSession::OpenL() calls. */
	TInt iRefCount;
	/** The single REComSession instance */
	REComSession iEComSession;
	/** Handles the loading of the dlls and the deletion of objects */
	CLoadManager* iLoadManager;
	};

/**
Increments the reference count of CGlobalData object. 
It should be called for every successfull call of REComSession::OpenL(). 
@return Reference count value after the increment.
*/
inline TInt CGlobalData::IncRefCnt()
	{
	return ++iRefCount;
	}

/**
Decrements the CGlobalData object's reference count. 
It should be called from REComSession::Close(). 
@return Reference count value after the decrement.
*/	
inline TInt CGlobalData::DecRefCnt()
	{
	return --iRefCount;
	}
		
/**
It is used in REComSession::FinalClose().
@return Reference count value.
*/	
inline TInt CGlobalData::RefCnt() const
	{
	return iRefCount;
	}
	
/**
@return A reference to the controlled REComSession object.
*/
inline REComSession& CGlobalData::EComSession()
	{
	return iEComSession;
	}
	
/**
@return A reference to the controlled CLoadManager object.
*/
inline CLoadManager& CGlobalData::LoadManager()
	{
	return *iLoadManager;
	}

#endif //__TLSDATA_H__
