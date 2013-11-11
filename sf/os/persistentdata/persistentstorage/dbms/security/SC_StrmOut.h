// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TPDStreamPersister class
// 
//

#ifndef __SC_STRMOUT_H__
#define __SC_STRMOUT_H__

#include "SC_Policy.h"

//Forward declarations
class RWriteStream;

namespace DBSC
{

/**
TPDStreamPersister class is an implementation of MPolicyDomainPersister interface and is used
for storing security policies to a binary policy file. The class uses TPolicyDomainReader
class functionality for retrieving the policies of the controlled by TPolicyDomainReader
instance CPolicyDomain collection.
@see MPolicyDomainPersister
@see TPolicyDomainReader
@see CPolicyBase::RPolicyCollection
@see CPolicyDomain
@internalComponent
*/
class TPDStreamPersister : public MPolicyDomainPersister
	{
public:
	inline TPDStreamPersister(RWriteStream& aStream) :
		iStream(aStream)
		{
		}
	virtual void RunL(const TPolicyDomainReader& aPolicyDomainReader);

private:
	void ExternalizePolicyCollectionL(const CPolicyBase::RPolicyCollection& aPolicyCollection);

private:
	RWriteStream& iStream;

	};

} //end of - namespace DBSC

#endif//__SC_STRMOUT_H__
