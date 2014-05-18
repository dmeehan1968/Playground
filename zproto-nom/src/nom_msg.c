/*  =========================================================================
    nom_msg - nom protocol

    Codec class for nom_msg.

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

    * The XML model used for this code generation: nom_msg.xml
    * The code generation script that built this file: zproto_codec_c
    ************************************************************************
    
    =========================================================================
*/

/*
@header
    nom_msg - nom protocol
@discuss
@end
*/

#include <czmq.h>
#include "../include/nom_msg.h"

//  Structure of our class

struct _nom_msg_t {
    zframe_t *routing_id;               //  Routing_id from ROUTER, if any
    int id;                             //  nom_msg message ID
    byte *needle;                       //  Read/write pointer for serialization
    byte *ceiling;                      //  Valid upper limit for read pointer
};

//  --------------------------------------------------------------------------
//  Network data encoding macros

//  Put a block of octets to the frame
#define PUT_OCTETS(host,size) { \
    memcpy (self->needle, (host), size); \
    self->needle += size; \
}

//  Get a block of octets from the frame
#define GET_OCTETS(host,size) { \
    if (self->needle + size > self->ceiling) \
        goto malformed; \
    memcpy ((host), self->needle, size); \
    self->needle += size; \
}

//  Put a 1-byte number to the frame
#define PUT_NUMBER1(host) { \
    *(byte *) self->needle = (host); \
    self->needle++; \
}

//  Put a 2-byte number to the frame
#define PUT_NUMBER2(host) { \
    self->needle [0] = (byte) (((host) >> 8)  & 255); \
    self->needle [1] = (byte) (((host))       & 255); \
    self->needle += 2; \
}

//  Put a 4-byte number to the frame
#define PUT_NUMBER4(host) { \
    self->needle [0] = (byte) (((host) >> 24) & 255); \
    self->needle [1] = (byte) (((host) >> 16) & 255); \
    self->needle [2] = (byte) (((host) >> 8)  & 255); \
    self->needle [3] = (byte) (((host))       & 255); \
    self->needle += 4; \
}

//  Put a 8-byte number to the frame
#define PUT_NUMBER8(host) { \
    self->needle [0] = (byte) (((host) >> 56) & 255); \
    self->needle [1] = (byte) (((host) >> 48) & 255); \
    self->needle [2] = (byte) (((host) >> 40) & 255); \
    self->needle [3] = (byte) (((host) >> 32) & 255); \
    self->needle [4] = (byte) (((host) >> 24) & 255); \
    self->needle [5] = (byte) (((host) >> 16) & 255); \
    self->needle [6] = (byte) (((host) >> 8)  & 255); \
    self->needle [7] = (byte) (((host))       & 255); \
    self->needle += 8; \
}

//  Get a 1-byte number from the frame
#define GET_NUMBER1(host) { \
    if (self->needle + 1 > self->ceiling) \
        goto malformed; \
    (host) = *(byte *) self->needle; \
    self->needle++; \
}

//  Get a 2-byte number from the frame
#define GET_NUMBER2(host) { \
    if (self->needle + 2 > self->ceiling) \
        goto malformed; \
    (host) = ((uint16_t) (self->needle [0]) << 8) \
           +  (uint16_t) (self->needle [1]); \
    self->needle += 2; \
}

//  Get a 4-byte number from the frame
#define GET_NUMBER4(host) { \
    if (self->needle + 4 > self->ceiling) \
        goto malformed; \
    (host) = ((uint32_t) (self->needle [0]) << 24) \
           + ((uint32_t) (self->needle [1]) << 16) \
           + ((uint32_t) (self->needle [2]) << 8) \
           +  (uint32_t) (self->needle [3]); \
    self->needle += 4; \
}

//  Get a 8-byte number from the frame
#define GET_NUMBER8(host) { \
    if (self->needle + 8 > self->ceiling) \
        goto malformed; \
    (host) = ((uint64_t) (self->needle [0]) << 56) \
           + ((uint64_t) (self->needle [1]) << 48) \
           + ((uint64_t) (self->needle [2]) << 40) \
           + ((uint64_t) (self->needle [3]) << 32) \
           + ((uint64_t) (self->needle [4]) << 24) \
           + ((uint64_t) (self->needle [5]) << 16) \
           + ((uint64_t) (self->needle [6]) << 8) \
           +  (uint64_t) (self->needle [7]); \
    self->needle += 8; \
}

//  Put a string to the frame
#define PUT_STRING(host) { \
    size_t string_size = strlen (host); \
    PUT_NUMBER1 (string_size); \
    memcpy (self->needle, (host), string_size); \
    self->needle += string_size; \
}

