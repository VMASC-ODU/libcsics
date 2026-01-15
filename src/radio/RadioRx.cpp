#include <csics/radio/RadioRx.hpp>

namespace csics::radio {

RadioDeviceArgs::RadioDeviceArgs(): device_type(DeviceType::DEFAULT) {
}

#ifdef CSICS_USE_UHD
    UsrpArgs::operator RadioDeviceArgs() const {
        RadioDeviceArgs args;
        args.device_type = DeviceType::USRP;
        args.args = *this;
        return args;
    }

#endif

};
