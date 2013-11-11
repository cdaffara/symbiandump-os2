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
* All trace objects may contain parameters
*
*/
package com.nokia.tracecompiler.model;

/**
 * Traces may contain parameters
 * 
 */
public class TraceParameter extends TraceObject {

	/**
	 * The type of this parameter
	 */
	private String type;

	/**
	 * Hexadecimal (%p), pointer type
	 */
	public static final String POINTER = "void*"; //$NON-NLS-1$
	
	/**
	 * Signed decimal (%d), 32-bit type
	 */
	public static final String SDEC32 = "int32"; //$NON-NLS-1$

	/**
	 * Hexadecimal (%x), 32-bit type
	 */
	public static final String HEX32 = "hex32"; //$NON-NLS-1$

	/**
	 * Unsigned decimal (%u), 32-bit type
	 */
	public static final String UDEC32 = "uint32"; //$NON-NLS-1$
	
	/**
	 * Octal (%o), 32-bit type
	 */
	public static final String OCT32 = "oct32"; //$NON-NLS-1$	

	/**
	 * Signed decimal (%hd), 16-bit type
	 */
	public static final String SDEC16 = "int16"; //$NON-NLS-1$

	/**
	 * Hexadecimal (%hx), 16-bit type
	 */
	public static final String HEX16 = "hex16"; //$NON-NLS-1$

	/**
	 * Unsigned decimal (%hu), 16-bit type
	 */
	public static final String UDEC16 = "uint16"; //$NON-NLS-1$
	
	/**
	 * Octal (%ho), 16-bit type
	 */
	public static final String OCT16 = "oct16"; //$NON-NLS-1$	

	/**
	 * Signed decimal (%hhd), 8-bit type
	 */
	public static final String SDEC8 = "int8"; //$NON-NLS-1$

	/**
	 * Hexadecimal (%hhx), 8-bit type
	 */
	public static final String HEX8 = "hex8"; //$NON-NLS-1$

	/**
	 * Unsigned decimal (%hhu), 8-bit type
	 */
	public static final String UDEC8 = "uint8"; //$NON-NLS-1$
	
	/**
	 * Octal (%hho), 8-bit type
	 */
	public static final String OCT8 = "oct8"; //$NON-NLS-1$

	/**
	 * Signed decimal (%ld), 64-bit type
	 */
	public static final String SDEC64 = "int64"; //$NON-NLS-1$

	/**
	 * Time, 64-bit type
	 */
	public static final String TIME = "time"; //$NON-NLS-1$	
	
	/**
	 * Hexadecimal (%lx), 64-bit type
	 */
	public static final String HEX64 = "hex64"; //$NON-NLS-1$

	/**
	 * Unsigned decimal (%lu), 64-bit type
	 */
	public static final String UDEC64 = "uint64"; //$NON-NLS-1$
	
	/**
	 * Octal (%lo), 64-bit type
	 */
	public static final String OCT64 = "oct64"; //$NON-NLS-1$	

	/**
	 * Ascii string (%s) type
	 */
	public static final String ASCII = "ascii"; //$NON-NLS-1$

	/**
	 * Unicode string (%S) type
	 */
	public static final String UNICODE = "unicode"; //$NON-NLS-1$

	/**
	 * Fixed floating point (%f), 64-bit
	 */
	public static final String FLOAT_FIX = "ffix"; //$NON-NLS-1$

	/**
	 * Exponent floating point (%e), 64-bit
	 */
	public static final String FLOAT_EXP = "fexp"; //$NON-NLS-1$

	/**
	 * Fixed / exponential floating point (%g), 64-bit
	 */
	public static final String FLOAT_OPT = "fopt"; //$NON-NLS-1$

	/**
	 * The trace this parameter belongs to
	 */
	private Trace owner;

	/**
	 * Constructor
	 * 
	 * @param owner
	 *            the object owning this parameter
	 */
	public TraceParameter(Trace owner) {
		setModel(owner.getModel());
		owner.addParameter(this);
		this.owner = owner;
	}

	/**
	 * Constructor, which inserts the parameter into specified index
	 * 
	 * @param owner
	 *            the object owning this parameter
	 * @param index
	 *            the parameter index
	 */
	TraceParameter(Trace owner, int index) {
		setModel(owner.getModel());
		owner.insertParameter(index, this);
		this.owner = owner;
	}

	/**
	 * Gets the trace this parameter is associated to.
	 * 
	 * @return the trace
	 */
	public Trace getTrace() {
		return owner;
	}

	/**
	 * Sets the parameter type. Generates propertyUpdated event to model
	 * listeners if type changes
	 * 
	 * @see TraceModelListener#propertyUpdated(TraceObject, int)
	 * @param type
	 *            the parameter type
	 * @throws TraceCompilerException 
	 */
	public void setType(String type) throws TraceCompilerException {
		if (!type.equals(this.type)) {
			removeTableReference();
			this.type = type;
			addTableReference();
			getModel().notifyPropertyUpdated(this, TraceModelListener.TYPE);
		}
	}

	/**
	 * Gets the parameter type
	 * 
	 * @return the parameter type
	 */
	public String getType() {
		return type;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObject#reset()
	 */
	@Override
	void reset() {
		removeTableReference();
		super.reset();
	}

	/**
	 * Adds a constant table reference
	 */
	private void addTableReference() {
		if (type != null) {
			TraceConstantTable table = getModel().findConstantTableByName(type);
			if (table != null) {
				table.addParameterReference(this);
			}
		}
	}

	/**
	 * Removes the constant table reference
	 */
	private void removeTableReference() {
		if (type != null) {
			TraceConstantTable table = getModel().findConstantTableByName(type);
			if (table != null) {
				table.removeParameterReference(this);
			}
		}
	}

}
