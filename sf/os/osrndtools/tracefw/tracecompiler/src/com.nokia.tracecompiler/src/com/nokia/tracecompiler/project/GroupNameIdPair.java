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
 * Class that include group name and id pair
 *
 */
package com.nokia.tracecompiler.project;

public class GroupNameIdPair {

	// class associates a group name with an integer id
	String name;
	int id;

	/**
	 * Conctructor
	 * 
	 * @param name
	 *            name of the group
	 * @param id
	 *            id of the group
	 */
	public GroupNameIdPair(String name, int id) {
		if (name == null) {
			this.name = ""; //$NON-NLS-1$
			this.id = -1; // TODO - what is appropriate value here ?
		} else {
			this.name = name;
			this.id = id;
		}
	}

	/**
	 * Get ID
	 * 
	 * @return id of the group
	 */
	public int Id() {
		return this.id;
	}

	/**
	 * Get name
	 * 
	 * @return name of the group
	 */
	public String Name() {
		return this.name;
	}
}
