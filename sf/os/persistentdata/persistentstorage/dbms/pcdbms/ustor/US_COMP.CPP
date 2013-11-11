// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Store database compression code
// 
//

#include "US_STD.H"
#include <s32mem.h>
#include "U32STD_DBMS.H"

#ifdef __WINS__
#define __EXTRA_DEFLATE
#endif

// deflation constants
const TInt KDeflateMinLength=3;
const TInt KDeflateMaxLength=258;
const TInt KDeflateMaxDistance=4096;
const TInt KDeflateDistCodeBase=0x200;
// huffman coding/decoding
const TInt KHuffMaxCodeLength=25;
const TInt KHuffTerminate=1;
const TUint KBitsEmpty=0x80000000u;
const TUint KBitsInit=KBitsEmpty>>1;
const TUint KBitsFull=KBitsEmpty>>8;
const TUint KBitsEOF=KBitsEmpty>>9;
const TUint KBitsNext=0x80u;
// encoding storage
const TInt KDeflateMetaCodes=26;
// hashing
const TUint KDeflateHashMultiplier=0xAC4B9B19u;
const TInt KDeflateHashShift=24;

class Huffman
	{
public:
	static void EncodingL(TUint32* aEncoding,TInt aCodes);
	static void Decoding(TUint32* aDecoding,TInt aCodes,TInt aBase=0);
private:
	typedef TUint16 THuff;
	enum {KLeaf=0x8000};
	struct TNode
		{
		THuff iLeft;
		THuff iRight;
		};
	struct TLeaf
		{
		TUint iCount;
		THuff iVal;
		};
private:
	static void Lengths(TUint32* aLengths,const TNode* aNodes,TInt aNode,TInt aLen);
	static TUint32* SubTree(TUint32* aPtr,const TUint32* aValue,TUint32** aLevel);
	};

class THuffEncoder
	{
public:
	THuffEncoder(RWriteStream& aStream);
//
	void EncodeL(TUint aCode,TInt aLength);
	void EncodeL(TUint32 aHuffCode);
	void CompleteL();
private:
	enum {EBufSize=0x100};
private:
	TUint8 iBuf[EBufSize];
	RWriteStream& iStream;
	TUint32 iCode;		// code in production
	TInt iBits;
	TUint8* iWrite;
	};

class HDeflateHash
	{
public:
	inline static HDeflateHash& NewLC(TInt aLinks);
//
	inline TInt First(const TUint8* aPtr,TInt aPos);
	inline TInt Next(TInt aPos,TInt aOffset) const;
private:
	inline HDeflateHash();
	inline static TInt Hash(const TUint8* aPtr);
private:
	typedef TUint16 TOffset;
private:
	TInt iHash[256];
	TOffset iOffset[1];	// or more
	};

class MDeflater
	{
public:
	void DeflateL(const TUint8* aBase,TInt aLength);
private:
	const TUint8* DoDeflateL(const TUint8* aBase,const TUint8* aEnd,HDeflateHash& aHash);
	static TInt Match(const TUint8* aPtr,const TUint8* aEnd,TInt aPos,HDeflateHash& aHas);
	void SegmentL(TInt aLength,TInt aDistance);
	virtual void LitLenL(TInt aCode) =0;
	virtual void OffsetL(TInt aCode) =0;
	virtual void ExtraL(TInt aLen,TUint aBits) =0;
	};

class TInflater
	{
public:
	TInflater(const TUint8* aIn,const CDbStoreCompression::TEncoding& aDecoding);
	TInt Inflate();
	inline const TUint8* Ptr() const;
	inline static TInt BufferSize();
private:
	const TUint8* iIn;
	TUint iBits;
	const TUint8* iRptr;			// partial segment
	TInt iLen;
	const TUint32* iLitLenTree;
	const TUint32* iDistTree;
	TUint8 iOut[KDeflateMaxDistance];	// circular buffer for distance matches
	};

NONSHARABLE_CLASS(TDeflateStats) : public MDeflater
	{
public:
	inline TDeflateStats(CDbStoreCompression::TEncoding& aEncoding);
private:
// from MDeflater
	void LitLenL(TInt aCode);
	void OffsetL(TInt aCode);
	void ExtraL(TInt aLen,TUint aBits);
private:
	CDbStoreCompression::TEncoding& iEncoding;
	};

NONSHARABLE_CLASS(TDeflater) : public MDeflater
	{
public:
	inline TDeflater(THuffEncoder& aEncoder,const CDbStoreCompression::TEncoding& aEncoding);
private:
// from MDeflater
	void LitLenL(TInt aCode);
	void OffsetL(TInt aCode);
	void ExtraL(TInt aLen,TUint aBits);
private:
	THuffEncoder& iEncoder;
	const CDbStoreCompression::TEncoding& iEncoding;
	};

