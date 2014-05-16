/*  =========================================================================
    nom_server - nom server

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

    * The XML model used for this code generation: nom_server.xml
    * The code generation script that built this file: zproto_server_c
    ************************************************************************

    =========================================================================
*/

#ifndef __NOM_SERVER_H_INCLUDED__
#define __NOM_SERVER_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _nom_server_t nom_server_t;

//  @interface
//  Create a new nom_server
nom_server_t *
    nom_server_new (void);

//  Destroy the nom_server
void
    nom_server_destroy (nom_server_t **self_p);

//  Load server configuration data
void
    nom_server_configure (nom_server_t *self, const char *config_file);

//  Set one configuration option value
void
    nom_server_set (nom_server_t *self, const char *path, const char *value);

//  Binds the server to an endpoint, formatted as printf string
long
    nom_server_bind (nom_server_t *self, const char *format, ...);

//  Self test of this class
void
    nom_server_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
