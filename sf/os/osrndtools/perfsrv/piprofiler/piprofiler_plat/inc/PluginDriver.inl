/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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


//
// LDD for thread time profiling
//

#include <piprofiler/ProfilerConfig.h>
#include <piprofiler/ProfilerVersion.h>


#include <kern_priv.h>
#include <sproperty.h>

#include <piprofiler/PluginSampler.h>


/*
 *
 *
 *	Class DPluginDriver implementation
 *
 *
 */
inline DPluginDriver::DPluginDriver() //: iSubsRequest(&HandleSubsComplete, this)
	{
    //iClientProcess = &Kern::CurrentProcess();
	}
inline DPluginDriver::~DPluginDriver()
	{
	
	}

/*
 *	Methods for controlling stream read option
 */
 
inline TInt DPluginDriver::ProcessStreamReadCancel()
{
	return KErrNone;
}

/*
 *	Mark traces active or inactive, this can be done
 *	only if sampling is not running
 */

inline TInt DPluginDriver::StopSampling()
{
	if(this->iState == ERunning)
	{
		this->iState = EStopping;
		return KErrNone;
	}
	else
	{
		return KErrGeneral;
	}
}