NONSHARABLE_CLASS(HDeflateBuf) : public TBufBuf
	{
public:
	enum TMode {EAnalysis,EDeflate};	// mirror CDbStoreCompression enum
public:
	static HDeflateBuf* NewL(MStreamBuf* aHost,CDbStoreCompression::TEncoding& aEncoding,TMode aMode);
private:
	inline HDeflateBuf(MStreamBuf* aHost,CDbStoreCompression::TEncoding& aEncoding,CBufBase* aBuf,TMode aMode);
	virtual inline ~HDeflateBuf();
// from MStreamBuf
	void DoSynchL();
	void DoRelease();
private:
	RWriteStream iHost;
	CDbStoreCompression::TEncoding& iEncoding;
	CBufBase* iBuf;
	TMode iMode;
	};

NONSHARABLE_CLASS(HInflateBuf) : public TBufBuf
	{
public:
	static HInflateBuf* NewL(MStreamBuf* aHost,const CDbStoreCompression::TEncoding& aEncoding);
private:
	inline HInflateBuf(CBufBase* aBuf);
	virtual inline ~HInflateBuf();
// from MStreamBuf
	void DoRelease();
private:
	CBufBase* iBuf;
	};

NONSHARABLE_CLASS(CDbStoreTable::CCompressor) : public CBase, public CCluster::MAlter
	{
public:
	inline CCompressor();
	~CCompressor();
	void ProcessL(CDbStoreTable* aTable);
private:
	TUint8* AlterRecordL(TUint8* aWPtr,const TUint8* aRPtr,TInt aLength);
private:
	CDbStoreTable* iTable;
	RDbRow iRow;
	};

// Class Huffman
//
// This class builds a huffman encoding from a frequency table and builds
// a decoding tree from a code-lengths table
//
// the encoding generated is based on the rule that given two symbols s1 and s2, with 
// code length l1 and l2, and huffman codes h1 and h2:
//
// if l1<l2 then h1<h2 when compared lexicographically
// if l1==l2 and s1<s2 then h1<h2 ditto
//
// This allows the encoding to be stored compactly as a table of code lengths

//
// recursive function to calculate the code lengths from the node tree
//
void Huffman::Lengths(TUint32* aLengths,const TNode* aNodes,TInt aNode,TInt aLen)
	{
	__ASSERT(aLen<KHuffMaxCodeLength);
	++aLen;
	const TNode& node=aNodes[aNode];
	if (node.iLeft&KLeaf)
		aLengths[node.iLeft&~KLeaf]=aLen;
	else
		Lengths(aLengths,aNodes,node.iLeft,aLen);
	if (node.iRight&KLeaf)
		aLengths[node.iRight&~KLeaf]=aLen;
	else
		Lengths(aLengths,aNodes,node.iRight,aLen);
	}

//
// write the subtree below aPtr and return the head
//
TUint32* Huffman::SubTree(TUint32* aPtr,const TUint32* aValue,TUint32** aLevel)
	{
	TUint32* l=*aLevel++;
	if (l>aValue)
		{
		TUint32* sub1=SubTree(aPtr,aValue,aLevel);	// 0-tree first
		aPtr=SubTree(sub1,aValue-(aPtr-sub1)-1,aLevel);			// 1-tree
		TInt branch=(TUint8*)sub1-(TUint8*)aPtr;
		*--aPtr=branch;
		}
	else if (l==aValue)
		{
		TUint term0=*aValue--;						// 0-term
		aPtr=SubTree(aPtr,aValue,aLevel);			// 1-tree
		*--aPtr=term0>>16;
		}
	else	// l<iNext
		{
		TUint term0=*aValue--;						// 0-term
		TUint term1=*aValue--;
		*--aPtr=(term1>>16<<16)|(term0>>16);
		}
	return aPtr;
	}

