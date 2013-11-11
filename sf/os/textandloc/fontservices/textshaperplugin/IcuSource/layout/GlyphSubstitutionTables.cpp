/*
 *
 * (C) Copyright IBM Corp. 1998-2005 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphFilter.h"
#include "OpenTypeTables.h"
#include "Lookups.h"
#include "GlyphDefinitionTables.h"
#include "GlyphSubstitutionTables.h"
#include "GlyphSubstLookupProc.h"
#include "ScriptAndLanguage.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"

U_NAMESPACE_BEGIN

le_int32 GlyphSubstitutionTableHeader::process(LEGlyphStorage &glyphStorage, le_bool rightToLeft, LETag scriptTag, LETag languageTag,
                                           const GlyphDefinitionTableHeader *glyphDefinitionTableHeader,
                                           LEErrorCode &success,
                                           const LEGlyphFilter *filter, const LETag *featureOrder) const
{
    if (LE_FAILURE(success)) {
        return 0;
    }

    GlyphSubstitutionLookupProcessor processor(this, scriptTag, languageTag, filter, featureOrder);

    if (processor.isBogus()) {
        success = LE_MEMORY_ALLOCATION_ERROR;
    	return 0;
    }

    return processor.process(glyphStorage, NULL, rightToLeft,
        glyphDefinitionTableHeader, NULL, success);
}

U_NAMESPACE_END
