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

#ifndef __ZMQ_HTTP_SOCKET_HPP_INCLUDED__
#define __ZMQ_HTTP_SOCKET_HPP_INCLUDED__

#include "tcp_socket.hpp"

namespace zmq
{

    //  The class encapsulating simple HTTP read/write et.

    class http_socket_t : public tcp_socket_t
    {
    public:

        http_socket_t ();
        ~http_socket_t ();

        //  Writes data with HTTP headers to the socket. Returns the number of bytes actually
        //  written (even zero is to be considered to be a success). In case
        //  of error or orderly shutdown by the other peer -1 is returned.
        int write (const void *data, int size);

        //  Reads data from the socket and removes HTTP headers (up to 'size' bytes). Returns the number
        //  of bytes actually read (even zero is to be considered to be
        //  a success). In case of error or orderly shutdown by the other
        //  peer -1 is returned.
        int read (void *data, int size);

	private:
		char *write_buf;	// buffer contains header to be written
		char *write_pos;	// current write position
		char *write_end;	// end of output header 

		char *read_buf;		// input buffer header
		char *read_pos;		// input buffer read position
    };

}

#endif
