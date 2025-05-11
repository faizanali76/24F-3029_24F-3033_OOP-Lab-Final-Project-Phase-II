#include "MultiplayerSystems.h"

CommunicationSystem::CommunicationSystem() : messageCount(0) {
    messageLog.open("messages_log.txt", ios::app);
}

void CommunicationSystem::sendMessage(const string& sender, const string& receiver, 
                                    const string& content, MessageType type) {
    if (messageCount >= MAX_MESSAGES) {
        cout << "Message system is full!" << endl;
        return;
    }

    messages[messageCount].senderKingdom = sender;
    messages[messageCount].receiverKingdom = receiver;
    messages[messageCount].content = content;
    messages[messageCount].type = type;
    messages[messageCount].isRead = false;

    // Log the message
    messageLog << "[" << sender << "] to [" << receiver << "]: " << content << endl;
    messageLog.flush();

    messageCount++;
}

void CommunicationSystem::displayMessages(const string& kingdom) {
    cout << "\n=== Messages for Kingdom: " << kingdom << " ===\n";
    bool hasMessages = false;

    for (int i = 0; i < messageCount; i++) {
        if (messages[i].receiverKingdom == kingdom && !messages[i].isRead) {
            hasMessages = true;
            cout << "\nFrom: " << messages[i].senderKingdom << endl;
            cout << "Message: " << messages[i].content << endl;
            cout << "Type: ";
            
            switch (messages[i].type) {
                case ALLIANCE_REQUEST:
                    cout << "Alliance Request";
                    break;
                case TRADE_OFFER:
                    cout << "Trade Offer";
                    break;
                case WAR_THREAT:
                    cout << "War Threat";
                    break;
                case DECEPTION:
                    cout << "Deception";
                    break;
            }
            cout << "\nRespond? (y/n): ";
            
            char response;
            cin >> response;
            messages[i].isRead = true;
        }
    }

    if (!hasMessages) {
        cout << "No new messages.\n";
    }
}

void CommunicationSystem::saveMessagesToFile() const {
    ofstream saveFile("messages_save.txt");
    if (saveFile.is_open()) {
        saveFile << messageCount << endl;
        for (int i = 0; i < messageCount; i++) {
            saveFile << messages[i].senderKingdom << endl;
            saveFile << messages[i].receiverKingdom << endl;
            saveFile << messages[i].content << endl;
            saveFile << static_cast<int>(messages[i].type) << endl;
            saveFile << messages[i].isRead << endl;
        }
        saveFile.close();
    }
}

void CommunicationSystem::loadMessagesFromFile() {
    ifstream loadFile("messages_save.txt");
    if (loadFile.is_open()) {
        loadFile >> messageCount;
        for (int i = 0; i < messageCount; i++) {
            loadFile >> messages[i].senderKingdom;
            loadFile >> messages[i].receiverKingdom;
            loadFile >> messages[i].content;
            int type;
            loadFile >> type;
            messages[i].type = static_cast<MessageType>(type);
            loadFile >> messages[i].isRead;
        }
        loadFile.close();
    }
} 