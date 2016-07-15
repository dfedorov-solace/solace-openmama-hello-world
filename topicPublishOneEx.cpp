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
 * PublishSubscribe tutorial - Topic Publisher
 * Demonstrates publishing one direct message to a topic
 */

#include <iostream>

#include <mama/mamacpp.h>


class Bridge
{
    public:
        explicit Bridge(const std::string& bridgeName, 
                const std::string& propertyFilenamePath = ".",
                const std::string& propertyFilename = "mama.properties")
        {
            bridge = Wombat::Mama::loadBridge(bridgeName.c_str());
            Wombat::Mama::open(propertyFilenamePath.c_str(), propertyFilename.c_str());
        }
        ~Bridge()
        {
            try
            {
                Wombat::Mama::close();
            }
            catch (const Wombat::MamaStatus& ex)
            {
                std::cerr << "OpenMAMA closing errror: " << ex.toString() << std::endl;
            }
            catch (...) { /* destructor swallow */ }
        }
        const mamaBridge getMamaBridge() const { return bridge; }
    private:
        Bridge();
        Bridge(const Bridge&);
        Bridge& operator=(const Bridge&);
        Bridge(const Bridge&&);
        Bridge& operator=(const Bridge&&);
    private:
        mamaBridge bridge;

};


class Transport
{
    public:
        explicit Transport(const std::string& transportName, const Bridge& bridge)
        {
            transport.create(transportName.c_str(), bridge.getMamaBridge());
        }
        const Wombat::MamaTransport& getMamaTransport() const { return transport; }
    private:
        Transport();
        Transport(const Transport&);
        Transport& operator=(const Transport&);
        Transport(const Transport&&);
        Transport& operator=(const Transport&&);
    private:
        Wombat::MamaTransport transport;
};


class Message
{
    public:
        Message()
        {
            message.create();
        }
        Message(const Message& right)
            : message(right.getMamaMsg()) {}
        Message& operator=(const Message& rvalue)
        {
            message.copy(rvalue.getMamaMsg());
            return *this;
        }
        Message& field(const std::string& fieldName, int fieldId, const std::string& value)
        {   
            message.addString(fieldName.c_str(), fieldId, value.c_str());
            return *this;
        }
        const Wombat::MamaMsg & getMamaMsg() const { return message; }
    private:
        Message(const Message&&);
        Message& operator=(const Message&&);
    private:
        Wombat::MamaMsg message;
};


class Publisher
{
    public:
        explicit Publisher(const std::string& topicName, const Transport& transport)
        {
            publisher.create(
                    const_cast<Wombat::MamaTransport*>(&transport.getMamaTransport()),
                    topicName.c_str());
        }
        Publisher& operator<<(const Message& rvalue)
        {
            publisher.send(const_cast<Wombat::MamaMsg*>(&rvalue.getMamaMsg()));
            return *this;
        }
    private:
        Publisher();
        Publisher(const Publisher&);
        Publisher& operator=(const Publisher&);
        Publisher(const Publisher&&);
        Publisher& operator=(const Publisher&&);
    private:
        Wombat::MamaPublisher publisher;
};


int main(int argc, const char** argv)
{
    std::cout <<
        "Solace OpenMAMA tutorial." << std::endl <<
        "Publishing one message with OpenMAMA." << std::endl;
    try
    {
        Bridge bridge("solace");
        Transport transport("vmr", bridge);
        Publisher publisher("tutorial.topic", transport);
        publisher << Message().field("MyGreetingField", 99, "Hello World");
    }
    catch (const Wombat::MamaStatus & ex)
    {
        std::cerr << "OpenMAMA errror: " << ex.toString() << std::endl;
    }
}


