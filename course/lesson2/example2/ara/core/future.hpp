// =============================================
// MODULE: ara::core::Future
// ---------------------------------------------
// Mục đích: Mô phỏng Future/Promise pattern
// Giải thích:
// - Future đại diện cho kết quả bất đồng bộ
// - Promise được dùng để set kết quả
// - Then() cho phép chain callbacks
// =============================================

#pragma once
#include <future>
#include <functional>
#include <memory>
#include <thread>

namespace ara {
namespace core {

template<typename T>
class Future {
public:
    Future() = default;
    
    explicit Future(std::future<T>&& fut) 
        : future_(std::make_shared<std::future<T>>(std::move(fut))) {}
    
    // Blocking wait for result
    T get() {
        if (future_) {
            return future_->get();
        }
        throw std::runtime_error("Invalid future");
    }
    
    // Check if result is ready
    bool is_ready() const {
        if (future_) {
            return future_->wait_for(std::chrono::seconds(0)) == 
                   std::future_status::ready;
        }
        return false;
    }
    
    // Wait with timeout
    template<typename Rep, typename Period>
    bool wait_for(const std::chrono::duration<Rep, Period>& timeout) const {
        if (future_) {
            return future_->wait_for(timeout) == std::future_status::ready;
        }
        return false;
    }
    
    // Chain callback khi future complete
    template<typename F>
    auto Then(F&& func) -> Future<decltype(func(std::declval<T>()))> {
        using ReturnType = decltype(func(std::declval<T>()));
        
        auto shared_future = future_;
        auto promise = std::make_shared<std::promise<ReturnType>>();
        
        std::thread([shared_future, func = std::forward<F>(func), promise]() mutable {
            try {
                if (shared_future) {
                    T value = shared_future->get();
                    if constexpr (std::is_void_v<ReturnType>) {
                        func(value);
                        promise->set_value();
                    } else {
                        promise->set_value(func(value));
                    }
                }
            } catch (...) {
                promise->set_exception(std::current_exception());
            }
        }).detach();
        
        return Future<ReturnType>(promise->get_future());
    }
    
    // Check validity
    bool valid() const {
        return future_ != nullptr && future_->valid();
    }

private:
    std::shared_ptr<std::future<T>> future_;
};

template<typename T>
class Promise {
public:
    Promise() : promise_(std::make_shared<std::promise<T>>()) {}
    
    Future<T> get_future() {
        return Future<T>(promise_->get_future());
    }
    
    void set_value(T value) {
        promise_->set_value(std::move(value));
    }
    
    void set_exception(std::exception_ptr eptr) {
        promise_->set_exception(eptr);
    }

private:
    std::shared_ptr<std::promise<T>> promise_;
};

// Specialization for void
template<>
class Promise<void> {
public:
    Promise() : promise_(std::make_shared<std::promise<void>>()) {}
    
    Future<void> get_future() {
        return Future<void>(promise_->get_future());
    }
    
    void set_value() {
        promise_->set_value();
    }
    
    void set_exception(std::exception_ptr eptr) {
        promise_->set_exception(eptr);
    }

private:
    std::shared_ptr<std::promise<void>> promise_;
};

}  // namespace core
}  // namespace ara
