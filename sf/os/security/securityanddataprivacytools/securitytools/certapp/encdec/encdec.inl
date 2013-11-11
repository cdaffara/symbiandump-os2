/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


template <class T>
inline REncodeWriteStream& operator<<(REncodeWriteStream& aStream,const T& anObject)
{
	const std::string &name = anObject.Name();

	if(!aStream.Quiet())
		{
		// Generate human readable progress
		REncodeWriteStream tmp(prog);
		tmp << anObject;
		}

	if(aStream.HumanReadable())
		{
		if(anObject.CommentOnlyInHumanMode() && !aStream.Verbose())
			{
			return aStream;
			}
		
		aStream.WriteIndent();
		if(anObject.CommentOnlyInHumanMode())
			{
			aStream.WriteByte('#');
			}
		aStream.WriteBin(name.data(), name.size());
		aStream.WriteSpace();
		EncodeHuman(aStream, anObject.Value());
		aStream.WriteLineEnd();
		}
	else
		{
		// Just write as binary "store" format
		aStream.StoreWriteStream() << anObject.Value();
		}
	return aStream;
}

template <typename T>
REncodeWriteStream& operator<<(REncodeWriteStream& aStream, const EncDecEnum<T> &aEncDecEnum)
{
	const std::string &name = aEncDecEnum.Name();

	if(!aStream.Quiet())
		{
		// Generate human readable progress
		REncodeWriteStream tmp(prog);
		tmp << aEncDecEnum;
		}

	if(aStream.HumanReadable())
		{
		if(aEncDecEnum.CommentOnlyInHumanMode() && !aStream.Verbose())
			{
			return aStream;
			}
		aStream.WriteIndent();
		if(aEncDecEnum.CommentOnlyInHumanMode())
			{
			aStream.WriteByte('#');
			}
		aStream.WriteBin(name.data(), name.size());
		aStream.WriteSpace();
		const char *enumName = aEncDecEnum.ValueName();
		if(enumName)
			{
			aStream.WriteByte('"');
			aStream.WriteBin(enumName, strlen(enumName));
			aStream.WriteByte('"');
			if(aStream.Verbose())
				{
				// In verbose mode so include numeric value as a comment
				aStream.WriteCStr(" # ");
				EncodeHuman(aStream, TUint32(aEncDecEnum.Value()));		
				}
			}
		else
			{
			// Symbolic value not available, so write as numeric
			EncodeHuman(aStream, TUint32(aEncDecEnum.Value()));		
			}
		aStream.WriteLineEnd();
		}
	else
		{
		// Just write as binary "store" format
		aStream.StoreWriteStream() << aEncDecEnum.Value();
		}
	return aStream;
}


template <class T>
inline RDecodeReadStream& operator>>(RDecodeReadStream& aStream,T& anObject)
{
	const std::string &name = anObject.Name();
	if(aStream.HumanReadable())
		{
		if(anObject.CommentOnlyInHumanMode())
			{
			return aStream; // Not accepted in human mode, so just return...
			}
		// Need to convert to ascii
		aStream.CheckName(name);
		DecodeHuman(aStream, anObject.Value());
		}
	else
		{
		// Just read binary
		aStream.iReadStream >> anObject.Value();
		}

	// Generate human readable progress
	//	prog.WriteIndent();
	REncodeWriteStream tmp(prog);
	tmp << anObject;

	return aStream;
}

template <typename T>
RDecodeReadStream& operator>>(RDecodeReadStream& aStream, EncDecEnum<T> &aEncDecEnum)
{
	const std::string &name = aEncDecEnum.Name();
	if(aStream.HumanReadable())
		{
		if(aEncDecEnum.CommentOnlyInHumanMode())
			{
			return aStream; // Not accepted in human mode, so just return...
			}
		// Need to convert to ascii
		aStream.CheckName(name);

		aStream.ReadNextToken();
		aEncDecEnum.SetValue(aStream.Token().c_str());
		}
	else
		{
		// Just read binary
		aStream.iReadStream >> aEncDecEnum.Value();
		}

	// Generate human readable progress
	//	prog.WriteIndent();
	REncodeWriteStream tmp(prog);
	tmp << aEncDecEnum;

	return aStream;
}



template<typename T>
EncDecEnum<T>::EncDecEnum(const char *aName, const EnumEntry *aEnumEntries, bool aCommentOnlyInHumanMode)
	: iName(aName), iEnumEntries(aEnumEntries), iCommentOnlyInHumanMode(aCommentOnlyInHumanMode), iValue(0)
{
	if(iEnumEntries == 0)
		{
		FatalError(); // programing error
		}
}



template<typename T>
void EncDecEnum<T>::SetValue(const T &aValue)
{
	// Search enum entires which one which matches
	if(ValueToName(aValue) == 0)
		{
		dbg << Log::Endl();
		dbg << Log::Indent() << "Unknown value for enum " << iName << " value " << TUint32(aValue) << Log::Endl();
		//		FatalError();
		}

	iValue = aValue;
	return;
}

template<typename T>
void EncDecEnum<T>::SetValue(const char *aName)
{
	// Search enum entires which one which matches
	const EnumEntry *p = iEnumEntries;
	while(p && p->iName)
		{
		// We ignore case when matching enums
		if(strcasecmp(aName, p->iName) == 0)
			{
			// Found it
			iValue = p->iValue;
			return;
			}
		++p;
		}

	dbg << Log::Endl();
	dbg << Log::Indent() << "Unknown value for enum " << iName << " value '" << aName << "'" << Log::Endl();
	std::string str(aName);
	iValue = ReadUnsignedNumber(str,sizeof(T));
}

template<typename T>
const char *EncDecEnum<T>::ValueToName(const T &aValue) const
{
	if(iEnumEntries == 0) FatalError();
	const EnumEntry *p = iEnumEntries;
	while(p && p->iName)
		{
		if(p->iValue == aValue)
			{
			return p->iName;
			}
		++p;
		}
	return 0;
}


// end of file

