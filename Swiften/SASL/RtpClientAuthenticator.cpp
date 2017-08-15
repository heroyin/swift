/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/Log.h>
#include <Swiften/Base/Concat.h>
#include <Swiften/SASL/RtpClientAuthenticator.h>
#include <Swiften/StringCodecs/Hexify.h>

//#include <openssl/aes.h>
//#include <openssl/evp.h>
//#include <openssl/sha.h>

#include "aes.h"

namespace Swift {

	RtpClientAuthenticator::RtpClientAuthenticator(CryptoProvider* crypto, std::string key) 
		: ClientAuthenticator("RTP-SECRET"), 
		m_key(key),
		m_crypto(crypto)
	{
}

/*
void encrypt(const unsigned char* in, int inl, unsigned char *out, int* len, unsigned char * key){
	unsigned char iv[8];
	EVP_CIPHER_CTX ctx;
	//此init做的仅是将ctx内存 memset为0  
	EVP_CIPHER_CTX_init(&ctx);

	//cipher  = EVP_aes_128_ecb();  
	//原型为int EVP_EncryptInit_ex(EVP_CIPHER_CTX *ctx,const EVP_CIPHER *cipher, ENGINE *impl, const unsigned char *key, const unsigned char *iv)   
	//另外对于ecb电子密码本模式来说，各分组独立加解密，前后没有关系，也用不着iv  
	EVP_EncryptInit_ex(&ctx, EVP_aes_128_ecb(), NULL, key, iv);

	*len = 0;
	int outl = 0;
	//这个EVP_EncryptUpdate的实现实际就是将in按照inl的长度去加密，实现会取得该cipher的块大小（对aes_128来说是16字节）并将block-size的整数倍去加密。
	//如果输入为50字节，则此处仅加密48字节，outl也为48字节。输入in中的最后两字节拷贝到ctx->buf缓存起来。  
	//对于inl为block_size整数倍的情形，且ctx->buf并没有以前遗留的数据时则直接加解密操作，省去很多后续工作。  
	EVP_EncryptUpdate(&ctx, out + *len, &outl, in + *len, inl);
	*len += outl;
	//余下最后n字节。此处进行处理。
	//如果不支持pading，且还有数据的话就出错，否则，将block_size-待处理字节数个数个字节设置为此个数的值，如block_size=16,数据长度为4，则将后面的12字节设置为16-4=12，补齐为一个分组后加密 
	//对于前面为整分组时，如输入数据为16字节，最后再调用此Final时，不过是对16个0进行加密，此密文不用即可，也根本用不着调一下这Final。
	int test = inl >> 4;
	if (inl != test << 4){
		EVP_EncryptFinal_ex(&ctx, out + *len, &outl);
		*len += outl;
	}
	EVP_CIPHER_CTX_cleanup(&ctx);
}
*/

SafeByteArray RtpClientAuthenticator::buildSecret(const SafeByteArray& text) const{
//	return text;
	
	//unsigned char key[] = "RTP-SECRET-SEED";
	//unsigned char key[] = { -28, 101, 114, -36, 52, -119, -15, -113, 12, -37, 55, -34, 52, -17, 65, -53 };

	unsigned char in[160], en[160], de[160];

	memset(in, 0, 160);
	memset(en, 0, 160);
	memset(de, 0, 160);

	ByteArray keyArray = m_crypto->getSHA1Hash(createByteArray(m_key));

	for (int i = 0; i < text.size(); i++) {
       in[i] = text[i];
    }

	int len = text.size() / 16 + 1;
	int padding = text.size() % 16;
	if(padding == 0){
		for (int i = text.size(); i < text.size()+16; i++) {
			in[i] = 0x10;
		}
	}
	else
	{
		for (int i = text.size(); i < text.size() + 16 - padding; i++) {
			in[i] = 16 - padding;
		}
	}

//	for (int i = 0; i < sizeof(in); ++i)
//		printf("%.2x", in[i]);


	for(int i=0;i<len;i++){
		AES_ECB_encrypt(in+(i*16), vecptr(keyArray), en+(i*16), 16);
	}

	ByteArray result = createByteArray((char *)en, len*16);

	std::string hex = Hexify::hexify(result);

	return createSafeByteArray(Hexify::hexify(result));

/*
	for(int i = 0; i < 4; ++i)
    {
        AES_ECB_encrypt(vecptr() + (i*16), key, buf+(i*16), 16);
        phex(buf + (i*16));
    }

/*
	SHA_CTX s;
	int i, size;
	unsigned char hash[40];
	memset(hash, 0, 40);

	const char * key = m_key.c_str();
	SHA1_Init(&s);
	SHA1_Update(&s, (char*)key, strlen((char*)key));
	SHA1_Final(hash, &s);

	encrypt(vecptr(text), text.size(), en, &len, hash);

	ByteArray result = createByteArray((char *)en, len);
	std::string hex = Hexify::hexify(result);

	return createSafeByteArray(hex);
	/*
	AES_KEY aes;
	unsigned char iv[AES_BLOCK_SIZE];
	memset(iv, 0, AES_BLOCK_SIZE);

	SafeByteArray keyArray = createSafeByteArray(m_key);
	if (AES_set_encrypt_key(vecptr(keyArray), 128, &aes) < 0) {
		return text;
	}

	int len = 0;
	if ((text.size() + 1) % AES_BLOCK_SIZE == 0) {
		len = text.size() + 1;
	}
	else {
		len = ((text.size() + 1) / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
	}
	std::vector<unsigned char> result(len);
	std::fill(result.begin(), result.end(), 0);
	AES_cbc_encrypt(vecptr(text), vecptr(result), len, &aes, iv, AES_ENCRYPT);

	std::string hex = Hexify::hexify(result);

	return createSafeByteArray(Hexify::hexify(result));
	*/
}

boost::optional<SafeByteArray> RtpClientAuthenticator::getResponse() const {
	SWIFT_LOG(info) << "use RTP-SECRET, build secret array" << std::endl;
	SafeByteArray password = buildSecret(getPassword());
	return concat(createSafeByteArray(getAuthorizationID()), createSafeByteArray('\0'), createSafeByteArray(getAuthenticationID()), createSafeByteArray('\0'), password);
}

bool RtpClientAuthenticator::setChallenge(const boost::optional<ByteArray>&) {
	return true;
}

}
