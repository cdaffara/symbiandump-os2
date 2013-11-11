/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/


#ifndef __OPENFONTS_PRIVATE_H__
#define __OPENFONTS_PRIVATE_H__

#include <hextree.h>
#include <e32hashtab.h>

class COpenFontShaperCacheEntry;

/* MSB is set to indicate a glyph code rather than a unicode value
   This is used for extracting the code value */
#define GLYPH_CODE(code) (code & 0x7fffffff)

/** COpenFontFile owned container for extra data. May be extended without affecting Binary Compatibility.

 @internalComponent
 */

class TOpenFontFileData
	{
public:
	CFontStore* iFontStore;			// pointer to the CFontStore instance that loaded the COpenFontFile
	};

/**
 Note: this class must be self-contained, since instances are added to an RHexTree,
 that is, it must be possible to destroy instances simply with RHeap::Free().
 @internalComponent
 */
class COpenFontGlyph
	{
public:
    static COpenFontGlyph* New(RHeap* aHeap, TInt aCode, TInt aGlyphIndex,
                               const TOpenFontCharMetrics& aMetrics, const TDesC8& aBitmap);
	inline static void Delete(RHeap* aHeap, COpenFontGlyph* aGlyph);
	inline const TUint8* Bitmap() const;

protected:
	COpenFontGlyph(TInt aCode, TInt aGlyphIndex, const TOpenFontCharMetrics& aMetrics)
		: iCode(aCode), iGlyphIndex(aGlyphIndex), iMetrics(aMetrics), iBitmapOffset(0) {}
	~COpenFontGlyph() {}
	void SetBitmap(const TAny* aBitmap);

public:
	const TInt iCode;						// the Unicode value of the character
	const TInt iGlyphIndex;					// the glyph index
	const TOpenFontCharMetrics iMetrics;	// the metrics

private:
	// an offset from this COpenFontGlyph object to a pointer to the run-length-encoded bitmap, 
	// calculated as (bitmapPointer)-(this)
	TInt iBitmapOffset;
	};

/**
 * Template for offset implementation of pointer array
 @internalComponent
 */
template<class T>
class ROffsetArray
    {
public:
    ROffsetArray();
    TInt Create(RHeap* aHeap, TInt aCount);
    void Close(RHeap* aHeap);
    TInt Count() const;
    T* operator[](TInt aIndex) const;
    void SetAt(TInt aIndex, T* aEntry);
private:
    TInt iOffset;
    TInt iCount;
    };


/**
 The per-font glyph cache. For now, just the members that used to be directly in
 COpenFont. Now it is a private class it can be elaborated to do character-to-glyph-index
 mapping when that is needed.

 @internalComponent
 */
class COpenFontGlyphCache
	{
public:
	COpenFontGlyphCache(RHeap* aHeap)
	  : iGlyphTreeById(aHeap),
		iGlyphTreeByUnicode(aHeap),
		iGlyphCacheMemory(0),
		iShaperCacheSentinel(NULL),
		iShapingInfoCacheMemory(0),
		iNumberOfShaperCacheEntries(0)
		{}
	TShapeHeader* SearchShaperCache(TInt aSessionHandle, TFontShapeFunctionParameters*& aParams);
	TShapeHeader* Insert(TInt aSessionHandle, RHeap* aHeap, CShaper::TInput aInput, TShapeHeader* aShapeHeader, TInt& aAddedBytes);
	TInt DeleteLeastRecentlyUsedEntry(RHeap* aHeap);
	TBool ShaperCacheIsEmpty();
	
public:
	RHexTree<COpenFontGlyph> iGlyphTreeById;				// a hex tree of glyphs indexed by glyph ID
	RHexTree<COpenFontGlyph> iGlyphTreeByUnicode;			// a hex tree of glyphs indexed by Unicode code point
	TInt iGlyphCacheMemory;									// memory used by the glyph tree in bytes
	COpenFontShaperCacheEntry* iShaperCacheSentinel;
	TInt iShapingInfoCacheMemory;
	TInt iNumberOfShaperCacheEntries;	
	};

