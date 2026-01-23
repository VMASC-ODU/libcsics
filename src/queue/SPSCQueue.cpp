#include <csics/queue/SPSCQueue.hpp>

#include <atomic>
#include <cstring>
#include <new>
#include <thread>
#include <iostream>

namespace csics::queue {
inline static constexpr std::size_t get_next_power_of_two(std::size_t v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    if constexpr (sizeof(std::size_t) == 8) v |= v >> 32;
    return ++v;
}



SPSCQueue::SPSCQueue(size_t capacity) noexcept
    : capacity_(std::max(kCacheLineSize, get_next_power_of_two(capacity))), // align to next power of 2
      buffer_(reinterpret_cast<std::byte*>(operator new(capacity_, std::align_val_t{kCacheLineSize}))
              ),
      read_index_(0),
      write_index_(0) {}

SPSCQueue::~SPSCQueue() noexcept {
    operator delete(buffer_, std::align_val_t{kCacheLineSize});
};

SPSCError SPSCQueue::acquire_write(WriteSlot& slot, std::size_t size) noexcept {
    if (size > capacity_) {
        return SPSCError::TOO_BIG;
    }

    const std::size_t read_index = read_index_.load(std::memory_order_acquire);
    const std::size_t write_index = write_index_.load(std::memory_order_relaxed);

    std::size_t mod_index = write_index & (capacity_ - 1);
    std::size_t pad_size = 0;
    std::size_t required_bytes = size + sizeof(QueueSlotHeader);
    QueueSlotHeader hdr{};

    if (mod_index + size > capacity_) {
        pad_size = capacity_ - mod_index - sizeof(QueueSlotHeader);
        required_bytes += pad_size + sizeof(QueueSlotHeader);
    }

    if (write_index - read_index + required_bytes >= capacity_) {
        return SPSCError::FULL;
    }
    
    if (pad_size > 0) {
        hdr.size = pad_size;
        hdr.padded = 1;
        std::memcpy(&buffer_[mod_index], &hdr, sizeof(QueueSlotHeader));
        mod_index = 0;
        write_index_.fetch_add(pad_size + sizeof(QueueSlotHeader), std::memory_order_release);
    }

    hdr.size = size;
    hdr.padded = 0;
    std::memcpy(&buffer_[mod_index], &hdr, sizeof(QueueSlotHeader));
    slot.data = &buffer_[mod_index] + sizeof(QueueSlotHeader);
    slot.size = size;

    return SPSCError::None;
};

SPSCError SPSCQueue::acquire_read(ReadSlot& slot) noexcept {
    
    const std::size_t read_index = read_index_.load(std::memory_order_relaxed);
    const std::size_t write_index =
        write_index_.load(std::memory_order_acquire);

    std::size_t mod_index = read_index & (capacity_ - 1);

    if (read_index == write_index) {
        return SPSCError::EMPTY;
    }

    QueueSlotHeader* hdr =
        reinterpret_cast<QueueSlotHeader*>(&buffer_[mod_index]);

    if (hdr->padded) {
        mod_index = 0;
        read_index_.fetch_add(hdr->size + sizeof(QueueSlotHeader),
                              std::memory_order_release);
        hdr = reinterpret_cast<QueueSlotHeader*>(&buffer_[mod_index]);
    }
    slot.size = hdr->size;
    slot.data = &buffer_[mod_index] + sizeof(QueueSlotHeader);
    return SPSCError::None;
}

SPSCError SPSCQueue::commit_write(const WriteSlot& slot) noexcept {
    auto new_index = write_index_.load(std::memory_order_acquire) + slot.size + sizeof(QueueSlotHeader);
    new_index = (new_index + kCacheLineSize - 1) & ~(kCacheLineSize - 1);

    write_index_.store(new_index, std::memory_order_release);

    return SPSCError::None;
}

SPSCError SPSCQueue::commit_read(const ReadSlot& slot) noexcept {
    auto new_index = read_index_.load(std::memory_order_acquire) + slot.size + sizeof(QueueSlotHeader);
    new_index = (new_index + kCacheLineSize - 1) & ~(kCacheLineSize - 1);
    read_index_.store(new_index, std::memory_order_release);

    return SPSCError::None;
}

SPSCQueueRange::SPSCQueueRange(SPSCQueue& queue) noexcept : queue_(&queue) {}

inline SPSCQueueRange::iterator SPSCQueueRange::begin() noexcept {
    return iterator(queue_);
};

inline SPSCQueueRange::sentinel SPSCQueueRange::end() const {
    return sentinel{};
};

SPSCQueueRange::iterator::iterator(SPSCQueue* q) noexcept
    : queue(q), end_reached(false), current_slot{} {
    if (this->queue) {
        SPSCError acquired = this->queue->acquire_read(current_slot);
        if (acquired != SPSCError::None) {
            end_reached = true;
            this->queue = nullptr;
        }
    } else {
        end_reached = true;
    }
}

SPSCQueueRange::iterator& SPSCQueueRange::iterator::operator++() noexcept {
    if (queue) {
        std::ignore = queue->commit_read(current_slot);
        end_reached = queue->acquire_read(current_slot) != SPSCError::None;
    }
    return *this;
}

void SPSCQueueRange::iterator::operator++(int) { ++(*this); }

SPSCQueue::ReadSlot SPSCQueueRange::iterator::operator*() const noexcept {
    return current_slot;
}

SPSCQueueRange::iterator::~iterator() noexcept {
    if (queue && !end_reached) {
        std::ignore = queue->commit_read(current_slot);
    }
}

};  // namespace csics::queue
