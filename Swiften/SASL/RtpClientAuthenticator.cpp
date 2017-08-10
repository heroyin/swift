/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/SASL/RtpClientAuthenticator.h>
#include <Swiften/Base/Concat.h>
#include <Swiften/StringCodecs/Hexify.h>

//#include <openssl/aes.h>
//#include <openssl/evp.h>
//#include <openssl/sha.h>

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
	return text;
	/*
	unsigned char content[400];
	//unsigned char key[] = "RTP-SECRET-SEED";
	//unsigned char key[] = { -28, 101, 114, -36, 52, -119, -15, -113, 12, -37, 55, -34, 52, -17, 65, -53 };

	unsigned char en[400], de[400], base64[400], base64_out[400];
	int len;
	memset(content, 0, 400);
	memset(en, 0, 400);
	memset(de, 0, 400);
	memset(base64, 0, 400);
	memset(base64_out, 0, 400);
	strcpy((char *)content, (char *)vecptr(text));

	ByteArray keyArray = m_crypto->getSHA1Hash(createByteArray(m_key));

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
	SafeByteArray password = buildSecret(getPassword());
	return concat(createSafeByteArray(getAuthorizationID()), createSafeByteArray('\0'), createSafeByteArray(getAuthenticationID()), createSafeByteArray('\0'), password);
}

bool RtpClientAuthenticator::setChallenge(const boost::optional<ByteArray>&) {
	return true;
}

}
