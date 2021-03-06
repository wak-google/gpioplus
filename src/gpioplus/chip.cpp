#include <cstring>
#include <fcntl.h>
#include <gpioplus/chip.hpp>
#include <linux/gpio.h>
#include <stdplus/fd/ops/create.hpp>
#include <stdplus/util/string.hpp>
#include <string>
#include <system_error>

namespace gpioplus
{

using stdplus::util::strCat;

LineFlags::LineFlags(uint32_t flags) :
    kernel(flags & GPIOLINE_FLAG_KERNEL), output(flags & GPIOLINE_FLAG_IS_OUT),
    active_low(flags & GPIOLINE_FLAG_ACTIVE_LOW),
    open_drain(flags & GPIOLINE_FLAG_OPEN_DRAIN),
    open_source(flags & GPIOLINE_FLAG_OPEN_SOURCE)
{
}

Chip::Chip(unsigned id, const internal::Sys* sys) :
    fd(stdplus::fd::ops::open(strCat("/dev/gpiochip", std::to_string(id)),
                              O_RDONLY, sys)),
    sys(sys)
{
}

ChipInfo Chip::getChipInfo() const
{
    struct gpiochip_info info;
    memset(&info, 0, sizeof(info));

    int r = sys->gpio_get_chipinfo(fd.getValue(), &info);
    if (r < 0)
    {
        throw std::system_error(-r, std::generic_category(),
                                "gpio_get_chipinfo");
    }

    return ChipInfo{info.name, info.label, info.lines};
}

LineInfo Chip::getLineInfo(uint32_t offset) const
{
    struct gpioline_info info;
    memset(&info, 0, sizeof(info));
    info.line_offset = offset;

    int r = sys->gpio_get_lineinfo(fd.getValue(), &info);
    if (r < 0)
    {
        throw std::system_error(-r, std::generic_category(),
                                "gpio_get_lineinfo");
    }

    return LineInfo{info.flags, info.name, info.consumer};
}

} // namespace gpioplus