//
// Build a huffman encoding table from a symbol frequency table
// aTable contains frequency data on input for aCodes symbols
// aTable contains the huffman encoding on output
//
void Huffman::EncodingL(TUint32* aTable,TInt aCodes)
	{
//
// step 1. Sort the values into decreasing order of frequency
//
	TLeaf* leaves=new(ELeave) TLeaf[aCodes];
	CleanupArrayDeletePushL(leaves);
	TInt lCount=0;
	TInt ii;
	for (ii=0;ii<aCodes;++ii)
		{
		TUint c=aTable[ii];
		if (c==0)
			continue;	// no coding for ii
		TInt jj;
		for (jj=0;jj<lCount;++jj)
			{
			if (leaves[jj].iCount<=c)
				break;
			}
		Mem::Move(leaves+jj+1,leaves+jj,sizeof(TLeaf)*(lCount-jj));
		leaves[jj].iCount=c;
		leaves[jj].iVal=THuff(ii|KLeaf);
		lCount++;
		}
//
// Huffman algorithm: pair off least frequent nodes and reorder
// result is the code lengths in aTable[]
//
	if (lCount==1)	// special case for a single value (always encode as "0")
		aTable[leaves[0].iVal&~KLeaf]=1;
	else if (lCount>1)
		{	//	don't encode for empty coding: leaves in order now
		TInt max=0;
		TNode* nodes=new(ELeave) TNode[lCount-1];
		while (--lCount>0)
			{
			TNode& node=nodes[max];
			node.iLeft=leaves[lCount-1].iVal;
			node.iRight=leaves[lCount].iVal;
			// re-order the leaves now to reflect new combined frequency
			TUint c=leaves[lCount-1].iCount+leaves[lCount].iCount;
			TInt jj=lCount;
			while (--jj>0)
				{
				if (leaves[jj-1].iCount>=c)
					break;
				}
			Mem::Move(leaves+jj+1,leaves+jj,sizeof(TLeaf)*(lCount-1-jj));
			// update new leaf
			leaves[jj].iCount=c;
			leaves[jj].iVal=THuff(max);
			max++;
			}
		Lengths(aTable,nodes,leaves[0].iVal,0);
		delete[] nodes;
		}
	CleanupStack::PopAndDestroy();		// leaves
//
// step 3: Generate the coding based on the code lengths
//
	TInt lenCount[KHuffMaxCodeLength];
	Mem::FillZ(lenCount,sizeof(lenCount));

	for (ii=aCodes;--ii>=0;)
		{
		TInt len=aTable[ii]-1;
		if (len>=0)
			++lenCount[len];
		}

	TUint nextCode[KHuffMaxCodeLength];
	TUint code=0;
	for (ii=0;ii<KHuffMaxCodeLength;++ii)
		{
		nextCode[ii]=code;
		code=(code+lenCount[ii])<<1;
		}

	for (ii=0;ii<aCodes;++ii)
		{
		TInt len=aTable[ii];
		if (len)
			{
			aTable[ii] = (nextCode[len-1]<<(KHuffMaxCodeLength-len))|(len<<KHuffMaxCodeLength);
			++nextCode[len-1];
			}
		}
	}

//
// generate the decoding tree from the huffman code length data
// output alphabet is [aBase,aBase+aCodes)
//
void Huffman::Decoding(TUint32* aDecoding,TInt aCodes,TInt aBase)
	{
	TInt counts[KHuffMaxCodeLength];
	Mem::FillZ(counts,sizeof(counts));
	TInt codes=0;
	TInt ii=aCodes;
	while (--ii>=0)
		{
		TUint len=aDecoding[ii];
		__ASSERT(len<=(TUint)KHuffMaxCodeLength);
		if (len)
			{
			++counts[len-1];
			++codes;
			}
		}
//
	TUint32* level[KHuffMaxCodeLength];
	TUint32* lit=aDecoding+codes;
	for (ii=0;ii<KHuffMaxCodeLength;++ii)
		{
		level[ii]=lit;
		lit-=counts[ii];
		}
	aBase=(aBase<<17)+(KHuffTerminate<<16);
	for (ii=0;ii<aCodes;++ii)
		{
		TUint len=TUint8(aDecoding[ii]);
		if (len)
			*--level[len-1]|=(ii<<17)+aBase;
		}
	if (codes==1)	// codes==1 special case: tree is not complete
		*aDecoding>>=16;	// 0-terminate at root
	else if (codes>1)
		{
		__DEBUG(TUint32* p =)SubTree(aDecoding+codes-1,aDecoding+codes-1,level);
		__ASSERT(p==aDecoding);
		}
	}

// Class HDeflateHash

inline HDeflateHash::HDeflateHash()
	{TInt* p=iHash+256;do *--p=-KDeflateMaxDistance-1; while (p>iHash);}

inline HDeflateHash& HDeflateHash::NewLC(TInt aLinks)
	{
	__ASSERT(!(KDeflateMaxDistance&(KDeflateMaxDistance-1)));	// ensure power of two
	return *new(User::AllocLC(_FOFF(HDeflateHash,iOffset[Min(aLinks,KDeflateMaxDistance)]))) HDeflateHash;
	}

inline TInt HDeflateHash::Hash(const TUint8* aPtr)
	{
	TUint x=aPtr[0]|(aPtr[1]<<8)|(aPtr[2]<<16);
	return (x*KDeflateHashMultiplier)>>KDeflateHashShift;
	}

inline TInt HDeflateHash::First(const TUint8* aPtr,TInt aPos)
	{
	TInt h=Hash(aPtr);
	TInt offset=Min(aPos-iHash[h],KDeflateMaxDistance<<1);
	iHash[h]=aPos;
	iOffset[aPos&(KDeflateMaxDistance-1)]=TOffset(offset);
	return offset;
	}

inline TInt HDeflateHash::Next(TInt aPos,TInt aOffset) const
	{return aOffset+iOffset[(aPos-aOffset)&(KDeflateMaxDistance-1)];}


// Class TDeflater
//
// generic deflation algorithm, can do either statistics and the encoder

