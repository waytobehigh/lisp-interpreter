#pragma once

#include <iostream>
#include <typeinfo>

struct PlaceHolder {
    virtual const std::type_info& TypeInfo() const = 0;
    virtual std::unique_ptr<PlaceHolder> Clone() const = 0;
    virtual ~PlaceHolder() {};
};

template <class T>
struct Holder: public PlaceHolder {
    Holder(const T& value): held(value) {}

    std::unique_ptr<PlaceHolder> Clone() const override {
        return std::make_unique<Holder<T>>(held);
    }

    const std::type_info& TypeInfo() const override {
        return typeid(held);
    }

    T held;
};

class Any {
public:
    Any() {}

    template<class T>
    Any(const T& value): content_{std::make_unique<Holder<T>>(value)} {}

    template<class T>
    Any& operator=(const T& value) {
        content_ = std::make_unique<Holder<T>>(value);
        return *this;
    }

    Any(const Any& rhs):
            content_{!rhs.Empty() ?
                     rhs.content_->Clone() :
                     std::unique_ptr<PlaceHolder>(nullptr)}
    {}

    Any& operator=(Any rhs) {
        Swap(rhs);
        return *this;
    }

    ~Any() {
        content_.reset();
    }

    void Swap(Any& rhs) {
        content_.swap(rhs.content_);
    }
    
    template<class T>
    T& TakeValue() const {
        if (typeid(T) != content_->TypeInfo()) {
            std::cout << "Wrong any cast!\n";
        }

        auto item = static_cast<Holder<T>*>(content_.get());
        return item->held;
    }

    bool Empty() const {
        return !content_;
    }

    void Clear() {
        Any().Swap(*this);
    }

private:
    std::unique_ptr<PlaceHolder> content_;
};
