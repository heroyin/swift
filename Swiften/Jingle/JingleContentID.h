/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Elements/JingleContentPayload.h>

namespace Swift {
	class JingleContentID {
		public:
			JingleContentID(const std::string& name, JingleContentPayload::Creator creator) : 
				name(name), creator(creator), senders(JingleContentPayload::InitiatorSender) {
			}
			
			const std::string getName() const {
				return this->name;
			}
			
			JingleContentPayload::Creator getCreator() const {
				return this->creator;
			}

			///hero
			JingleContentPayload::Senders getSenders() const {
				return this->senders;
			}

			void setSenders(JingleContentPayload::Senders senders) {
				this->senders = senders;
			}
	private:
			std::string name;
			JingleContentPayload::Creator creator;
			///hero
			JingleContentPayload::Senders senders;
	};
}