TInt MDeflater::Match(const TUint8* aPtr,const TUint8* aEnd,TInt aPos,HDeflateHash& aHash)
	{
	TInt offset=aHash.First(aPtr,aPos);
	if (offset>KDeflateMaxDistance)
		return 0;
	TInt match=0;
	aEnd=Min(aEnd,aPtr+KDeflateMaxLength);
	TUint8 c=*aPtr;
	do
		{
		const TUint8* p=aPtr-offset;
		if (p[match>>16]==c)
			{	// might be a better match
			const TUint8* m=aPtr;
			for (;;)
				{
				if (*p++!=*m++)
					break;
				if (m<aEnd)
					continue;
				return ((m-aPtr)<<16)|offset;
				}
			TInt l=m-aPtr-1;
			if (l>match>>16)
				{
				match=(l<<16)|offset;
				c=m[-1];
				}
			}
		offset=aHash.Next(aPos,offset);
		} while (offset<=KDeflateMaxDistance);
	return match;
	}

//
// Apply the deflation algorithm to the data [aBase,aEnd)
// Return a pointer after the last byte that was deflated (which may not be aEnd)
//
const TUint8* MDeflater::DoDeflateL(const TUint8* aBase,const TUint8* aEnd,HDeflateHash& aHash)
	{
	__ASSERT(aEnd-aBase>KDeflateMinLength);
//
	const TUint8* ptr=aBase;
#ifdef __EXTRA_DEFLATE
	TInt prev=0;		// the previous deflation match
#endif
	do
		{
		TInt match=Match(ptr,aEnd,ptr-aBase,aHash);
#ifdef __EXTRA_DEFLATE
// Extra deflation applies two optimisations which double the time taken
// 1. If we have a match at p, then test for a better match at p+1 before using it
// 2. When we have a match, add the hash links for all the data which will be skipped 
		if (match>>16 < prev>>16)
			{	// use the previous match--it was better
			TInt len=prev>>16;
			SegmentL(len,prev-(len<<16));
			// fill in missing hash entries for better compression
			const TUint8* e=ptr+len-2;
			do
				{
				++ptr;
				aHash.First(ptr,ptr-aBase);
				} while (ptr<e);
			prev=0;
			}
		else if (match<=(KDeflateMinLength<<16))
			LitLenL(*ptr);			// no deflation match here
		else
			{	// save this match and test the next position
			if (prev)	// we had a match at ptr-1, but this is better
				LitLenL(ptr[-1]);
			prev=match;
			}
		++ptr;
#else
// Basic deflation will store any match found, and not update the hash links for the
// data which is skipped
		if (match<=(KDeflateMinLength<<16))		// no match
			LitLenL(*ptr++);
		else
			{	// store the match
			TInt len=match>>16;
			SegmentL(len,match-(len<<16));
			ptr+=len;
			}
#endif
		} while (ptr+KDeflateMinLength-1<aEnd);
#ifdef __EXTRA_DEFLATE
	if (prev)
		{		// emit the stored match
		TInt len=prev>>16;
		SegmentL(len,prev-(len<<16));
		ptr+=len-1;
		}
#endif
	return ptr;
	}

//
// The generic deflation algorithm
//
void MDeflater::DeflateL(const TUint8* aBase,TInt aLength)
	{
	const TUint8* end=aBase+aLength;
	if (aLength>KDeflateMinLength)
		{	// deflation kicks in if there is enough data
		HDeflateHash& hash=HDeflateHash::NewLC(aLength);
		aBase=DoDeflateL(aBase,end,hash);
		CleanupStack::PopAndDestroy();
		}
	while (aBase<end)					// emit remaining bytes
		LitLenL(*aBase++);
	LitLenL(CDbStoreCompression::TEncoding::EEos);	// eos marker
	}

//
// Turn a (length,offset) pair into the deflation codes+extra bits before calling
// the specific LitLen(), Offset() and Extra() functions.
//
void MDeflater::SegmentL(TInt aLength,TInt aDistance)
	{
	__ASSERT(aLength>=KDeflateMinLength && aLength<=KDeflateMaxLength);
	aLength-=KDeflateMinLength;
	TInt extralen=0;
	TUint len=aLength;
	while (len>=8)
		{
		++extralen;
		len>>=1;
		}
	__ASSERT((extralen<<2)+len<CDbStoreCompression::TEncoding::ELengths);
	LitLenL((extralen<<2)+len+CDbStoreCompression::TEncoding::ELiterals);
	if (extralen)
		ExtraL(extralen,TUint(aLength)<<(32-extralen));
//
	__ASSERT(aDistance>0 && aDistance<=KDeflateMaxDistance);
	aDistance--;
	extralen=0;
	TUint dist=aDistance;
	while (dist>=8)
		{
		++extralen;
		dist>>=1;
		}
	__ASSERT((extralen<<2)+dist<CDbStoreCompression::TEncoding::EDistances);
	OffsetL((extralen<<2)+dist);
	if (extralen)
		ExtraL(extralen,TUint(aDistance)<<(32-extralen));
	}

// Class TDeflateStats
//
// This class analyses the data stream to generate the frequency tables 
// for the deflation algorithm

