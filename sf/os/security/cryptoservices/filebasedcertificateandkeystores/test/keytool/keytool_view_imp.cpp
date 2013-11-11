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


#include "keytool_defs.h"
#include "keytool_view_imp.h"

#include <e32cons.h>
#include "keytool_utils.h"


/*static*/ CKeytoolConsoleView* CKeytoolConsoleView::NewLC(CConsoleBase& aConsole)
	{
	CKeytoolConsoleView* self = new (ELeave) CKeytoolConsoleView(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CKeytoolConsoleView::CKeytoolConsoleView(CConsoleBase& aConsole) : iConsole(aConsole)
	{	
	}
	
CKeytoolConsoleView::~CKeytoolConsoleView()
	{	
	}
	
void CKeytoolConsoleView::ConstructL()
	{
	
	}

void CKeytoolConsoleView::DisplayUsage()
	{	
	}
	
void CKeytoolConsoleView::BoilerPlate()
	{
	
	}
	
void CKeytoolConsoleView::DisplayKeyInfoL(CCTKeyInfo& aKey, TBool aIsDetailed, TBool aPageWise)
	{
	// Display the key infos
	KeyToolUtils::PrintInfoL(_L("\n"), aPageWise);
	KeyToolUtils::PrintKeyInfoL(aKey, aIsDetailed, aPageWise);
	}
	
 void CKeytoolConsoleView::DisplayErrorL(const TDesC& aError, TInt aErrorCode, TBool aPageWise)
	{
	KeyToolUtils::PrintInfoL(_L("\n"), aPageWise);
	KeyToolUtils::PrintInfoL(aError);
	KeyToolUtils::PrintInfoL(_L("\nError code: "));
	KeyToolUtils::WriteErrorL(aErrorCode);
	}
	 	
 void CKeytoolConsoleView::DisplayErrorL(const TDesC& aError, TBool aPageWise)
	{
	KeyToolUtils::PrintInfoL(_L("\n"), aPageWise);
	KeyToolUtils::PrintInfoL(aError);
	}
	
 void CKeytoolConsoleView::DisplayCertL(CCTCertInfo& aCert, CCertificate& aCertificate, RUidArray aApps, TBool aTrusted, TBool aIsDetailed, TBool aPageWise)
 	{
	// Display the cert infos
 	KeyToolUtils::PrintInfoL(_L("\n"), aPageWise);
	KeyToolUtils::PrintCertInfoL(aCert, aCertificate, aApps, aTrusted, aIsDetailed, aPageWise);
 	}


