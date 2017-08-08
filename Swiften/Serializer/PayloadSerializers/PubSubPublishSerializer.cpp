/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubPublishSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubItemSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

///hero
#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>

using namespace Swift;

///hero add publish options
PubSubPublishOptionsSerializer::PubSubPublishOptionsSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubPublishOptionsSerializer::~PubSubPublishOptionsSerializer() {
}

std::string PubSubPublishOptionsSerializer::serializePayload(std::shared_ptr<PubSubPublishOptions> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("publish-options", "http://jabber.org/protocol/pubsub");
	element.addNode(std::make_shared<XMLRawTextNode>(FormSerializer().serialize(payload->getData())));
	return element.serialize();
}
///end

PubSubPublishSerializer::PubSubPublishSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubPublishSerializer::~PubSubPublishSerializer() {
}

std::string PubSubPublishSerializer::serializePayload(std::shared_ptr<PubSubPublish> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("publish", "http://jabber.org/protocol/pubsub");
    element.setAttribute("node", payload->getNode());
    for (const auto& item : payload->getItems()) {
        element.addNode(std::make_shared<XMLRawTextNode>(PubSubItemSerializer(serializers).serialize(item)));
    }
    return element.serialize();
}


