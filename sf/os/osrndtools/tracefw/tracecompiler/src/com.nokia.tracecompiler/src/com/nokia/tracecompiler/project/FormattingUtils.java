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
* Utilities for trace formatting
*
*/
package com.nokia.tracecompiler.project;

//import java.util.*;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceGroup;
import com.nokia.tracecompiler.model.TraceModel;


/**
 * Utilities for trace formatting
 * 
 */
public final class FormattingUtils {

	/**
	 * Separator for prefix
	 */
	private static final String PREFIX_SEPARATOR = ": "; //$NON-NLS-1$

	/**
	 * Gets a group ID based on group name
	 * 
	 * @param model
	 *            the trace model
	 * @param name
	 *            the group name
	 * @return the group ID
	 * @throws TraceCompilerException
	 */
	public static int getGroupID(TraceModel model, String name)
			throws TraceCompilerException {

		// First check that is group one of the default groups
		int retval = GroupNames.getIdByName(name);
		// If group was not one of the default groups then get next group Id from model
		if (retval == 0) {
			retval = model.getNextGroupID();
			if (retval < GroupNames.USER_GROUP_ID_FIRST) {
				retval = GroupNames.USER_GROUP_ID_FIRST;
			}
			else if(retval > GroupNames.USER_GROUP_ID_LAST){
				String msg = "You have exceeded the number of Group IDs you have allocated."; //$NON-NLS-1$
				TraceCompilerEngineGlobals.getEvents().postErrorMessage(msg, null, true);
				throw new TraceCompilerException(
						TraceCompilerErrorCode.RUN_OUT_OF_GROUP_IDS);		
			}
		}
		return retval;
	}

	/**
	 * Gets the default component prefix
	 * 
	 * @param model
	 *            the model
	 * @return the prefix
	 */
	public static String getDefaultComponentPrefix(TraceModel model) {
		return model.getName() + PREFIX_SEPARATOR;
	}

	/**
	 * Gets the default component suffix
	 * 
	 * @param model
	 *            the model
	 * @return the suffix
	 */
	public static String getDefaultComponentSuffix(TraceModel model) {
		return ""; //$NON-NLS-1$
	}

	/**
	 * Gets the default group prefix
	 * 
	 * @param group
	 *            the group
	 * @return the prefix
	 */
	public static String getDefaultGroupPrefix(TraceGroup group) {
		String name = group.getName();
		if (name.startsWith(GroupNames.DEFAULT_GROUP_PREFIX)) {
			int start = GroupNames.DEFAULT_GROUP_PREFIX.length();
			// First character retains case, others are converted to lower case
			name = name.charAt(start) + name.substring(start + 1).toLowerCase();
		}
		name += PREFIX_SEPARATOR;
		return name;
	}

	/**
	 * Gets the default group suffix
	 * 
	 * @param group
	 *            the group
	 * @return the suffix
	 */
	public static String getDefaultGroupSuffix(TraceGroup group) {
		return ""; //$NON-NLS-1$
	}

}