inline TDeflateStats::TDeflateStats(CDbStoreCompression::TEncoding& aEncoding)
	:iEncoding(aEncoding)
	{}

void TDeflateStats::LitLenL(TInt aCode)
	{
	++iEncoding.iLitLen[aCode];
	}

void TDeflateStats::OffsetL(TInt aCode)
	{
	++iEncoding.iDistance[aCode];
	}

void TDeflateStats::ExtraL(TInt,TUint)
	{}

// Class THuffEncoder
//
// This class generates the byte stream of huffman codes, writing them out to the stream

THuffEncoder::THuffEncoder(RWriteStream& aStream)
	:iStream(aStream),iCode(0),iBits(-8),iWrite(iBuf)
	{}

//
// Store a huffman code generated by Huffman::EncodingL()
//
void THuffEncoder::EncodeL(TUint32 aHuffCode)
	{
	EncodeL(aHuffCode<<(32-KHuffMaxCodeLength),aHuffCode>>KHuffMaxCodeLength);
	}

//
// Store aLength bits from the most significant bits of aCode
//
void THuffEncoder::EncodeL(TUint aCode,TInt aLength)
	{
	TInt bits=iBits;
	TUint code=iCode|(aCode>>(bits+8));
	bits+=aLength;
	if (bits>=0)
		{
		TUint8* write=iWrite;
		do
			{
			if (write-EBufSize==iBuf)
				{
				iStream.WriteL(iBuf,EBufSize);
				write=iBuf;
				}
			*write++=TUint8(code>>24);
			code<<=8;
			bits-=8;
			} while (bits>=0);
		iWrite=write;
		}
	iCode=code;
	iBits=bits;
	}

//
// Terminate the huffman coding. The longest code is always 1111111111
//
void THuffEncoder::CompleteL()
	{
	if (iBits>-8)
		EncodeL(0xffffffffu,-iBits);
	if (iWrite>iBuf)
		iStream.WriteL(iBuf,iWrite-iBuf);
	}

// Class TDeflater
//
// Extends MDeflater to provide huffman encoding of the output

//
// construct for encoding
//
inline TDeflater::TDeflater(THuffEncoder& aEncoder,const CDbStoreCompression::TEncoding& aEncoding)
	:iEncoder(aEncoder),iEncoding(aEncoding)
	{}

void TDeflater::LitLenL(TInt aCode)
	{
	iEncoder.EncodeL(iEncoding.iLitLen[aCode]);
	}

void TDeflater::OffsetL(TInt aCode)
	{
	iEncoder.EncodeL(iEncoding.iDistance[aCode]);
	}

void TDeflater::ExtraL(TInt aLen,TUint aBits)
	{
	iEncoder.EncodeL(aBits,aLen);
	}

// Class TInflater
//
// The inflation algorithm, complete with huffman decoding

TInflater::TInflater(const TUint8* aIn,const CDbStoreCompression::TEncoding& aEncoding)
	:iIn(aIn),iBits(KBitsInit),iLen(0),iLitLenTree(aEncoding.iLitLen),iDistTree(aEncoding.iDistance)
	{}

//
// consume all data lag in the history buffer, then decode to fill up the output buffer
//
TInt TInflater::Inflate()
	{
// empty the history buffer into the output
	const TUint8* data=iIn;
	TUint bits=iBits;
	const TUint8* from=iRptr;
	TInt len=iLen;
	TUint8* out=iOut;
	TUint8* const end=out+KDeflateMaxDistance;
	const TUint32* node;
	if (len)
		goto useHistory;
//
	if (bits&KBitsEOF)
		return 0;
//
	node=iLitLenTree;
	while (out<end)
		{
		// get a huffman code
		{
		TUint huff;
		for (;;)
			{
			huff=*node++;
			if ((bits<<=1)&KBitsEmpty)
				bits=*data++|KBitsFull;
			if (bits&KBitsNext)
				{
				if (huff&(KHuffTerminate<<16))
					break;
				}
			else
				{
				if (huff&KHuffTerminate)
					{
					huff<<=16;
					break;
					}
				else
					node=PtrAdd(node,huff);
				}
			}
		TInt val=TInt(huff>>17)-CDbStoreCompression::TEncoding::ELiterals;
		if (val<0)
			{
			*out++=TUint8(val);
			node=iLitLenTree;
			continue;			// another literal/length combo
			}
		if (val==CDbStoreCompression::TEncoding::EEos-CDbStoreCompression::TEncoding::ELiterals)
			{	// eos marker. we're done
			bits=KBitsEOF;
			break;
			}
		// get the extra bits for the code
		TInt code=val&0xff;
		if (code>=8)
			{	// xtra bits
			TInt xtra=(code>>2)-1;
			code-=xtra<<2;
			do
				{
				if ((bits<<=1)&KBitsEmpty)
					bits=*data++|KBitsFull;
				code<<=1;
				if (bits&KBitsNext)
					code|=1;
				} while (--xtra!=0);
			}
		if (val<KDeflateDistCodeBase-CDbStoreCompression::TEncoding::ELiterals)
			{	// length code... get the code
			len=code+KDeflateMinLength;
			__ASSERT(len<=KDeflateMaxLength);
			node=iDistTree;
			continue;			// read the huffman code
			}
		// distance code
		__ASSERT(code<KDeflateMaxDistance);
		from=out-(code+1);
		if (from+KDeflateMaxDistance<end)
			from+=KDeflateMaxDistance;
		}
useHistory:
		TInt tfr=Min(end-out,len);
		len-=tfr;
		do
			{
			*out++=*from++;
			if (from==end)
				from-=KDeflateMaxDistance;
			} while (--tfr!=0);
		node=iLitLenTree;
		};
	iIn=data;
	iBits=bits;
	iRptr=from;
	iLen=len;
	return out-iOut;
	}