/**
 @internalComponent
 */
class COpenFontSessionCacheEntry: public COpenFontGlyph
	{
public:
	static COpenFontSessionCacheEntry* New(RHeap* aHeap, const COpenFont* aFont, TInt aCode, TInt aGlyphIndex,
										   const TOpenFontCharMetrics& aMetrics, const TDesC8& aBitmap);
	inline static void Delete(RHeap* aHeap, COpenFontSessionCacheEntry* aEntry);
	inline const COpenFont* Font()const;

private:
	inline COpenFontSessionCacheEntry(const COpenFont* aFont, TInt aCode, TInt aGlyphIndex, const TOpenFontCharMetrics& aMetrics);
	~COpenFontSessionCacheEntry();

private: 
    TInt iFontOffset;          // offset of the font that contains this glyph, (not owned by this class!)    
	};

/**
 A glyph cache for a particular session.
 Because session caches are not shared they can shrink as well as grow.

 @internalComponent
 */
class COpenFontSessionCache
    {
	friend class COpenFontSessionCacheList;
public:
    static COpenFontSessionCache* NewL(RHeap* aHeap, TInt aSessionHandle, TInt aEntries);
    void Delete(RHeap* aHeap);
    
    TInt SessionHandle() { return iSessionHandle; }
    const COpenFontGlyph* Glyph(const COpenFont* aFont, TInt aCode, TInt& aIndex) const;
    void Insert(RHeap* aHeap, COpenFontSessionCacheEntry* aEntry, TInt aIndex);
    
private:
    COpenFontSessionCache(TInt aSessionHandle);
    ~COpenFontSessionCache();
private:
    TInt iSessionHandle;
    TInt64 iRandomSeed;
    ROffsetArray<COpenFontSessionCacheEntry> iEntryArray;
    };


class TFontTableGlyphOutlineCacheMemMonitor
    {
public:
    TFontTableGlyphOutlineCacheMemMonitor();
    void Inc(TInt aBytes);
    void Dec(TInt aBytes);
    TInt GetMemUsage();
private:
    TInt iBytes;
    };

struct TCacheUserInfo {
    TInt iSessionHandle;
    TInt iRefCount;
    TCacheUserInfo(TInt aSessionHandle, TInt aRefCount = 0): 
            iSessionHandle(aSessionHandle), iRefCount(aRefCount) { } 
};

class CFontTableCache;

class CFontTableCacheItem
    {
friend class CFontTableCache ;

public:
    CFontTableCacheItem(TUid &aFileUid, const TUint32 aTag, 
            TInt aOffset, TInt aLength);
    ~CFontTableCacheItem(); 

    TInt DecRefCount(TInt aSessionHandle);
    TInt IncRefCount(TInt aSessionHandle);

    TBool HasOutstandingRefCount();
    TInt FindUser(TInt aSessionHandle, TInt *id);
    
#ifdef _DEBUG
    void SetUser(RPointerArray<TCacheUserInfo> users)
        {
        TInt len = users.Count();
        for( TInt i = 0; i < len ; i++ )
            {
            iUsers.Append(users[i]);
            }
        }    
#endif
    
private:
    CFontTableCacheItem(const CFontTableCacheItem &); // disallow copy construction.
    CFontTableCacheItem& operator =(const CFontTableCacheItem &); // disallow assignment.
    
    TUid iFileUid; 
    TUint32 iTag; 
 

    TInt iOffset;
    TInt iLength; 
    RPointerArray<TCacheUserInfo> iUsers;
    }; 


