/*
 **********************************************************************
 *   Copyright (C) 2003, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 **********************************************************************
 */

#include "layout/LETypes.h"

#include "FontTableCache.h"

#define TABLE_CACHE_INIT 5
#define TABLE_CACHE_GROW 5

struct FontTableCacheEntry
{
    LETag tag;
    const void *table;
};

FontTableCache::FontTableCache()
    : fTableCacheCurr(0), fTableCacheSize(TABLE_CACHE_INIT)
{
    fTableCache = LE_NEW_ARRAY(FontTableCacheEntry, fTableCacheSize);

    if (fTableCache == NULL) {
        fTableCacheSize = 0;
        return;
    }

    for (int i = 0; i < fTableCacheSize; i += 1) {
        fTableCache[i].tag   = 0;
        fTableCache[i].table = NULL;
    }
}

FontTableCache::~FontTableCache()
{
    for (int i = fTableCacheCurr - 1; i >= 0; i -= 1) {
        LE_DELETE_ARRAY(fTableCache[i].table);

        fTableCache[i].tag   = 0;
        fTableCache[i].table = NULL;
    }

    fTableCacheCurr = 0;
    /* Nokia defect fix for memory leak - Jas: 23/06/2005 */
	LE_DELETE_ARRAY(fTableCache);
}

const void *FontTableCache::find(LETag tableTag) const
{
    for (int i = 0; i < fTableCacheCurr; i += 1) {
        if (fTableCache[i].tag == tableTag) {
            return fTableCache[i].table;
        }
    }

    const void *table = readFontTable(tableTag);

    if (table) {
        ((FontTableCache *) this)->add(tableTag, table);
    }

    return table;
}

void FontTableCache::add(LETag tableTag, const void *table)
{
    if (fTableCacheCurr >= fTableCacheSize) {
        le_int32 newSize = fTableCacheSize + TABLE_CACHE_GROW;

        FontTableCacheEntry *newTableCache =
            (FontTableCacheEntry *) LE_GROW_ARRAY(fTableCache, newSize);
        if (!newTableCache) {
            // could not grow the cache, so we won't cache the table.
            return;
        }
        fTableCache = newTableCache;

        for (le_int32 i = fTableCacheSize; i < newSize; i += 1) {
            fTableCache[i].tag   = 0;
            fTableCache[i].table = NULL;
        }

        fTableCacheSize = newSize;
    }

    fTableCache[fTableCacheCurr].tag   = tableTag;
    fTableCache[fTableCacheCurr].table = table;

    fTableCacheCurr += 1;
}
