/*
    Copyright (c) 2007-2011 iMatix Corporation
    Copyright (c) 2007-2011 Other contributors as noted in the AUTHORS file

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "http_socket.hpp"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <algorithm>

#include "logger.hpp"

static const int HTTP_BUF_SIZE = 512;

zmq::http_socket_t::http_socket_t()
	: tcp_socket_t()
	, write_buf(NULL)
    , write_pos(NULL)
    , write_end(NULL) 
	, read_buf(NULL)
    , read_pos(NULL) {
}

zmq::http_socket_t::~http_socket_t() {
	free(write_buf);
	free(read_buf);
}

int zmq::http_socket_t::write (const void *data, int size) {
	if (!write_buf) {
		// Lazy initialization
		write_buf = (char *)malloc(HTTP_BUF_SIZE);
	}

	//LOGD() << "zmq::http_socket_t::write" << LOG_ENDL();

	// Init header
	if (!write_end) {
		int length = sprintf(write_buf
					, "HTTP/1.1 POST\r\n"\
					  "Connection: Keep-Alive\r\n"\
					  "Content-Type: application/octet-stream\r\n"\
					  "Content-Length: %d\r\n"\
					  "\r\n\r\n", size);

		assert(length > 0 && length < HTTP_BUF_SIZE);

		write_pos = write_buf;
		write_end = write_buf + length;

		//{	// Logging out header
        //    std::string header(write_buf, write_end);
        //    LOGD() << header << LOG_ENDL();
        //}
	}

	// Sending HTTP header first
	if (write_pos < write_end) {
		int remains = write_end - write_pos; 
		int nbytes = tcp_socket_t::write(write_pos, remains);
		
		if (nbytes <= 0) { return nbytes; }
		
		write_pos += nbytes;		
		if (write_pos < write_end) { return 0; }
	}

	// Sending data
	int nbytes = tcp_socket_t::write(data, size);
	
	// Check if data fully sent
	if (nbytes == size) { write_end = NULL; }
	
	return nbytes;
}

int zmq::http_socket_t::read (void *data, int size) {
	if (!read_buf) {
		read_buf = (char *)malloc(HTTP_BUF_SIZE);	
		read_pos = read_buf;
	}

	//LOGD() << "zmq::http_socket_t::read" << LOG_ENDL();

	int remains = HTTP_BUF_SIZE - (read_pos - read_buf);
	int nbytes = tcp_socket_t::read(read_buf, remains);
	if (nbytes <= 0) { return nbytes; }
	
	read_pos += nbytes;
	*read_pos = NULL;

	// Determing header position
	char *it = strstr(read_buf, "HTTP/");
	
	if (it == read_buf) {
		// Check if header is complete
		it = strstr(it, "\r\n\r\n\r\n");
		if (!it) { return 0; }

		//{ // Logging out received header
		//	std::string header(read_buf, it);
		//	LOGD() << header << LOG_ENDL();
		//}

		it += 6; // skipping \r\n

		// Moving data
		std::copy(it, read_pos, read_buf);
		*(read_pos -= it - read_buf) = NULL;

		// Next header position
		it = strstr(read_buf, "HTTP/");
	}

	// Check if requested data is less then one packet	
	if (it) { assert((it - read_buf) >= size); }

	// Copy data
	nbytes = std::min(size, (int)((it ? it : read_pos) - read_buf));
	memcpy(data, read_buf, nbytes);
	
	// Moving data
	std::copy(read_buf + nbytes, read_pos, read_buf);
	*(read_pos -= nbytes) = NULL;

	return nbytes; 
}

