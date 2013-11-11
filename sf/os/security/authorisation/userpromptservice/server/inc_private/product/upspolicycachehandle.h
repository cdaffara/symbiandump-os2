/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Class to manage CPolicyCache
*
*/


/**
 @file
 @internalComponent
 @released
*/
#ifndef UPSPOLICYCACHEMANAGER_H
#define UPSPOLICYCACHEMANAGER_H
#include <e32base.h>

class RFs;
namespace UserPromptService
{
class CPolicyCache;
class CPolicyCacheContainer;

NONSHARABLE_CLASS(RPolicyCacheCountedHandle)
	/**
	This class manages the CPolicyCache handle used by the UPS.

	Code can either access the database via the master (ie. this class) -> operator, or can use an
	instance of the RUpsDbHandleSlave which provides for notification that the handle is about to be destroyed.

	Typically the user will not need to explicitly OpenL the handle, but should ensure that Close is
	called before the class instance is deleted.

	In practise this means that anything using an asynchronous database operation (ie. CreateViewL)
	should use the slave class.
	*/
	{
public:
	RPolicyCacheCountedHandle(RFs &aFs);
	RPolicyCacheCountedHandle(RPolicyCacheCountedHandle &aPolicyCacheManager);
	RPolicyCacheCountedHandle &operator=(const RPolicyCacheCountedHandle &);

	virtual ~RPolicyCacheCountedHandle();

	void OpenL();
	void Release();

	TBool IsOpen() const;

	CPolicyCache *operator->();

	void NotifyOnRef1(TRequestStatus &aStatus);
	void CancelNotifyOnRef1();

private:
	RFs &iFs;
	CPolicyCacheContainer *iContainer;
	};

} // End of UserPromptService namespace

#endif
