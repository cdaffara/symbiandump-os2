/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Default names for trace groups
 *
 */
package com.nokia.tracecompiler.project;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Vector;

import com.nokia.tracecompiler.TraceCompilerLogger;
import com.nokia.tracecompiler.TraceCompilerRootException;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.model.TraceCompilerException;

/**
 * Default names for trace groups
 * 
 */
public class GroupNames {

	public static Vector<GroupNameIdPair> DEFAULT_GROUPS;

	/**
	 * epoc32 directory
	 */
	private static final String EPOC32_DIRECTORY = "epoc32"; //$NON-NLS-1$	
	
	/**
	 * Start of user-defined groups
	 */
	public static int USER_GROUP_ID_FIRST = -1;

	/**
	 * end of user-defined groups
	 */
	public static int USER_GROUP_ID_LAST = -1;

	/**
	 * Constant to be used for EPOCROOT
	 */
	public static String EPOCROOT = "EPOCROOT"; //$NON-NLS-1$

	/**
	 * Constant to be used for the location of opensystemtrace_types.h
	 */
	public static String OST_TYPES = "epoc32" + File.separator + "include" + File.separator + "platform" + File.separator + "opensystemtrace_types.h"; //$NON-NLS-1$ //$NON-NLS-2$ //$NON-NLS-3$ //$NON-NLS-4$

	/**
	 * Prefix for all default groups
	 */
	public static String DEFAULT_GROUP_PREFIX = "TRACE_"; //$NON-NLS-1$

	/**
	 * TRACE_BORDER group name
	 */
	public static final String TRACE_BORDER = "TRACE_BORDER"; //$NON-NLS-1$

	/**
	 * TRACE_API group name
	 */
	public static final String TRACE_API = "TRACE_API"; //$NON-NLS-1$

	/**
	 * TRACE_DUMP group name
	 */
	public static final String TRACE_DUMP = "TRACE_DUMP"; //$NON-NLS-1$

	/**
	 * TRACE_DEBUG group name
	 */
	public static final String TRACE_DEBUG = "TRACE_DEBUG"; //$NON-NLS-1$

	/**
	 * TRACE_DETAILED group name
	 */
	public static final String TRACE_DETAILED = "TRACE_DETAILED"; //$NON-NLS-1$

	/**
	 * TRACE_INTERNALS group name
	 */
	public static final String TRACE_INTERNALS = "TRACE_INTERNALS"; //$NON-NLS-1$

	/**
	 * Calculates location of opensystemtrace_types.h Calls getGroupName Returns
	 * false if there have been any errors
	 * 
	 * @throws TraceCompilerRootException
	 */
	public static void initialiseGroupName() throws TraceCompilerRootException {

		String e32 = System.getenv(EPOCROOT);
		TraceCompilerLogger.printInfo(e32);
		if (e32 == null || (e32.length() == 0)) {
			String msg = Messages.getString("GroupNames.0"); //$NON-NLS-1$
			throw new TraceCompilerRootException(msg, null);
		}

		// need to check that e32 ends in a file separator
		if (!e32.endsWith(File.separator)) {
			TraceCompilerLogger.printInfo(Messages.getString("GroupNames.2")); //$NON-NLS-1$
			e32 += File.separator;
		}

		// Check that EPOCROOT is valid
		File epocRoot = new File(e32 + EPOC32_DIRECTORY);
		if (epocRoot.exists() == false) {
			throw new TraceCompilerRootException(Messages.getString("GroupNames.invalidEpocRoot"), null); //$NON-NLS-1$
		}
		
		// tests parsing the opensystemtrace_types.h for expected values
		String ostHeaderFilename = e32 + OST_TYPES;

		// add the predefined names / ids
		// this was changed from the previous mechanism to break array indexing
		// dependency
		// for linking group names to group ids
		DEFAULT_GROUPS = new Vector<GroupNameIdPair>();

		// The testing ranges are being hardcoded as the values are linked to
		// e32btrace.h
		// The TC would have to be redesigned to read in the values from this
		// file, which would impact performance

		DEFAULT_GROUPS.add(new GroupNameIdPair("TRACE_TESTING1", 254)); //$NON-NLS-1$
		TraceCompilerLogger.printInfo(Messages.getString("GroupNames.1")); //$NON-NLS-1$
		DEFAULT_GROUPS.add(new GroupNameIdPair("TRACE_TESTING2", 255)); //$NON-NLS-1$
		TraceCompilerLogger.printInfo(Messages.getString("GroupNames.3")); //$NON-NLS-1$
		try {
			getGroupName(ostHeaderFilename);
		} catch (Exception e) {
			throw new TraceCompilerRootException(
					Messages.getString("GroupNames.failedToProcessOstHeaderText"), e); //$NON-NLS-1$
		}
	}

