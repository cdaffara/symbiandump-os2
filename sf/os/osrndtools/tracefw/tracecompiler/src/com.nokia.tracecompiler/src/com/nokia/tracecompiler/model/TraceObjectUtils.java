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
* Utility functions for checking properties of trace objects
*
*/
package com.nokia.tracecompiler.model;

import java.util.Comparator;

/**
 * Utility functions for checking properties of trace objects.
 * 
 */
public class TraceObjectUtils {

	/**
	 * Compares a trace to trace ID
	 */
	static Comparator<Object> traceToIDComparator = new Comparator<Object>() {

		/**
		 * Compares a trace to trace name
		 * 
		 * @param t1
		 *            the trace
		 * @param t2
		 *            the trace name
		 * @return the comparison result
		 */
		public int compare(Object t1, Object t2) {
			int n1 = ((Trace) t1).getID();
			int n2 = (Integer) t2;
			return n1 > n2 ? 1 : n1 < n2 ? -1 : 0;
		}

	};

	/**
	 * Compares a trace to trace name
	 */
	static Comparator<Object> traceToNameComparator = new Comparator<Object>() {

		/**
		 * Compares a trace to trace name
		 * 
		 * @param t1
		 *            the trace
		 * @param t2
		 *            the trace name
		 * @return the comparison result
		 */
		public int compare(Object t1, Object t2) {
			String n1 = ((Trace) t1).getName();
			String n2 = (String) t2;
			if (n1 == null) {
				n1 = ""; //$NON-NLS-1$
			}
			if (n2 == null) {
				n2 = ""; //$NON-NLS-1$
			}
			return n1.compareTo(n2);
		}

	};

	/**
	 * Compares a trace to trace text
	 */
	static Comparator<Object> traceToTextComparator = new Comparator<Object>() {

		/**
		 * Compares a trace to trace text
		 * 
		 * @param t1
		 *            the trace
		 * @param t2
		 *            the trace text
		 * @return the comparison result
		 */
		public int compare(Object t1, Object t2) {
			String n1 = ((Trace) t1).getTrace();
			String n2 = (String) t2;
			if (n1 == null) {
				n1 = ""; //$NON-NLS-1$
			}
			if (n2 == null) {
				n2 = ""; //$NON-NLS-1$
			}
			return n1.compareTo(n2);
		}

	};

	/**
	 * Compares trace objects by ID
	 */
	static Comparator<TraceObject> traceObjectIDComparator = new Comparator<TraceObject>() {

		/**
		 * Compares ID's of trace objects
		 * 
		 * @param t1
		 *            trace 1
		 * @param t2
		 *            trace 2
		 * @return the comparison result
		 */
		public int compare(TraceObject t1, TraceObject t2) {
			int n1 = t1.getID();
			int n2 = t2.getID();
			return n1 > n2 ? 1 : n1 < n2 ? -1 : 0;
		}

	};

	/**
	 * Compares trace objects by name
	 */
	static Comparator<TraceObject> traceObjectNameComparator = new Comparator<TraceObject>() {

		/**
		 * Compares names of trace objects
		 * 
		 * @param t1
		 *            trace 1
		 * @param t2
		 *            trace 2
		 * @return the comparison result
		 */
		public int compare(TraceObject t1, TraceObject t2) {
			String n1 = t1.getName();
			String n2 = t2.getName();
			if (n1 == null) {
				n1 = ""; //$NON-NLS-1$
			}
			if (n2 == null) {
				n2 = ""; //$NON-NLS-1$
			}
			return n1.compareTo(n2);
		}

	};

	/**
	 * Prevents construction
	 */
	private TraceObjectUtils() {
	}

	/**
	 * Checks if trace parameter name is already in use and changes if it is.
	 * 
	 * @param owner
	 *            the owner of the parameter
	 * @param name
	 *            the parameter name
	 * @return the modifier interface
	 */
	public static TraceObjectModifier modifyDuplicateParameterName(Trace owner,
			String name) {
		DuplicateParameterNameModifier modifier = new DuplicateParameterNameModifier(
				owner, name);
		modifier.processName();
		return modifier;
	}

	/**
	 * Gets the duplicate modifier from given text
	 * 
	 * @param text
	 *            the text
	 * @return the duplicate modifier
	 */
	public static String removeDuplicateModifier(String text) {
		String retval;
		String s = DuplicateValueModifier.getModifier(text);
		if (s != null) {
			retval = text.substring(s.length());
		} else {
			retval = text;
		}
		return retval;
	}

	/**
	 * Finds a property from a trace object. This returns an empty string if not
	 * found
	 * 
	 * @param object
	 *            the object
	 * @param name
	 *            the property name
	 * @return the property value
	 */
	public static String findProperty(TraceObject object, String name) {
		String retval = null;
		TraceObjectPropertyList propertyList = object
				.getExtension(TraceObjectPropertyList.class);
		if (propertyList != null) {
			TraceObjectProperty property = propertyList.getProperty(name);
			if (property != null) {
				retval = property.getValue();
			}
		}
		if (retval == null) {
			retval = ""; //$NON-NLS-1$
		}
		return retval;
	}

}
