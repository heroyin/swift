/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/SASL/ClientAuthenticator.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Crypto/CryptoProvider.h>

namespace Swift {
	class SWIFTEN_API RtpClientAuthenticator : public ClientAuthenticator {
		public:
			RtpClientAuthenticator(CryptoProvider* crypto, std::string key);

			virtual boost::optional<SafeByteArray> getResponse() const;
			virtual bool setChallenge(const boost::optional<ByteArray>&);
		private:
			SafeByteArray buildSecret(const SafeByteArray& text) const;
		private:
			std::string m_key;
			CryptoProvider* m_crypto;
	};
}