inline const TUint8* TInflater::Ptr() const
	{return iOut;}
inline TInt TInflater::BufferSize()
	{return KDeflateMaxDistance;}

// Class HDeflateBuf
//
// This stream buffer applies the analysis or deflation and huffman coding
// on the entire stream data when it is committed

inline HDeflateBuf::HDeflateBuf(MStreamBuf* aHost,CDbStoreCompression::TEncoding& aEncoding,CBufBase* aBuf,TMode aMode)
	:iHost(aHost),iEncoding(aEncoding),iBuf(aBuf),iMode(aMode)
	{Set(*aBuf,0);}

HDeflateBuf* HDeflateBuf::NewL(MStreamBuf* aHost,CDbStoreCompression::TEncoding& aEncoding,TMode aMode)
	{
	CBufBase* buf=CBufFlat::NewL(512);
	CleanupStack::PushL(buf);
	HDeflateBuf* self=new(ELeave) HDeflateBuf(aHost,aEncoding,buf,aMode);
	CleanupStack::Pop();
	return self;
	}

inline HDeflateBuf::~HDeflateBuf()
	{delete iBuf;iHost.Release();}

void HDeflateBuf::DoRelease()
	{
	delete this;
	}

//
// This is where it all happens
//
void HDeflateBuf::DoSynchL()
	{
	if (iMode==EAnalysis)
		{
		TDeflateStats deflater(iEncoding);
		deflater.DeflateL(iBuf->Ptr(0).Ptr(),iBuf->Size());
		}
	else
		{
		THuffEncoder encoder(iHost);
		TDeflater deflater(encoder,iEncoding);
		deflater.DeflateL(iBuf->Ptr(0).Ptr(),iBuf->Size());
		encoder.CompleteL();
		iHost.CommitL();
		}
	}

// Class HInflateBuf
//
// Inflate the input stream. This is not a filter, it reads all the input, inflates it and
// keeps it in a memory buffer.

const TInt KInflateBufSize=0x800;	// 2K

HInflateBuf::HInflateBuf(CBufBase* aBuf)
	:iBuf(aBuf)
	{
	Set(*aBuf,0,ERead);
	}

inline HInflateBuf::~HInflateBuf()
	{delete iBuf;}

void HInflateBuf::DoRelease()
	{
	delete this;
	}

HInflateBuf* HInflateBuf::NewL(MStreamBuf* aHost,const CDbStoreCompression::TEncoding& aEncoding)
	{
	CBufFlat* host=CBufFlat::NewL(256);
	CleanupStack::PushL(host);
	TUint8 buffer[KInflateBufSize];
	for (;;)
		{
		TInt len=aHost->ReadL(buffer,KInflateBufSize);
		if (len)
			host->InsertL(host->Size(),buffer,len);
		if (len<KInflateBufSize)
			break;
		}
	CBufSeg* out=CBufSeg::NewL(256);
	CleanupStack::PushL(out);
	TInflater* inflater=new(ELeave) TInflater(host->Ptr(0).Ptr(),aEncoding);
	CleanupStack::PushL(inflater);
	for (;;)
		{
		TInt len=inflater->Inflate();
		if (len)
			out->InsertL(out->Size(),inflater->Ptr(),len);
		if (len<inflater->BufferSize())
			break;
		}
	HInflateBuf* buf=new(ELeave) HInflateBuf(out);
	CleanupStack::PopAndDestroy();	// inflater
	CleanupStack::Pop();			// out
	CleanupStack::PopAndDestroy();	// host
	aHost->Release();				// don't need this anymore
	return buf;
	}

// Class CDbStoreTable::Compressor
//
// This class processes an entire table for analysis or compression, using the
// CDbStoreRecords::AlterL() functionality and call back to ensure that all clusters
// and BLOBs are read and written.

inline CDbStoreTable::CCompressor::CCompressor()
	{}

CDbStoreTable::CCompressor::~CCompressor()
	{
	if (iTable)
		iTable->Close();
	iRow.Close();
	}

