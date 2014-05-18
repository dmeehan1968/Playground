/*  =========================================================================
    nom_server - nom server

    =========================================================================
*/

/*
@header
    Description of class for man page.
@discuss
    Detailed discussion of the class, if any.
@end
*/

#include <czmq.h>
#include "../include/nom_msg.h"
#include "../include/nom_server.h"

//  ---------------------------------------------------------------------
//  Forward declarations for the two main classes we use here

typedef struct _server_t server_t;
typedef struct _client_t client_t;

//  This structure defines the context for each running server. Store
//  whatever properties and structures you need for the server.

struct _server_t {
    //  These properties must always be present in the server_t
    //  and are set by the generated engine; do not modify them!
    zctx_t *ctx;                //  Parent ZeroMQ context
    zlog_t *log;                //  For any logging needed
    zconfig_t *config;          //  Current loaded configuration
    
    //  Add any properties you need here
};

//  ---------------------------------------------------------------------
//  This structure defines the state for each client connection. It will
//  be passed to each action in the 'self' argument.

struct _client_t {
    //  These properties must always be present in the client_t
    //  and are set by the generated engine; do not modify them!
    server_t *server;           //  Reference to parent server
    nom_msg_t *request;         //  Last received request
    nom_msg_t *reply;           //  Reply to send out, if any

    //  These properties are specific for this application
};

//  Include the generated server engine
#include "nom_server_engine.h"

//  Allocate properties and structures for a new server instance.
//  Return 0 if OK, or -1 if there was an error.

static int
server_initialize (server_t *self)
{
    //  Construct properties here
    return 0;
}

//  Free properties and structures for a server instance

static void
server_terminate (server_t *self)
{
    //  Destroy properties here
}


//  Allocate properties and structures for a new client connection and
//  optionally engine_set_next_event (). Return 0 if OK, or -1 on error.

static int
client_initialize (client_t *self)
{
    //  Construct properties here
    return 0;
}

//  Free properties and structures for a client connection

static void
client_terminate (client_t *self)
{
    //  Destroy properties here
}

//  --------------------------------------------------------------------------
//  Selftest

void
nom_server_test (bool verbose)
{
    printf (" * nom_server: \n");

    //  @selftest
    zctx_t *ctx = zctx_new ();

    nom_server_t *self = nom_server_new ();
    assert (self);

    nom_server_set(self, "server/timeout", "500");
    nom_server_set(self, "server/animate", "1");

    nom_server_bind (self, "tcp://127.0.0.1:5670");

    void *dealer = zsocket_new (ctx, ZMQ_DEALER);
    assert (dealer);
    zsocket_set_rcvtimeo (dealer, 2000);
    zsocket_connect (dealer, "tcp://127.0.0.1:5670");

    nom_msg_t *request, *reply;

    // Attempt to open peering by sending an OHAI

    request = nom_msg_new (NOM_MSG_OHAI);
    //  Set properties for test request
    nom_msg_send (&request, dealer);
    reply = nom_msg_recv (dealer);
    assert (reply);
    assert (nom_msg_id (reply) == NOM_MSG_OHAI_OK);
    nom_msg_destroy (&reply);

    // Attempt to use Peering

    // Send a ICANHAZ and see if we get CHEEZBURGER

    request = nom_msg_new (NOM_MSG_ICANHAZ);
    //  Set properties for test request
    nom_msg_send (&request, dealer);
    reply = nom_msg_recv (dealer);
    assert (reply);
    assert (nom_msg_id (reply) == NOM_MSG_CHEEZBURGER);
    nom_msg_destroy (&reply);

    // Send HUGZ and get and HUGZ-OK

    request = nom_msg_new (NOM_MSG_HUGZ);
    //  Set properties for test request
    nom_msg_send (&request, dealer);
    reply = nom_msg_recv (dealer);
    assert (reply);
    assert (nom_msg_id (reply) == NOM_MSG_HUGZ_OK);
    nom_msg_destroy (&reply);

    // Wait for a timeout, server should send HUGZ, we reply HUGZ-OK

    reply = nom_msg_recv (dealer);
    assert (reply);
    assert (nom_msg_id (reply) == NOM_MSG_HUGZ);
    nom_msg_destroy (&reply);
    request = nom_msg_new(NOM_MSG_HUGZ_OK);
    nom_msg_send(&request, dealer);

    // Make sure the session is still alive with an ICANHAZ

    request = nom_msg_new (NOM_MSG_ICANHAZ);
    //  Set properties for test request
    nom_msg_send (&request, dealer);
    reply = nom_msg_recv (dealer);
    assert (reply);
    assert (nom_msg_id (reply) == NOM_MSG_CHEEZBURGER);
    nom_msg_destroy (&reply);

    nom_server_destroy (&self);

    zctx_destroy (&ctx);
    //  @end

    //  No clean way to wait for a background thread to exit
    //  Under valgrind this will randomly show as leakage
    //  Reduce this by giving server thread time to exit
    zclock_sleep (200);
    zclock_log("OK");
}
