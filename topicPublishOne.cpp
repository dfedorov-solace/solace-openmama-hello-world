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


int main(int argc, const char** argv)
{
    std::cout << 
        "Solace OpenMAMA tutorial." << std::endl << 
        "Publishing one message with OpenMAMA." << std::endl;
    try
    {
        // load Solace middleware bridge and open it
        mamaBridge bridge = Wombat::Mama::loadBridge("solace");
        Wombat::Mama::open(".", "mama.properties");
        // scope for transport, publisher and message
        {
            // 1. create transport
            Wombat::MamaTransport transport;
            transport.create("vmr", bridge);

            // 2. create publisher
            Wombat::MamaPublisher publisher;
            publisher.create(&transport, "tutorial.topic");
            
            // 3. create message and add a string field to it 
            Wombat::MamaMsg message;
            message.create();
            message.addString("MyGreetingField", 99, "Hello World");

            // 4. send the message
            publisher.send(&message);
        }
        Wombat::Mama::close();
    }
    catch (const Wombat::MamaStatus & ex)
    {
        std::cerr << "OpenMAMA errror: " << ex.toString() << std::endl;
    }
}



