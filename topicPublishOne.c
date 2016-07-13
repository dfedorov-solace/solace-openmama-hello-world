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


