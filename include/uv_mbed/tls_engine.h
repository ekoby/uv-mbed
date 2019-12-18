/*
Copyright 2019 NetFoundry, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef UV_MBED_TLS_ENGINE_H
#define UV_MBED_TLS_ENGINE_H

#include <stdlib.h>

typedef enum tls_handshake_st {
    TLS_HS_CONTINUE,
    TLS_HS_COMPLETE,
    TLS_HS_ERROR
} tls_handshake_state;

enum TLS_RESULT {
    TLS_OK = 0,
    TLS_ERR = -1,
    TLS_EOF = -2,
    TLS_READ_AGAIN = -3,
    TLS_MORE_AVAILABLE = -4,
};

typedef struct {

    /**
     * Set the file descriptor for the i/o facility for the TLS. fd will typically be the socket file descriptor of a network connection.
     * @param engine
     * @param fd socker descriptor
     */
    int (*set_fd)(void *engine, int fd);

    tls_handshake_state (*handshake_state)(void *engine);

    /**
     * Initiates/continues TLS handshake.
     * @param engine
     * @param in data received from TLS peer
     * @param in_bytes number of bytes in inbound buffer
     * @param out data to be send to TLS peer
     * @param out_bytes number of bytes to be sent
     * @param maxout outbound buffer size
     */
    tls_handshake_state
    (*handshake)(void *engine, char *in, size_t in_bytes, char *out, size_t *out_bytes, size_t maxout);

    /**
     * Genereate TLS close notify.
     * @param engine
     * @param out outbound buffer
     * @param out_bytes number of outbound bytes written
     * @param maxout size of outbound buffer
     */
    int (*close)(void *engine, char *out, size_t *out_bytes, size_t maxout);

    /**
      * wraps application data into ssl stream format, out bound buffer contains bytes to be sent to TLS peer
      * @param engine
      * @param data
      * @param data_len
      * @param out
      * @param out_bytes
      * @param maxout
      */
    int (*write)(void *engine, const char *data, size_t data_len, char *out, size_t *out_bytes, size_t maxout);

    /**
     * process bytes received from TLS peer. Application data is placed in out buffer.
     * @param engine
     * @param ssl_in bytes received from TLS peer
     * @param ssl_in_len number of bytes received
     * @param out buffer for application data
     * @param out_bytes number of bytes received
     * @param maxout size of out buffer
     */
    int (*read)(void *engine, const char *ssl_in, size_t ssl_in_len, char *out, size_t *out_bytes, size_t maxout);

    int (*strerror)(void *engine, char *err_out, size_t out_len);
} tls_engine_api;

typedef struct {
    void *engine;
    tls_engine_api *api;
} tls_engine;

typedef struct tls_context_s tls_context;

typedef struct {
    /* creates new TLS engine for a host */
    tls_engine *(*new_engine)(void *ctx, const char *host);

    void (*free_engine)(tls_engine *);

    void (*free_ctx)(tls_context *ctx);

    /**
     * (Optional): if you bring your own engine this is probably not needed.
     * This method is provided to set client/server side cert on the default TLS context.
     */
    int (*set_own_cert)(void *ctx, const char *cert_buf, size_t cert_len, const char *key_buf, size_t key_len);

    /**
     * (Optional): if you bring your own engine this is probably not needed.
     * This method is provided to set client/server-side cert backed by an HSM(hardware key) on the default TLS context.
     * if `cert_buf is NULL` certificate is loaded from HSM as well, matched by `key_id` (TODO: not implemented yet)
     */
    int (*set_own_cert_pkcs11)(void *ctx, const char *cert_buf, size_t cert_len,
            const char *pkcs11_lib, const char *pin, const char *slot, const char *key_id);

} tls_context_api;

struct tls_context_s {
    void *ctx;
    tls_context_api *api;
};

tls_context *default_tls_context(const char *ca, size_t ca_len);

#endif //UV_MBED_TLS_ENGINE_H