	/**
	 * Reads opensystemtrace_types.h and extracts the GroupIds
	 * 
	 * @throws IOException
	 *             if fail to read the header file
	 * @throws TraceCompilerException
	 */

	private static void getGroupName(String headerFilename) throws Exception {
		// TODO this method is buggy as it could process group ids even if there
		// are commented out
		// should use regular expression or better parser.

		TraceCompilerLogger
				.printInfo(Messages.getString("GroupNames.8") + headerFilename); //$NON-NLS-1$

		BufferedReader in = new BufferedReader(new FileReader(headerFilename));
		try {
			String str;
			while ((str = in.readLine()) != null) {
				if (str.length() != 0) {
					if (((str.indexOf("EUserDefinedRangeFirst")) != -1) && (USER_GROUP_ID_FIRST == -1)) { //$NON-NLS-1$
						int val = getGroupIdFromString(str);
						if (val != -1) {
							// set user group id first
							USER_GROUP_ID_FIRST = val;
							TraceCompilerLogger.printInfo(Messages
									.getString("GroupNames.4") + val); //$NON-NLS-1$
						}
					}

					if (((str.indexOf("EUserDefinedRangeLast")) != -1) && (USER_GROUP_ID_LAST == -1)) { //$NON-NLS-1$
						int val = getGroupIdFromString(str);
						if (val != -1) {
							// set user group id last
							USER_GROUP_ID_LAST = val;
							TraceCompilerLogger.printInfo(Messages
									.getString("GroupNames.12") + val); //$NON-NLS-1$
						}
					}

					if ((str.indexOf("enum TGroupIdReserved")) != -1) { //$NON-NLS-1$
						// search for reserved
						while ((str.indexOf("};") < 1) && (str.indexOf("} ;") < 1)) { //$NON-NLS-1$//$NON-NLS-2$

							str = in.readLine();
							if (((str.indexOf("=")) != -1) && ((str.indexOf("*")) < 1)) { //$NON-NLS-1$ //$NON-NLS-2$
								String groupName = str.substring(0, (str
										.indexOf("="))); //$NON-NLS-1$
								groupName = groupName.trim();
								String substr = str
										.substring(
												(str.indexOf("=")) + 1, (str.indexOf(','))); //$NON-NLS-1$
								int groupId = -1;
								String trim = substr.trim();
								try {
									groupId = Integer.parseInt(trim);
								} catch (NumberFormatException e) {
									// not an Int so could be a renaming
									groupId = getIdByName(trim);
									if (groupId == 0) {
										TraceCompilerEngineGlobals
												.getEvents()
												.postErrorMessage(
														Messages.getString("GroupNames.canNotresolveGroupIdPrefix") + trim, null, true); //$NON-NLS-1$
										// not resolved to an existing name
										throw new TraceCompilerException(null,
												true);
									}
								}
								DEFAULT_GROUPS.add(new GroupNameIdPair(
										groupName, groupId));
								TraceCompilerLogger.printInfo(groupName
										+ " = " + groupId); //$NON-NLS-1$
							}
						}
					}
				}
			}
		} catch (Exception e) {
			TraceCompilerEngineGlobals.getEvents().postErrorMessage(
					e.getMessage(), null, true);
			throw e;
		} finally {
			in.close();
		}
	}

	/**
	 * Reads opensystemtrace_types.h and extracts the GroupIds value from a
	 * string containing GroupId name and value. 
	 * Returns GroupId value, returns -1 if not found or resolved in case of group renaming
	 */

	private static int getGroupIdFromString(String str) {
		int groupId = -1;
		if ((str.indexOf("=")) != -1) { //$NON-NLS-1$
			String substr = str.substring(
					(str.indexOf("=")) + 1, (str.indexOf(','))); //$NON-NLS-1$

			try {
				// try to read the id from the header file
				groupId = Integer.parseInt(substr.trim());
			} catch (NumberFormatException e) {
				// it's probably a rename, so get the id for the old name if
				// there is one
				groupId = getIdByName(substr.trim());
				if (groupId == 0) {
					// return -1 because we could not resolve it
					groupId = -1;
				}
			}
		}
		return groupId;
	}

	/**
	 * Searches all known GroupIds for name and then returns value. 
	 * Returns GroupId value, 0 if not found in DEFAULT_GROUPS
	 */

	public static int getIdByName(String name) {
		TraceCompilerLogger
				.printInfo(Messages.getString("GroupNames.23") + name); //$NON-NLS-1$
		for (Enumeration<GroupNameIdPair> e = GroupNames.DEFAULT_GROUPS
				.elements(); e.hasMoreElements();) {
			GroupNameIdPair gnidp = e.nextElement();
			if (name.equals(gnidp.Name())) {
				TraceCompilerLogger.printInfo(Messages
						.getString("GroupNames.24") + gnidp.Id()); //$NON-NLS-1$
				return gnidp.Id();
			}
		}
		// the caller should decide what to do with value 0
		return 0;
	}

}
