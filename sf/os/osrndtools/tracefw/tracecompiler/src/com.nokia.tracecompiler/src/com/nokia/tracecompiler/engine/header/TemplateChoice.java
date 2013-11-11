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
 * If-check in template
 * 
 */
class TemplateChoice {

	/**
	 * Templates to be used if check is true
	 */
	private Object[] templateTrue;

	/**
	 * Templates to be used if check is false
	 */
	private Object[] templateFalse;

	/**
	 * Choice class
	 */
	private Class<? extends TemplateCheckBase> choiceClass;

	/**
	 * Constructors
	 * 
	 * @param choiceClass
	 *            the class which does the check
	 * @param templateTrue
	 *            the template to be used if check is true
	 * @param templateFalse
	 *            the template to be used if check is false
	 */
	TemplateChoice(Class<? extends TemplateCheckBase> choiceClass,
			Object[] templateTrue, Object[] templateFalse) {
		this.choiceClass = choiceClass;
		this.templateTrue = templateTrue;
		this.templateFalse = templateFalse;
	}

	/**
	 * Gets the choice class
	 * 
	 * @return the class
	 */
	Class<? extends TemplateCheckBase> getChoiceClass() {
		return choiceClass;
	}

	/**
	 * Gets the template if choice returns true
	 * 
	 * @return the template
	 */
	Object[] getTrueTemplate() {
		return templateTrue;
	}

	/**
	 * Gets the template if choice returns false
	 * 
	 * @return the template
	 */
	Object[] getFalseTemplate() {
		return templateFalse;
	}

}