class CFontTableCache 
    {
public:
    CFontTableCache(RHeap* aHeap, TFontTableGlyphOutlineCacheMemMonitor& aMon);
    ~CFontTableCache();
    TInt Append(TUid aFileUid, TUint32 aTag, 
            TAny*& aContent, TInt aLength);
    TInt Find(TUid aFileUid, TUint32 aTag, TAny*& aContent, TInt& aLength, TInt* id);
    TInt IncRefCount(TUid FileUid, TUint32 aTag, TInt aSessionHandle);
    TInt DecRefCount(TUid aFileUid, TUint32 aTag, TInt aSessionHandle);
    TBool HasOutstandingRefCount();
    TBool HasOutstandingRefCountWithUid(TUid aFileUid);  
    void CleanupCacheOnFbsSessionTermination(TInt aSessionHandle);
    void CleanupCacheOnOpenFontFileRemoval(COpenFontFile*);
#ifdef _DEBUG    
    void SetFontItem(RPointerArray<CFontTableCacheItem> cacheItems)
        {
        TInt len = cacheItems.Count();
        for(TInt i = 0; i < len; i++)
            {
            iCacheItems.Append(cacheItems[i]);
            }
        }
#endif   
    
private:
    CFontTableCache(const CFontTableCache &); // no copy construction.
    CFontTableCache& operator =(const CFontTableCache &); // no assignment.
#ifdef _DEBUG
    TInt GetCacheState(const char *func);
#endif
    
    TFontTableGlyphOutlineCacheMemMonitor &iCacheMemMon;
    RHeap *iHeap;
    RPointerArray<CFontTableCacheItem> iCacheItems;
    };


class TUnhintedOutlineCache;

class TUnhintedOutlineId 
    {
public:
    TUnhintedOutlineId(TUid aFileUid, TInt aFaceIndex, TUint aId);
    TUid iFileUid;
    TInt iFaceIndex;
    TUint iId;
    };

class COutlineCacheItem {
friend class CUnhintedOutlineCache;
friend class CHintedOutlineCache;
    
public:
    COutlineCacheItem(TInt aOffset, TInt aLength);
    ~COutlineCacheItem() ;

    TInt DecRefCount(TInt aSessionHandle);
    TInt IncRefCount(TInt aSessionHandle);
#ifdef _DEBUG
    void SetUser(RPointerArray<TCacheUserInfo> users)
        {
        TInt len = users.Count();
        for( TInt i = 0; i < len ; i++ )
            {
            iUsers.Append(users[i]);
            }
        }    
#endif
 
private:
    TInt FindUser(TInt aSessionHandle, TInt *id);
        

    TInt iOffset;
    TInt iLength;
    RPointerArray<TCacheUserInfo> iUsers;
};    

class CUnhintedOutlineCache {
public:
    CUnhintedOutlineCache(RHeap* aHeap, TFontTableGlyphOutlineCacheMemMonitor& aMon);
    TInt Find(const TUnhintedOutlineId &aOutlineId, TAny*& aData, TInt& aLength);
    TInt IncRefCount(const TUnhintedOutlineId& aOutlineId, TInt aSessionHandle);
    TInt DecRefCount(const TUnhintedOutlineId& aOutlineId, TInt aSessionHandle);
    TInt CacheUnhintedOutline(const TUnhintedOutlineId& aOutlineId, 
            TAny * const aData, const TInt aLength, TAny*& aOutline, TInt &aLen);
    TInt CleanupCacheOnOpenFontFileRemoval(COpenFontFile* aFontFile);
    TInt CleanupCacheOnFbsSessionTermination(TInt aSessionHandle);
#ifdef _DEBUG
    TInt GetCacheState(const char *func);
#endif
    ~CUnhintedOutlineCache();
    
#ifdef _DEBUG
    void SetUnHintedItem(TUnhintedOutlineId id, COutlineCacheItem* item)
        {
        iItemIdMap.Insert(id, item);
        }
#endif
    
private:
    // disallow assignment and copy-construction
    CUnhintedOutlineCache(const CUnhintedOutlineCache &); 
    CUnhintedOutlineCache& operator =(const CUnhintedOutlineCache &);
    
