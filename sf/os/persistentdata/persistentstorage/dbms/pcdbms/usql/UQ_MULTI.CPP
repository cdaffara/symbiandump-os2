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
// SQL AND and OR node class
// 
//

#include "UQ_STD.H"

// class CSqlMultiNode

inline CSqlMultiNode::CSqlMultiNode( TType aType )
 :	CSqlSearchCondition( aType )
	{ __ASSERT( aType == EOr || aType == EAnd ); }

inline TInt CSqlMultiNode::Size() const
	{ return ( const TUint8* )iEnd - ( const TUint8* )this; }

inline TBool CSqlMultiNode::IsFull() const
	{ return ( Size() & ( EGranularity - 1 ) ) == 0; }

inline TInt CSqlMultiNode::Size( TInt aLeaves )
	{ return _FOFF( CSqlMultiNode, iLeaves[ aLeaves ] ); }

CSqlMultiNode::~CSqlMultiNode()
	{
	TEntry* p = iLeaves;
	TEntry* e = iEnd;
	while ( p < e )
		delete *p++;
	}

CSqlMultiNode* CSqlMultiNode::New( TType aType, TInt aSize )
	{
	aSize = ( aSize + ( EGranularity - 1 ) ) & ~( EGranularity - 1 );
	__ASSERT( aSize >= static_cast<TInt>( sizeof( CSqlMultiNode ) ) );
	aSize -= sizeof( CSqlMultiNode );
	return new( aSize )  CSqlMultiNode( aType );	// get the extra size for the entries
	}

CSqlMultiNode* CSqlMultiNode::Grow( CSqlMultiNode* aNode, TInt aExtraSize )
	{
	TInt size = aNode->Size();
	CSqlMultiNode* self = New( aNode->NodeType(), size + aExtraSize );
	if ( self )
		{
		aNode->iEnd = aNode->iLeaves;
		self->iEnd = ( TEntry* )Mem::Copy( self->iLeaves, aNode->iLeaves, size - Size( 0 ) );
		}
	delete aNode;
	return self;
	}

CSqlMultiNode* CSqlMultiNode::Concatenate( CSqlMultiNode* aLeft, CSqlMultiNode* aRight )
	{
	TInt lsize = aLeft->Size();
	TInt rsize = aRight->Size();
	if ( rsize > lsize )
		return Concatenate( aRight, aLeft );	// right node larger--flip over
	rsize -= Size( 0 );
	if ( (aLeft->IsFull()) ||											// if left node is already full 
		 (( lsize & ( EGranularity - 1 ) ) + rsize > EGranularity ) )	// or available space in the left node is not enough for a new data
			aLeft = Grow( aLeft, rsize );								// then increase left node to accomadate right node
	if ( aLeft )
		{
		aRight->iEnd = aRight->iLeaves;
		aLeft->iEnd = ( TEntry* )Mem::Copy( aLeft->iEnd, aRight->iLeaves, rsize );
		}
	delete aRight;
	return aLeft;
	}

CSqlSearchCondition* CSqlMultiNode::New( TType aType, CSqlSearchCondition* aLeft, CSqlSearchCondition* aRight )
//
// Full NULL propagation, cleaning up any of either node on failure
//
	{
	if ( aLeft && aRight )
		{
		if ( aLeft->NodeType() == aType )
			{	// we can merge into aLeft
			CSqlMultiNode* left = aLeft->MultiNode();
			if ( aRight->NodeType() == aType )
				return Concatenate( left, aRight->MultiNode() );
			
			// append right subnode onto left
			if ( left->IsFull() )
				{
				left = Grow( left, sizeof( TEntry ) );
				if ( !left )
					goto grow_failed;
				}
			TEntry* e = left->iEnd;
			*e++ = aRight;
			left->iEnd = e;
			return left;
			}
		else if ( aRight->NodeType() == aType )
			return New( aType, aRight, aLeft );	// swap left and right
		else
			{	// neither node is not of the required type
			CSqlMultiNode* self = New( aType, Size( 2 ) );
			if ( self )
				{
				TEntry* e = self->iLeaves;
				*e++ = aLeft;
				*e++ = aRight;
				self->iEnd = e;
				return self;
				}
			}
		}
	delete aLeft;
grow_failed:
	delete aRight;
	return 0;
	}

void CSqlMultiNode::Remove( CSqlSearchCondition* aSubNode )
//
// Remove the subnode from the leaf-set
//
	{
	TEntry* p = iLeaves - 1;
	TEntry* e = --iEnd;
	do
		{
		__ASSERT( p < e );
		} while ( *++p != aSubNode );
	while ( p<e )
		{
		TEntry e = *++p;
		p[ -1 ] = e;
		}
	delete aSubNode;
	}

CSqlSearchCondition* CSqlMultiNode::Reduce (CSqlMultiNode* aNode )
//
// If we have less than two leaves, then simplify the expression
//
	{
	CSqlSearchCondition* node = aNode;
	if ( aNode->iEnd - aNode->iLeaves <= 1 )
		{
		if ( aNode->iEnd - aNode->iLeaves == 1 )
			{
			aNode->iEnd = aNode->iLeaves;
			node = aNode->iLeaves[ 0 ];
			}
		else
			node = 0;
		delete aNode;
		}
	return node;
	}

void CSqlMultiNode::BindL( const RDbTableRow& aSource )
	{
	__ASSERT( iEnd - iLeaves > 1 );
	const TEntry* p = iLeaves;
	const TEntry* e = iEnd;
	do ( *p++ )->BindL( aSource );
		while ( p < e );
	}

TBool CSqlMultiNode::EvaluateL( const TTextOps& aTextOp ) const
	{
	__ASSERT( iEnd - iLeaves > 1 );
	const TEntry* p = iLeaves;
	const TEntry* e = iEnd - 1;			// all except the last sub-node
	do
		{
		TBool b = ( *p++ )->EvaluateL( aTextOp );
		if ( NodeType() == EOr )
			{
			if ( b )
				return b;
			}
		else
			{
			if ( !b )
				return b;
			}
		} while ( p < e );
	return ( *p )->EvaluateL( aTextOp );	// the last subnode
	}
