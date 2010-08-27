/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include "3rdParty/hippomocks.h"

#include "Swift/Controllers/Chat/ChatsManager.h"

#include "Swift/Controllers/UIInterfaces/ChatWindow.h"
#include "Swift/Controllers/UIInterfaces/ChatWindowFactory.h"
#include "Swift/Controllers/UIInterfaces/ChatListWindowFactory.h"
#include "Swiften/Client/Client.h"
#include "Swift/Controllers/Chat/ChatController.h"
#include "Swift/Controllers/EventController.h"
#include "Swift/Controllers/Chat/MUCController.h"
#include "Swiften/Presence/PresenceSender.h"
#include "Swiften/Avatars/AvatarManager.h"
#include "Swiften/Avatars/AvatarMemoryStorage.h"
#include "Swiften/VCards/VCardManager.h"
#include "Swiften/VCards/VCardMemoryStorage.h"
#include "Swift/Controllers/NickResolver.h"
#include "Swiften/Roster/XMPPRoster.h"
#include "Swift/Controllers/UnitTest/MockChatWindow.h"
#include "Swiften/Client/DummyStanzaChannel.h"
#include "Swiften/Queries/DummyIQChannel.h"
#include "Swiften/Presence/PresenceOracle.h"
#include "Swift/Controllers/UIEvents/RequestChatUIEvent.h"
#include "Swift/Controllers/UIEvents/JoinMUCUIEvent.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"


using namespace Swift;

class ChatsManagerTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ChatsManagerTest);
	CPPUNIT_TEST(testFirstOpenWindowIncoming);
	CPPUNIT_TEST(testSecondOpenWindowIncoming);
	CPPUNIT_TEST(testFirstOpenWindowOutgoing);
	CPPUNIT_TEST(testFirstOpenWindowBareToFull);
	CPPUNIT_TEST(testSecondWindow);
	CPPUNIT_TEST(testUnbindRebind);
	CPPUNIT_TEST(testNoDuplicateUnbind);
	CPPUNIT_TEST(testThreeMUCWindows);
	CPPUNIT_TEST_SUITE_END();
	
