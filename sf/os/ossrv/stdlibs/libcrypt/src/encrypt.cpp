/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains implementation for encryption/decryption.
*
*/


// INCLUDE FILES

#define EMULATOR ((defined(__WINS__) || defined(__WINSCW__)))

#include <e32def.h>
//#include <des.h>
#include <hash.h>
#include <errno.h>
#include <string.h>

// EXTERNAL FUNCTION PROTOTYPES
extern "C" char *crypt_des(const char *key, const char *setting);
extern "C" char *crypt_md5(const char *pw, const char *salt);

// LOCAL CONSTANTS AND MACROS
#define BYTE_SIZE	8
#define ENCRYPTION  0
#define DECRYPTION  1

// STATIC DATA
#if !EMULATOR
TBuf8<BYTE_SIZE> desKey;				// For persistence between calls 
										// to setkey() and encrypt()
static TInt bSetkeyInvoked = 0;
#else
#include <sys/types.h>
#include "wsd_solution.h"
#define bSetkeyInvoked (GetGlobals()->bSetkeyInvoked)
#endif

// LOCAL FUNCTION PROTOTYPES
static unsigned char GetByte(const char *bitVector);
static void DesEncryptionL(const TDes8& aKey, TDes8& aInputBlock);
static void DesDecryptionL(const TDes8& aKey, TDes8& aInputBlock);

// LOCAL class declaration
class CEncDecHack : public CBase
{
public:
	virtual void Transform(TDes8& aBlock){}
};

typedef CEncDecHack* (*LookupFuncEncDecObjCreator)(const TDesC8& aKey, TBool aCheckWeakKey);

_LIT(KCryptoDll,"cryptography.dll");

// -----------------------------------------------------------------------------
// function_name: _setkey
//
// Prepares a byte array for the key from the contents of the incoming bit vector.
// Key thus constructed is statically stored for use during encryption/decryption.
//
// Returns: void
// -----------------------------------------------------------------------------
//
extern "C"
void _setkey (const char *key)
{
#if !EMULATOR
    // Reset the contents of the 'key' descriptor
    desKey.Delete(0,desKey.Length());
#endif

#if !EMULATOR    
	// Pack the contents of the bit vector into a TDes derived object
	for( int i = 0 ; i < BYTE_SIZE ; ++i)
	{
		desKey.Append( GetByte( &key[i * BYTE_SIZE] ) );
	}
#else
	for(int i=0 ; i<64 ; ++i)
	{
		(GetGlobals()->desKey)[i] = key[i];
	}
#endif

	bSetkeyInvoked = 1;
}

// -----------------------------------------------------------------------------
// function_name: _encrypt
//
// Performs either encryption or decryption of the data block. Prior to invoking
// Symbian OS cryptography APIs for encryptions/decryption, this function
// packs the contents of the bit vector into a byte array of size eight. The byte
// array obtained after encryption/decryption is unpacked to present the output
// in the form of a bit vector. The incoming data block is modified in place 
// during the process.
//
// Assumption: User of the libcrypt library is expected the create a cleanupstack
//
// Returns: void
// -----------------------------------------------------------------------------
//
extern "C"
void _encrypt (char block[], int edflag)
{
#if EMULATOR
	TBuf8<BYTE_SIZE> desKey;
#endif

	// Determine if setkey() is invoked by the user
	if(!bSetkeyInvoked)
	{
		// Initialize the key with default values
		for(int i = 0 ; i < BYTE_SIZE ; ++i)
		{
			desKey.Append((unsigned char)0);
		}
		bSetkeyInvoked = 1;
	}
#if EMULATOR
	else
	{
		for(int i=0 ; i<BYTE_SIZE ; ++i)
		{
			desKey.Append( GetByte( (const char*)&(GetGlobals()->desKey)[i * BYTE_SIZE] ));
		}
	}
#endif

	// Determine whether encryption or decryption is requested
	if(edflag != ENCRYPTION)
	{
		if(edflag != DECRYPTION)
		{
			// Unrecognized flag parameter
			errno = EPERM;
			return;
		}
	}
	
	// Pack the contents of the input bit vector into a "byte" array
	TBuf8<BYTE_SIZE> inputBlock;
	TInt nIterator;
	for(nIterator = 0 ; nIterator < BYTE_SIZE ; ++nIterator)
	{
		inputBlock.Append( GetByte( &block[nIterator * BYTE_SIZE] ) );
	}
	
	TInt error = KErrNone;
	typedef void (*DesOperation)(const TDes8&, TDes8&);
	DesOperation funcOperationL = NULL;

	switch(edflag)
	{
		case ENCRYPTION:		  // Encryption
		funcOperationL = DesEncryptionL;
		break;

		case DECRYPTION:		  // Decryption
		funcOperationL = DesDecryptionL;
		break;
	}

	TRAP(error, (*funcOperationL)(desKey, inputBlock));
	
	if(error == KErrNone)
	{
		unsigned char chTemp;
		int k = 0;

		// Create the bit vector from the "byte" array (unpack)
		for(int i = 0 ; i < BYTE_SIZE ; ++i)
		{
			chTemp = inputBlock[i];
			for(int j = 0 ; j < BYTE_SIZE ; ++j)
			{
				block[k++] = ((chTemp & 0x80) >> 7);
				chTemp <<= 1;
			}
		}
	}
	else
	{
		// Set the errno flag to indicate failure
		errno = EPERM;
	}
}

