#pragma once

#include <atomic>
#include <cstddef>

namespace csics::queue {

#ifdef CACHE_LINE_SIZE
constexpr size_t kCacheLineSize = CACHE_LINE_SIZE;
#else
constexpr size_t kCacheLineSize = 64;
#endif

struct ReadSlot {
    const void* data;
    size_t size;

    template <typename T>
    const T& as() {
        return *static_cast<const T*>(data);
    }
};

struct WriteSlot {
    void* data;
    size_t size;

    template <typename T>
    T& as() {
        return *static_cast<T*>(data);
    }
};

class SPSCQueue {
   public:
    SPSCQueue(size_t capacity);
    ~SPSCQueue();

    // Acquire a read slot.
    // ReadSlot will be populated with the data pointer and size.
    bool acquire_read(ReadSlot& slot);

    // Release a previously acquired read slot.
    bool commit_read(const ReadSlot& slot);

    // Acquire a write slot of the given size. Returns true if successful, false
    // otherwise. WriteSlot will be populated with the data pointer and size.
    bool acquire_write(WriteSlot& slot, std::size_t size);

    // Release a previously acquired write slot.
    bool commit_write(const WriteSlot& slot);

    std::size_t capacity() const { return capacity_; }

   private:
    std::size_t capacity_;
    std::byte* buffer_;

    struct QueueSlotHeader {  // extendable header, realistically only a size.
        bool padded : 1;
        size_t size : 63;
    };

    alignas(kCacheLineSize) std::atomic<size_t> read_index_;
    alignas(kCacheLineSize) std::atomic<size_t> write_index_;
};
};  // namespace csics::queue
