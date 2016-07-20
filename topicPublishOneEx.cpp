/*
 * Copyright 2016 Solace Systems, Inc. All rights reserved.
 *
 * http://www.solacesystems.com
 *
 * This source is distributed under the terms and conditions
 * of any contract or contracts between Solace and you or
 * your company. If there are no contracts in place use of
 * this source is not authorized. No support is provided and
 * no distribution, sharing with others or re-use of this
 * source is authorized unless specifically stated in the
 * contracts referred to above.
 */

/*
 * Solace messaging with OpenMAMA
 * PublishSubscribe tutorial - Topic TopicPublisher
 * Demonstrates publishing one direct message to a topic
 */

#include <iostream>

#include <mama/mamacpp.h>


class TopicPublisher
{
    public:
        explicit TopicPublisher(const std::string& topicName,
                const std::string& transportName = "vmr",
                const std::string& bridgeName = "solace",
                const std::string& propertyFilenamePath = ".",
                const std::string& propertyFilename = "mama.properties")
            : transport(new Wombat::MamaTransport())
        {
            bridge = Wombat::Mama::loadBridge(bridgeName.c_str());
            Wombat::Mama::open(propertyFilenamePath.c_str(), propertyFilename.c_str());
            transport->create(transportName.c_str(), bridge);
            publisher.create(
                    const_cast<Wombat::MamaTransport*>(transport),
                    topicName.c_str());
        }
        ~TopicPublisher()
        {
            try
            {
                publisher.destroy();
                delete transport;
                Wombat::Mama::close();
            }
            catch (const Wombat::MamaStatus& ex)
            {
                std::cerr << "OpenMAMA closing errror: " << ex.toString() << std::endl;
            }
            catch (...) { /* destructor swallow */ }
        }
        TopicPublisher& operator<<(const std::string& rvalue)
        {
            Wombat::MamaMsg message;
            message.create();
            message.addString("StringField", 99, rvalue.c_str());
            publisher.send(const_cast<Wombat::MamaMsg*>(&message));
            return *this;
        }
    private:
        TopicPublisher();
        TopicPublisher(const TopicPublisher&);
        TopicPublisher& operator=(const TopicPublisher&);
        TopicPublisher(const TopicPublisher&&);
        TopicPublisher& operator=(const TopicPublisher&&);

    private:
        mamaBridge bridge;
        Wombat::MamaTransport* transport;
        Wombat::MamaPublisher publisher;
};



int main(int argc, const char** argv)
{
    std::cout <<
        "Solace OpenMAMA tutorial." << std::endl <<
        "Publishing one message with OpenMAMA." << std::endl;
    try
    {
        TopicPublisher topicPublisher("tutorial.topic");
        topicPublisher << "Hello World";
    }
    catch (const Wombat::MamaStatus & ex)
    {
        std::cerr << "OpenMAMA errror: " << ex.toString() << std::endl;
    }
}


