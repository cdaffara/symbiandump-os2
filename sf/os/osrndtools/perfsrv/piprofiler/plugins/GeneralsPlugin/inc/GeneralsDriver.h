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


#ifndef __GENERALSSAMPLER_H__
#define __GENERALSSAMPLER_H__

/*
 * The user-interface to the sampling device driver sued by the profiling engine
 */
#include <piprofiler/PluginSampler.h>

#ifndef __KERNEL_MODE__
#include <utf.h>
#endif


/**
 * The user device driver class for controlling the plugin sampler.
 */

class RGeneralsSampler :  public RPluginSampler 
{	
	public:
		#ifndef __KERNEL_MODE__
		
		/** Open a channel to the sampling device **/
		inline TInt Open();
		
		#endif	// !__KERNEL_MODE__	
};
	
_LIT(KPluginSamplerName,"PIProfilerGeneralsSampler");

#ifndef __KERNEL_MODE__

inline TInt RGeneralsSampler::Open()
{
	return DoCreate(KPluginSamplerName,TVersion(1,0,1),KNullUnit,NULL,NULL);
}


#endif
#endif

