#include <csics/radio/usrp/USRPRadioRx.hpp>

#include "USRPConfigs.hpp"

namespace csics::radio {

const char* data_type_to_uhd(StreamDataType data_type) {
    switch (data_type) {
        case StreamDataType::SC8:
            return "sc8";
        case StreamDataType::SC16:
            return "sc16";
        case StreamDataType::FC32:
            return "fc32";
        default:
            return "sc16";  // default to sc16
    }
}

USRPRadioRx::~USRPRadioRx() {
    stop_stream();
    if (queue_ != nullptr) delete queue_;
};

USRPRadioRx::USRPRadioRx(const RadioDeviceArgs& device_args)
    : queue_(nullptr),
      usrp_(uhd::usrp::multi_usrp::make(
          std::get<UsrpArgs>(device_args.args).device_addr)) {}

USRPRadioRx::StartStatus USRPRadioRx::start_stream(
    const StreamConfiguration& stream_config) noexcept {
    if (is_streaming()) {
        stop_stream();
        delete queue_;
        queue_ = nullptr;
    }

    queue_ =
        new csics::queue::SPSCQueue(stream_config.sample_length.get_num_bytes(
            current_config_.sample_rate, stream_config.data_type));
    rx_streamer_ = usrp_->get_rx_stream(
        uhd::stream_args_t(data_type_to_uhd(stream_config.data_type), "sc16"));

    if (!rx_streamer_) {
        delete queue_;
        queue_ = nullptr;
        return {StartStatus::Code::HARDWARE_FAILURE, nullptr};
    }

    streaming_.store(true, std::memory_order_release);
    rx_thread_ = std::thread(&USRPRadioRx::rx_loop, this);
    return {StartStatus::Code::SUCCESS, RxQueue(*queue_)};
}

bool USRPRadioRx::is_streaming() const noexcept {
    return streaming_.load(std::memory_order_acquire);
}

double USRPRadioRx::get_sample_rate() const noexcept {
    return current_config_.sample_rate;
}

double USRPRadioRx::get_max_sample_rate() const noexcept {
    return 0;
}

double USRPRadioRx::get_center_frequency() const noexcept {
    return current_config_.center_frequency;
}

double USRPRadioRx::get_gain() const noexcept {
    return current_config_.gain;
}

// When setting certain parameters,
// we might need to stop and restart the stream.
// or maybe just dump a certain amount of samples.
// idk yet.

void USRPRadioRx::set_gain(double gain) noexcept {
    current_config_.gain = gain;
    usrp_->set_rx_gain(gain);
}

void USRPRadioRx::set_sample_rate(double rate) noexcept {
    current_config_.sample_rate = rate;
    usrp_->set_rx_rate(rate);
}

void USRPRadioRx::set_center_frequency(double freq) noexcept {
    current_config_.center_frequency = freq;
    usrp_->set_rx_freq(freq);
}

void USRPRadioRx::stop_stream() noexcept {
    if (is_streaming()) {
        stop_signal_.store(true, std::memory_order_release);
        if (rx_thread_.joinable()) {
            rx_thread_.join();
        }
        streaming_.store(false, std::memory_order_release);
        stop_signal_.store(false, std::memory_order_release);
    }
}

// may need to optimize later just in case we need to update multiple params
void USRPRadioRx::set_configuration(
    const RadioConfiguration& config) noexcept {
    if (config.sample_rate != current_config_.sample_rate)
        set_sample_rate(config.sample_rate);
    if (config.center_frequency != current_config_.center_frequency)
        set_center_frequency(config.center_frequency);
    if (config.gain != current_config_.gain)
        set_gain(config.gain);
    current_config_ = config;
}

void USRPRadioRx::rx_loop() noexcept {
    while (!stop_signal_.load(std::memory_order_acquire)) {
        
    }
}

};  // namespace csics::radio
