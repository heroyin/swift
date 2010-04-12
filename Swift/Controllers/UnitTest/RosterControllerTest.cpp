
/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swift/Controllers/RosterController.h"
#include "Swift/Controllers/UnitTest/MockMainWindowFactory.h"
#include "Swiften/Roster/UnitTest/MockTreeWidgetFactory.h"
// #include "Swiften/Elements/Payload.h"
// #include "Swiften/Elements/RosterItemPayload.h"
// #include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Queries/DummyIQChannel.h"
#include "Swiften/Client/DummyStanzaChannel.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Roster/XMPPRoster.h"
#include "Swift/Controllers/EventController.h"
#include "Swiften/Presence/PresenceOracle.h"
#include "Swift/Controllers/NickResolver.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"

using namespace Swift;

class RosterControllerTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(RosterControllerTest);
		CPPUNIT_TEST(testAdd);
		CPPUNIT_TEST(testAddSubscription);
		CPPUNIT_TEST(testRename);
		CPPUNIT_TEST_SUITE_END();

	public:
		RosterControllerTest() {};

		void setUp() {
			jid_ = JID("testjid@swift.im/swift");
			xmppRoster_ = boost::shared_ptr<XMPPRoster>(new XMPPRoster());
			avatarManager_ = NULL;//new AvatarManager();
			treeWidgetFactory_ = new MockTreeWidgetFactory();
			mainWindowFactory_ = new MockMainWindowFactory(treeWidgetFactory_);
			nickResolver_ = new NickResolver(xmppRoster_);
			channel_ = new DummyIQChannel();
			router_ = new IQRouter(channel_);
			stanzaChannel_ = new DummyStanzaChannel();
			presenceOracle_ = new PresenceOracle(stanzaChannel_);
			eventController_ = new EventController();
			uiEventStream_ = new UIEventStream();
			rosterController_ = new RosterController(jid_, xmppRoster_, avatarManager_, mainWindowFactory_, treeWidgetFactory_, nickResolver_, presenceOracle_, eventController_, uiEventStream_, router_);


		};

		void tearDown() {
			delete rosterController_;
			delete nickResolver_;
			delete mainWindowFactory_;
			delete avatarManager_;
			delete channel_;
			delete router_;
			delete eventController_;
			delete presenceOracle_;
			delete stanzaChannel_;
			delete uiEventStream_;
			delete treeWidgetFactory_;
		};

		void testAdd() {
			std::vector<String> groups;
			groups.push_back("testGroup1");
			groups.push_back("testGroup2");
			xmppRoster_->addContact(JID("test@testdomain.com/bob"), "name", groups, RosterItemPayload::Both);
			
			CPPUNIT_ASSERT_EQUAL(2, (int)treeWidgetFactory_->getGroups().size());
			//CPPUNIT_ASSERT_EQUAL(String("Bob"), xmppRoster_->getNameForJID(JID("foo@bar.com")));
		};

 		void testAddSubscription() {
			std::vector<String> groups;
			JID jid("test@testdomain.com");
			xmppRoster_->addContact(jid, "name", groups, RosterItemPayload::None);
			
			CPPUNIT_ASSERT_EQUAL(1, (int)treeWidgetFactory_->getGroups().size());
			CPPUNIT_ASSERT_EQUAL(1, (int)treeWidgetFactory_->getGroupMembers("Contacts").size());
			xmppRoster_->addContact(jid, "name", groups, RosterItemPayload::To);
			CPPUNIT_ASSERT_EQUAL(1, (int)treeWidgetFactory_->getGroups().size());
			CPPUNIT_ASSERT_EQUAL(1, (int)treeWidgetFactory_->getGroupMembers("Contacts").size());

			xmppRoster_->addContact(jid, "name", groups, RosterItemPayload::Both);
			CPPUNIT_ASSERT_EQUAL(1, (int)treeWidgetFactory_->getGroups().size());
			CPPUNIT_ASSERT_EQUAL(1, (int)treeWidgetFactory_->getGroupMembers("Contacts").size());

		};

		void testRename() {
			std::vector<String> groups;
			JID jid("test@testdomain.com");
			xmppRoster_->addContact(jid, "name", groups, RosterItemPayload::Both);
			
			CPPUNIT_ASSERT_EQUAL(1, (int)treeWidgetFactory_->getGroups().size());
			CPPUNIT_ASSERT_EQUAL(1, (int)treeWidgetFactory_->getGroupMembers("Contacts").size());
			xmppRoster_->addContact(jid, "NewName", groups, RosterItemPayload::Both);
			CPPUNIT_ASSERT_EQUAL(1, (int)treeWidgetFactory_->getGroups().size());
			CPPUNIT_ASSERT_EQUAL(1, (int)treeWidgetFactory_->getGroupMembers("Contacts").size());
			CPPUNIT_ASSERT_EQUAL(String("NewName"), treeWidgetFactory_->getGroupMembers("Contacts")[0]->getText());
		};

	private:
		JID jid_;
		boost::shared_ptr<XMPPRoster> xmppRoster_;
		AvatarManager* avatarManager_;
		MainWindowFactory* mainWindowFactory_;
		MockTreeWidgetFactory* treeWidgetFactory_;
		NickResolver* nickResolver_;
		RosterController* rosterController_;
		DummyIQChannel* channel_;
		DummyStanzaChannel* stanzaChannel_;	
		IQRouter* router_;
		PresenceOracle* presenceOracle_;
		EventController* eventController_;
		UIEventStream* uiEventStream_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(RosterControllerTest);
