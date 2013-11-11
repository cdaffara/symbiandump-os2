// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//


#include <e32std.h>
#include "te_headersteststep.h"
#include <xml/utils/xmlengxestrings.h>


//test S60_header_files
#include "libxml2_c14n.h"
#include "libxml2_chvalid.h"
#include "libxml2_dict.h"
#include "libxml2_encoding.h"
#include "libxml2_entities.h"
#include "libxml2_globals.h"
#include "libxml2_hash.h"
#include "libxml2_list.h"
#include "libxml2_modules.h"
#include "libxml2_parser.h"
#include "libxml2_parserinternals.h"
#include "libxml2_sax.h"
#include "libxml2_sax2.h"
#include "libxml2_schemasinternals.h"
#include "libxml2_threads.h"
#include "libxml2_tree.h"
#include "libxml2_uri.h"
#include "libxml2_valid.h"
#include "libxml2_xlink.h"
#include "libxml2_xmlerror.h"
#include "libxml2_xmlexports.h"
#include "libxml2_xmlio.h"
#include "libxml2_xmlmemory.h"
#include "libxml2_xmlschemasglobals.h"
#include "libxml2_xmlstring.h"
#include "libxml2_xmlversion.h"
#include "libxml2_xpath.h"
#include "libxml2_xpathinternals.h"
#include "libxml2_xpointer.h"
#include "xmlengconfig.h"
#include "xmlengtriodef.h"
#include "xmlengtrionan.h"
#include "xmlengxeconfig.h"
//libxml2 utils
#include "xmlengmem.h"
#include "xmlengutils.h"
#include "xmlengxestd.h"
#include "xmlengxestrings.h"
#include "xmlengxesys.h"

//test .inl files
//utils
#include "xmlengxestrings.h"


/**
 * A compiler test to ensure that our headerfiles will be included as the 
 *   customer(s) expect them to.  Since these header files are not used
 *   by our code, but only by our customer's, this may be the only way 
 *   to verify any changes to them.
 * 
 * SYSLIB-XML-UT-4002, DEF129353
 */ 
TVerdict CLibXmlHeadersTestStep::doTestStepL()
	{
	SetTestStepResult(EPass);
	
	INFO_PRINTF1(_L("Compiler Test -- SYSLIB-XML-UT-4002."));
	
	return TestStepResult();
	}

