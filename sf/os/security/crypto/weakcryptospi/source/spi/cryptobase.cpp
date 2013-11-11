/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* crypto base API implementation
* crypto base API implementation
*
*/


/**
 @file
*/

#include <cryptospi/plugincharacteristics.h>
#include <cryptospi/cryptobaseapi.h>
#include <cryptospi/cryptoplugin.h>
#include <e32std.h>

using namespace CryptoSpi;

//
// CCryptoBase
//
CCryptoBase::CCryptoBase(MPlugin* aPlugin, TInt aLibHandle)
: iPlugin(aPlugin), iLibHandle(aLibHandle)
	{
	}

EXPORT_C void CCryptoBase::GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics)
	{
	iPlugin->GetCharacteristicsL(aPluginCharacteristics);	
	}

EXPORT_C const CExtendedCharacteristics* CCryptoBase::GetExtendedCharacteristicsL()
	{
	return iPlugin->GetExtendedCharacteristicsL();
	}

EXPORT_C void CCryptoBase::Reset()
	{
	iPlugin->Reset();	
	}

CCryptoBase::~CCryptoBase()
	{
	//Close the plugin
	iPlugin->Close();
	
	//Close the library
	RLibrary lib;
	lib.SetHandle(iLibHandle);
	lib.Close();
	}

