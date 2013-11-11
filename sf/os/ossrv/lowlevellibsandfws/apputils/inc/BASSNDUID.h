// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).

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



#ifndef BASSNDUID_H_

#define BASSNDUID_H_



/** 

This is the UID of the repository where the system sound data is stored.

The UID is published to UI platforms to allow them to use the key value in 

the CRepository::NotifyRequest() API to monitor for changes in the system

sound data.



@see CRepository

@see BaSystemSound::SystemSoundFile

@publishedPartner

@released

*/

const TUid KSystemSoundRepositoryUID = {0x100048AA};



#endif /*BASSNDUID_H_*/

