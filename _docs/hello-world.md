# Solace OpenMAMA "Hello World"

* [Assumptions](#assumptions)
* [Goals](#goals)
* [Solace Message Router Properties](#solace-message-router-properties)
* [Trying It Yourself](#trying-it-yourself)
* [Hello World](#hello-world)

---

## Assumptions

This tutorial assumes the following:

*   You are familiar with OpenMAMA [core concepts](https://sftp.solacesystems.com/Portal_Docs/OpenMAMA_User_Guide/01_Introduction.html).
*   You are familiar with Solace [core concepts](http://dev.solacesystems.com/docs/core-concepts/).
*   You have access to a properly installed OpenMAMA [release](https://github.com/OpenMAMA/OpenMAMA/releases).
    *   Solace middleware bridge with its dependencies is also installed
*   You have access to a running Solace message router with the following configuration:
    *   Enabled message VPN
    *   Enabled client username

One simple way to get access to a Solace message router is to start a Solace VMR load [as outlined here](http://dev.solacesystems.com/docs/get-started/setting-up-solace-vmr_vmware/). By default the Solace VMR will run with the “default” message VPN configured and ready for messaging. Going forward, this tutorial assumes that you are using the Solace VMR. If you are using a different Solace message router configuration, adapt the instructions to match your configuration.

Simplified installation instructions for OpenMAMA with Solace middleware bridge [are available](install.md).

---

## Goals

The goal of this tutorial is to demonstrate the most basic messaging interaction using OpenMAMA with the Solace middleware bridge.

This tutorial will show you how to publish a message with one string field to a specific topic on a Solace message router.

---

## Solace Message Router Properties

In order to send or receive messages to a Solace message router, you need to know a few details of how to connect to the Solace message router. Specifically you need to know the following:

Resource | Value | Description
------------ | ------------- | -------------
Host | String of the form `DNS name` or `IP:Port` | This is the address clients use when connecting to the Solace message router to send and receive messages. For a Solace VMR this there is only a single interface so the IP is the same as the management IP address. For Solace message router appliances this is the host address of the message-backbone.
Message VPN | String | The Solace message router Message VPN that this client should connect to. The simplest option is to use the `default` message-vpn which is present on all Solace message routers and fully enabled for message traffic on Solace VMRs.
Client Username | String | The client username. For the Solace VMR default message VPN, authentication is disabled by default, so this can be any value.
Client Password | String | The optional client password. For the Solace VMR default message VPN, authentication is disabled by default, so this can be any value or omitted.

---

## Trying It Yourself

This tutorial is available in [GitHub](https://github.com/dfedorov-solace/solace-openmama-hello-world).

Clone the GitHub repository containing the tutorial.

```
$ git clone git://github.com/dfedorov-solace/solace-openmama-hello-world.git
$ cd solace-openmama-hello-world
```

Building instructions for both **Linux** (with `GCC`) and **Windows** (with `cl`) are provided in this tutorial at the of the [Initialize](#initialize) section.

To run the application on **Linux**:

```
$ ./topicPublishOne
```

To run the application on **Windows**:

```
$ topicPublishOne.exe
```

---

## Hello World

In our first program we’re going to publish one *"Hello World"* message to a specific topic on a Solace message router using OpenMAMA with the Solace middleware bridge.

The program will consist of two major parts:

1. [Initialize](#initialize) Solace middleware bridge
2. [Publish Message](#publish-message)

### Initialize

Any OpenMAMA program begins with initialization that consists of loading a bridge and opening it, in this particular order:
* load
* open

This is how it is done.

Begin by declaring the bridge pointer:

```c
mamaBridge bridge = NULL;
```

Then we need to load the bridge, referring to it by its name (**"solace"**), and open it by calling `mama_open()`:

```c
mama_loadBridge(&bridge, "solace");
mama_open();
```

Opening of the bridge must have a corresponding closing `mama_close()` call:

```c
mama_close();
```

This is already a program that can be compiled and executed, let's add to it some console messages that would help us to watch it running, and some rudimentary error handling.

[*source*](https://github.com/dfedorov-solace/solace-openmama-hello-world/blob/328221ea38af106ae987e6af1c6d5d18536dc2f5/topicPublishOne.c)

```c
#include <stdio.h>
#include <mama/mama.h>

int main(int argc, const char** argv)
{
    printf("Solace OpenMAMA tutorial.\nPublishing one message with OpenMAMA.\n");

    mama_status status;
    mamaBridge bridge = NULL;
    // load Solace middleware bridge and open it
    if (((status = mama_loadBridge(&bridge, "solace")) == MAMA_STATUS_OK) &&
        ((status = mama_open()) == MAMA_STATUS_OK))
    {
        printf("Closing Solace middleware bridge.\n");
        mama_close();
        // normal exit
        exit(0);
    }
    printf("OpenMAMA error: %s\n", mamaStatus_stringForStatus(status));
    exit(status);
}
```

At this point our program needs to be linked with `libmama` (`libmamac` or `libmamacmdd`on Windows) and it requires OpenMAMA headers to compile.

On **Linux**, assuming OpenMAMA installed into `/opt/openmama`:
```
$ gcc -o topicPublishOne topicPublishOne.c -I/opt/openmama/include -L/opt/openmama/lib -lmama
```

On **Windows**, assuming OpenMAMA is at `<openmama>` directory:
```
$ cl topicPublishOne.c /I<openmama>\mama\c_cpp\src\c /I<openmama>\common\c_cpp\src\c\windows -I<openmama>\common\c_cpp\src\c <openmama>\Debug\libmamacmdd.lib
```

When we run this program, it seems like nothing happened:

```
$ ./topicPublishOne
Solace OpenMAMA tutorial.
Publishing one message with OpenMAMA.
2016-07-12 13:58:27: Failed to open properties file.
2016-07-12 13:58:27:
********************************************************************************
Note: This build of the MAMA API is not enforcing entitlement checks.
Please see the Licensing file for details
**********************************************************************************
Closing Solace middleware bridge.
```

But in fact OpenMAMA has successfully initialized with the **Solace middleware bridge** and is ready to publish a message.

### Publish Message

You have definitely noticed the `“Failed to open properties file”` log message (if it appears twice, ignore one of them, it is a well known OpenMAMA [pickle](https://github.com/OpenMAMA/OpenMAMA/issues/37)).

It means that MAMA at run-time is looking for the **properties file** and we will create that file a bit later.

At this point we can successfully load the Solace middleware bridge and open it, which means a successful initialization of OpenMAMA. Now we can publish a message.

In order to publish a message we need to do the following **in this particular order**:

1. [Create a transport](#create-transport) for the Solace middleware bridge

2. [Create a publisher](#create-publisher) that uses this transport

3. [Create a message](#create-message)

4. Use the publisher to [send the message](#send-message)

#### Create transport

Creating of transport includes two steps in this particular order:
* allocate
* create

```c
mamaTransport transport = NULL;
mamaTransport_allocate(&transport);
mamaTransport_create(transport, "vmr", bridge);
```

Notice the name of the transport: `“vmr”`. This is the alias we will refer to from the **properties file**.

Don’t forget to destroy the transport before closing the bridge:

```c
mamaTransport_destroy(transport);
```

This is how our program looks now, let's compile and run it.

[*source*](https://github.com/dfedorov-solace/solace-openmama-hello-world/blob/1fd2e2050b1393c2843d670968e8f43721c3369d/topicPublishOne.c)

```c
#include <stdio.h>
#include <mama/mama.h>

int main(int argc, const char** argv)
{
    printf("Solace OpenMAMA tutorial.\nPublishing one message with OpenMAMA.\n");

    mama_status status;
    mamaBridge bridge = NULL;
    // load Solace middleware bridge and open it
    if (((status = mama_loadBridge(&bridge, "solace")) == MAMA_STATUS_OK) &&
        ((status = mama_open()) == MAMA_STATUS_OK))
    {
        // create transport
        mamaTransport transport = NULL;
        if (((status = mamaTransport_allocate(&transport)) == MAMA_STATUS_OK) &&
            ((status = mamaTransport_create(transport, "vmr", bridge)) == MAMA_STATUS_OK))
        {
            printf("Closing Solace middleware bridge.\n");
            mamaTransport_destroy(transport);
            mama_close();
            // normal exit
            exit(0);
        }
    }
    printf("OpenMAMA error: %s\n", mamaStatus_stringForStatus(status));
    exit(status);
}
```

Now our program runs with a failure when creating of the Solace middleware bridge transport, the failure error code is `STATUS_PLATFORM`:

```
2016-06-28 14:08:11: Warn SOLACE-MW-Bridge: (mama/c_cpp/src/c/bridge/solace/logging.c:129): [API] solClientOS.c:3394 (7f1192289700) TCP connection failure for fd 7, error = Connection refused (111)
2016-06-28 14:08:11: Warn SOLACE-MW-Bridge: (mama/c_cpp/src/c/bridge/solace/logging.c:129): [API] solClient.c:11775 (7f1192289700) Session '(c0,s1)' error attempting transport connection, client name 'localhost.localdomain/13964/#00000001', peer address 'IP 127.0.0.1:55555', connection 'tcp_TxRx' local address 'IP 127.0.0.1:57885'
2016-06-28 14:08:11: Warn SOLACE-MW-Bridge: (mama/c_cpp/src/c/bridge/solace/logging.c:129): [API] solClient.c:11276 (7f1192289700) Protocol or communication error when attempting to login for session '(c0,s1)'; are session HOST and PORT correct? client name 'localhost.localdomain/13964/#00000001', peer address 'IP 127.0.0.1:55555', connection 'tcp_TxRx' local address 'IP 127.0.0.1:57885'
2016-06-28 14:08:11: Warn SOLACE-MW-Bridge: (mama/c_cpp/src/c/bridge/solace/transport.c:480): Error in solClient_session_connect() - ReturnCode='Not ready', SubCode='SOLCLIENT_SUBCODE_COMMUNICATION_ERROR', ResponseCode=0, Info='solClientOS.c:3394                   (7f1192289700) TCP connection failure for fd 7, error = Connection refused (111)'
OpenMAMA error: STATUS_PLATFORM
```

This means we cannot go on without configuring a transport for the Solace middleware bridge, and that transport is the **Solace message router**.

Configuring transport for the Solace middleware bridge means creating and editing a configuration file. The recommended name for this file is **mama.properties** and its location needs to be known to the bridge.

Create a text file named **mama.properties** and add to it a minimum set of properties for the **Solace message router**:

[*source*](https://github.com/dfedorov-solace/solace-openmama-hello-world/blob/master/mama.properties)

```
mama.solace.transport.vmr.session_host=192.168.1.75
mama.solace.transport.vmr.session_username=default
mama.solace.transport.vmr.session_vpn_name=default
mama.solace.transport.vmr.allow_recover_gaps=false
```

Notice how `solace` and `vmr` property token names are the same as in `mama_loadBridge(&bridge, "solace")` and `mamaTransport_create(transport, "vmr", bridge)` calls.

The `session_host` parameter is the IP address of the **Solace message router**.

Now we need to modify our program to refer to this **properties file** by its name and location (in the current directory: `"."`):

```c
mama_openWithProperties(".","mama.properties");
```

This is how our program looks now, let's compile and run it.

[*source*](https://github.com/dfedorov-solace/solace-openmama-hello-world/blob/831b7f3c20cba6c6a5e14abe10b9036a9a0865e7/topicPublishOne.c)

```c
#include <stdio.h>
#include <mama/mama.h>

int main(int argc, const char** argv)
{
    printf("Solace OpenMAMA tutorial.\nPublishing one message with OpenMAMA.\n");

    mama_status status;
    mamaBridge bridge = NULL;
    // load Solace middleware bridge and open it
    if (((status = mama_loadBridge(&bridge, "solace")) == MAMA_STATUS_OK) &&
        ((status = mama_openWithProperties(".","mama.properties")) == MAMA_STATUS_OK))
    {
        // create transport
        mamaTransport transport = NULL;
        if (((status = mamaTransport_allocate(&transport)) == MAMA_STATUS_OK) &&
            ((status = mamaTransport_create(transport, "vmr", bridge)) == MAMA_STATUS_OK))
        {
            printf("Closing Solace middleware bridge.\n");
            mamaTransport_destroy(transport);
            mama_close();
            // normal exit
            exit(0);
        }
    }
    printf("OpenMAMA error: %s\n", mamaStatus_stringForStatus(status));
    exit(status);
}
```

Now our program runs without any errors and it successfully connects to the **Solace message router**. If you sleep the main thread before the `mamaTransport_destroy(transport)` call, you can see use **SolAdmin** to see this program as a client connected to the **Solace message router**.

#### Create publisher

The only thing we want to happen with this program is to publish the "Hello World" message and for that we need to create a publisher.

A publisher is created for a specific topic (named `“tutorial.topic”` in this tutorial) and the already created transport:

```c
mamaPublisher publisher = NULL;
mamaPublisher_create(&publisher, transport, "tutorial.topic", NULL, NULL);
```

It needs to have a corresponding `destroy` call:

```c
mamaPublisher_destroy(publisher);
```

The publisher is ready, let's create the message we're going to publish.

#### Create message

A message is created with one call, but it is created empty, so we need to add a field with the string `"Hello World"` in it.

```c
mamaMsg message = NULL;
mamaMsg_create(&message);
mamaMsg_addString(message, "MyGreetingField", 99, "Hello World");
```

Notice that our field has a *name* (`"MyGreetingField"`) and a *field identifier* a.k.a. *FID* (`99`). Both are of arbitrary values in this tutorial, but if your program publishes market data, their values need to be assigned according to the **data dictionary**.

#### Send message

At this point the only things left are to send the message and to delete it afterwards to avoid memory leaks:

```c
mamaPublisher_send(publisher, message);
mamaMsg_destroy(message);
```

This is the final look of our program, let's compile and run it.

[*source*](https://github.com/dfedorov-solace/solace-openmama-hello-world/blob/master/topicPublishOne.c)

```c
#include <stdio.h>
#include <mama/mama.h>

int main(int argc, const char** argv)
{
    printf("Solace OpenMAMA tutorial.\nPublishing one message with OpenMAMA.\n");

    mama_status status;
    mamaBridge bridge = NULL;
    // load Solace middleware bridge and open it
    if (((status = mama_loadBridge(&bridge, "solace")) == MAMA_STATUS_OK) &&
        ((status = mama_openWithProperties(".","mama.properties")) == MAMA_STATUS_OK))
    {
        // 1. create transport and 2. create publisher
        mamaTransport transport = NULL;
        mamaPublisher publisher = NULL;
        if (((status = mamaTransport_allocate(&transport)) == MAMA_STATUS_OK) &&
            ((status = mamaTransport_create(transport, "vmr", bridge)) == MAMA_STATUS_OK) &&
            ((status = mamaPublisher_create(&publisher, transport, "tutorial.topic", NULL, NULL)) == MAMA_STATUS_OK))
        {
            // 3. create message and add a string field to it
            mamaMsg message = NULL;
            if (((status = mamaMsg_create(&message)) == MAMA_STATUS_OK) &&
                ((status = mamaMsg_addString(message, "MyGreetingField", 99, "Hello World")) == MAMA_STATUS_OK))
            {
                // 4. send the message
                if ((status = mamaPublisher_send(publisher, message)) == MAMA_STATUS_OK)
                {
                    // notice that "destroy" calls are in the opposite order of "create" calls
                    mamaMsg_destroy(message);
                    printf("Message published, closing Solace middleware bridge.\n");
                    mamaPublisher_destroy(publisher);
                    mamaTransport_destroy(transport);
                    mama_close();
                    // normal exit
                    exit(0);
                }
            }
        }
    }
    printf("OpenMAMA error: %s\n", mamaStatus_stringForStatus(status));
    exit(status);
}
```

This is the program's console output (on **Linux**):

```
$ ./topicPublishOne
Solace OpenMAMA tutorial.
Publishing one message with OpenMAMA.
2016-07-13 11:46:10:
********************************************************************************
Note: This build of the MAMA API is not enforcing entitlement checks.
Please see the Licensing file for details
**********************************************************************************
2016-07-13 11:46:10: mamaTransport_create(): No entitlement bridge specified for transport vmr. Defaulting to noop.
Message published, closing Solace middleware bridge.
```

You can see the message published by listening for it on the **Solace message router** with the [`sdkperf_c` utility](http://dev.solacesystems.com/docs/sdkperf-user-guide/):

```
$ ./sdkperf_c -cip=192.168.1.75 -cu=default@default -stl=">" -md

CPU mask currently set to: 0x0003.  To modify use linux cmd: taskset
CPU Speed calculated (Hz): 2980249271
Client naming used:
    logging ID   = 000001
    username     = default
    vpn          = default
    client names = sdk generated.

APP NOTICE Wed Jul 13 11:45:59.762 2016 PerfClientCollection.cpp:95          (7f65461f9740) Master random seed used : 261933454
APP NOTICE Wed Jul 13 11:45:59.786 2016 AbstractClientCollection.cpp:1744    (7f65461f9740) Router capabilities: (PFG=1, SFG=1, TEMP=1, JNDI=1, Z=1)
Receiving messages.  Ctrl-c to exit.
^^^^^^^^^^^^^^^^^^ Start Message ^^^^^^^^^^^^^^^^^^^^^^^^^^^
Destination:                            Topic 'tutorial/topic'
Class Of Service:                       COS_1
DeliveryMode:                           DIRECT
User Property Map:
  Key 'U' (STRING) dfedorov
  Key 'H' (STRING) localhost.localdomain
  Key 'P' (INT32) 5086
Binary Attachment:                      len=44
  31 01 2f 00 00 00 2a 0c  04 00 63 18 13 65 4d 79      1./...*.   ..c..eMy
  47 72 65 65 74 69 6e 67  46 69 65 6c 64 00 1c 0e      Greeting   Field...
  48 65 6c 6c 6f 20 57 6f  72 6c 64 00                  Hello Wo   rld.

^^^^^^^^^^^^^^^^^^ End Message ^^^^^^^^^^^^^^^^^^^^^^^^^^^
```

Between the *Start Message* and *End Message* console output you can see the published message topic `'tutorial/topic'` as **Destination** and the message with the string field **"Hello World"** as **Binary Attachment**.

Congratulations! You have now successfully published a message on a Solace message router using OpenMAMA with the Solace middleware bridge.

If you have any issues with this program, check the [Solace community](http://dev.solacesystems.com/community/) for answers to common issues.
