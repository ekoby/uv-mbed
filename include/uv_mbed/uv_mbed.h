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

#ifndef UV_MBED_H
#define UV_MBED_H

#include <uv.h>
#include <stdbool.h>

#include "tls_engine.h"

typedef struct uv_mbed_s uv_mbed_t;
#if !ENABLE_OPENSSL
typedef struct bio BIO;
#endif

int uv_mbed_init(uv_loop_t *l, uv_mbed_t *mbed, tls_context *tls);
int uv_mbed_keepalive(uv_mbed_t *mbed, int keepalive, unsigned int delay);
int uv_mbed_nodelay(uv_mbed_t *mbed, int nodelay);

int uv_mbed_connect(uv_connect_t *req, uv_mbed_t* mbed, const char* host, int port, uv_connect_cb cb);
int uv_mbed_connect_addr(uv_connect_t *req, uv_mbed_t* mbed, const struct addrinfo *addr, uv_connect_cb cb);
int uv_mbed_set_blocking(uv_mbed_t* mbed, int blocking);

int uv_mbed_read(uv_mbed_t* client, uv_alloc_cb, uv_read_cb);
int uv_mbed_write(uv_write_t *req, uv_mbed_t *mbed, uv_buf_t *buf, uv_write_cb cb);
int uv_mbed_close(uv_mbed_t* session, uv_close_cb close_cb);

int uv_mbed_free(uv_mbed_t* session);

struct uv_mbed_s {
    uv_stream_t _stream;
    uv_tcp_t socket;

    tls_context *tls;
    tls_engine *tls_engine;

    uv_connect_t *conn_req; //a place to stash a connection request
};

#endif //UV_MBED_H
