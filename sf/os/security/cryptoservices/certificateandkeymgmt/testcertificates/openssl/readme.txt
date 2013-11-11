The oscpResponder.pl script requires the ocsp responder to support DSA

the following patch can be applied to openssl 0.9.7b or 0.9.7c:

--- openssl-0.9.7b.ORIG/apps/ocsp.c	2003-03-26 02:47:06.000000000 +0200
+++ openssl-0.9.7b/apps/ocsp.c	2004-02-22 16:11:18.000000000 +0200
@@ -1115,7 +1115,16 @@
 
 	OCSP_copy_nonce(bs, req);
 		
-	OCSP_basic_sign(bs, rcert, rkey, EVP_sha1(), rother, flags);
+	{
+		/*in case of DSA keys we should use EVP_dss1()*/
+		const EVP_MD *evp_md;
+		/*
+		 * - EVP_dss1 only or can be EVP_dss for some DSA keys ?
+		 * - should we use method EVP_PKEY_type() ?
+		 */
+		evp_md = (rkey->type == EVP_PKEY_DSA) ? EVP_dss1() : EVP_sha1();
+		OCSP_basic_sign(bs, rcert, rkey, evp_md, rother, flags);
+	}
 
 	*resp = OCSP_response_create(OCSP_RESPONSE_STATUS_SUCCESSFUL, bs);
 
