/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* View adapter
*
*/
package com.nokia.tracecompiler.engine;


/**
 * View adapter implements the view interface.
 * 
 */
public class ViewAdapter implements TraceCompilerEngineView {

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engi.TraceCompilerEngineView#getConfiguration()
	 */
	public TraceCompilerEngineConfiguration getConfiguration() {
		// This can be null, the configuration delegate checks it
		return null;
	}
}