//
// Walk through every cluster in the table
//
void CDbStoreTable::CCompressor::ProcessL(CDbStoreTable* aTable)
	{
	iTable=aTable;
	CDbStoreRecords& rec=aTable->StoreRecordsL();
	for (TClusterId cluster=rec.Head();cluster!=KNullClusterId;cluster=rec.AlterL(cluster,*this))
		;
	}

//
// Compress every blob, and transfer the record from aRPtr to aWPtr
//
TUint8* CDbStoreTable::CCompressor::AlterRecordL(TUint8* aWPtr,const TUint8* aRPtr,TInt aLength)
	{
	if (iTable->Def().Columns().HasLongColumns())
		{
		iTable->CopyToRowL(iRow,TPtrC8(aRPtr,aLength));
		CDbBlobSpace* blobs=iTable->BlobsL();
		TDbColNo col=1;
		HDbColumnSet::TIteratorC iter=iTable->Def().Columns().Begin();
		const HDbColumnSet::TIteratorC end=iTable->Def().Columns().End();
		do
			{
			if (!TDbCol::IsLong(iter->Type()))
				continue;
			TDbBlob& blob=CONST_CAST(TDbBlob&,TDbColumnC(iRow,col).Blob());
			if (blob.IsInline())
				continue;
			// do what has to be done...?
			TUint8* data=(TUint8*)User::AllocLC(blob.Size());
			blobs->ReadLC(blob.Id(),iter->Type())->ReadL(data,blob.Size());
			CleanupStack::PopAndDestroy();	// stream buffer
			// re-write the Blob to compress it
			blobs->DeleteL(blob.Id());
			blob.SetId(blobs->CreateL(iter->Type(),data,blob.Size()));
			CleanupStack::PopAndDestroy();	// data
			} while (++col,++iter<end);
		iTable->CopyFromRow(aWPtr,iRow);
		}
	else
		Mem::Copy(aWPtr,aRPtr,aLength);
	return aWPtr+aLength;
	}

// Class CDbStoreCompression
//
// This class manages the compression for the database, applying filters as appropriate
// It also defines the extrenalisation format for the huffman trees

const TInt KDeflationCodes=3*(CDbStoreCompression::TEncoding::ELitLens+CDbStoreCompression::TEncoding::EDistances);

inline CDbStoreCompression::CDbStoreCompression()
//	:iState(EAnalysis)
	{}

CDbStoreCompression* CDbStoreCompression::NewL()
	{
	return new(ELeave) CDbStoreCompression;
	}

//
// Build huffman codings from the freqeuncy tables
//
void CDbStoreCompression::EncodeL()
	{
	__ASSERT(iState==EAnalysis);
	TUint32* p=iEncoding[0].iLitLen;
	TUint32* end=p+KDeflationCodes;
	do
		{
		Huffman::EncodingL(p,TEncoding::ELitLens);
		p+=TEncoding::ELitLens;
		Huffman::EncodingL(p,TEncoding::EDistances);
		p+=TEncoding::EDistances;
		} while (p<end);
	iState=EEncoding;
	}

//
// Store the encoding tables as a sequence of code lengths
// The code lengths (0-25) are themselves huffman coded, and the meta coding is stored first
//
void CDbStoreCompression::ExternalizeL(RWriteStream& aStream) const
	{
	__ASSERT(iState==EEncoding);
	const TUint32* base=iEncoding[0].iLitLen;
	const TUint32* end=base+KDeflationCodes;
	TUint32 codes[KDeflateMetaCodes];
	Mem::FillZ(codes,sizeof(codes));
	const TUint32* p=base;
	do ++codes[*p++>>KHuffMaxCodeLength]; while (p<end);
	Huffman::EncodingL(codes,KDeflateMetaCodes);
// save the meta encoding
	p=codes+KDeflateMetaCodes;
	do
		{
		TUint c0=*--p;
		TUint c1=*--p;
		c0>>=KHuffMaxCodeLength;
		c1>>=KHuffMaxCodeLength;
		aStream.WriteUint8L((c0<<4)|c1);
		} while (p>codes);
// write the encoding
	THuffEncoder encoder(aStream);
	p=base;
	do encoder.EncodeL(codes[*p++>>KHuffMaxCodeLength]); while (p<end);
	encoder.CompleteL();
	}

//
// Internalize a previous saved encoding
//
void CDbStoreCompression::InternalizeL(RReadStream& aStream)
	{
	__ASSERT(iState!=EEncoding);
//
// read the meta encoding
	TUint32 decode[KDeflateMetaCodes];
	TUint32* p=decode+KDeflateMetaCodes;
	do
		{
		TUint8 c=aStream.ReadUint8L();
		*--p=c>>4;
		*--p=c&0xf;
		} while (p>decode);
	Huffman::Decoding(decode,KDeflateMetaCodes);
// decode the encoding
	p=iEncoding[0].iLitLen;
	TUint32* end=p+KDeflationCodes;
	TUint bits=KBitsInit;
	do
		{
		const TUint32* node=decode;
		TUint huff;
		for (;;)
			{
			huff=*node++;
			if ((bits<<=1)&KBitsEmpty)
				bits=aStream.ReadUint8L()|KBitsFull;
			if (bits&KBitsNext)
				{
				if (huff&(KHuffTerminate<<16))
					break;
				}
			else
				{
				if (huff&KHuffTerminate)
					{
					huff<<=16;
					break;
					}
				else
					node=PtrAdd(node,huff);
				}
			}
		*p++=huff>>17;
		} while (p<end);
// convert the length tables into huffman decoding trees
	p=iEncoding[0].iLitLen;
	do
		{
		Huffman::Decoding(p,TEncoding::ELitLens);
		p+=TEncoding::ELitLens;
		Huffman::Decoding(p,TEncoding::EDistances,KDeflateDistCodeBase);
		p+=TEncoding::EDistances;
		} while (p<end);
	if (iState==EAnalysis)
		iState=EDecoding;
	}

