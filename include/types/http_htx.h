/*
 * include/types/http_htx.h
 * This file defines everything related to HTTP manipulation using the internal
 * representation.
 *
 * Copyright (C) 2018 HAProxy Technologies, Christopher Faulet <cfaulet@haproxy.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, version 2.1
 * exclusively.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _TYPES_HTTP_HTX_H
#define _TYPES_HTTP_HTX_H

#include <ebistree.h>

#include <common/buf.h>
#include <common/http.h>
#include <common/htx.h>
#include <common/ist.h>

/* Context used to find/remove an HTTP header. */
struct http_hdr_ctx {
	struct htx_blk *blk;
	struct ist     value;
	uint16_t       lws_before;
	uint16_t       lws_after;
};


/* Structure used to build the header list of an HTTP reply */
struct http_reply_hdr {
	struct ist  name;  /* the header name */
	struct list value; /* the log-format string value */
	struct list list;  /* header chained list */
};

#define HTTP_REPLY_EMPTY    0x00 /* the reply has no payload */
#define HTTP_REPLY_ERRMSG   0x01 /* the reply is an error message */
#define HTTP_REPLY_ERRFILES 0x02 /* the reply references an http-errors section */
#define HTTP_REPLY_RAW      0x03 /* the reply use a raw payload */
#define HTTP_REPLY_LOGFMT   0x04 /* the reply use a log-format payload */

/* Uses by HAProxy to generate internal responses */
struct http_reply {
	unsigned char type;                   /* HTTP_REPLY_* */
	int status;                           /* The response status code */
	char *ctype;                          /* The response content-type, may be NULL */
	struct list hdrs;                     /* A list of http_reply_hdr */
	union {
		struct list   fmt;            /* A log-format string (type = HTTP_REPLY_LOGFMT) */
		struct buffer obj;            /* A raw string (type = HTTP_REPLY_RAW) */
		struct buffer *errmsg;        /* The error message to use as response (type = HTTP_REPLY_ERRMSG).
					       * may be NULL, if so rely on the proxy error messages */
		char          *http_errors;   /* The http-errors section to use (type = HTTP_REPLY_ERRFILES).
					       * Should be resolved during post-check */
	} body;
};

/* A custom HTTP error message load from a row file and converted in HTX. The
 * node key is the file path.
 */
struct http_error {
	struct buffer msg;
	struct ebpt_node node;
};

/* http-errors section and parameters. */
struct http_errors {
	char *id;                             /* unique identifier */
	struct {
		char *file;                   /* file where the section appears */
		int   line;                   /* line where the section appears */
	} conf;                               /* config information */

	struct buffer *errmsg[HTTP_ERR_SIZE]; /* customized error messages for known errors */
	struct list list;                     /* http-errors list */
};

#endif /* _TYPES_HTTP_HTX_H */
