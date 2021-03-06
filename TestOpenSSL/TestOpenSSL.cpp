// TestOpenSSL.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#include <string.h>

#include "openssl/evp.h"
#include "openssl/err.h"
#include "openssl/ec.h"
#include "openssl/sha.h"
#include "openssl/x509.h"
#include "openssl/pem.h"
#include "openssl/aes.h"

#include "CMyRand.h"

using namespace std;


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
	fopen_s(&fp, "test_key.pem", "wb");
	PEM_write_PrivateKey(fp, pKey, EVP_aes_256_cbc_hmac_sha256(),
						 (unsigned char *)"my password", 11, NULL, NULL);
	fclose(fp);

	// write public key
	fopen_s(&fp, "test_cert.pem", "wb");
	PEM_write_X509(fp, x509);
	fclose(fp);

	// write cerification.
	fopen_s(&fp, "test_cert.cer", "wb");
	i2d_X509_fp(fp, x509);
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


void PrintByteBuffer(const char *strName, unsigned char *bData, int size)
{
	puts("");
	puts(strName);
	for (int i = 0; i < size; i++)
	{
		printf("%02x ", bData[i]);
		if (i % 16 == 15)		puts("");
	}
	puts("\n---------------------------------------");
}


void AESEncodingDecoding()
{
	SHA256_CTX		c;
	AES_KEY			aes_key = {0};
	unsigned char	in[64];
	unsigned char	out[64];
	unsigned char	restore[64];
	int				seed = time(NULL);
	const char		*key = "This is my AES key by tebek... "\
						   "One Little Two Little Three Little Endian? Indian!";
	unsigned char	m[SHA256_DIGEST_LENGTH];
	unsigned char	iv[SHA256_DIGEST_LENGTH];
	unsigned char	iv2[SHA256_DIGEST_LENGTH];

	
	// Generate symmetric key 
	SHA256_Init(&c);
	SHA256_Update(&c, key, strlen(key));
	SHA256_Final(m, &c);
	OPENSSL_cleanse(&c, sizeof(c));

	// Generate iv
	SHA256_Init(&c);
	SHA256_Update(&c, "IV test 1", 9);
	SHA256_Final(iv, &c);
	OPENSSL_cleanse(&c, sizeof(c));

	srand(seed);
	memcpy(iv2, iv, SHA256_DIGEST_LENGTH);
	
	// fill input data randomly
	for (int i = 0; i < 64; i+=2)	*(short *)(in+i) = rand();

	// Encrypt
	AES_set_encrypt_key(m, SHA256_DIGEST_LENGTH * 8, &aes_key);
	AES_cbc_encrypt(in, out, 64, &aes_key, iv, AES_ENCRYPT);
	
	// Decrypt
	AES_set_decrypt_key(m, SHA256_DIGEST_LENGTH * 8, &aes_key);
	AES_cbc_encrypt(out, restore, 64, &aes_key, iv2, AES_DECRYPT);

	// Display Result
	PrintByteBuffer("in", in, 64);
	PrintByteBuffer("out", out, 64);
	PrintByteBuffer("restore", restore, 64);

	if (memcmp(in, restore, 64) == 0)	puts("AES Test Successed !");
	else								puts("AES Test Failed !");
}


void TestEVP_Cypher()
{
	string				strInput = "Yoda said, Do or do not. There is no try.";
	string				strCyper, strOutput;
	SHA256_CTX			c;
	const char			*key = "This is my AES key by tebek... "\
						"One Little Two Little Three Little Endian? Indian!";
	unsigned char		keyCipher[32], iv[16];
	unsigned char		m[SHA256_DIGEST_LENGTH];
	EVP_CIPHER_CTX		*pCtxCipher;
	const EVP_CIPHER	*pCipher;
	CMyRand				rand(MT19937, UNIFORM_INT, 1, 0xFF);
	unsigned char		buf[128];
	unsigned char		bufEncrypted[128];
	unsigned char		bufOut[128];
	int					len_enc, len_dec, len_final;


	SHA256_Init(&c);
	SHA256_Update(&c, key, strlen(key));
	SHA256_Final(m, &c);
	OPENSSL_cleanse(&c, sizeof(c));

	memcpy(keyCipher, m, 32);
	memcpy(iv, m, 16);

	rand.Rand(buf, 78);

	pCtxCipher = EVP_CIPHER_CTX_new();
	pCipher = EVP_get_cipherbyname("aes-256-cfb");

	// Encrypt
	EVP_CipherInit(pCtxCipher, pCipher, keyCipher, iv, 1);
	
	EVP_CipherUpdate(pCtxCipher, bufEncrypted, &len_enc, buf, 78);
	len_final = len_enc;
	EVP_CipherFinal(pCtxCipher, bufEncrypted + len_enc, &len_enc);
	len_enc += len_final;

	// Decrypt
	EVP_CipherInit(pCtxCipher, pCipher, keyCipher, iv, 0);

	EVP_CipherUpdate(pCtxCipher, bufOut, &len_dec, bufEncrypted, len_enc);
	len_final = len_dec;
	EVP_CipherFinal(pCtxCipher, bufOut + len_dec, &len_dec);
	len_dec += len_final;

	EVP_CIPHER_CTX_free(pCtxCipher);

	PrintByteBuffer("aes-256-cfb input", buf, 78);
	PrintByteBuffer("aes-256-cfb encrypt", bufEncrypted, len_enc);
	PrintByteBuffer("aes-256-cfb decrypt", bufOut, len_dec);
}