// -----------------------------------------------------------------------------
// function_name: _crypt
//
// Uses MD5-based algorithm or DES encryption mechanism to encode a constant
// string using "key" as the key. Salt determines the algorithm to be used.
//
// Returns: pointer to a static data buffer containing the encoded "string"
// -----------------------------------------------------------------------------
//
extern "C" 
char* _crypt (const char *key, const char *salt)
{
	// Identify the algorithm to be used as part of crypt
	if(strstr(salt, "$1$"))
	{
		// MD5-based algorithm
		return crypt_md5(key, salt);
	}
	else
	{
		return crypt_des(key, salt);
	}
}

// -----------------------------------------------------------------------------
// function_name: GetByte
//
// Packs the "bits" in the bit vector into a byte
//
// Returns: Byte composed of the bits from the bit vector
// -----------------------------------------------------------------------------
//
LOCAL_C unsigned char GetByte(const char *bitVector)
{
	unsigned char chTemp = 0;

	for(int nIterator = 0 ; nIterator < BYTE_SIZE ; ++nIterator)
	{
		chTemp |= ( bitVector[nIterator] << (BYTE_SIZE - nIterator - 1) );
	}
	return chTemp;
}

// -----------------------------------------------------------------------------
// function_name: DesEncryptionL
//
// Function to encrypt the input data bytes by invoking Symbian OS API for
// DES algorithm for encryption
//
// Assumption: 1. BLOCKSIZE within the cryptography library is 8 for 
// 				  DES encryption
//			   2. The input key is not checked against a set of known
//				  weak key values
//
// Returns: void, however, this function leaves if there is insufficient
//          memory
// -----------------------------------------------------------------------------
//
LOCAL_C void DesEncryptionL(const TDes8& aKey, TDes8& aInputBlock)
{
	// Construct the encryptor object
/*	CDESEncryptor *pEncryptor = CDESEncryptor::NewL(aKey, EFalse);
	
	if(!pEncryptor)
	{
		User::Leave(KErrNoMemory);
	}
	
	// Invoke DES trasnformation to encrypt the input data
	pEncryptor->Transform(aInputBlock);
	
	delete pEncryptor;
*/
	RLibrary library;
    User::LeaveIfError(library.Load(KCryptoDll));
    
    #ifdef __WINSCW__
   	TLibraryFunction func = library.Lookup(102); // CDESEncryptor::NewL
    #else
    TLibraryFunction func = library.Lookup(59);  //CDESEncryptor::NewL
	#endif // ifdef __WINSCW__
	
    if (func == NULL)
    {
        library.Close();
        User::Leave(KErrNotFound);
    }
    LookupFuncEncDecObjCreator  objCreatorFuncion = reinterpret_cast<LookupFuncEncDecObjCreator> (func);
    CEncDecHack* pEncryptor = reinterpret_cast<CEncDecHack*>(objCreatorFuncion(aKey, EFalse));
    pEncryptor->Transform(aInputBlock);
    delete pEncryptor;
    library.Close();
}

