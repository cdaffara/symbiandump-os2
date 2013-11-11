// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// UTrace Performance Tests cache flush routine caller
// containing the definition of the functions contained in te_cacheflush.s
//



/**
 @file te_perfcacheflush.h
 @internalTechnology
 @prototype
*/

#ifndef TE_UPTCACHEFLUSH_H
#define TE_UPTCACHEFLUSH_H

#ifndef __WINSCW__				
extern "C"
	{
	  unsigned int Te_Flush_The_Cache();
	}
#else 
	inline unsigned int Te_Flush_The_Cache(){return KErrNotSupported;};
#endif //cacheflush routine


#endif //TE_UPTCACHEFLUSH_H


