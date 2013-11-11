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



#ifndef BASSNDPRIORITYDECODER_H_

#define BASSNDPRIORITYDECODER_H_



#include <bassnd.h>





class TBaSoundPriorityDecoder : public TBaSoundPriorityBase

/**

Decode a restored TBaSystemSoundInfo::iPriority

@internalComponent

*/

	{

public:

	inline TBaSoundPriorityDecoder(TInt aVal);

	};





inline TBaSoundPriorityDecoder::TBaSoundPriorityDecoder(TInt aVal)

	{iPriority=aVal;}



#endif /*BASSNDPRIORITYDECODER_H_*/

