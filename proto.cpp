#include <replog/proto.hpp>

namespace replog {

msg_base_header* msg_base_header::decode_header(char* a_buf, size_t len)
{
    if (len < sizeof(msg_base_header))
        throw replog_error("Bad buffer size:", len);
    msg_base_header* p = reinterpret_cast<msg_base_header*>(a_buf);
    if (p->magic() != s_magic_header)
        throw replog_error("Wrong magic number:", p->magic());
    uint16_t n  = p->header_size();
    uint16_t min_sz;
    switch (p->cmd()) {
        case GET_SIZE:          min_sz = sizeof(msg_get_size);          break;
        case GET_SIZE_RESPONSE: min_sz = sizeof(msg_get_size_response); break;
        case MOVE_FILE:         min_sz = sizeof(msg_move_file);         break;
        case DELETE_FILE:       min_sz = sizeof(msg_delete_file);       break;
        case APPEND:            min_sz = sizeof(msg_append);            break;
        case ERROR_RESPONSE:    min_sz = sizeof(msg_error_response);    break;
        case RESEND_REQUEST:    min_sz = sizeof(msg_resend_request);    break;
        default:
            throw replog_error("Unknown command type:", p->cmd());
    }
    if (n < min_sz)
        throw replog_error("Bad header size (got=", n, ", expected=", min_sz, ")");
}

} // namespace replog