void TestEVP_MessageDigest(const char *alg, const char *pP1, const char *pP2, const char *pP3)
{
	EVP_MD_CTX *mctx;
	const EVP_MD *digest;
	digest = EVP_get_digestbyname(alg);
	unsigned char md[EVP_MAX_MD_SIZE];
	unsigned int md_len;
	
	
	mctx = EVP_MD_CTX_new();

	EVP_DigestInit_ex(mctx, digest, NULL);
	EVP_DigestUpdate(mctx, pP1, strlen(pP1));
	EVP_DigestUpdate(mctx, pP2, strlen(pP3));
	EVP_DigestUpdate(mctx, pP3, strlen(pP2));
	EVP_DigestFinal(mctx, md, &md_len);

	EVP_MD_CTX_free(mctx);

	string strName = "EVP ";
	strName += alg;
	PrintByteBuffer(strName.c_str(), md, md_len);
}


void TestEVP_DigestSignVerify(char *pMsg)
{
	EVP_MD_CTX		*mctx;
	EVP_PKEY_CTX	*pkctx;
	EVP_PKEY		*keypair;
	size_t			siglen;
	unsigned char	*sig;
	int				result;


	// RSA Generate.
	keypair = EVP_PKEY_new();

	pkctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
	EVP_PKEY_keygen_init(pkctx);
	EVP_PKEY_CTX_set_rsa_keygen_bits(pkctx, 2048);
	EVP_PKEY_keygen(pkctx, &keypair);
	EVP_PKEY_CTX_free(pkctx);

	// RSA Signing
	mctx = EVP_MD_CTX_new();
	EVP_DigestSignInit(mctx, NULL, EVP_sha256(), NULL, keypair);
	EVP_DigestSignUpdate(mctx, pMsg, strlen(pMsg));
	EVP_DigestSignFinal(mctx, NULL, &siglen);
	sig = (unsigned char *)OPENSSL_malloc(siglen);
	EVP_DigestSignFinal(mctx, sig, &siglen);

	// RSA Verify
	EVP_DigestVerifyInit(mctx, NULL, EVP_sha256(), NULL, keypair);
	EVP_DigestVerifyUpdate(mctx, pMsg, strlen(pMsg));
	result = EVP_DigestVerifyFinal(mctx, sig, siglen);
	if (result == 1)	puts("Sign Verify Success!");
	else				puts("Sign Verify False!");

	// Wrong Sig verify
	sig[1]++;
	EVP_DigestVerifyInit(mctx, NULL, EVP_sha256(), NULL, keypair);
	EVP_DigestVerifyUpdate(mctx, pMsg, strlen(pMsg));
	result = EVP_DigestVerifyFinal(mctx, sig, siglen);
	if (result == 1)	puts("Wrong Sig : Sign Verify Success!");
	else				puts("Wrong Sig : Sign Verify False!");

	// Wrong Message verify
	EVP_DigestVerifyInit(mctx, NULL, EVP_sha256(), NULL, keypair);
	EVP_DigestVerifyUpdate(mctx, "Wrong Message... Message was changed", strlen(pMsg));
	result = EVP_DigestVerifyFinal(mctx, sig, siglen);
	if (result == 1)	puts("Wrong Message : Sign Verify Success!");
	else				puts("Wrong Message : Sign Verify False!");

	OPENSSL_free(sig);
	EVP_MD_CTX_free(mctx);
	EVP_PKEY_free(keypair);
}


int main()
{
	unsigned char buf[72];
	CMyRand  rand1(STDLIB, RAW, 1, 0xF0);
	CMyRand  rand2(MSTD1988, FISHER_F, 1, 0xF0);
	CMyRand  rand3(MT19937, UNIFORM_INT, 1, 0xF0);
	CMyRand  rand4(LANLUX48B, POISSON, 1, 0xF0);
	CMyRand  rand5(KNUTH_B, PIECEWISE_LINEAR, 1, 0xF0);


	OpenSSL_add_all_algorithms();
	ERR_load_crypto_strings();

	rand1.Rand(buf, 72);		PrintByteBuffer("rand 1", buf, 72);
	rand2.Rand(buf, 72);		PrintByteBuffer("rand 2", buf, 72);
	rand3.Rand(buf, 72);		PrintByteBuffer("rand 3", buf, 72);
	rand4.Rand(buf, 72);		PrintByteBuffer("rand 4", buf, 72);
	rand5.Rand(buf, 72);		PrintByteBuffer("rand 5", buf, 72);

	SignAndVerifyTest();

	GenerateCertificatinECDSA();
	SignAndVerifyTestWithKeyFiles("key.pem", "cert.pem");

	AESEncodingDecoding();

	TestEVP_MessageDigest("md5", "This is my Test Message Digest!", "OpenSSL Used", "2018-08-09 by tebek");
	TestEVP_MessageDigest("sha256", "This is my Test Message Digest!", "OpenSSL Used", "2018-08-09 by tebek");

	TestEVP_Cypher();
	TestEVP_DigestSignVerify((char *)"When I'm with you is paradise, No place on earth could be so nice.");

    return 0;
}