    static TUint32 IdHash(const TUnhintedOutlineId& aOutlineId);
    static TBool IdIdentity(const TUnhintedOutlineId& id1, const TUnhintedOutlineId& id2);
    
    TFontTableGlyphOutlineCacheMemMonitor& iCacheMemMon;
    RHeap* iHeap; 
    RHashMap<TUnhintedOutlineId, COutlineCacheItem*> iItemIdMap; // map the identity to an index in 'iCacheItems'.
};

class CHintedOutlineCache;

class THintedOutlineId 
    {
public:
    THintedOutlineId(COpenFont* aFont, TUint aId);
    COpenFont *iFont;
    TUint iId;
    };


class CHintedOutlineCache {
public:
    CHintedOutlineCache(RHeap* aHeap, TFontTableGlyphOutlineCacheMemMonitor& aMon);
    TInt Find(const THintedOutlineId& aOutlineId, TAny*& aData, TInt& aLength);
    TInt IncRefCount(const THintedOutlineId& aOutlineId, TInt aSessionHandle);
    TInt DecRefCount(const THintedOutlineId& aOutlineId, TInt aSessionHandle);
    TInt CacheHintedOutline(const THintedOutlineId& aOutlineId, 
            TAny* aData, TInt aLength, TAny*& aOutline, TInt& aLen);
    TInt CleanupCacheOnOpenFontRemoval(COpenFont* aFont);
    TInt CleanupCacheOnFbsSessionTermination(TInt aSessionHandle);
#ifdef _DEBUG
    TInt GetCacheState(const char *func);
    void SetHintedItem(THintedOutlineId id, COutlineCacheItem* item)
        {
        iItemIdMap.Insert(id, item);
        }
    RHashMap<THintedOutlineId, COutlineCacheItem*> GetHintedMap()
    {
    return iItemIdMap;
    }
#endif
    
private:
    // disallow assignment and copy-construction
    CHintedOutlineCache(const CHintedOutlineCache &); 
    CHintedOutlineCache& operator =(const CHintedOutlineCache &);
    static TUint32 IdHash(const THintedOutlineId& aOutlineId);
    static TBool IdIdentity(const THintedOutlineId& id1, const THintedOutlineId& id2);
        
    TFontTableGlyphOutlineCacheMemMonitor& iCacheMemMon;
    RHeap* iHeap; 
    RHashMap<THintedOutlineId, COutlineCacheItem*> iItemIdMap; // map the identity to an index in 'iCacheItems'.
    };


// inline functions

inline void COpenFontGlyph::Delete(RHeap* aHeap, COpenFontGlyph* aGlyph)
	{
	aHeap->Free(aGlyph);
	}

/**
@return A pointer to the bitmap data stored with this glyph, or NULL
if no bitmap has been stored with this glyph.
 */
inline const TUint8* COpenFontGlyph::Bitmap() const
	{
	if (iBitmapOffset)
		{
		return reinterpret_cast<const TUint8*>(PtrAdd(this, iBitmapOffset));
		}
	return NULL;
	}

inline COpenFontSessionCacheEntry::COpenFontSessionCacheEntry(const COpenFont* aFont, TInt aCode, TInt aGlyphIndex,const TOpenFontCharMetrics& aMetrics) :
    COpenFontGlyph(aCode, aGlyphIndex, aMetrics)
    {
    iFontOffset = aFont ? reinterpret_cast<TInt>(aFont) - reinterpret_cast<TInt>(this) : 0;
    }

inline void COpenFontSessionCacheEntry::Delete(RHeap* aHeap, COpenFontSessionCacheEntry* aEntry)
	{
	COpenFontGlyph::Delete(aHeap, aEntry);
	}

inline const COpenFont* COpenFontSessionCacheEntry::Font() const
    {
    if (iFontOffset)
        {
        return reinterpret_cast<const COpenFont*> (PtrAdd(this, iFontOffset));
        }
    return NULL;
    }

#endif	// __OPENFONTSPRIVATE_H__
