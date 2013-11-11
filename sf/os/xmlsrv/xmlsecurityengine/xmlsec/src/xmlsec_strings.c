/** 
 * XML Security Library (http://www.aleksey.com/xmlsec).
 *
 * All the string constants.
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#include "xmlsec_globals.h"

#include <stdapis/libxml2/libxml2_tree.h>


#include "xmlsec_xmlsec.h"
#include "xmlsec_strings.h"
		
const xmlSecGlobalConstData xmlSecGlobalConsts = {

/*************************************************************************
 *
 * Global Namespaces
 *
 ************************************************************************/
BAD_CAST "http://www.aleksey.com/xmlsec/2002",
BAD_CAST "http://www.w3.org/2000/09/xmldsig#",
BAD_CAST "http://www.w3.org/2001/04/xmlenc#",
BAD_CAST "http://www.w3.org/2002/03/xkms#",
BAD_CAST "http://www.w3.org/TR/1999/REC-xpath-19991116",
BAD_CAST "http://www.w3.org/2002/06/xmldsig-filter2",
BAD_CAST "http://www.w3.org/2001/04/xmldsig-more/xptr",
BAD_CAST "http://schemas.xmlsoap.org/soap/envelope/",
BAD_CAST "http://www.w3.org/2002/06/soap-envelope",

/*************************************************************************
 *
 * DSig Nodes
 *
 ************************************************************************/
BAD_CAST "Signature",
BAD_CAST "SignedInfo",
BAD_CAST "CanonicalizationMethod",
BAD_CAST "SignatureMethod",
BAD_CAST "SignatureValue",
BAD_CAST "DigestMethod",
BAD_CAST "DigestValue",
BAD_CAST "Object",
BAD_CAST "Manifest",
BAD_CAST "SignatureProperties",

/*************************************************************************
 *
 * Encryption Nodes
 *
 ************************************************************************/
BAD_CAST "EncryptedData",
BAD_CAST "EncryptionMethod",
BAD_CAST "EncryptionProperties",
BAD_CAST "EncryptionProperty",
BAD_CAST "CipherData",
BAD_CAST "CipherValue",
BAD_CAST "CipherReference",
BAD_CAST "ReferenceList",
BAD_CAST "DataReference",
BAD_CAST "KeyReference",

BAD_CAST "CarriedKeyName",

BAD_CAST "http://www.w3.org/2001/04/xmlenc#Content",
BAD_CAST "http://www.w3.org/2001/04/xmlenc#Element",

/*************************************************************************
 *
 * XKMS Nodes
 *
 ************************************************************************/
#ifndef XMLSEC_NO_XKMS
BAD_CAST "result-response",
BAD_CAST "status-request",
BAD_CAST "locate-request",
BAD_CAST "validate-request",
BAD_CAST "compound-request",

BAD_CAST "Result",
BAD_CAST "StatusRequest",
BAD_CAST "StatusResult",
BAD_CAST "LocateRequest",
BAD_CAST "LocateResult",
BAD_CAST "ValidateRequest",
BAD_CAST "ValidateResult",
BAD_CAST "CompoundRequest",
BAD_CAST "CompoundResult",

BAD_CAST "MessageExtension",
BAD_CAST "OpaqueClientData",
BAD_CAST "ResponseMechanism",
BAD_CAST "RespondWith",
BAD_CAST "PendingNotification",
BAD_CAST "QueryKeyBinding",
BAD_CAST "KeyUsage",
BAD_CAST "UseKeyWith",
BAD_CAST "TimeInstant",
BAD_CAST "RequestSignatureValue",
BAD_CAST "UnverifiedKeyBinding",
BAD_CAST "ValidityInterval",
BAD_CAST "Status",
BAD_CAST "ValidReason",
BAD_CAST "InvalidReason",
BAD_CAST "IndeterminateReason",

BAD_CAST "Service",
BAD_CAST "Nonce",
BAD_CAST "OriginalRequestId",
BAD_CAST "ResponseLimit",
BAD_CAST "Mechanism[",
BAD_CAST "Identifier",
BAD_CAST "Application",
BAD_CAST "ResultMajor",
BAD_CAST "ResultMinor",
BAD_CAST "RequestId",
BAD_CAST "NotBefore",
BAD_CAST "NotOnOrAfter",
BAD_CAST "Time",
BAD_CAST "StatusValue",

BAD_CAST "Pending",
BAD_CAST "Represent",
BAD_CAST "RequestSignatureValue",

