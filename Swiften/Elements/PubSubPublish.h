/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubItem.h>
#include <Swiften/Elements/PubSubPayload.h>

///hero
#include <Swiften/Elements/Form.h>

namespace Swift {

	///hero add publish options
	class SWIFTEN_API PubSubPublishOptions : public Payload {
	public:

		PubSubPublishOptions();

		virtual ~PubSubPublishOptions();

		std::shared_ptr<Form> getData() const {
			return data;
		}

		void setData(std::shared_ptr<Form> value) {
			this->data = value;
		}
	private:
		std::shared_ptr<Form> data;
	};
	///end


    class SWIFTEN_API PubSubPublish : public PubSubPayload {
        public:

            PubSubPublish();

            virtual ~PubSubPublish();

            const std::string& getNode() const {
                return node;
            }

            void setNode(const std::string& value) {
                this->node = value ;
            }

            const std::vector< std::shared_ptr<PubSubItem> >& getItems() const {
                return items;
            }

            void setItems(const std::vector< std::shared_ptr<PubSubItem> >& value) {
                this->items = value ;
            }

            void addItem(std::shared_ptr<PubSubItem> value) {
                this->items.push_back(value);
            }

///hero add publish options
			std::shared_ptr<PubSubPublishOptions> getOptions() const {
				return options;
			}

			///hero add publish options
			void setOptions(std::shared_ptr<PubSubPublishOptions> value) {
				this->options = value;
			}
        private:
            std::string node;
            std::vector< std::shared_ptr<PubSubItem> > items;
///hero add publish options
	    std::shared_ptr<PubSubPublishOptions> options;
    };
}
