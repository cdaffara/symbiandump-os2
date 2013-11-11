/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <ecom/ecom.h>
#include "agentinfo.h"
#include <caf/agent.h>
#include <caf/agentfactory.h>
#include <caf/agentinterface.h>

using namespace ContentAccess;

// Default buffer size of zero
const TInt KCafApparcBufferSize = 0;

CAgentInfo* CAgentInfo::NewLC(const CImplementationInformation& aImplInfo)
	{
	CAgentInfo* self = new (ELeave) CAgentInfo;
	CleanupStack::PushL(self);
	self->ConstructL(aImplInfo);
	return self;
	}

CAgentInfo::CAgentInfo()
	{
	}

CAgentInfo::~CAgentInfo()
	{
	iSupplierMimeTypes.ResetAndDestroy();
	iSupplierMimeTypes.Close();
	iConsumerMimeTypes.ResetAndDestroy();
	iConsumerMimeTypes.Close();
	
	// Delete the agent manager and agent factory, this may unload the agent DLL
	delete iAgentManager;
	delete iAgentFactory;
	}


void CAgentInfo::ConstructL(const CImplementationInformation& aImplInfo)
	{
	// Set up the agent member with the name and Uid of the agent
	iAgent.SetValue(aImplInfo.DisplayName(), aImplInfo.ImplementationUid());

	// get the name of the agents private directory
	TInt length = aImplInfo.OpaqueData().Length();
	if(length > KMaxSIDLength)
		{
		User::Leave(KErrCorrupt);
		}
	else
		{
		iPrivateDirectoryName.Copy(aImplInfo.OpaqueData().Left(KMaxSIDLength));
		}
			
	// Extract from the data field from the info.
	// The data field should be of the form:
	// "<bufferlength>|<supplier1>,<supplier2>,....,<suppliern>:<consumer1>,<consumer2>"
	// bufferlength is the desired length of the buffer passed from apparc to DoRecognize
	TPtrC8 data(aImplInfo.DataType());
	TPtrC8 supplier(KNullDesC8());
	TPtrC8 consumer(KNullDesC8());

	// Search for the "|" field to delimit buffersize and supplier mime types
	TInt index = data.LocateF('|');
	if(index < 0)
		{
		// No default buffersize, ie. corrupt
		User::Leave(KErrCorrupt);
		}
	else
		{
		TPtrC8 buffersize(data.Left(index));
		TLex8 bufferLex(buffersize);
		bufferLex.Val(iPreferredBufferSize);

		// Make sure a buffer size was actually specified before the | character
		if(iPreferredBufferSize == 0)
			{
			iPreferredBufferSize = KCafApparcBufferSize;
			}

		// Make sure '|' is not the last character
		if (index + 1 < data.Length())
			{
			data.Set(aImplInfo.DataType().Mid(index + 1));
			}
		else 
			{
			User::Leave(KErrCorrupt);
			}
		}

	
	// Search for the ":" field to delimit supplier and consumer
	index = data.LocateF(':');
	
	// If the colon was present, then set the pointers appropriately
	if (index >= 0)
		{
		// Set supplier pointer
		supplier.Set(data.Left(index));

		// Check that ':' is not the last character before setting consumer pointer
		if (index + 1 < data.Length())
			{
			consumer.Set(data.Mid(index + 1));
			}
		}
	else
		{
		User::Leave(KErrCorrupt);
		}

	// Now parse the supplier mime types
	ParseMimeTypesL(supplier, iSupplierMimeTypes);
	
	// Do the same for the consumer mime types
	ParseMimeTypesL(consumer, iConsumerMimeTypes);
	}

CAgentFactory& CAgentInfo::AgentFactoryL()
	{
	// Create the agent factory if it hasn't been done already
	if (!iAgentFactory)
		{
		iAgentFactory = CAgentFactory::NewL(iAgent.ImplementationUid());
		}
	return *iAgentFactory;
	}

CAgentManager& CAgentInfo::AgentManagerL()
	{
	// Create the agent manager
	if (!iAgentManager)
		{
		iAgentManager = AgentFactoryL().CreateManagerL();
		}
	return *iAgentManager;
	}	

TBool CAgentInfo::IsSupportedSupplier(const TDesC8& aSupplierMime) const
	{
	for (TInt i = 0; i < iSupplierMimeTypes.Count(); ++i)
		{
		if (aSupplierMime == *iSupplierMimeTypes[i])
			{
			return ETrue;
			}
		}
	return EFalse;
	}

TBool CAgentInfo::IsSupportedConsumer(const TDesC8& aConsumerMime) const
	{

	for (TInt i = 0; i < iConsumerMimeTypes.Count(); ++i)
		{
		if (aConsumerMime == *iConsumerMimeTypes[i])
			{
			return ETrue;
			}
		}
	return EFalse;
	}
		
const RPointerArray<HBufC8>& CAgentInfo::SupplierMimeTypes() const
	{
	return iSupplierMimeTypes;
	}

const RPointerArray<HBufC8>& CAgentInfo::ConsumerMimeTypes() const
	{
	return iConsumerMimeTypes;
	}

void CAgentInfo::AddToArrayL(const TDesC8& aElement, 
							 RPointerArray<HBufC8>& aArray)
	{
	// Don't bother adding empty elements
	if (aElement.Length())
		{
		HBufC8* newElem = aElement.AllocLC();
		TPtr8 lowerCasePtr = newElem->Des();
		lowerCasePtr.LowerCase();
		User::LeaveIfError(aArray.Append(newElem));
		CleanupStack::Pop(newElem);
		}
	}

void CAgentInfo::ParseMimeTypesL(const TDesC8& aBuf, 
								 RPointerArray<HBufC8>& aMimeTypes)
	{
	TPtrC8 ptr(aBuf);
	TInt pos = 0;
	while ((pos = ptr.LocateF(',')) >= 0)
		{
		// Take into account possibility of ,,
		if (pos > 0)
			{
			AddToArrayL(ptr.Left(pos), aMimeTypes);
			}

		// Now, move the pointer to the position after the ','. BUT if the
		// ',' is the last position, then we are done, so return (a bit
		// dirty, but makes things easier
		if (pos + 1 < ptr.Length())
			{
			ptr.Set(ptr.Mid(pos + 1));
			}
		else
			{
			// The ',' is the last character, so quit
			return;
			}
		}

	// Now extract the last mime type.
	AddToArrayL(ptr, aMimeTypes);
	}

TInt CAgentInfo::PreferredBufferSize() const
	{
	return iPreferredBufferSize;
	}

const TDesC& CAgentInfo::PrivateDirectoryName() const
	{
	return iPrivateDirectoryName;
	}
	
TAgent& CAgentInfo::Agent()
	{
	return iAgent;
	}
