#pragma once

#include <cstdint>
#include <memory>
#include <iterator>
#include <exception>
#include <vector>
#include <algorithm>
#include <cassert>
#include <type_traits>

#define STD_INJECT

namespace stl {

template <typename T>
class RandomAccessIter
{
#ifndef STD_INJECT 
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = T;
    using difference_type   = ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;
#endif /* !STD_INJECT */

public:
    explicit RandomAccessIter( T* element)
        : element_{ element}
    {}

    /* LegacyInputRandomAccessIter part */
    T& operator*() { return (*element_); }

    bool operator==( const RandomAccessIter& rhs) const { return !!(element_ == rhs.element_); }
    bool operator!=( const RandomAccessIter& rhs) const { return !(*this == rhs); }

    /* LegacyBidirectionalRandomAccessIter part */
    RandomAccessIter& operator++() { return (*this += 1); }
    RandomAccessIter operator++(int) { return (*this += 1); }
    RandomAccessIter& operator--() { return (*this -= 1); }
    RandomAccessIter operator--(int) { return (*this -= 1); }

    /* LegacyRandomAccessRandomAccessIter part */
    RandomAccessIter& operator+=( ptrdiff_t n)
    {
        element_ += n;
        return *this;
    }

    RandomAccessIter& operator-=( ptrdiff_t n)
    {
        return (*this) += (-n);
    }

    ptrdiff_t operator-( const RandomAccessIter& rhs) const
    {
        return (this->element_ - rhs.element_);
    }

    /*
    RandomAccessIter operator+( ptrdiff_t n)
    {
        return (*this += n);
    }

    RandomAccessIter operator-( ptrdiff_t n)
    {
        return (*this -= n);
    }
    */

    T& operator[]( ptrdiff_t n)
    {
        return element_[n];
    }

    bool operator<( const RandomAccessIter& rhs) const { return (*this - rhs) > 0; }
    bool operator>( const RandomAccessIter& rhs) const { return (rhs < *this); }
    bool operator>=( const RandomAccessIter& rhs) const { return !(*this < rhs); }
    bool operator<=( const RandomAccessIter& rhs) const { return !(*this > rhs); }

private:
    T* element_;
};

template <typename T>
RandomAccessIter<T>
operator+( RandomAccessIter<T> it,
           ptrdiff_t n)
{
    return it += n;
}

template <typename T>
RandomAccessIter<T>
operator+( ptrdiff_t n,
           RandomAccessIter<T> it)
{
    return it += n;
}

template <typename T>
RandomAccessIter<T>
operator-( RandomAccessIter<T> it,
           ptrdiff_t n)
{
    return it -= n;
}

template <typename T>
RandomAccessIter<T>
operator-( ptrdiff_t n,
           RandomAccessIter<T> it)
{
    return it -= n;
}

template <class T>
void destroy( T* object) noexcept
{
    (*object).~T();
}

template <class ForwardIt>
void destroy( ForwardIt start,
              ForwardIt end) noexcept
{
    while ( start++ != end )
    {
        destroy( start);
    }
}

template <typename T>
void construct( T* addr,
                T&& rhs)
{
    ::new( static_cast<void*>( addr)) T{ std::forward<T>( rhs)};
}

template <typename T>
void construct( T* addr)
{
    ::new( static_cast<void*>( addr)) T{};
}

template <typename T>
class Storage
{
public:
    Storage( const Storage& other) = delete;
    Storage& operator=( const Storage& other) = delete;

    Storage( Storage&& other) noexcept
        : data_{ other.data_}
        , capacity_{ other.capacity_}
        , size_{ other.size_}
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    Storage& operator=( Storage&& other) noexcept
    {
        std::swap( data_, other.data_);
        std::swap( size_, other.size_);
        std::swap( capacity_, other.capacity_);
        return *this;
    }

    Storage( size_t capacity)
        : data_ { (capacity == 0) ? nullptr
                                  : static_cast<T*>( ::operator new( sizeof( T) * capacity)) }
        , capacity_{ capacity}
        , size_{}
    {}

    ~Storage()
    {
        destroy( data_, data_ + size_);
        ::operator delete( data_);
    }

protected:
    T* data_;
    size_t capacity_;
    size_t size_;
};

template <typename T>
class Vector
    : private Storage<T>
{
private:
    using Storage<T>::data_;
    using Storage<T>::size_;
    using Storage<T>::capacity_;
    using Iterator = RandomAccessIter<T>;

public:

public:
    explicit Vector( size_t capacity = 0)
        : Storage<T>{ capacity}
    {}

    Vector( Vector&& other) = default;
    Vector& operator=( Vector&& other) = default;

    Vector( const Vector& other)
        : Storage<T>{ other.capacity_}
    {
        while ( size_ < other.size_ )
        {
            construct( data_ + size_, other.data_[ size_]);
            size_++;
        }
    }

    T& operator[]( size_t index)
    {
        if ( index >= capacity_ )
        {
            throw std::out_of_range{ "Vector::range_check"};
        }

        return data_[index];
    }

    const T& operator[]( size_t index) const
    {
        if ( index >= size_ )
        {
            throw std::out_of_range{ "Vector::range_check"};
        }

        return data_[index];
    }

    T& front()
    {
        return const_cast<T&>( const_cast<Vector*>( this)->front());
    }

    const T& front() const
    {
        return (*this)[0];
    }

    T& back()
    {
        return const_cast<T&>( const_cast<Vector*>( this)->back());
    }

    const T& back() const
    {
        return (*this)[size_ - 1];
    }

    Iterator begin()
    {
        return Iterator{ data_};
    }

    Iterator end()
    {
        return Iterator{ data_ + size_};
    }

    void push_back( T&& value)
    {
        static_assert( std::is_nothrow_move_constructible_v<T>, "T must be nothrow move constructible");
        if ( size_ == capacity_ )
        {
            Vector<T> tmp{ (1 + capacity_) * 2};
            while ( tmp.size_ != size_ )
            {
                tmp.push_back( std::move( data_[tmp.size_]));
            }

            tmp.push_back( std::forward<T>( value));
            std::swap( *this, tmp);
        }

        construct( data_ + size_, std::forward<T>( value));
        size_++;
    }

    bool empty()
    {
        return !!(size_ == 0);
    }

    void clear()
    {
        destroy( data_, data_ + size_);
        size_ = 0;
    }

    void reserve( size_t new_capacity)
    {
        if ( new_capacity <= capacity_ )
        {
            return;
        }

        Vector<T> tmp{ new_capacity};
        while ( tmp.size_ != size_ )
        {
            tmp.push_back( std::move( data_[tmp.size_]));
        }

        std::swap( *this, tmp);
    }

    ~Vector() = default;
};
}

#ifdef STD_INJECT 
namespace std {

template <typename T>
struct iterator_traits<stl::RandomAccessIter<T>>
{
    using iterator_category = random_access_iterator_tag;
    using value_type        = T;
    using difference_type   = ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;
};

}
#endif /* STD_INJECT */

