/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/JingleContentPayload.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
///hero
#include <Swiften/Serializer/PayloadSerializerCollection.h>

namespace Swift {
    class SWIFTEN_API JingleContentPayloadSerializer : public GenericPayloadSerializer<JingleContentPayload> {
        public:
/**
			old
			JingleContentPayloadSerializer();
			*/
			///hero
			JingleContentPayloadSerializer(PayloadSerializerCollection*);

            virtual std::string serializePayload(std::shared_ptr<JingleContentPayload>)  const;

        private:
            std::string creatorToString(JingleContentPayload::Creator creator) const;
			///hero
			std::string sendersToString(JingleContentPayload::Senders senders) const;


		///hero
		private:
			PayloadSerializerCollection* serializers;
    };
}
