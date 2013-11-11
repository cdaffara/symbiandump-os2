// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef CENTRALREPOSITORYINTERNAL_H
#define CENTRALREPOSITORYINTERNAL_H

namespace NCentralRepositoryConstants
/** Namespace encapsulating the CentralRepository internal constants.
@internalAll
@released
*/
{
/** Initial buffer size for Find~ API. Initial buffer is used to cut on the number
of IPC calls required for each Find~ API call. If the number of settings found is 
less than or equal to KCentRepFindBufSize then all the settings can be retrieved with
a single IPC.
@internalAll
*/
const TInt KCentRepFindBufSize = 16;

} // namespace NCentralRepositoryConstants

#endif // __CENTRALREPOSITORYINTERNAL_H__
