#pragma once

enum class USRPDevice { DEFAULT, N210, N310, B205, B210, X310, X410 };

struct USRPConfiguration {
    struct {
        double min;
        double max;
    } frequency_range;
    const double* master_clocks;
    double max_gain;
};

constexpr double n210_master_clocks[] = {100e6};

constexpr USRPConfiguration n210_config = {
    .frequency_range = {70e6, 6e9},
    .master_clocks = n210_master_clocks,
    .max_gain = 76.0,
};

constexpr double n310_master_clocks[] = {122.88e6, 125e6, 153.6e6};
constexpr USRPConfiguration n310_config = {
    .frequency_range = {10e6, 6e9},
    .master_clocks = n310_master_clocks,
    .max_gain = 76.0,
};

constexpr const char* preferred_otw = "sc16";

constexpr const char* supported_otw_n210[] = {
    "sc16",  // for sample rates > 25 MHz
    "sc8"    // for sample rates <= 25 MHz
};

constexpr const char* n210_preferred_otw(double sample_rate) {
    if (sample_rate <= 25e6) {
        return supported_otw_n210[1];  // sc8
    } else {
        return supported_otw_n210[0];  // sc16
    }
}