BAD_CAST "KeyName",
BAD_CAST "KeyValue",
BAD_CAST "X509Cert",
BAD_CAST "X509Chain",
BAD_CAST "X509CRL",
BAD_CAST "OCSP",
BAD_CAST "RetrievalMethod",
BAD_CAST "PGP",
BAD_CAST "PGPWeb",
BAD_CAST "SPKI",
BAD_CAST "PrivateKey",

BAD_CAST "Success",
BAD_CAST "Failed",
BAD_CAST "Pending",

BAD_CAST "Encryption",
BAD_CAST "Signature",
BAD_CAST "Exchange",

BAD_CAST "Valid",
BAD_CAST "Invalid",
BAD_CAST "Indeterminate",

BAD_CAST "IssuerTrust",
BAD_CAST "RevocationStatus",
BAD_CAST "ValidityInterval",
BAD_CAST "Signature",

BAD_CAST "Success",
BAD_CAST "VersionMismatch",
BAD_CAST "Sender",
BAD_CAST "Receiver",
BAD_CAST "Represent",
BAD_CAST "Pending",

BAD_CAST "NoMatch",
BAD_CAST "TooManyResponses",
BAD_CAST "Incomplete",
BAD_CAST "Failure",
BAD_CAST "Refused",
BAD_CAST "NoAuthentication",
BAD_CAST "MessageNotSupported",
BAD_CAST "UnknownResponseId",
BAD_CAST "NotSynchronous",

BAD_CAST "MessageNotSupported",
BAD_CAST "BadMessage",

BAD_CAST "en",
BAD_CAST "Unsupported SOAP version",
BAD_CAST "Unable to process %s",
BAD_CAST "Service temporarily unable",
BAD_CAST "%s message not supported",
BAD_CAST "%s message invalid",

BAD_CAST "plain",
BAD_CAST "soap-1.1",
BAD_CAST "soap-1.2",

#endif /* XMLSEC_NO_XKMS */

/*************************************************************************
 *
 * KeyInfo Nodes
 *
 ************************************************************************/
BAD_CAST "KeyInfo",
BAD_CAST "Reference",
BAD_CAST "Transforms",
BAD_CAST "Transform",

/*************************************************************************
 *
 * Attributes
 *
 ************************************************************************/
BAD_CAST "Id",
BAD_CAST "URI",
BAD_CAST "Type",
BAD_CAST "MimeType",
BAD_CAST "Encoding",
BAD_CAST "Algorithm",
BAD_CAST "Filter",
BAD_CAST "Recipient",
BAD_CAST "Target",

/*************************************************************************
 *
 * AES strings
 *
 ************************************************************************/
BAD_CAST "aes",
BAD_CAST "AESKeyValue",
BAD_CAST "http://www.aleksey.com/xmlsec/2002#AESKeyValue",

BAD_CAST "aes128-cbc",
BAD_CAST "http://www.w3.org/2001/04/xmlenc#aes128-cbc",

BAD_CAST "aes192-cbc",
BAD_CAST "http://www.w3.org/2001/04/xmlenc#aes192-cbc",

BAD_CAST "aes256-cbc",
BAD_CAST "http://www.w3.org/2001/04/xmlenc#aes256-cbc",

BAD_CAST "kw-aes128",
BAD_CAST "http://www.w3.org/2001/04/xmlenc#kw-aes128",

BAD_CAST "kw-aes192",
BAD_CAST "http://www.w3.org/2001/04/xmlenc#kw-aes192",

BAD_CAST "kw-aes256",
BAD_CAST "http://www.w3.org/2001/04/xmlenc#kw-aes256",

/*************************************************************************
 *
 * BASE64 strings
 *
 ************************************************************************/
BAD_CAST "base64",
BAD_CAST "http://www.w3.org/2000/09/xmldsig#base64",

/*************************************************************************
 *
 * C14N strings
 *
 ************************************************************************/
BAD_CAST "c14n",
BAD_CAST "http://www.w3.org/TR/2001/REC-xml-c14n-20010315",

BAD_CAST "c14n-with-comments",
BAD_CAST "http://www.w3.org/TR/2001/REC-xml-c14n-20010315#WithComments",

BAD_CAST "exc-c14n",
BAD_CAST "http://www.w3.org/2001/10/xml-exc-c14n#",

BAD_CAST "exc-c14n-with-comments",
BAD_CAST "http://www.w3.org/2001/10/xml-exc-c14n#WithComments",