//  Get a string from the frame
#define GET_STRING(host) { \
    size_t string_size; \
    GET_NUMBER1 (string_size); \
    if (self->needle + string_size > (self->ceiling)) \
        goto malformed; \
    (host) = (char *) malloc (string_size + 1); \
    memcpy ((host), self->needle, string_size); \
    (host) [string_size] = 0; \
    self->needle += string_size; \
}

//  Put a long string to the frame
#define PUT_LONGSTR(host) { \
    size_t string_size = strlen (host); \
    PUT_NUMBER4 (string_size); \
    memcpy (self->needle, (host), string_size); \
    self->needle += string_size; \
}

//  Get a long string from the frame
#define GET_LONGSTR(host) { \
    size_t string_size; \
    GET_NUMBER4 (string_size); \
    if (self->needle + string_size > (self->ceiling)) \
        goto malformed; \
    (host) = (char *) malloc (string_size + 1); \
    memcpy ((host), self->needle, string_size); \
    (host) [string_size] = 0; \
    self->needle += string_size; \
}


//  --------------------------------------------------------------------------
//  Create a new nom_msg

nom_msg_t *
nom_msg_new (int id)
{
    nom_msg_t *self = (nom_msg_t *) zmalloc (sizeof (nom_msg_t));
    self->id = id;
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the nom_msg

void
nom_msg_destroy (nom_msg_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        nom_msg_t *self = *self_p;

        //  Free class properties
        zframe_destroy (&self->routing_id);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Parse a nom_msg from zmsg_t. Returns a new object, or NULL if
//  the message could not be parsed, or was NULL. If the socket type is
//  ZMQ_ROUTER, then parses the first frame as a routing_id. Destroys msg
//  and nullifies the msg refernce.

nom_msg_t *
nom_msg_decode (zmsg_t **msg_p)
{
    assert (msg_p);
    zmsg_t *msg = *msg_p;
    if (msg == NULL)
        return NULL;
        
    nom_msg_t *self = nom_msg_new (0);
    //  Read and parse command in frame
    zframe_t *frame = zmsg_pop (msg);
    if (!frame) 
        goto empty;             //  Malformed or empty

    //  Get and check protocol signature
    self->needle = zframe_data (frame);
    self->ceiling = self->needle + zframe_size (frame);
    uint16_t signature;
    GET_NUMBER2 (signature);
    if (signature != (0xAAA0 | 0))
        goto empty;             //  Invalid signature

    //  Get message id and parse per message type
    GET_NUMBER1 (self->id);

    switch (self->id) {
        case NOM_MSG_OHAI:
            break;

        case NOM_MSG_OHAI_OK:
            break;

        case NOM_MSG_WTF:
            break;

        case NOM_MSG_ICANHAZ:
            break;

        case NOM_MSG_CHEEZBURGER:
            break;

        case NOM_MSG_HUGZ:
            break;

        case NOM_MSG_HUGZ_OK:
            break;

        default:
            goto malformed;
    }
    //  Successful return
    zframe_destroy (&frame);
    zmsg_destroy (msg_p);
    return self;

    //  Error returns
    malformed:
        printf ("E: malformed message '%d'\n", self->id);
    empty:
        zframe_destroy (&frame);
        zmsg_destroy (msg_p);
        nom_msg_destroy (&self);
        return (NULL);
}


//  --------------------------------------------------------------------------
//  Encode nom_msg into zmsg and destroy it. Returns a newly created
//  object or NULL if error. Use when not in control of sending the message.
//  If the socket_type is ZMQ_ROUTER, then stores the routing_id as the
//  first frame of the resulting message.

zmsg_t *
nom_msg_encode (nom_msg_t **self_p)
{
    assert (self_p);
    assert (*self_p);
    
    nom_msg_t *self = *self_p;
    zmsg_t *msg = zmsg_new ();

    size_t frame_size = 2 + 1;          //  Signature and message ID
    switch (self->id) {
        case NOM_MSG_OHAI:
            break;
            
        case NOM_MSG_OHAI_OK:
            break;
            
        case NOM_MSG_WTF:
            break;
            
        case NOM_MSG_ICANHAZ:
            break;
            
        case NOM_MSG_CHEEZBURGER:
            break;
            
        case NOM_MSG_HUGZ:
            break;
            
        case NOM_MSG_HUGZ_OK:
            break;
            
        default:
            printf ("E: bad message type '%d', not sent\n", self->id);
            //  No recovery, this is a fatal application error
            assert (false);
    }
    //  Now serialize message into the frame
    zframe_t *frame = zframe_new (NULL, frame_size);
    self->needle = zframe_data (frame);
    PUT_NUMBER2 (0xAAA0 | 0);
    PUT_NUMBER1 (self->id);

    switch (self->id) {
        case NOM_MSG_OHAI:
            break;

        case NOM_MSG_OHAI_OK:
            break;

        case NOM_MSG_WTF:
            break;

        case NOM_MSG_ICANHAZ:
            break;

        case NOM_MSG_CHEEZBURGER:
            break;

        case NOM_MSG_HUGZ:
            break;

        case NOM_MSG_HUGZ_OK:
            break;

    }
    //  Now send the data frame
    if (zmsg_append (msg, &frame)) {
        zmsg_destroy (&msg);
        nom_msg_destroy (self_p);
        return NULL;
    }
    //  Destroy nom_msg object
    nom_msg_destroy (self_p);
    return msg;
}


//  --------------------------------------------------------------------------
//  Receive and parse a nom_msg from the socket. Returns new object or
//  NULL if error. Will block if there's no message waiting.

nom_msg_t *
nom_msg_recv (void *input)
{
    assert (input);
    zmsg_t *msg = zmsg_recv (input);
    //  If message came from a router socket, first frame is routing_id
    zframe_t *routing_id = NULL;
    if (zsocket_type (zsock_resolve (input)) == ZMQ_ROUTER) {
        routing_id = zmsg_pop (msg);
        //  If message was not valid, forget about it
        if (!routing_id || !zmsg_next (msg))
            return NULL;        //  Malformed or empty
    }
    nom_msg_t * nom_msg = nom_msg_decode (&msg);
    if (zsocket_type (zsock_resolve (input)) == ZMQ_ROUTER)
        nom_msg->routing_id = routing_id;

    return nom_msg;
}


//  --------------------------------------------------------------------------
//  Receive and parse a nom_msg from the socket. Returns new object,
//  or NULL either if there was no input waiting, or the recv was interrupted.

nom_msg_t *
nom_msg_recv_nowait (void *input)
{
    assert (input);
    zmsg_t *msg = zmsg_recv_nowait (input);
    //  If message came from a router socket, first frame is routing_id
    zframe_t *routing_id = NULL;
    if (zsocket_type (zsock_resolve (input)) == ZMQ_ROUTER) {
        routing_id = zmsg_pop (msg);
        //  If message was not valid, forget about it
        if (!routing_id || !zmsg_next (msg))
            return NULL;        //  Malformed or empty
    }
    nom_msg_t * nom_msg = nom_msg_decode (&msg);
    if (zsocket_type (zsock_resolve (input)) == ZMQ_ROUTER)
        nom_msg->routing_id = routing_id;

    return nom_msg;
}


//  --------------------------------------------------------------------------
//  Send the nom_msg to the socket, and destroy it
//  Returns 0 if OK, else -1

int
nom_msg_send (nom_msg_t **self_p, void *output)
{
    assert (self_p);
    assert (*self_p);
    assert (output);

    //  Save routing_id if any, as encode will destroy it
    nom_msg_t *self = *self_p;
    zframe_t *routing_id = self->routing_id;
    self->routing_id = NULL;

    //  Encode nom_msg message to a single zmsg
    zmsg_t *msg = nom_msg_encode (&self);
    
    //  If we're sending to a ROUTER, send the routing_id first
    if (zsocket_type (zsock_resolve (output)) == ZMQ_ROUTER) {
        assert (routing_id);
        zmsg_prepend (msg, &routing_id);
    }
    else
        zframe_destroy (&routing_id);
        
    if (msg && zmsg_send (&msg, output) == 0)
        return 0;
    else
        return -1;              //  Failed to encode, or send
}


//  --------------------------------------------------------------------------
//  Send the nom_msg to the output, and do not destroy it

int
nom_msg_send_again (nom_msg_t *self, void *output)
{
    assert (self);
    assert (output);
    self = nom_msg_dup (self);
    return nom_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Encode OHAI message

zmsg_t * 
nom_msg_encode_ohai (
)
{
    nom_msg_t *self = nom_msg_new (NOM_MSG_OHAI);
    return nom_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode OHAI_OK message

zmsg_t * 
nom_msg_encode_ohai_ok (
)
{
    nom_msg_t *self = nom_msg_new (NOM_MSG_OHAI_OK);
    return nom_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode WTF message

zmsg_t * 
nom_msg_encode_wtf (
)
{
    nom_msg_t *self = nom_msg_new (NOM_MSG_WTF);
    return nom_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode ICANHAZ message

zmsg_t * 
nom_msg_encode_icanhaz (
)
{
    nom_msg_t *self = nom_msg_new (NOM_MSG_ICANHAZ);
    return nom_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode CHEEZBURGER message

zmsg_t * 
nom_msg_encode_cheezburger (
)
{
    nom_msg_t *self = nom_msg_new (NOM_MSG_CHEEZBURGER);
    return nom_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode HUGZ message

zmsg_t * 
nom_msg_encode_hugz (
)
{
    nom_msg_t *self = nom_msg_new (NOM_MSG_HUGZ);
    return nom_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode HUGZ_OK message

zmsg_t * 
nom_msg_encode_hugz_ok (
)
{
    nom_msg_t *self = nom_msg_new (NOM_MSG_HUGZ_OK);
    return nom_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Send the OHAI to the socket in one step

int
nom_msg_send_ohai (
    void *output)
{
    nom_msg_t *self = nom_msg_new (NOM_MSG_OHAI);
    return nom_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the OHAI_OK to the socket in one step

int
nom_msg_send_ohai_ok (
    void *output)
{
    nom_msg_t *self = nom_msg_new (NOM_MSG_OHAI_OK);
    return nom_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the WTF to the socket in one step

int
nom_msg_send_wtf (
    void *output)
{
    nom_msg_t *self = nom_msg_new (NOM_MSG_WTF);
    return nom_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the ICANHAZ to the socket in one step

int
nom_msg_send_icanhaz (
    void *output)
{
    nom_msg_t *self = nom_msg_new (NOM_MSG_ICANHAZ);
    return nom_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the CHEEZBURGER to the socket in one step

int
nom_msg_send_cheezburger (
    void *output)
{
    nom_msg_t *self = nom_msg_new (NOM_MSG_CHEEZBURGER);
    return nom_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the HUGZ to the socket in one step

int
nom_msg_send_hugz (
    void *output)
{
    nom_msg_t *self = nom_msg_new (NOM_MSG_HUGZ);
    return nom_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the HUGZ_OK to the socket in one step

int
nom_msg_send_hugz_ok (
    void *output)
{
    nom_msg_t *self = nom_msg_new (NOM_MSG_HUGZ_OK);
    return nom_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Duplicate the nom_msg message

nom_msg_t *
nom_msg_dup (nom_msg_t *self)
{
    if (!self)
        return NULL;
        
    nom_msg_t *copy = nom_msg_new (self->id);
    if (self->routing_id)
        copy->routing_id = zframe_dup (self->routing_id);
    switch (self->id) {
        case NOM_MSG_OHAI:
            break;

        case NOM_MSG_OHAI_OK:
            break;

        case NOM_MSG_WTF:
            break;

        case NOM_MSG_ICANHAZ:
            break;

        case NOM_MSG_CHEEZBURGER:
            break;

        case NOM_MSG_HUGZ:
            break;

        case NOM_MSG_HUGZ_OK:
            break;

    }
    return copy;
}



//  --------------------------------------------------------------------------
//  Print contents of message to stdout

void
nom_msg_print (nom_msg_t *self)
{
    assert (self);
    switch (self->id) {
        case NOM_MSG_OHAI:
            puts ("OHAI:");
            break;
            
        case NOM_MSG_OHAI_OK:
            puts ("OHAI_OK:");
            break;
            
        case NOM_MSG_WTF:
            puts ("WTF:");
            break;
            
        case NOM_MSG_ICANHAZ:
            puts ("ICANHAZ:");
            break;
            
        case NOM_MSG_CHEEZBURGER:
            puts ("CHEEZBURGER:");
            break;
            
        case NOM_MSG_HUGZ:
            puts ("HUGZ:");
            break;
            
        case NOM_MSG_HUGZ_OK:
            puts ("HUGZ_OK:");
            break;
            
    }
}


//  --------------------------------------------------------------------------
//  Get/set the message routing_id

zframe_t *
nom_msg_routing_id (nom_msg_t *self)
{
    assert (self);
    return self->routing_id;
}

void
nom_msg_set_routing_id (nom_msg_t *self, zframe_t *routing_id)
{
    if (self->routing_id)
        zframe_destroy (&self->routing_id);
    self->routing_id = zframe_dup (routing_id);
}


//  --------------------------------------------------------------------------
//  Get/set the nom_msg id

int
nom_msg_id (nom_msg_t *self)
{
    assert (self);
    return self->id;
}

void
nom_msg_set_id (nom_msg_t *self, int id)
{
    self->id = id;
}

//  --------------------------------------------------------------------------
//  Return a printable command string

const char *
nom_msg_command (nom_msg_t *self)
{
    assert (self);
    switch (self->id) {
        case NOM_MSG_OHAI:
            return ("OHAI");
            break;
        case NOM_MSG_OHAI_OK:
            return ("OHAI_OK");
            break;
        case NOM_MSG_WTF:
            return ("WTF");
            break;
        case NOM_MSG_ICANHAZ:
            return ("ICANHAZ");
            break;
        case NOM_MSG_CHEEZBURGER:
            return ("CHEEZBURGER");
            break;
        case NOM_MSG_HUGZ:
            return ("HUGZ");
            break;
        case NOM_MSG_HUGZ_OK:
            return ("HUGZ_OK");
            break;
    }
    return "?";
}


//  --------------------------------------------------------------------------
//  Selftest

int
nom_msg_test (bool verbose)
{
    printf (" * nom_msg: ");

    //  @selftest
    //  Simple create/destroy test
    nom_msg_t *self = nom_msg_new (0);
    assert (self);
    nom_msg_destroy (&self);

    //  Create pair of sockets we can send through
    zsock_t *input = zsock_new (ZMQ_ROUTER);
    assert (input);
    zsock_connect (input, "inproc://selftest");

    zsock_t *output = zsock_new (ZMQ_DEALER);
    assert (output);
    zsock_bind (output, "inproc://selftest");

    //  Encode/send/decode and verify each message type
    int instance;
    nom_msg_t *copy;
    self = nom_msg_new (NOM_MSG_OHAI);
    
    //  Check that _dup works on empty message
    copy = nom_msg_dup (self);
    assert (copy);
    nom_msg_destroy (&copy);

    //  Send twice from same object
    nom_msg_send_again (self, output);
    nom_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = nom_msg_recv (input);
        assert (self);
        assert (nom_msg_routing_id (self));
        
        nom_msg_destroy (&self);
    }
    self = nom_msg_new (NOM_MSG_OHAI_OK);
    
    //  Check that _dup works on empty message
    copy = nom_msg_dup (self);
    assert (copy);
    nom_msg_destroy (&copy);

    //  Send twice from same object
    nom_msg_send_again (self, output);
    nom_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = nom_msg_recv (input);
        assert (self);
        assert (nom_msg_routing_id (self));
        
        nom_msg_destroy (&self);
    }
    self = nom_msg_new (NOM_MSG_WTF);
    
    //  Check that _dup works on empty message
    copy = nom_msg_dup (self);
    assert (copy);
    nom_msg_destroy (&copy);

    //  Send twice from same object
    nom_msg_send_again (self, output);
    nom_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = nom_msg_recv (input);
        assert (self);
        assert (nom_msg_routing_id (self));
        
        nom_msg_destroy (&self);
    }
    self = nom_msg_new (NOM_MSG_ICANHAZ);
    
    //  Check that _dup works on empty message
    copy = nom_msg_dup (self);
    assert (copy);
    nom_msg_destroy (&copy);

    //  Send twice from same object
    nom_msg_send_again (self, output);
    nom_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = nom_msg_recv (input);
        assert (self);
        assert (nom_msg_routing_id (self));
        
        nom_msg_destroy (&self);
    }
    self = nom_msg_new (NOM_MSG_CHEEZBURGER);
    
    //  Check that _dup works on empty message
    copy = nom_msg_dup (self);
    assert (copy);
    nom_msg_destroy (&copy);

    //  Send twice from same object
    nom_msg_send_again (self, output);
    nom_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = nom_msg_recv (input);
        assert (self);
        assert (nom_msg_routing_id (self));
        
        nom_msg_destroy (&self);
    }
    self = nom_msg_new (NOM_MSG_HUGZ);
    
    //  Check that _dup works on empty message
    copy = nom_msg_dup (self);
    assert (copy);
    nom_msg_destroy (&copy);

    //  Send twice from same object
    nom_msg_send_again (self, output);
    nom_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = nom_msg_recv (input);
        assert (self);
        assert (nom_msg_routing_id (self));
        
        nom_msg_destroy (&self);
    }
    self = nom_msg_new (NOM_MSG_HUGZ_OK);
    
    //  Check that _dup works on empty message
    copy = nom_msg_dup (self);
    assert (copy);
    nom_msg_destroy (&copy);

    //  Send twice from same object
    nom_msg_send_again (self, output);
    nom_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = nom_msg_recv (input);
        assert (self);
        assert (nom_msg_routing_id (self));
        
        nom_msg_destroy (&self);
    }

    zsock_destroy (&input);
    zsock_destroy (&output);
    //  @end

    printf ("OK\n");
    return 0;
}
