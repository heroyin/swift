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
	//��init���Ľ��ǽ�ctx�ڴ� memsetΪ0  
	EVP_CIPHER_CTX_init(&ctx);

	//cipher  = EVP_aes_128_ecb();  
	//ԭ��Ϊint EVP_EncryptInit_ex(EVP_CIPHER_CTX *ctx,const EVP_CIPHER *cipher, ENGINE *impl, const unsigned char *key, const unsigned char *iv)   
	//�������ecb�������뱾ģʽ��˵������������ӽ��ܣ�ǰ��û�й�ϵ��Ҳ�ò���iv  
	EVP_EncryptInit_ex(&ctx, EVP_aes_128_ecb(), NULL, key, iv);

	*len = 0;
	int outl = 0;
	//���EVP_EncryptUpdate��ʵ��ʵ�ʾ��ǽ�in����inl�ĳ���ȥ���ܣ�ʵ�ֻ�ȡ�ø�cipher�Ŀ��С����aes_128��˵��16�ֽڣ�����block-size��������ȥ���ܡ�
	//�������Ϊ50�ֽڣ���˴�������48�ֽڣ�outlҲΪ48�ֽڡ�����in�е�������ֽڿ�����ctx->buf����������  
	//����inlΪblock_size�����������Σ���ctx->buf��û����ǰ����������ʱ��ֱ�Ӽӽ��ܲ�����ʡȥ�ܶ����������  
	EVP_EncryptUpdate(&ctx, out + *len, &outl, in + *len, inl);
	*len += outl;
	//�������n�ֽڡ��˴����д���
	//�����֧��pading���һ������ݵĻ��ͳ������򣬽�block_size-�������ֽ����������ֽ�����Ϊ�˸�����ֵ����block_size=16,���ݳ���Ϊ4���򽫺����12�ֽ�����Ϊ16-4=12������Ϊһ���������� 
	//����ǰ��Ϊ������ʱ������������Ϊ16�ֽڣ�����ٵ��ô�Finalʱ�������Ƕ�16��0���м��ܣ������Ĳ��ü��ɣ�Ҳ�����ò��ŵ�һ����Final��
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
