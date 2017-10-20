/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/StreamStack/XMPPLayer.h>

#include <Swiften/Elements/ProtocolHeader.h>
#include <Swiften/Parser/XMPPParser.h>
#include <Swiften/Serializer/XMPPSerializer.h>

///hero
#include <Swiften/Base/sleep.h>

namespace Swift {

XMPPLayer::XMPPLayer(
        PayloadParserFactoryCollection* payloadParserFactories,
        PayloadSerializerCollection* payloadSerializers,
        XMLParserFactory* xmlParserFactory,
        StreamType streamType,
        bool setExplictNSonTopLevelElements) :
		///hero
			stopThread_(false),
            payloadParserFactories_(payloadParserFactories),
            payloadSerializers_(payloadSerializers),
            xmlParserFactory_(xmlParserFactory),
            setExplictNSonTopLevelElements_(setExplictNSonTopLevelElements),
            resetParserAfterParse_(false),
            inParser_(false) {
    xmppParser_ = new XMPPParser(this, payloadParserFactories_, xmlParserFactory);
    xmppSerializer_ = new XMPPSerializer(payloadSerializers_, streamType, setExplictNSonTopLevelElements);

	///hero
	writeThread_ = new std::thread(boost::bind(&XMPPLayer::doWriteElement, this));
}

XMPPLayer::~XMPPLayer() {
    delete xmppSerializer_;
    delete xmppParser_;

	///hero
	stopThread_ = true;
	writeThread_->join();
	delete writeThread_;
}

void XMPPLayer::writeHeader(const ProtocolHeader& header) {
    writeDataInternal(createSafeByteArray(xmppSerializer_->serializeHeader(header)));
}

void XMPPLayer::writeFooter() {
    writeDataInternal(createSafeByteArray(xmppSerializer_->serializeFooter()));
}

void XMPPLayer::writeElement(std::shared_ptr<ToplevelElement> element) {
	///hero
	std::unique_lock<std::mutex> lock(elmentsMutex_);
	elements_.push_back(element);

    ///writeDataInternal(xmppSerializer_->serializeElement(element));
}

void XMPPLayer::writeData(const std::string& data) {
	///hero
	std::unique_lock<std::mutex> lock(elmentsMutex_);
	strings_.push_back(data);
    ///writeDataInternal(createSafeByteArray(data));
}

///hero
void XMPPLayer::doWriteElement() {
	std::unique_lock<std::mutex> lock(elmentsMutex_, std::defer_lock);

	while (!stopThread_){

		if (elements_.empty() && strings_.empty())
			sleep(1);
		else {
			
			lock.lock();
			while (!elements_.empty()){
				std::shared_ptr<ToplevelElement> element = elements_.back();
				elements_.pop_back();
				writeDataInternal(xmppSerializer_->serializeElement(element));
			}

			while (!strings_.empty()){
				std::string str = strings_.back();
				strings_.pop_back();
				writeDataInternal(createSafeByteArray(str));
			}
			lock.unlock();
		}


	}
}

void XMPPLayer::writeDataInternal(const SafeByteArray& data) {
    onWriteData(data);
    writeDataToChildLayer(data);
}

void XMPPLayer::handleDataRead(const SafeByteArray& data) {
    onDataRead(data);
    inParser_ = true;
    // FIXME: Converting to unsafe string. Should be ok, since we don't take passwords
    // from the stream in clients. If servers start using this, and require safe storage,
    // we need to fix this.
    if (!xmppParser_->parse(byteArrayToString(ByteArray(data.begin(), data.end())))) {
        inParser_ = false;
        onError();
        return;
    }
    inParser_ = false;
    if (resetParserAfterParse_) {
        doResetParser();
    }
}

void XMPPLayer::doResetParser() {
    delete xmppParser_;
    xmppParser_ = new XMPPParser(this, payloadParserFactories_, xmlParserFactory_);
    resetParserAfterParse_ = false;
}

void XMPPLayer::handleStreamStart(const ProtocolHeader& header) {
    onStreamStart(header);
}

void XMPPLayer::handleElement(std::shared_ptr<ToplevelElement> stanza) {
    onElement(stanza);
}

void XMPPLayer::handleStreamEnd() {
    onStreamEnd();
}

void XMPPLayer::resetParser() {
    if (inParser_) {
        resetParserAfterParse_ = true;
    }
    else {
        doResetParser();
    }
}

}