// -----------------------------------------------------------------------------
// function_name: DesDecryptionL
//
// Function to encrypt the input data bytes by invoking Symbian OS API for
// DES algorithm for decryption
//
// Assumption: 1. BLOCKSIZE within the cryptography library is 8 for 
// 				  DES decryption
//			   2. The input key is not checked against a set of known
//				  weak key values
//
// Returns: void, however, this function leaves if there is insufficient
//          memory
// -----------------------------------------------------------------------------
//
LOCAL_C void DesDecryptionL(const TDes8& aKey, TDes8& aInputBlock)
{
	// Construct the decryptor object
/*	CDESDecryptor *pDecryptor = CDESDecryptor::NewL(aKey, EFalse);
	
	if(!pDecryptor)
	{
		User::Leave(KErrNoMemory);
	}
	
	// Invoke DES decryption on the cipher text
	pDecryptor->Transform(aInputBlock);
	
	delete pDecryptor;
*/
    RLibrary library;
    User::LeaveIfError(library.Load(KCryptoDll));
    
	#ifdef __WINSCW__
    TLibraryFunction func = library.Lookup(101); // CDESDecryptor::NewL
	#else
    TLibraryFunction func = library.Lookup(57);  //CDESDecryptor::NewL
	#endif // ifdef __WINSCW
	
    if (func == NULL)
    {
        library.Close();
        return;
    }
    LookupFuncEncDecObjCreator  objCreatorFuncion = reinterpret_cast<LookupFuncEncDecObjCreator> (func);
    CEncDecHack* pDecryptor = reinterpret_cast<CEncDecHack*>(objCreatorFuncion(aKey, EFalse));
    pDecryptor->Transform(aInputBlock);
    delete pDecryptor;
    library.Close();
}

extern "C" {

// -----------------------------------------------------------------------------
// function_name: Deallocate2DimensionalUchar
//
// To deallocate storage alloted for the two dimensional array
//
// Returns: void
// -----------------------------------------------------------------------------
//
void Deallocate2DimensionalUchar(unsigned char **buffer, int row)
{
		int m;
		for(m=0 ; m<row ; ++m)
		{
			User::Free((TAny *)buffer[m]);
		}
		User::Free((TAny *)buffer);
}
	
// -----------------------------------------------------------------------------
// function_name: Deallocate2DimensionalUint
//
// To deallocate storage alloted for the two dimensional array
//
// Returns: void
// -----------------------------------------------------------------------------
//
void Deallocate2DimensionalUint(__uint32_t **buffer, int row)
{
		int m;
		for(m=0 ; m<row ; ++m)
		{
			User::Free((TAny *)buffer[m]);
		}
		User::Free((TAny *)buffer);
}

// -----------------------------------------------------------------------------
// function_name: Allocate2DimensionalUchar
//
// Function to allocate storage for a two dimensional array from the heap. 
// If heap exhaustion occurs during the course of allocating memory to the array,
// the previously allocated storage will be deleted prior to returning to the
// caller.
//
// Returns: non-zero if allocation is successful, 0 if it fails
// -----------------------------------------------------------------------------
//
int Allocate2DimensionalUchar(unsigned char ***buffer, int row, int column)
{
	*buffer = (unsigned char **)User::Alloc(row * sizeof(unsigned char *));
	if(NULL == *buffer)
	{
		return 0;
	}
	for(int m=0 ; m<row ; ++m)
	{
		(*buffer)[m] = (unsigned char *)User::Alloc(column * sizeof(unsigned char));
		if(NULL == (*buffer)[m])
		{
			// Insufficient heap memory
			if(m)
			{
				// Deallocate the previously allocated storage
				Deallocate2DimensionalUchar(*buffer, m);
			}
			return 0;
		}
	}
	
	return 1;
}

// -----------------------------------------------------------------------------
// function_name: Allocate2DimensionalUint
//
// Function to allocate storage for a two dimensional array of type unsigned int
// from the heap. If heap exhaustion occurs while allocating memory to the array,
// the previously allocated storage will be deleted prior to returning to the
// caller.
//
// Returns: non-zero if allocation is successful, 0 if it fails
// -----------------------------------------------------------------------------
//
int Allocate2DimensionalUint(__uint32_t ***buffer, int row, int column)
{
	*buffer = (__uint32_t **)User::Alloc(row * sizeof(__uint32_t *));
	if(NULL == *buffer)
	{
		return 0;
	}
	for(int m=0 ; m<row ; ++m)
	{
		(*buffer)[m] = (__uint32_t *)User::Alloc(column * sizeof(__uint32_t));
		if(NULL == (*buffer)[m])
		{
			// Insufficient heap memory
			if(m)
			{
				// Deallocate the previously allocated storage
				Deallocate2DimensionalUint(*buffer, m);
			}
			return 0;
		}
	}
	
	return 1;
}

}	// <<end extern "C">>
