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
// TPDStreamLoader class
// 
//

#ifndef __SC_STRMIN_H__
#define __SC_STRMIN_H__

#include "SC_Policy.h"

namespace DBSC
{

/**
TPDStreamLoader class is an implementation of MPolicyDomainLoader interface and is used
for loading security policies from a binary policy file. The class uses TPolicyDomainBuilder
class functionality for adding loaded policies to the controlled by TPolicyDomainBuilder
instance CPolicyDomain collection.
@see MPolicyDomainLoader
@see TPolicyDomainBuilder
@see CPolicyBase::RPolicyCollection
@see CPolicyDomain
@internalComponent
*/
NONSHARABLE_CLASS(TPDStreamLoader) : public MPolicyDomainLoader
	{
public:
	inline TPDStreamLoader(RReadStream& aStream) :
		iStream(aStream)
		{
		}
	virtual void RunL(TPolicyDomainBuilder& aPolicyDomainBuilder);

private:
	void LoadPolicyCollectionL(CPolicyBase::RPolicyCollection& aPolicyColl);

private:
	RReadStream& iStream;
	};

} //end of - namespace DBSC

#endif//__SC_STRMIN_H__
