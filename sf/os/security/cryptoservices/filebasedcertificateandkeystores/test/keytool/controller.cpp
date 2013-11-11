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


#include "controller.h"

CController::CController(MKeyToolView& aView) : iView(aView)
	{
	
	}

MKeyToolView& CController::GetView()
	{
	return iView;
	}
	
void CController::DisplayErrorL(const TDesC& aError, TInt aErrorCode, TBool aPageWise)
	{
	iView.DisplayErrorL(aError, aErrorCode, aPageWise);	
	}
	
	
void CController::DisplayErrorL(const TDesC& aError, TBool aPageWise)
	{
	iView.DisplayErrorL(aError, aPageWise);
	}
	
	
void CController::DisplayLocalisedMsgL(TInt aResourceId, TInt aErrorCode)
	{
	HBufC8* buf = NULL;
    buf = iResourceFile.AllocReadL(aResourceId); 
	TBuf<512> bPtr;

	bPtr.Copy(reinterpret_cast<const TUint16*>(buf->Ptr()), buf->Size()/2);
	iView.DisplayErrorL(bPtr, aErrorCode, 0);
	delete buf;	
	}
	
	
void CController::DisplayLocalisedMsgL(TInt aResourceId)
	{
	HBufC8* buf = NULL;
	buf = iResourceFile.AllocReadL(aResourceId); 
	TBuf<512> bPtr;

	// resource file having rls_string which is 16-bit Unicode string 
	// So we can cast it to get a 8 bit without changing the data.
	bPtr.Copy(reinterpret_cast<const TUint16*>(buf->Ptr()), buf->Size()/2);
	iView.DisplayErrorL(bPtr, 0);
	delete buf;
	}

void CController::DisplayKeyL(CCTKeyInfo& aKey, TBool aIsDetailed, TBool aPageWise)
	{
	iView.DisplayKeyInfoL(aKey, aIsDetailed, aPageWise);
	}
	

void CController::DisplayCertL(CCTCertInfo& aCert, CCertificate& aCertificate, RUidArray aApps, TBool aTrusted, TBool aIsDetailed, TBool aPageWise)
	{
	iView.DisplayCertL(aCert, aCertificate, aApps, aTrusted, aIsDetailed, aPageWise);
	}
	
CController::~CController()
	{
	iResourceFile.Close();
	iFsSession.Close();
	}


