/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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



#if (!defined __T_ECOM_API_SERVER_H__)
#define __T_ECOM_ADDR_API_SERVER_H__

//	EPOC Includes
#include <test/testblockcontroller.h>
#include <test/testserver2.h>

//	Test Includes
#include "T_EComResolverParamsData.h"
#include "T_ImplementationInformationData.h"
#include "T_EComSessionData.h"

class CT_ECOMServer : public CTestServer2
	{
private:
	class CT_ECOMBlock : public CTestBlockController
		{
	public:
		inline CT_ECOMBlock();
		inline ~CT_ECOMBlock();

		inline CDataWrapper* CreateDataL( const TDesC& aData );
		};

public:
	inline CT_ECOMServer();
	inline ~CT_ECOMServer();

	static CT_ECOMServer* NewL();

	inline CTestBlockController*	CreateTestBlock();
	};

#include "T_EComServer.inl"

#endif /* __T_ECOM_API_SERVER_H__ */
