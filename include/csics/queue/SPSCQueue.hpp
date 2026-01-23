#pragma once

#include <atomic>
#include <cstddef>
#include <iterator>
#include <new>

namespace csics::queue {

#ifdef CACHE_LINE_SIZE
constexpr size_t kCacheLineSize = CACHE_LINE_SIZE;
#else
constexpr size_t kCacheLineSize = std::hardware_destructive_interference_size;
#endif
class SPSCQueue;

class SPSCQueueRange {
   public:
    explicit SPSCQueueRange(SPSCQueue& queue) noexcept;

    struct iterator;
    struct sentinel {};

    inline iterator begin() noexcept;
    inline sentinel end() const;

   private:
    SPSCQueue* queue_;
};

enum class SPSCError {
    None,
    FULL,
    EMPTY,
    TOO_BIG,
};

// Single Producer Single Consumer Queue
// Uses a circular buffer with atomic indices for read and write.
// API uses acquire/commit semantics for both read and write.
// Used for low-level communication between threads with minimal overhead.
// Supports blocking and non-blocking acquire methods.
// Supports stopping the queue to unblock waiting threads.
class SPSCQueue {
   public:
    struct ReadSlot;
    struct WriteSlot;
    SPSCQueue(const SPSCQueue&) = delete;
    SPSCQueue& operator=(const SPSCQueue&) = delete;
    explicit SPSCQueue(size_t capacity) noexcept;
    ~SPSCQueue() noexcept;

    // Acquire a read slot.
    // ReadSlot will be populated with the data pointer and size.
    // Returns true if successful, false otherwise.
    // Will return false if the queue is stopped and there is no data to read.
    [[nodiscard]]
    SPSCError acquire_read(ReadSlot& slot) noexcept;

    // Release a previously acquired read slot.
    [[nodiscard]]
    SPSCError commit_read(const ReadSlot& slot) noexcept;

    // Acquire a write slot.
    // WriteSlot will be populated with the data pointer and size.
    // Blocks until a slot is available or the queue is stopped.
    [[nodiscard]]
    SPSCError acquire_write(WriteSlot& slot, std::size_t size) noexcept;

    // Release a previously acquired write slot.
    [[nodiscard]]
    SPSCError commit_write(const WriteSlot& slot) noexcept;

    inline std::size_t capacity() const noexcept { return capacity_; }

    inline bool has_pending_data() const noexcept {
        return read_index_.load(std::memory_order_acquire) <
               write_index_.load(std::memory_order_acquire);
    }

    inline SPSCQueueRange read_range() noexcept {
        return SPSCQueueRange(*this);
    }

   private:
    std::size_t capacity_;
    std::byte* buffer_;

    struct QueueSlotHeader {  // extendable header, realistically only a size.
        uint64_t padded : 1;
        uint64_t size : 63;
    };

#ifdef _MSC_VER
#pragma warning(disable : 4324)  // disable MSVC warning 4324. We don't care
                                 // about the padding here
#endif
    alignas(kCacheLineSize) std::atomic<size_t> read_index_;
    alignas(kCacheLineSize) std::atomic<size_t> write_index_;

    inline bool is_full();

   public:
    struct ReadSlot {
        std::byte* data;
        size_t size;

        template <typename T>
        const T* as() const noexcept {
            return *reinterpret_cast<T*>(data);
        }

        template<typename Header, typename Data>
        void as_block(Header*& header, Data*& data) const noexcept {
            header = reinterpret_cast<Header*>(this->data);
            data = reinterpret_cast<Data*>(this->data + sizeof(Header));
        }
    };

    struct WriteSlot {
        std::byte* data;
        size_t size;

        template <typename T>
        T* as() {
            return *static_cast<T*>(data);
        }

        template <typename Header, typename Data>
        void as_block(Header*& header, Data*& data) const noexcept {
            header = reinterpret_cast<Header*>(this->data);
            data = reinterpret_cast<Data*>(this->data + sizeof(Header));
        }
    };
};

// Range-based iterator for reading from SPSCQueue
// Allows for easy iteration over available read slots in the queue.
// Exits when the queue is stopped, not when empty. The producer must stop the
// queue. Typically used in a consumer thread to process incoming data. Will
// manage committing read slots automatically.

struct SPSCQueueRange::iterator {
    using iterator_category = std::input_iterator_tag;
    using value_type = SPSCQueue::ReadSlot;
    using difference_type = std::ptrdiff_t;

    SPSCQueue* queue = nullptr;
    SPSCQueue::ReadSlot current_slot;
    bool end_reached = false;

    iterator() noexcept = default;
    explicit iterator(SPSCQueue* q) noexcept;

    iterator& operator++() noexcept;

    void operator++(int);

    friend bool operator==(const iterator& it, const sentinel&) noexcept {
        return it.queue == nullptr || it.end_reached;
    }

    friend bool operator!=(const iterator& it, const sentinel&) noexcept {
        return !(it == sentinel{});
    }

    SPSCQueue::ReadSlot operator*() const noexcept;

    ~iterator() noexcept;
};

};  // namespace csics::queue