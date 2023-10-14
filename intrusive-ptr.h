#include <atomic>


template <typename T>
struct intrusive_ptr {
    template <typename Y>
    friend struct intrusive_ptr;

    using element_type = T;

    intrusive_ptr() noexcept = default;

    intrusive_ptr(T* p, bool add_ref = true) : ptr(p) {
        if (ptr && add_ref) {
            intrusive_ptr_add_ref(p);
        }
    }

    intrusive_ptr(intrusive_ptr const& r) : ptr(r.get()) {
        if (ptr) {
            intrusive_ptr_add_ref(ptr);
        }
    }

    template <class Y>
    intrusive_ptr(intrusive_ptr<Y> const& r)
            : ptr(static_cast<element_type*>(r.get())) {
        if (ptr) {
            intrusive_ptr_add_ref(ptr);
        }
    }

    intrusive_ptr(intrusive_ptr&& r) : ptr(r.get()) {
        r.ptr = nullptr;
    }

    template <class Y>
    intrusive_ptr(intrusive_ptr<Y>&& r)
            : ptr(static_cast<element_type*>(r.get())) {
        r.ptr = nullptr;
    }

    ~intrusive_ptr() {
        if (ptr) {
            intrusive_ptr_release(ptr);
        }
    }

    intrusive_ptr& operator=(intrusive_ptr const& r) {
        intrusive_ptr(r).swap(*this);
        return *this;
    }

    template <class Y>
    intrusive_ptr& operator=(intrusive_ptr<Y> const& r) {
        intrusive_ptr(r).swap(*this);
        return *this;
    }

    intrusive_ptr& operator=(T* r) {
        intrusive_ptr(r).swap(*this);
        return *this;
    }

    intrusive_ptr& operator=(intrusive_ptr&& r) {
        intrusive_ptr(std::move(r)).swap(*this);
        return *this;
    }

    template <class Y>
    intrusive_ptr& operator=(intrusive_ptr<Y>&& r) {
        intrusive_ptr(std::move(r)).swap(*this);
        return *this;
    }

    void reset() {
        intrusive_ptr().swap(*this);
    }

    void reset(T* r) {
        intrusive_ptr(r).swap(*this);
    }

    void reset(T* r, bool add_ref) {
        intrusive_ptr(r, add_ref).swap(*this);
    }

    T& operator*() const noexcept {
        return *ptr;
    }

    T* operator->() const noexcept {
        return ptr;
    }

    T* get() const noexcept {
        return ptr;
    }

    T* detach() noexcept {
        auto pointer = ptr;
        ptr = nullptr;
        return pointer;
    }

    explicit operator bool() const noexcept {
        return get() != nullptr;
    }

    void swap(intrusive_ptr& b) noexcept {
        std::swap(ptr, b.ptr);
    }

private:
    element_type* ptr = nullptr;
};

template <class T, class U>
bool operator==(intrusive_ptr<T> const& a, intrusive_ptr<U> const& b) noexcept {
    return a.get() == b.get();
}

template <class T, class U>
bool operator!=(intrusive_ptr<T> const& a, intrusive_ptr<U> const& b) noexcept {
    return a.get() != b.get();
}

template <class T, class U>
bool operator==(intrusive_ptr<T> const& a, U* b) noexcept {
    return a.get() == b;
}

template <class T, class U>
bool operator!=(intrusive_ptr<T> const& a, U* b) noexcept {
    return a.get() != b;
}

template <class T, class U>
bool operator==(T* a, intrusive_ptr<U> const& b) noexcept {
    return a == b.get();
}

template <class T, class U>
bool operator!=(T* a, intrusive_ptr<U> const& b) noexcept {
    return a != b.get();
}

template <class T>
bool operator<(intrusive_ptr<T> const& a, intrusive_ptr<T> const& b) noexcept {
    return a.get() < b.get();
}

template <class T>
bool operator>(intrusive_ptr<T> const& a, intrusive_ptr<T> const& b) noexcept {
    return a.get() > b.get();
}

template <class T>
void swap(intrusive_ptr<T>& a, intrusive_ptr<T>& b) noexcept {
    a.swap(b);
}

template <typename T>
struct intrusive_ref_counter {
    intrusive_ref_counter() noexcept = default;
    intrusive_ref_counter(const intrusive_ref_counter& v) noexcept
    { }

    intrusive_ref_counter& operator=(const intrusive_ref_counter& v) noexcept {
        //    this->count_ref = v.count_ref;
    }

    unsigned int use_count() const noexcept {
        return count_ref;
    }

    template <class Derived>
    friend void intrusive_ptr_add_ref(const intrusive_ref_counter<Derived>* p) noexcept;

    template <class Derived>
    friend void intrusive_ptr_release(const intrusive_ref_counter<Derived>* p) noexcept;

protected:
    mutable std::atomic<unsigned int> count_ref = 0;
    ~intrusive_ref_counter() = default;
};

template <class Derived>
void intrusive_ptr_add_ref(const intrusive_ref_counter<Derived>* p) noexcept {
    p->count_ref.fetch_add(1, std::memory_order_relaxed);
}

template <class Derived>
void intrusive_ptr_release(const intrusive_ref_counter<Derived>* p) noexcept {
    if (p->count_ref.fetch_sub(1, std::memory_order_acq_rel) == 1) {
        delete static_cast<const Derived*>(p);
    }
}

