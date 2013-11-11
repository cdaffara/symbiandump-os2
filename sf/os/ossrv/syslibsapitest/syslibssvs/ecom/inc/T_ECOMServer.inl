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



/*@{*/
_LIT(KDataImplInf,		"CImplementationInformation");
_LIT(KDataEComResParam,	"TEComResolverParams");
_LIT(KDataEComSession,	"REComSession");
/*@}*/

inline CT_ECOMServer::CT_ECOMBlock::CT_ECOMBlock()
    {
    }

inline CT_ECOMServer::CT_ECOMBlock::~CT_ECOMBlock()
    {
    }

inline CDataWrapper* CT_ECOMServer::CT_ECOMBlock::CreateDataL( const TDesC& aData )
	{
	CDataWrapper*	wrapper = NULL;
	if( KDataImplInf() == aData )
		{
		wrapper = CT_ImplementationInformationData::NewL();
		}
	else if( KDataEComResParam() == aData )
		{
		wrapper = CT_EComResolverParamsData::NewL();
		}
	else if( KDataEComSession() == aData )
		{
		wrapper = CT_EComSessionData::NewL();
		}
	return wrapper;
	}

inline CT_ECOMServer::CT_ECOMServer()
    {
    }
    
inline CT_ECOMServer::~CT_ECOMServer()
    {
    }

inline CTestBlockController* CT_ECOMServer::CreateTestBlock()
	{
	return new CT_ECOMBlock();
	}