public:
	ChatsManagerTest() {};
	
	void setUp() {
		mocks_ = new MockRepository();
		jid_ = JID("test@test.com/resource");
		stanzaChannel_ = new DummyStanzaChannel();
		iqChannel_ = new DummyIQChannel();
		iqRouter_ = new IQRouter(iqChannel_);
		eventController_ = new EventController();
		chatWindowFactory_ = mocks_->InterfaceMock<ChatWindowFactory>();
		xmppRoster_ = boost::shared_ptr<XMPPRoster>(new XMPPRoster());
		nickResolver_ = new NickResolver(jid_.toBare(), xmppRoster_, NULL);
		presenceOracle_ = new PresenceOracle(stanzaChannel_);
		serverDiscoInfo_ = boost::shared_ptr<DiscoInfo>(new DiscoInfo());
		presenceSender_ = new PresenceSender(stanzaChannel_);
		uiEventStream_ = new UIEventStream();
		chatListWindowFactory_ = mocks_->InterfaceMock<ChatListWindowFactory>();
		mocks_->ExpectCall(chatListWindowFactory_, ChatListWindowFactory::createWindow).With(uiEventStream_).Return(NULL);
		manager_ = new ChatsManager(jid_, stanzaChannel_, iqRouter_, eventController_, chatWindowFactory_, nickResolver_, presenceOracle_, serverDiscoInfo_, presenceSender_, uiEventStream_, chatListWindowFactory_, true, NULL);

		vcardStorage_ = new VCardMemoryStorage();
		vcardManager_ = new VCardManager(jid_, iqRouter_, vcardStorage_);
		avatarStorage_ = new AvatarMemoryStorage();
		avatarManager_ = new AvatarManager(vcardManager_, stanzaChannel_, avatarStorage_, NULL);
		manager_->setAvatarManager(avatarManager_);
	};
	
	void tearDown() {
		delete avatarManager_;
		delete avatarStorage_;
		delete vcardManager_;
		delete vcardStorage_;
		delete manager_;
		delete presenceSender_;
		delete presenceOracle_;
		delete nickResolver_;
		delete stanzaChannel_;
		delete eventController_;
		delete iqChannel_;
		delete iqRouter_;
		delete mocks_;
		delete uiEventStream_;
	}

	void testFirstOpenWindowIncoming() {
		JID messageJID("testling@test.com/resource1");
		
		MockChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);

		boost::shared_ptr<Message> message(new Message());
		message->setFrom(messageJID);
		String body("This is a legible message. >HEH@)oeueu");
		message->setBody(body);
		manager_->handleIncomingMessage(message);
		CPPUNIT_ASSERT_EQUAL(body, window->lastMessageBody_);
	}

	void testSecondOpenWindowIncoming() {
		JID messageJID1("testling@test.com/resource1");
		
		MockChatWindow* window1 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID1, uiEventStream_).Return(window1);

		boost::shared_ptr<Message> message1(new Message());
		message1->setFrom(messageJID1);
		String body1("This is a legible message. >HEH@)oeueu");
		message1->setBody(body1);
		manager_->handleIncomingMessage(message1);
		CPPUNIT_ASSERT_EQUAL(body1, window1->lastMessageBody_);

		JID messageJID2("testling@test.com/resource2");
		
		MockChatWindow* window2 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID2, uiEventStream_).Return(window2);

		boost::shared_ptr<Message> message2(new Message());
		message2->setFrom(messageJID2);
		String body2("This is a legible message. .cmaulm.chul");
		message2->setBody(body2);
		manager_->handleIncomingMessage(message2);
		CPPUNIT_ASSERT_EQUAL(body2, window2->lastMessageBody_);
	}

	void testFirstOpenWindowOutgoing() {
		String messageJIDString("testling@test.com");
		
		ChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString), uiEventStream_).Return(window);
		
		uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(JID(messageJIDString))));
	}


	void testFirstOpenWindowBareToFull() {
		String bareJIDString("testling@test.com");
		String fullJIDString("testling@test.com/resource1");
		
		MockChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(bareJIDString), uiEventStream_).Return(window);
		uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(JID(bareJIDString))));

		boost::shared_ptr<Message> message(new Message());
		message->setFrom(JID(fullJIDString));
		String body("This is a legible message. mjuga3089gm8G(*>M)@*(");
		message->setBody(body);
		manager_->handleIncomingMessage(message);
		CPPUNIT_ASSERT_EQUAL(body, window->lastMessageBody_);
	}

	void testSecondWindow() {
		String messageJIDString1("testling1@test.com");
		ChatWindow* window1 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString1), uiEventStream_).Return(window1);
		uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(JID(messageJIDString1))));

		String messageJIDString2("testling2@test.com");
		ChatWindow* window2 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString2), uiEventStream_).Return(window2);

		uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(JID(messageJIDString2))));
	}

	/** Complete cycle.
		Create unbound window.
		Bind it.
		Unbind it.
		Rebind it.
	 */
	void testUnbindRebind() {
		String bareJIDString("testling@test.com");
		String fullJIDString1("testling@test.com/resource1");
		String fullJIDString2("testling@test.com/resource2");
		
		MockChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(bareJIDString), uiEventStream_).Return(window);
		uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(JID(bareJIDString))));

		boost::shared_ptr<Message> message1(new Message());
		message1->setFrom(JID(fullJIDString1));
		String messageBody1("This is a legible message.");
		message1->setBody(messageBody1);
		manager_->handleIncomingMessage(message1);
		CPPUNIT_ASSERT_EQUAL(messageBody1, window->lastMessageBody_);
		
		boost::shared_ptr<Presence> jid1Online(new Presence());
		jid1Online->setFrom(JID(fullJIDString1));
		boost::shared_ptr<Presence> jid1Offline(new Presence());
		jid1Offline->setFrom(JID(fullJIDString1));
		jid1Offline->setType(Presence::Unavailable);
		presenceOracle_->onPresenceChange(jid1Offline, jid1Online);

		boost::shared_ptr<Message> message2(new Message());
		message2->setFrom(JID(fullJIDString2));
		String messageBody2("This is another legible message.");
		message2->setBody(messageBody2);
		manager_->handleIncomingMessage(message2);
		CPPUNIT_ASSERT_EQUAL(messageBody2, window->lastMessageBody_);
	}

	/**
	 * Test that MUC PMs get opened in the right windows
	 */
	void testThreeMUCWindows() {
		JID muc("testling@test.com");
		ChatWindow* mucWindow = new MockChatWindow();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(muc, uiEventStream_).Return(mucWindow);
		uiEventStream_->send(boost::shared_ptr<JoinMUCUIEvent>(new JoinMUCUIEvent(muc, String("nick"))));


		String messageJIDString1("testling@test.com/1");
		ChatWindow* window1 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString1), uiEventStream_).Return(window1);
		uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(JID(messageJIDString1))));

		String messageJIDString2("testling@test.com/2");
		ChatWindow* window2 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString2), uiEventStream_).Return(window2);

		uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(JID(messageJIDString2))));

		String messageJIDString3("testling@test.com/3");
		ChatWindow* window3 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString3), uiEventStream_).Return(window3);

		uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(JID(messageJIDString3))));

		/* Refetch an earlier window */
		/* We do not expect a new window to be created */
		uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(JID(messageJIDString1))));

	}

	/**
		 Test that a second window isn't unbound where there's already an unbound one.
		 Bind 1
		 Bind 2
		 Unbind 1
		 Unbind 2 (but it doesn't)
		 Sent to bound 2
		 Rebind 1
	 */
	void testNoDuplicateUnbind() {
		JID messageJID1("testling@test.com/resource1");
		
		MockChatWindow* window1 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID1, uiEventStream_).Return(window1);

		boost::shared_ptr<Message> message1(new Message());
		message1->setFrom(messageJID1);
		message1->setBody("This is a legible message1.");
		manager_->handleIncomingMessage(message1);

		JID messageJID2("testling@test.com/resource2");
		
		MockChatWindow* window2 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID2, uiEventStream_).Return(window2);

		boost::shared_ptr<Message> message2(new Message());
		message2->setFrom(messageJID2);
		message2->setBody("This is a legible message2.");
		manager_->handleIncomingMessage(message2);

		boost::shared_ptr<Presence> jid1Online(new Presence());
		jid1Online->setFrom(JID(messageJID1));
		boost::shared_ptr<Presence> jid1Offline(new Presence());
		jid1Offline->setFrom(JID(messageJID1));
		jid1Offline->setType(Presence::Unavailable);
		presenceOracle_->onPresenceChange(jid1Offline, jid1Online);
 
		boost::shared_ptr<Presence> jid2Online(new Presence());
		jid2Online->setFrom(JID(messageJID2));
		boost::shared_ptr<Presence> jid2Offline(new Presence());
		jid2Offline->setFrom(JID(messageJID2));
		jid2Offline->setType(Presence::Unavailable);
		presenceOracle_->onPresenceChange(jid2Offline, jid2Online);

		JID messageJID3("testling@test.com/resource3");

		boost::shared_ptr<Message> message3(new Message());
		message3->setFrom(messageJID3);
		String body3("This is a legible message3.");
		message3->setBody(body3);
		manager_->handleIncomingMessage(message3);
		CPPUNIT_ASSERT_EQUAL(body3, window1->lastMessageBody_);

		boost::shared_ptr<Message> message2b(new Message());
		message2b->setFrom(messageJID2);
		String body2b("This is a legible message2b.");
		message2b->setBody(body2b);
		manager_->handleIncomingMessage(message2b);
		CPPUNIT_ASSERT_EQUAL(body2b, window2->lastMessageBody_);
	}
	
private:
	JID jid_;
	ChatsManager* manager_;
	StanzaChannel* stanzaChannel_;
	IQChannel* iqChannel_;
	IQRouter* iqRouter_;
	EventController* eventController_;
	ChatWindowFactory* chatWindowFactory_;
	NickResolver* nickResolver_;
	PresenceOracle* presenceOracle_;
	VCardStorage* vcardStorage_;
	VCardManager* vcardManager_;
	AvatarStorage* avatarStorage_;
	AvatarManager* avatarManager_;
	boost::shared_ptr<DiscoInfo> serverDiscoInfo_;
	boost::shared_ptr<XMPPRoster> xmppRoster_;
	PresenceSender* presenceSender_;
	MockRepository* mocks_;
	UIEventStream* uiEventStream_;
	ChatListWindowFactory* chatListWindowFactory_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ChatsManagerTest);

