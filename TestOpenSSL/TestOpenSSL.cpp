// TestOpenSSL.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#include "openssl/evp.h"
#include "openssl/err.h"
#include "openssl/ec.h"
#include "openssl/sha.h"
#include "openssl/x509.h"
#include "openssl/pem.h"


#ifdef _DEBUG
	#pragma comment (lib, "libcryptoMDd.lib")
	#pragma comment (lib, "libsslMDd.lib")
#else
	#pragma comment (lib, "libcryptoMD.lib")
	#pragma comment (lib, "libsslMD.lib")
#endif


void GenerateCertificatinECDSA()
{
	EC_KEY			*ecKey = NULL;
	EVP_PKEY		*pKey = NULL;
	X509			*x509;
	X509_NAME		*nameX509;
	int				nidEcc;
	FILE			*fp;


	// Set Key Type.
	nidEcc = OBJ_txt2nid("secp521r1");
	ecKey = EC_KEY_new_by_curve_name(nidEcc);
	if (ecKey == NULL)	ERR_print_errors_fp(stderr);

	// Generate Key.
	EC_KEY_generate_key(ecKey);

	//
	// Create Certificate
	//
	x509 = X509_new();

	// Set Elements
	ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);
	X509_gmtime_adj(X509_get_notBefore(x509), 0);			// current time
	X509_gmtime_adj(X509_get_notAfter(x509), 31536000L);	// span 60 * 60 * 24 * 365

	nameX509 = X509_get_subject_name(x509);
	X509_NAME_add_entry_by_txt(nameX509, "C", MBSTRING_ASC, (unsigned char *)"KR", -1, -1, 0);
	X509_NAME_add_entry_by_txt(nameX509, "O", MBSTRING_ASC, (unsigned char *)"TEBEK NET", -1, -1, 0);
	X509_NAME_add_entry_by_txt(nameX509, "CN", MBSTRING_ASC, (unsigned char *)"localhost", -1, -1, 0);
	X509_set_issuer_name(x509, nameX509);

	// Set Public Key x.509
	pKey = EVP_PKEY_new();
	EVP_PKEY_assign_EC_KEY(pKey, ecKey);
	X509_set_pubkey(x509, pKey);
	
	// Self Signing.
	X509_sign(x509, pKey, EVP_sha256());

	// write private key
	fopen_s(&fp, "key.pem", "wb");
	try
	{
		PEM_write_PrivateKey(fp, pKey, EVP_aes_256_cbc_hmac_sha256(),
			(unsigned char *)"my password", 11, NULL, NULL);
	}
	catch (...)
	{
		printf("%d", 123);
	}

	fclose(fp);

	// write public key
	fopen_s(&fp, "cert.pem", "wb");
	PEM_write_X509(fp, x509);
	fclose(fp);

//	EC_KEY_free(ecKey);		// maybe free ecKey at X509_free()
	EVP_PKEY_free(pKey);
	X509_free(x509);
}


void SignAndVerifyTestWithKeyFiles(const char *pathPriv, const char *pathPub)
{
	SHA256_CTX		c;
	EC_KEY			*ecKey = NULL;
	int				nidEcc;
	unsigned char	m[SHA256_DIGEST_LENGTH];
	unsigned char	sig[256];					// Must greater than ECDSA_size(ecKey)
	unsigned int	lenSig;
	int				iRet;
	FILE			*fpPub, *fpPriv;
	pem_password_cb	password;

	
	// Read Keys
	ecKey = EC_KEY_new();
	fopen_s(&fpPub, pathPub, "rb");
	fopen_s(&fpPriv, pathPriv, "rb");
	PEM_read_ECPrivateKey(fpPub, &ecKey, NULL, NULL);
	PEM_read_EC_PUBKEY(fpPub, &ecKey, NULL, NULL);
	fclose(fpPub);
	fclose(fpPriv);

	// Generate Hash for signing
	SHA256_Init(&c);
	SHA256_Update(&c, "This is Data for Signing.", 25);
	SHA256_Final(m, &c);
	OPENSSL_cleanse(&c, sizeof(c));

	// Set Key Type.
	nidEcc = OBJ_txt2nid("secp521r1");
	ecKey = EC_KEY_new_by_curve_name(nidEcc);
	if (ecKey == NULL)	ERR_print_errors_fp(stderr);

	// Generate Key.
	EC_KEY_generate_key(ecKey);

	// Sign Message Digest.
	ECDSA_sign(0, m, SHA256_DIGEST_LENGTH, sig, &lenSig, ecKey);
	iRet = ECDSA_verify(0, m, SHA256_DIGEST_LENGTH, sig, lenSig, ecKey);
	printf("Before Fake : Verify Result is %d \n", iRet);

	// Change Message Digest.
	m[0]++;
	iRet = ECDSA_verify(0, m, SHA256_DIGEST_LENGTH, sig, lenSig, ecKey);
	printf("After Fake  : Verify Result is %d \n", iRet);
	puts("\n------------------------------\n");

	EC_KEY_free(ecKey);
}


void SignAndVerifyTest()
{
	SHA256_CTX		c;
	EC_KEY			*ecKey = NULL;
	int				nidEcc;
	unsigned char	m[SHA256_DIGEST_LENGTH];
	unsigned char	sig[256];					// Must greater than ECDSA_size(ecKey)
	unsigned int	lenSig;
	int				iRet;


	// Generate Hash for signing
	SHA256_Init(&c);
	SHA256_Update(&c, "This is Data for Signing.", 25);
	SHA256_Final(m, &c);
	OPENSSL_cleanse(&c, sizeof(c));

	// Set Key Type.
	nidEcc = OBJ_txt2nid("secp521r1");
	ecKey = EC_KEY_new_by_curve_name(nidEcc);
	if (ecKey == NULL)	ERR_print_errors_fp(stderr);

	// Generate Key.
	EC_KEY_generate_key(ecKey);

	// Sign Message Digest.
	ECDSA_sign(0, m, SHA256_DIGEST_LENGTH, sig, &lenSig, ecKey);
	iRet = ECDSA_verify(0, m, SHA256_DIGEST_LENGTH, sig, lenSig, ecKey);
	printf("Before Fake : Verify Result is %d \n", iRet);

	// Change Message Digest.
	m[0]++;
	iRet = ECDSA_verify(0, m, SHA256_DIGEST_LENGTH, sig, lenSig, ecKey);
	printf("After Fake  : Verify Result is %d \n", iRet);
	puts("\n------------------------------\n");

	EC_KEY_free(ecKey);
}


int main()
{
	OpenSSL_add_all_algorithms();
	ERR_load_crypto_strings();

	SignAndVerifyTest();

	GenerateCertificatinECDSA();
	SignAndVerifyTestWithKeyFiles("key.pem", "cert.pem");

    return 0;
}