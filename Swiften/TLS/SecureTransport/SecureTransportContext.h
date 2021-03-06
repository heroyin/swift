/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Security/SecureTransport.h>

#include <Swiften/TLS/TLSContext.h>

///hero
#include <mutex>

namespace Swift {

class SecureTransportContext : public TLSContext {
    public:
        SecureTransportContext(bool checkCertificateRevocation);
        virtual ~SecureTransportContext();

        virtual void connect();

        virtual bool setClientCertificate(CertificateWithKey::ref cert);

        virtual void handleDataFromNetwork(const SafeByteArray&);
        virtual void handleDataFromApplication(const SafeByteArray&);

        virtual std::vector<Certificate::ref> getPeerCertificateChain() const;
        virtual CertificateVerificationError::ref getPeerCertificateVerificationError() const;

        virtual ByteArray getFinishMessage() const;

    private:
        static OSStatus SSLSocketReadCallback(SSLConnectionRef connection, void *data, size_t *dataLength);
        static OSStatus SSLSocketWriteCallback(SSLConnectionRef connection, const void *data, size_t *dataLength);

    private:
        enum State { None, Handshake, HandshakeDone, Error};
        static std::string stateToString(State state);
        void setState(State newState);

        static std::shared_ptr<TLSError> nativeToTLSError(OSStatus error);
        std::shared_ptr<CertificateVerificationError> CSSMErrorToVerificationError(OSStatus resultCode);

        void processHandshake();
        void verifyServerCertificate();

        void fatalError(std::shared_ptr<TLSError> error, std::shared_ptr<CertificateVerificationError> certificateError);

    private:
        std::shared_ptr<SSLContext> sslContext_;
        SafeByteArray readingBuffer_;
        State state_;
        CertificateVerificationError::ref verificationError_;
        CertificateWithKey::ref clientCertificate_;
        bool checkCertificateRevocation_;
		///hero
		std::recursive_mutex contextMutex_;
};

}
