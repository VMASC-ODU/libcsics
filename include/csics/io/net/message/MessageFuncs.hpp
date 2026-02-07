#pragma once
#include <csics/io/net/message/message.hpp>
namespace csics::io::net {

    template <typename Endpoint, typename MessageParameter>
    inline MessageResult send_message(Endpoint* endpoint, Payload message, MessageParameter* parameter) {
        return endpoint->send(message);
    };

};
