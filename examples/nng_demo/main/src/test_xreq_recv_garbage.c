#include <nng/nng.h>
#include <nng/protocol/reqrep0/req.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <testing/nuts.h>




int main(int argc, char *argv[])
{
	nng_socket rep;
	nng_socket req;
	nng_msg *  m;
	uint32_t   req_id;

	nng_rep0_open_raw(&rep);
	nng_req0_open_raw(&req);
	nng_socket_set_ms(req, NNG_OPT_RECVTIMEO, 100);
	nng_socket_set_ms(req, NNG_OPT_SENDTIMEO, 1000);
	nng_socket_set_ms(rep, NNG_OPT_SENDTIMEO, 1000);
	nng_socket_set_ms(rep, NNG_OPT_SENDTIMEO, 1000);


    nuts_marry(req, rep);

	nng_msg_alloc(&m, 0);
	nng_msg_append_u32(m, 0x80000000);
	nng_sendmsg(req, m, 0);


    printf("nng_msg_header_len:%ld\n", nng_msg_header_len(m));

	nng_recvmsg(rep, &m, 0);

	// // The message will have a header that contains the 32-bit pipe ID,
	// // followed by the 32-bit request ID.  We will discard the request
	// // ID before sending it out.

    printf("nng_msg_header_len:%ld\n", nng_msg_header_len(m));



	nng_msg_header_chop_u32(m, &req_id);


	nng_sendmsg(rep, m, 0);
	nng_recvmsg(req, &m, 0), NNG_ETIMEDOUT;

    nng_close(req);
    nng_close(rep);




    return 0;
}