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
*
*/


#ifndef __CONTROLLER_H_
#define __CONTROLLER_H_

#include <e32base.h>
#include <barsc.h> 
#include <mctkeystore.h>
#include <x509cert.h>
#include <wtlscert.h>
#include <cctcertinfo.h>
#include <e32cons.h>

#include "keytool_view.h"


class CController : public CBase
	{
	public:
		void DisplayErrorL(const TDesC& aError, TInt aErrorCode, TBool aPageWise);	
		void DisplayErrorL(const TDesC& aError, TBool aPageWise);
		void DisplayLocalisedMsgL(TInt aResourceId);
		void DisplayLocalisedMsgL(TInt aResourceId, TInt aErrorCode);

		void DisplayKeyL(CCTKeyInfo& aKey, TBool aIsDetailed, TBool aPageWise);

		void DisplayCertL(CCTCertInfo& aCert, CCertificate& aCertificate, RUidArray aApps, TBool aTrusted, TBool aIsDetailed, TBool aPageWise);
		
		~CController();
		MKeyToolView& GetView();
		
	protected:
		CController(MKeyToolView& aView);

	protected:
		MKeyToolView&    iView;	
		RFs 			iFsSession;
		RResourceFile 	iResourceFile;			
	};
	
#endif