//
// Apply an inflation filter to a read stream
//
MStreamBuf* CDbStoreCompression::FilterL(MStreamBuf* aHost,TUint32,RDbStoreReadStream::TType aType)
	{
	if (iState==EDecoding || iState==EInflating)
		return HInflateBuf::NewL(aHost,iEncoding[aType]);
	return aHost;
	}

//
// Apply a statistics or inflation filter to a write stream
//
MStreamBuf* CDbStoreCompression::FilterL(MStreamBuf* aHost,TUint32,RDbStoreWriteStream::TType aType)
	{
	TState s=iState;
	if (s==EDecoding)
		__LEAVE(KErrWrite);		// read-only database
	else if (s!=EInflating)
		{
		__ASSERT(TInt(EAnalysis)==TInt(HDeflateBuf::EAnalysis));
		__ASSERT(TInt(EEncoding)==TInt(HDeflateBuf::EDeflate));
		return HDeflateBuf::NewL(aHost,iEncoding[aType],HDeflateBuf::TMode(s));
		}
	return aHost;
	}

// Class CDbStoreDatabase
//
// Compression related code is maintained in this source file

//
// Iterate across all tables applying analysis or compression to them
//
void CDbStoreDatabase::CompressTablesL()
	{
	TSglQueIterC<CDbStoreDef> iter(SchemaL());
	const CDbStoreDef* def;
	while ((def=iter++)!=0)
		{
		CDbStoreTable::CCompressor* comp=new(ELeave) CDbStoreTable::CCompressor;
		CleanupStack::PushL(comp);
		comp->ProcessL(STATIC_CAST(CDbStoreTable*,TableL(*def)));
		CleanupStack::PopAndDestroy();	// comp
		}
	}

//
// Compress or decompress the whole database
//
void CDbStoreDatabase::CompressL(TStreamId aStreamId,TZipType aZip)
	{
	__ASSERT(iStore);
	iSchemaId=aStreamId;
// read the databse header for encryption information
	RStoreReadStream strm;
	strm.OpenLC(Store(),aStreamId);
	ReadHeaderL(strm);
	CleanupStack::PopAndDestroy();	// strm
	InitPagePoolL();
//
	if (iVersion==EDbStoreCompressed)
		{
		iCompression->Inflate();
		if (aZip==EDeflate)
			__LEAVE(KErrArgument);		// already compressed
		}
	else if (aZip==EInflate)
		__LEAVE(KErrArgument);		// not compressed
	else
		{	// deflate pass #1: analyse the database
		CompressionL();				// construct the compression filter
		Transaction().DDLBeginLC();
		CompressTablesL();
		iClusterCache->FlushL();		// force through the stats buffer
		ReplaceSchemaL();				// force through the stats buffer
		CleanupStack::PopAndDestroy();	// rollback after analysis!
		iCompression->EncodeL();
		}
// now inflate or deflate the data
	Transaction().DDLBeginLC();
	CompressTablesL();
	iVersion=TUint8(aZip==EDeflate ? EDbStoreCompressed : EDbStoreVersion2);
	Transaction().DDLCommitL();
	CleanupStack::Pop();		// rollback not required
	}

void CDbStoreDatabase::CompressL(CStreamStore* aStore,TStreamId aStreamId,TZipType aZip)
	{
	CDbStoreDatabase* self=NewLC(aStore);
	CDbObject* db=self->InterfaceL();	// a reference to the database is required
	CleanupStack::Pop();	// self
	db->PushL();
	self->Transaction().DDLPrepareL(*db);
	self->CompressL(aStreamId,aZip);
	CleanupStack::PopAndDestroy();	// db
	}

// Class RDbStoreDatabase

EXPORT_C void RDbStoreDatabase::CompressL(CStreamStore& aStore,TStreamId aId)
	{
	CDbStoreDatabase::CompressL(&aStore,aId,CDbStoreDatabase::EDeflate);
	}

EXPORT_C void RDbStoreDatabase::DecompressL(CStreamStore& aStore,TStreamId aId)
	{
	CDbStoreDatabase::CompressL(&aStore,aId,CDbStoreDatabase::EInflate);
	}
