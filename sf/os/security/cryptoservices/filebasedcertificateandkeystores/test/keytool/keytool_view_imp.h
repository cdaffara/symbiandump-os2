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


#ifndef __KEYTOOL_VIEW_IMP_H_
#define __KEYTOOL_VIEW_IMP_H_

#include <e32base.h>
#include <mctkeystore.h>
#include <cctcertinfo.h>

#include "keytool_view.h"

class CKeytoolConsoleView : public CBase, public MKeyToolView
	{
public:
	static CKeytoolConsoleView* NewLC(CConsoleBase& aConsole);
	~CKeytoolConsoleView();
	void ConstructL();
		
public:  // From MKeyToolView
	 void DisplayUsage();
	 void BoilerPlate();
	 void DisplayKeyInfoL(CCTKeyInfo& aKey, TBool aIsDetailed, TBool aPageWise);
	 void DisplayErrorL(const TDesC& aError, TBool aPageWise);
	 void DisplayErrorL(const TDesC& aError, TInt aErrorCode, TBool aPageWise);
	 void DisplayCertL(CCTCertInfo& aCert, CCertificate& aCertificate, RUidArray aApps, TBool aTrusted, TBool aIsDetailed, TBool aPageWise);
	 	
protected:

	
private:
	CKeytoolConsoleView(CConsoleBase& aConsole);
	
private:
	CConsoleBase	& iConsole;
	};

#endif

