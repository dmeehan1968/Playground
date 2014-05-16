/*  =========================================================================
    nom_msg - nom protocol
    
    Codec header for nom_msg.

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

#ifndef __NOM_MSG_H_INCLUDED__
#define __NOM_MSG_H_INCLUDED__

/*  These are the nom_msg messages:

    OHAI - 

    OHAI_OK - 

    WTF - 

    ICANHAZ - 

    CHEEZBURGER - 

    HUGZ - 

    HUGZ_OK - 
*/


#define NOM_MSG_OHAI                        1
#define NOM_MSG_OHAI_OK                     2
#define NOM_MSG_WTF                         3
#define NOM_MSG_ICANHAZ                     4
#define NOM_MSG_CHEEZBURGER                 5
#define NOM_MSG_HUGZ                        6
#define NOM_MSG_HUGZ_OK                     7

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _nom_msg_t nom_msg_t;

//  @interface
//  Create a new nom_msg
CZMQ_EXPORT nom_msg_t *
    nom_msg_new (int id);

//  Destroy the nom_msg
CZMQ_EXPORT void
    nom_msg_destroy (nom_msg_t **self_p);

//  Parse a nom_msg from zmsg_t. Returns a new object, or NULL if
//  the message could not be parsed, or was NULL. If the socket type is
//  ZMQ_ROUTER, then parses the first frame as a routing_id. Destroys msg
//  and nullifies the msg refernce.
CZMQ_EXPORT nom_msg_t *
    nom_msg_decode (zmsg_t **msg_p);

//  Encode nom_msg into zmsg and destroy it. Returns a newly created
//  object or NULL if error. Use when not in control of sending the message.
//  If the socket_type is ZMQ_ROUTER, then stores the routing_id as the
//  first frame of the resulting message.
CZMQ_EXPORT zmsg_t *
    nom_msg_encode (nom_msg_t **self_p);

//  Receive and parse a nom_msg from the socket. Returns new object, 
//  or NULL if error. Will block if there's no message waiting.
CZMQ_EXPORT nom_msg_t *
    nom_msg_recv (void *input);

//  Receive and parse a nom_msg from the socket. Returns new object, 
//  or NULL either if there was no input waiting, or the recv was interrupted.
CZMQ_EXPORT nom_msg_t *
    nom_msg_recv_nowait (void *input);

//  Send the nom_msg to the output, and destroy it
CZMQ_EXPORT int
    nom_msg_send (nom_msg_t **self_p, void *output);

//  Send the nom_msg to the output, and do not destroy it
CZMQ_EXPORT int
    nom_msg_send_again (nom_msg_t *self, void *output);

//  Encode the OHAI 
CZMQ_EXPORT zmsg_t *
    nom_msg_encode_ohai (
);

//  Encode the OHAI_OK 
CZMQ_EXPORT zmsg_t *
    nom_msg_encode_ohai_ok (
);

//  Encode the WTF 
CZMQ_EXPORT zmsg_t *
    nom_msg_encode_wtf (
);

//  Encode the ICANHAZ 
CZMQ_EXPORT zmsg_t *
    nom_msg_encode_icanhaz (
);

//  Encode the CHEEZBURGER 
CZMQ_EXPORT zmsg_t *
    nom_msg_encode_cheezburger (
);

//  Encode the HUGZ 
CZMQ_EXPORT zmsg_t *
    nom_msg_encode_hugz (
);

//  Encode the HUGZ_OK 
CZMQ_EXPORT zmsg_t *
    nom_msg_encode_hugz_ok (
);


//  Send the OHAI to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
CZMQ_EXPORT int
    nom_msg_send_ohai (void *output);
    
//  Send the OHAI_OK to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
CZMQ_EXPORT int
    nom_msg_send_ohai_ok (void *output);
    
//  Send the WTF to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
CZMQ_EXPORT int
    nom_msg_send_wtf (void *output);
    
//  Send the ICANHAZ to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
CZMQ_EXPORT int
    nom_msg_send_icanhaz (void *output);
    
//  Send the CHEEZBURGER to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
CZMQ_EXPORT int
    nom_msg_send_cheezburger (void *output);
    
//  Send the HUGZ to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
CZMQ_EXPORT int
    nom_msg_send_hugz (void *output);
    
//  Send the HUGZ_OK to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
CZMQ_EXPORT int
    nom_msg_send_hugz_ok (void *output);
    
//  Duplicate the nom_msg message
CZMQ_EXPORT nom_msg_t *
    nom_msg_dup (nom_msg_t *self);

//  Print contents of message to stdout
CZMQ_EXPORT void
    nom_msg_print (nom_msg_t *self);

//  Get/set the message routing id
CZMQ_EXPORT zframe_t *
    nom_msg_routing_id (nom_msg_t *self);
CZMQ_EXPORT void
    nom_msg_set_routing_id (nom_msg_t *self, zframe_t *routing_id);

//  Get the nom_msg id and printable command
CZMQ_EXPORT int
    nom_msg_id (nom_msg_t *self);
CZMQ_EXPORT void
    nom_msg_set_id (nom_msg_t *self, int id);
CZMQ_EXPORT const char *
    nom_msg_command (nom_msg_t *self);

//  Self test of this class
CZMQ_EXPORT int
    nom_msg_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
