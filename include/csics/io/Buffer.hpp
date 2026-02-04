#pragma once

#include <cstddef>
#include <span>
namespace csics::io {
class BufferView {
   public:
    explicit BufferView(void* buf, std::size_t size)
        : buf_(reinterpret_cast<unsigned char*>(buf)), size_(size) {};
    BufferView()
        : buf_(nullptr), size_(0){};

    template <typename T>
    operator std::span<T>() const noexcept {
        return std::span<T>(reinterpret_cast<T*>(buf_), size_);
    };

    unsigned char* data() const noexcept { return buf_; }
    std::size_t size() const noexcept { return size_; }

    bool empty() const noexcept { return size_ == 0; }

    operator bool() const noexcept { return buf_ != nullptr && size_ > 0; }

    bool operator==(const BufferView& other) const noexcept {
        return buf_ == other.buf_ && size_ == other.size_;
    }

    bool operator!=(const BufferView& other) const noexcept {
        return !(*this == other);
    }

    BufferView operator+(std::size_t offset) const noexcept {
        if (offset > size_) {
            return BufferView(nullptr, 0);
        }
        return BufferView(buf_ + offset, size_ - offset);
    }

    BufferView& operator+=(std::size_t offset) noexcept {
        if (offset > size_) {
            buf_ = nullptr;
            size_ = 0;
        } else {
            buf_ += offset;
            size_ -= offset;
        }
        return *this;
    };

   private:
    unsigned char* buf_;
    std::size_t size_;
};
};  // namespace csics::io
