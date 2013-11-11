/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* sisregistry - server classes
*
*/


/**
 @file 
 @test
 @internalComponent
*/
#ifndef __RTASERVER_H__
#define __RTASERVER_H__

#include <e32base.h>
#include "clientserver.h"
#include "shutdown.h"
#include <caf/caf.h>
#include "rightsobject.h"

namespace ReferenceTestAgent
	{
	const TUint32 KRtaSignature = 0x72746100; // ASCII code of 'r', 't' and 'a'
	const TUint8  KRightsDbVersion = 1;
	const TUint32 KRtaSigMask = 0xFFFFFF00;
	const TUint32 KMaxRightsObjectsInDbFile = 65535;

	class CRtaSession;
	class CRightsObject;

	class CRtaServer : public CPolicyServer
		{
	public:
		static CRtaServer* NewLC();
		void AddSession();
		void DropSession();
		void AddRightsL(CRightsObject* aRightsObject);
		void EvaluateIntentL(const TDesC8& aCid, ContentAccess::TIntent aIntent);
		void ExecuteIntentL(const TDesC8& aCid, ContentAccess::TIntent aIntent);
		void ClearAllRightsL();
		void DumpRightsDatabaseL(const TDesC& aFileName);	
		void DeleteRightsByKeyL(const TDesC8& aDeleteKey);
		void EraseDeleteKeyL(const TDesC8& aDeleteKey);
		void ReloadRightsDatabaseL(const TDesC& aFileName);	
		RFs& Fs();
		TInt CheckPermittedFileName(const TDesC& aFileName);
		
	private:
		CRtaServer();
		void ConstructL();
		~CRtaServer();
		CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

		void LoadRightsDatabaseL();
		void InternalizeRightsL(RReadStream& aStream, TUint8 aVersion, TUint32 aRightsCount);
		void SaveRightsDatabaseL();

	private:
		TInt iSessionCount;
		CShutdown iShutdown;
		RPointerArray <CRightsObject> iRights;
		RFs iFs;
		TFileName iRightsDatabaseFileName;
			
	private:
		// Server Policies
  		static const TUint iRangeCount = 2;
  		static const TInt iRanges[iRangeCount];
  		static const TUint8 iElementsIndex[iRangeCount];
  		static const CPolicyServer::TPolicyElement iPolicyElements[2];
  		static const CPolicyServer::TPolicy iPolicy;
  	
		};

inline RFs& CRtaServer::Fs()
		{
		return iFs;
		}
		
	} //namespace

#endif // __RTASERVER_H__
