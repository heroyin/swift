/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/JingleContentPayloadSerializer.h>

#include <memory>

#include <Swiften/Base/Log.h>
#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferDescriptionSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JingleIBBTransportPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JingleS5BTransportPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLNode.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

namespace Swift {

/**
old
JingleContentPayloadSerializer::JingleContentPayloadSerializer() {
}

std::string JingleContentPayloadSerializer::serializePayload(std::shared_ptr<JingleContentPayload> payload) const {
    XMLElement payloadXML("content");
    payloadXML.setAttribute("creator", creatorToString(payload->getCreator()));
    payloadXML.setAttribute("name", payload->getName());

    if (!payload->getDescriptions().empty()) {
        // JingleFileTransferDescription
        JingleFileTransferDescriptionSerializer ftSerializer;
        JingleFileTransferDescription::ref filetransfer;

        for (auto&& desc : payload->getDescriptions()) {
            if ((filetransfer = std::dynamic_pointer_cast<JingleFileTransferDescription>(desc))) {
                payloadXML.addNode(std::make_shared<XMLRawTextNode>(ftSerializer.serializePayload(filetransfer)));
            }
        }
    }

    if (!payload->getTransports().empty()) {
        // JingleIBBTransportPayload
        JingleIBBTransportPayloadSerializer ibbSerializer;
        JingleIBBTransportPayload::ref ibb;

        // JingleS5BTransportPayload
        JingleS5BTransportPayloadSerializer s5bSerializer;
        JingleS5BTransportPayload::ref s5b;

        for (auto&& transport : payload->getTransports()) {
            if ((ibb = std::dynamic_pointer_cast<JingleIBBTransportPayload>(transport))) {
                payloadXML.addNode(std::make_shared<XMLRawTextNode>(ibbSerializer.serializePayload(ibb)));
            } else if ((s5b = std::dynamic_pointer_cast<JingleS5BTransportPayload>(transport))) {
                payloadXML.addNode(std::make_shared<XMLRawTextNode>(s5bSerializer.serializePayload(s5b)));
            }
        }
    }
    return payloadXML.serialize();
}
*/


///hero
JingleContentPayloadSerializer::JingleContentPayloadSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

///hero
std::string JingleContentPayloadSerializer::serializePayload(std::shared_ptr<JingleContentPayload> payload) const {
	XMLElement payloadXML("content");
	payloadXML.setAttribute("creator", creatorToString(payload->getCreator()));
	payloadXML.setAttribute("name", payload->getName());
	payloadXML.setAttribute("senders", sendersToString(payload->getSenders())); 

	if (!payload->getDescriptions().empty()) {
		for (auto&& desc : payload->getDescriptions()) {
			PayloadSerializer* serializer = serializers->getPayloadSerializer(desc);
			if (serializer) {
				payloadXML.addNode(std::make_shared<XMLRawTextNode>(serializer->serialize(desc)));
			}
		}
	}

	if (!payload->getTransports().empty()) {
		for (auto&& transport : payload->getTransports()) {
			PayloadSerializer* serializer = serializers->getPayloadSerializer(transport);
			if (serializer) {
				payloadXML.addNode(std::make_shared<XMLRawTextNode>(serializer->serialize(transport)));
			}
		}
	}
	return payloadXML.serialize();
}

std::string JingleContentPayloadSerializer::creatorToString(JingleContentPayload::Creator creator) const {
    switch(creator) {
        case JingleContentPayload::InitiatorCreator:
            return "initiator";
        case JingleContentPayload::ResponderCreator:
            return "responder";
        case JingleContentPayload::UnknownCreator:
            SWIFT_LOG(error) << "Serializing unknown creator value.";
            return "ERROR ERROR ERROR";
    }
    assert(false);
    return "";
}


std::string JingleContentPayloadSerializer::sendersToString(JingleContentPayload::Senders senders) const {
	switch (senders) {
	case JingleContentPayload::InitiatorSender:
		return "initiator";
	case JingleContentPayload::ResponderSender:
		return "responder";
	case JingleContentPayload::BothSenders:
		return "both";
	default:
		return "none";
	}
}

}