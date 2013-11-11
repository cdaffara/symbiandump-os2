// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implementation of serializer
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGSERIALIZER_H
#define XMLENGSERIALIZER_H

#include <e32base.h>
#include <xml/dom/xmlengserializationoptions.h>
#include <xml/dom/xmlengnode.h>

class MXmlEngOutputStream;
class RFs;

/** Controls the format of serialization */
enum TXmlEngSerializerType
	{
	/** Default serialization (XML) */
	ESerializerDefault,
	/** Serialization to MIME Multipart containing XOP */
	ESerializerXOP,
	/** Serialization to XOP Infoset */
	ESerializerXOPInfoset,
	/** Serialization to GZip */
	ESerializerGZip
	};

/** Controls the serialization format */
enum TXmlEngSerializationOutput
	{
	ESerializeToFile,
	ESerializeToBuffer,
	ESerializeToStream
	};

/**
Provides the serializer interface and provides common functionality to all
serializers.  Implements default serialization (plain XML).  Derived classes
implement serialization to other formats.

@see TXmlEngSerializerType
*/
class CXmlEngSerializer: public CBase
    {
	friend class CXmlEngSerializerXOP;
	friend class CXmlEngSerializerGZIP;

public:
    /**
	Creates a serializer of the given type.  Returns an instance of this
	class or a derived class.

    @param aType Serializer type    
    @return The serializer
	@leave - One of the system-wide error codes
    */
    IMPORT_C static CXmlEngSerializer* NewL(TXmlEngSerializerType aType = ESerializerDefault);
   
	/**
	Sets the output type to file and saves the file name for later
	serialization.
	@param aFileName The file name of the file to serialize
	@leave - One of the system-wide error codes
    */
    IMPORT_C void SetOutputL(const TDesC& aFileName);

	/**
	Sets the output type to buffer and saves the buffer for later serialization.
	@param aBuffer The buffer to serialize
    */
    IMPORT_C void SetOutput(RBuf8& aBuffer);

	/**
	Sets the output type to stream and saves the stream for later serialization.
	@param aBuffer The stream to serialize
    */
    IMPORT_C void SetOutput(MXmlEngOutputStream& aStream);

    /**
    Sets the serialization options
	@param aOptions The serialization options to set.  Ownership is not
	transferred and aOptions must stay in scope for the lifetime of the
	serializer.
    */
    IMPORT_C void SetSerializationOptions(TXmlEngSerializationOptions& aOptions);	

	/**
	Serializes a DOM tree to the buffer, file or stream set previously with
	SetOutputL() or SetOutput().

    @param aRoot The root node of the DOM tree to be serialized
    @return The number of bytes written
	@leave KXmlEngErrNoParameters No previous call to SetOutputL() or SetOutput().
	@leave KErrNotSupported Unsupported serialization type
	@leave KXmlEngErrWrongEncoding Encoding not understood
	@leave KXmlEngErrWrongUseofAPI Document of root node is NULL
    @leave KXmlEngErrNegativeOutputSize The data to be serialized has a negative size
    @leave - One of the system-wide error codes
    */
    IMPORT_C virtual TInt SerializeL(const TXmlEngNode aRoot = TXmlEngNode());

    /**
	Serializes a DOM tree to file.  Any filename previously set with
	SetOutputL(const TDesC&) is ignored.
    
    @param aFileName The file name to serialize to
    @param aRoot The root node of the DOM tree to be serialized     
    @param aOptions The serialization options	 	 
    @return The number of bytes written
	@leave KXmlEngErrWrongEncoding Encoding not understood
	@leave KXmlEngErrWrongUseofAPI Document of root node is NULL
    @leave KXmlEngErrNegativeOutputSize The data to be serialized has a negative size
	@leave - One of the system-wide error codes
    */
    IMPORT_C virtual TInt SerializeL(const TDesC& aFileName, 
			    const TXmlEngNode aRoot = TXmlEngNode(),
			    const TXmlEngSerializationOptions& aOptions = TXmlEngSerializationOptions());
                                        									  		
    /**
    Serializes a DOM tree to file.  Any filename previously set with
	SetOutputL(const TDesC&) is ignored. 

    @param aRFs File Server session
    @param aFileName The file name to serialize to
    @param aRoot The root node of the DOM tree to be serialized     
    @param aOptions The serialization options	 	 
    @return The number of bytes written
	@leave KXmlEngErrWrongEncoding Encoding not understood
	@leave KXmlEngErrWrongUseofAPI Document of root node is NULL
    @leave KXmlEngErrNegativeOutputSize The data to be serialized has a negative size
	@leave - One of the system-wide error codes
    */
    IMPORT_C virtual TInt SerializeL(RFs& aRFs, 
			const TDesC& aFileName, 
			const TXmlEngNode aRoot = TXmlEngNode(),
			const TXmlEngSerializationOptions& aOptions = TXmlEngSerializationOptions());
				
    /**
	Serializes a DOM tree to buffer.  Any buffer previously set with
	SetOutputL(RBuf8&) is ignored.  Any existing data in aBuffer is destroyed.
	This function allocates memory for the buffer and the caller must close the
	buffer when finished.
     
    @param aBuffer The buffer to serialize to
    @param aRoot The root node of DOM tree
    @param aOptions The serialization options
    @return The number of bytes written
	@leave KXmlEngErrWrongEncoding Encoding not understood
	@leave KXmlEngErrWrongUseofAPI Document of root node is NULL
    @leave KXmlEngErrNegativeOutputSize The data to be serialized has a negative size
	@leave - One of the system-wide error codes
    */
    IMPORT_C virtual TInt SerializeL(RBuf8& aBuffer, 
                            const TXmlEngNode aRoot = TXmlEngNode(),
                            const TXmlEngSerializationOptions& 
                            aOptions = TXmlEngSerializationOptions());
                            
    /** Destructor */
    virtual ~CXmlEngSerializer();

private:
    /** Default constructor */
    CXmlEngSerializer();
    
   /** Second phase constructor. */
    void ConstructL();
    
private:
	HBufC*                           iOutFileName;
	RBuf8*                           iBuffer;
	MXmlEngOutputStream*		     iOutputStream;
	TXmlEngSerializationOptions*     iSerializationOptions;
	TXmlEngSerializationOutput       iSerializationOutput;
    };

#endif /* XMLENGSERIALIZER_H */