BAD_CAST "http://www.w3.org/2001/10/xml-exc-c14n#",
BAD_CAST "http://www.w3.org/2001/10/xml-exc-c14n#WithComments",

BAD_CAST "InclusiveNamespaces",
BAD_CAST "PrefixList",
/*************************************************************************
 *
 * DES strings
 *
 ************************************************************************/
BAD_CAST "des",
BAD_CAST "DESKeyValue",
BAD_CAST "http://www.aleksey.com/xmlsec/2002#DESKeyValue",

BAD_CAST "tripledes-cbc",
BAD_CAST "http://www.w3.org/2001/04/xmlenc#tripledes-cbc",

BAD_CAST "kw-tripledes",
BAD_CAST "http://www.w3.org/2001/04/xmlenc#kw-tripledes",

/*************************************************************************
 *
 * DSA strings
 *
 ************************************************************************/
BAD_CAST "dsa",
BAD_CAST "DSAKeyValue",
BAD_CAST "http://www.w3.org/2000/09/xmldsig#DSAKeyValue",
BAD_CAST "P",
BAD_CAST "Q",
BAD_CAST "G",
BAD_CAST "J",
BAD_CAST "X",
BAD_CAST "Y",
BAD_CAST "Seed",
BAD_CAST "PgenCounter",

BAD_CAST "dsa-sha1",
BAD_CAST "http://www.w3.org/2000/09/xmldsig#dsa-sha1",

/*************************************************************************
 *
 * EncryptedKey
 *
 ************************************************************************/
BAD_CAST "enc-key",
BAD_CAST "EncryptedKey",
BAD_CAST "http://www.w3.org/2001/04/xmlenc#EncryptedKey",

/*************************************************************************
 *
 * Enveloped transform strings
 *
 ************************************************************************/
BAD_CAST "enveloped-signature",
BAD_CAST "http://www.w3.org/2000/09/xmldsig#enveloped-signature",

/*************************************************************************
 *
 * HMAC strings
 *
 ************************************************************************/
BAD_CAST "hmac",
BAD_CAST "HMACKeyValue",
BAD_CAST "http://www.aleksey.com/xmlsec/2002#HMACKeyValue",

BAD_CAST "HMACOutputLength",

BAD_CAST "hmac-md5",
BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#hmac-md5",

BAD_CAST "hmac-ripemd160",
BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#hmac-ripemd160",

BAD_CAST "hmac-sha1",
BAD_CAST "http://www.w3.org/2000/09/xmldsig#hmac-sha1",

BAD_CAST "hmac-sha224",
BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#hmac-sha224",

BAD_CAST "hmac-sha256",
BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#hmac-sha256",

BAD_CAST "hmac-sha384",
BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#hmac-sha384",

BAD_CAST "hmac-sha512",
BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#hmac-sha512",

/*************************************************************************
 *
 * KeyName strings
 *
 ************************************************************************/
BAD_CAST "key-name",
BAD_CAST "KeyName",

/*************************************************************************
 *
 * KeyValue strings
 *
 ************************************************************************/
BAD_CAST "key-value",
BAD_CAST "KeyValue",

/*************************************************************************
 *
 * Memory Buffer strings
 *
 ************************************************************************/
BAD_CAST "membuf-transform",

/*************************************************************************
 *
 * MD5 strings
 *
 ************************************************************************/
BAD_CAST "md5",
BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#md5",

/*************************************************************************
 *
 * RetrievalMethod
 *
 ************************************************************************/
BAD_CAST "retrieval-method",
BAD_CAST "RetrievalMethod",

/*************************************************************************
 *
 * RIPEMD160 strings
 *
 ************************************************************************/
BAD_CAST "ripemd160",
BAD_CAST "http://www.w3.org/2001/04/xmlenc#ripemd160",

/*************************************************************************
 *
 * RSA strings
 *
 ************************************************************************/
BAD_CAST "rsa",
BAD_CAST "RSAKeyValue",
BAD_CAST "http://www.w3.org/2000/09/xmldsig#RSAKeyValue",
BAD_CAST "Modulus",
BAD_CAST "Exponent",
BAD_CAST "PrivateExponent",

BAD_CAST "rsa-md5",
BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#rsa-md5",

BAD_CAST "rsa-ripemd160",
BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#rsa-ripemd160",

BAD_CAST "rsa-sha1",
BAD_CAST "http://www.w3.org/2000/09/xmldsig#rsa-sha1",

