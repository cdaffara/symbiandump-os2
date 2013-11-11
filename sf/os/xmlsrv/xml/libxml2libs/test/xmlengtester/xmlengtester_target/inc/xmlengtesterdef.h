// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definitions file
//

#ifndef LIBXML2TESTER_DEF_H
#define LIBXML2TESTER_DEF_H



// serializer types
_LIT(XOP, "xop");
_LIT(GZIP, "gzip");
_LIT(DEFAULT, "default");
_LIT(INFOSET, "infoset");

// compare files utils
_LIT(XOP_SKIP, "xop-skip");
_LIT8(CONTENT_ID, "Content-ID:");
_LIT8(HREF_CID, "href=\"cid:");
_LIT8(GT_SIGN, ">");

// cid values
_LIT8( CID_1, "111111111@11111111111111111");
_LIT8( CID_2, "222222222@22222222222222222");
_LIT8( CID_3, "333333333@33333333333333333");

// multi serialize utils
_LIT8(Node1, "Reference");
_LIT8(Node2, "Signature");
_LIT8(Node3, "Envelope");

// inside control
_LIT(EMPTY_STRING, ""); 											// empty string
_LIT(SINGLE, "single");
_LIT(MULTI, "multi");
_LIT(ONE_PARA, "one-para");
_LIT(THREE_PARA, "three-para");
_LIT(TO_RFS, "to-rfs");
_LIT(FILE, "file");
_LIT(BUFFER, "buffer");
_LIT(NONE, "none");
// outside control

// - binary conatainer
_LIT(CREATE_BC, "create-bc");
_LIT(CONTENTS_BC, "contents-bc");
_LIT(SIZE_BC, "size-bc");
_LIT(NODETYPE_BC, "nodetype-bc");
_LIT(MOVE_BC, "move-bc");
_LIT(REMOVE_BC, "remove-bc");
_LIT(COPY_BC, "copy-bc");
_LIT(REPLACE_BC, "replace-bc");


// - serialization
_LIT(SINGLE_TO_FILE_ONE_PARA, "single-to-file-one-para");			// single node binary data, serialize to file
_LIT(MULTI_TO_FILE_ONE_PARA, "multi-to-file-one-para");				// multi node binary data, serialize to file

_LIT(SINGLE_TO_FILE_THREE_PARA, "single-to-file-three-para");		// single node binary data, serialize to file
_LIT(MULTI_TO_FILE_THREE_PARA, "multi-to-file-three-para");			// multi node binary data, serialize to file

_LIT(SINGLE_TO_RFS, "single-to-rfs");								// single node binary data, serialize to RFs
_LIT(MULTI_TO_RFS, "multi-to-rfs");									// multi node binary data, serialize to RFs

_LIT(SINGLE_TO_BUFFER, "single-to-buffer");							// single node binary data, serialize to buffer
_LIT(MULTI_TO_BUFFER, "multi-to-buffer");							// multi node binary data, serialize to buffer

_LIT(SINGLE_TO_STREAM, "single-to-stream");							// single node binary data, serialize to MOutputStream
_LIT(MULTI_TO_STREAM, "multi-to-stream");							// multi node binary data, serialize to MOutputStream



// errors
_LIT(KSourceFileError,"Source file error.");
_LIT(KContentTypeError, "Content Type Error");

#endif // LIBXML2TESTER_DEF_H

// end of file

