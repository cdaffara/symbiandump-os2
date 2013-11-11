/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Kernel #endif flag check
*
*/
package com.nokia.tracecompiler.engine.header;

import com.nokia.tracecompiler.source.SymbianConstants;

/**
 * Check if __KERNEL_MODE__ flags needs to be added
 * 
 */
public final class CheckIsKernelModeFlagNeeded extends TemplateCheckBase {

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.header.TemplateCheckBase#check()
	 */
	@Override
	boolean check() {
		boolean retval;
		if (writer.getCurrentTraceFormatted().contains(
				SymbianConstants.CONST_TDESC16_REF)) {
			retval = true;
			writer.setKernelModeEndifNeeded(true);
		} else {
			retval = false;
			writer.setKernelModeEndifNeeded(false);
		}
		return retval;
	}
}