BAD_CAST "rsa-sha224",
BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#rsa-sha224",

BAD_CAST "rsa-sha256",
BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#rsa-sha256",

BAD_CAST "rsa-sha384",
BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#rsa-sha384",

BAD_CAST "rsa-sha512",
BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#rsa-sha512",

BAD_CAST "rsa-1_5",
BAD_CAST "http://www.w3.org/2001/04/xmlenc#rsa-1_5",

BAD_CAST "rsa-oaep-mgf1p",
BAD_CAST "http://www.w3.org/2001/04/xmlenc#rsa-oaep-mgf1p",
BAD_CAST "OAEPparams",

/*************************************************************************
 *
 * SHA1 strings
 *
 ************************************************************************/
BAD_CAST "sha1",
BAD_CAST "http://www.w3.org/2000/09/xmldsig#sha1",

BAD_CAST "sha224",
BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#sha224",

BAD_CAST "sha256",
BAD_CAST "http://www.w3.org/2001/04/xmlenc#sha256",

BAD_CAST "sha384",
BAD_CAST "http://www.w3.org/2001/04/xmldsig-more#sha384",

BAD_CAST "sha512",
BAD_CAST "http://www.w3.org/2001/04/xmlenc#sha512",

/*************************************************************************
 *
 * X509 strings
 *
 ************************************************************************/
BAD_CAST "x509",
BAD_CAST "X509Data",
BAD_CAST "http://www.w3.org/2000/09/xmldsig#X509Data",

BAD_CAST "X509Certificate",
BAD_CAST "X509CRL",
BAD_CAST "X509SubjectName",
BAD_CAST "X509IssuerSerial",
BAD_CAST "X509IssuerName",
BAD_CAST "X509SerialNumber",
BAD_CAST "X509SKI",

BAD_CAST "raw-x509-cert",
BAD_CAST "http://www.w3.org/2000/09/xmldsig#rawX509Certificate",

BAD_CAST "x509-store",

/*************************************************************************
 *
 * PGP strings
 *
 ************************************************************************/
BAD_CAST "pgp",
BAD_CAST "PGPData",
BAD_CAST "http://www.w3.org/2000/09/xmldsig#PGPData",

/*************************************************************************
 *
 * SPKI strings
 *
 ************************************************************************/
BAD_CAST "spki",
BAD_CAST "SPKIData",
BAD_CAST "http://www.w3.org/2000/09/xmldsig#SPKIData",

/*************************************************************************
 *
 * XPath/XPointer strings
 *
 ************************************************************************/
BAD_CAST "xpath",
BAD_CAST "XPath",

BAD_CAST "xpath2",
BAD_CAST "XPath",
BAD_CAST "intersect",
BAD_CAST "subtract",
BAD_CAST "union",

BAD_CAST "xpointer",
BAD_CAST "XPointer",

/*************************************************************************
 *
 * Xslt strings
 *
 ************************************************************************/
BAD_CAST "xslt",
BAD_CAST "http://www.w3.org/TR/1999/REC-xslt-19991116",

#ifndef XMLSEC_NO_SOAP
/*************************************************************************
 *
 * SOAP 1.1/1.2 strings
 *
 ************************************************************************/
BAD_CAST "Envelope",
BAD_CAST "Header",
BAD_CAST "Body",
BAD_CAST "Fault",
BAD_CAST "faultcode",
BAD_CAST "faultstring",
BAD_CAST "faultactor",
BAD_CAST "detail",
BAD_CAST "Code",
BAD_CAST "Reason",
BAD_CAST "Node",
BAD_CAST "Role",
BAD_CAST "Detail",
BAD_CAST "Value",
BAD_CAST "Subcode",
BAD_CAST "Text",


BAD_CAST "VersionMismatch",
BAD_CAST "MustUnderstand",
BAD_CAST "Client",
BAD_CAST "Server",
BAD_CAST "Receiver",
BAD_CAST "Sender",
BAD_CAST "DataEncodingUnknown",


#endif /* XMLSEC_NO_SOAP */

/*************************************************************************
 *
 * Utility strings
 *
 ************************************************************************/
BAD_CAST "",
BAD_CAST "\n"


};	//	xmlSecGlobalConsts			
EXPORT_C
const	xmlSecGlobalConstData*	xmlSecGetGlobalConsts()		
{				
	return	&xmlSecGlobalConsts;			

}
			






