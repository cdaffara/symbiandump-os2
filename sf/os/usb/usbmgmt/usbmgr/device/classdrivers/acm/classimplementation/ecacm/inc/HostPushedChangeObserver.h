/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __HOSTPUSHEDCHANGEOBSERVER_H__
#define __HOSTPUSHEDCHANGEOBSERVER_H__

#include <e32std.h>
#include <d32comm.h>
 
class MHostPushedChangeObserver
/**
 * The CSY callback interface. This class provides methods that enable the ACM 
 * class to inform the CSY of changes pushed by the host.
 */
	{
public:
	virtual void HostConfigChange(const TCommConfigV01& aConfig) = 0;
	virtual void HostSignalChange(TBool aDtr, TBool aRts) = 0;
	};

#endif // __HOSTPUSHEDCHANGEOBSERVER_H__
