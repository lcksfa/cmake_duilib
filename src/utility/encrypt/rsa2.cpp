// #include "../pch.h"
#include "encrypt/rsa2.h"
#include "openssl/sha.h"
#include "openssl/rsa.h"
#include "openssl/pem.h"
#include <string>
#define XRSA_KEY_BITS 2048

std::string base64Encode(const unsigned char *bytes, int len)
{

    BIO *bmem = NULL;
    BIO *b64 = NULL;
    BUF_MEM *bptr = NULL;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, bytes, len);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    std::string str = std::string(bptr->data, bptr->length);
    BIO_free_all(b64);
    return str;
}

std::string sha256WithRsaSign(const std::string &content, const std::string &strKey)
{

    std::string signed_str;
    const char *key_cstr = strKey.c_str();
    int key_len = strlen(key_cstr);
    BIO *p_key_bio = BIO_new_mem_buf((void *) key_cstr, key_len);
    RSA *p_rsa = PEM_read_bio_RSAPrivateKey(p_key_bio, NULL, NULL, NULL);

    if (p_rsa != NULL) {

        const char *cstr = content.c_str();
        unsigned char hash[SHA256_DIGEST_LENGTH] = {0};
        SHA256((unsigned char *) cstr, strlen(cstr), hash);
        unsigned char sign[XRSA_KEY_BITS / 8] = {0};
        unsigned int sign_len = sizeof(sign);
        int r = RSA_sign(NID_sha256, hash, SHA256_DIGEST_LENGTH, sign, &sign_len, p_rsa);

        if (0 != r /*&& sizeof(sign) == sign_len*/) {
            signed_str = base64Encode(sign, sign_len);
        }
    }

    RSA_free(p_rsa);
    BIO_free(p_key_bio);
    return signed_str;
}

std::string CRSA2::sign(const std::string &srcstring, const std::string &pKey)
{
    return sha256WithRsaSign(srcstring, pKey);
}