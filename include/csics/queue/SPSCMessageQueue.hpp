
#include <optional>

#include "csics/queue/SPSCQueue.hpp"
namespace csics::queue {
template <typename T>
class SPSCMessageQueue {
   public:
    SPSCMessageQueue(size_t capacity) : queue_(capacity) {}
    ~SPSCMessageQueue() = default;

    [[nodiscard]]
    std::optional<T> try_pop() {
        SPSCQueue::ReadSlot slot;
        auto ret = queue_.acquire_read(slot);
        if (ret != SPSCError::None) {
            return std::nullopt;
        }
        T* data = slot.as<T>();
        T value = std::move(*data);
        return value;
    }

    [[nodiscard]]
    SPSCError try_push(const T& value) {
        SPSCQueue::WriteSlot slot;
        auto ret = queue_.acquire_write(slot, sizeof(T));
        if (ret != SPSCError::None) {
            return ret;
        }
        new (slot.data) T(value);
        queue_.commit_write(std::move(slot));
        return SPSCError::None;
    }

    [[nodiscard]]
    SPSCError try_push(T&& value) {
        SPSCQueue::WriteSlot slot;
        auto ret = queue_.acquire_write(slot, sizeof(T));
        if (ret != SPSCError::None) {
            return ret;
        }
        new (slot.data) T(std::move(value));
        queue_.commit_write(std::move(slot));
        return SPSCError::None;
    }

       private:
        SPSCQueue queue_;
    };
};
