#pragma once

#include <csics/Buffer.hpp>
#include <csics/io/net/NetTypes.hpp>

namespace csics::io::net {

class MQTTMessage {
   public:
    MQTTMessage();
    ~MQTTMessage();
    MQTTMessage(const MQTTMessage&) = delete;
    MQTTMessage& operator=(const MQTTMessage&) = delete;
    MQTTMessage(MQTTMessage&& other) noexcept;
    MQTTMessage& operator=(MQTTMessage&& other) noexcept;

   private:
    BufferView payload_;
    StringView topic_;
};

class MQTTEndpoint {
   public:
    using ConnectionParams = URI;

    MQTTEndpoint(BufferView client_id);
    ~MQTTEndpoint();
    MQTTEndpoint(const MQTTEndpoint&) = delete;
    MQTTEndpoint& operator=(const MQTTEndpoint&) = delete;
    MQTTEndpoint(MQTTEndpoint&& other) noexcept;
    MQTTEndpoint& operator=(MQTTEndpoint&& other) noexcept;

    NetStatus connect(const URI& broker_uri);
    NetStatus publish(const MQTTMessage& message);

    NetStatus subscribe(const StringView topic);

    NetStatus recv(MQTTMessage& message);

    PollStatus poll(int timeoutMs);

   private:
    struct Internal;
    Internal* internal_;
};
};  // namespace csics::io::net
