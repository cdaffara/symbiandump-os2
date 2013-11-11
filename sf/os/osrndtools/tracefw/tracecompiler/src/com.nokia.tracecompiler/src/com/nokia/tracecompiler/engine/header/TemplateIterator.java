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
* Template iterator
*
*/
package com.nokia.tracecompiler.engine.header;

/**
 * Template iterator
 * 
 */
final class TemplateIterator {

	/**
	 * Iterator class
	 */
	private Class<? extends TemplateIteratorEntry> iteratorClass;

	/**
	 * Template to be run by the iterator
	 */
	private Object[] template;

	/**
	 * Constructor
	 * 
	 * @param iteratorClass
	 *            the iterator class
	 * @param template
	 *            the template
	 */
	TemplateIterator(Class<? extends TemplateIteratorEntry> iteratorClass,
			Object[] template) {
		this.iteratorClass = iteratorClass;
		this.template = template;
	}

	/**
	 * Gets the iterator class
	 * 
	 * @return the class
	 */
	final Class<? extends TemplateIteratorEntry> getIteratorClass() {
		return iteratorClass;
	}

	/**
	 * Gets the template
	 * 
	 * @return the template
	 */
	final Object[] getTemplate() {
		return template;
	}

}
