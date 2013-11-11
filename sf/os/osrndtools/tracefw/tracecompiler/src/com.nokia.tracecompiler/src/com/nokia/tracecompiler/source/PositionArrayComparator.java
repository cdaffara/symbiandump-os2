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
* Comparator for position arrays
*
*/
package com.nokia.tracecompiler.source;

import java.util.Comparator;

/**
 * Comparator for position arrays
 * 
 */
public final class PositionArrayComparator implements
		Comparator<SourceLocationBase> {

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.util.Comparator#compare(java.lang.Object, java.lang.Object)
	 */
	public int compare(SourceLocationBase arrayElement, SourceLocationBase key) {
		int retval;
		if (key.getOffset() < arrayElement.getOffset()) {
			// Start is inclusive
			retval = 1;
		} else if (key.getOffset() >= arrayElement.getOffset()
				+ arrayElement.getLength()) {
			// End is exclusive
			retval = -1;
		} else {
			retval = 0;
		}
		return retval;
	}

}