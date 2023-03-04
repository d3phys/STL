#include <cstdio>
#include <cstdint>
#include <memory>
#include <iterator>
#include <exception>

namespace stl {

using T_t = float;

class Vector
{
public:
    using size_type = std::size_t;
    using T = T_t;
    using value_type = T;

public:
    Vector()
        : data_{}
        , size_{}
        , capacity_{}
    {}

    /* Disable copy at all. Zero-cost! */
    Vector( const Vector& other) = delete;

    Vector( Vector&& other)
        : Vector{}
    {
        swap( other);
    }

    /**
     * Extended swap trick.
     * Copy and move operator= at the same time.
     *
     * When you pass rvalue the other will be created by move constructor,
     * while if argument is lvalue it will be copied into other.
     */
    Vector& operator=( Vector other)
    {
        swap( other);
        return *this;
    }

    explicit Vector( size_t count)
        : data_{}
        , size_{}
        , capacity_{}
    {
        reserve( count);
        uninitializedDefaultConstruct( data_, data_ + count);
        size_ = count;
    }

    void swap( Vector& other)
    {
        std::swap( data_, other.data_);
        std::swap( size_, other.size_);
        std::swap( capacity_, other.capacity_);
    }

    T& operator[]( size_t index)
    {
        /**
         * Reusing the constant version trick.
         * Note! We need to cast this to const to call const version of operator[].
         */
        return const_cast<T&>( (*const_cast<const Vector*>( this))[index]);
    }

    const T& operator[]( size_t index) const
    {
        if ( index >= size_ )
        {
            throw std::out_of_range{ "Vector::range_check"};
        }

        return data_[index];
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

        /**
         * Need to allocate raw memory to handle non default constructible objects.
         * Also it can prevent unnecessary resource acquisition (RAII types).
         */
        T* new_data = reinterpret_cast<T*>( ::new char[sizeof( T) * new_capacity]);

        /* If we have something to relocate */
        if ( size_ )
        {
            try
            {
                uninitializedCopy( data_, data_ + size_, new_data);
            } catch ( ... )
            {
                delete[] reinterpret_cast<char*>( new_data);
                throw;
            }
        }

        delete[] reinterpret_cast<char*>( data_);
        capacity_ = new_capacity;
        data_ = new_data;
    }

    void resize( size_t count)
    {
        if ( size_ >= count )
        {
            destroy( data_ + count, data_ + size_);
            size_ = count;
            return;
        }

        /**
         * Actually this will not guarantee strong exception safety.
         * In case of exception in default constructor, data will be placed
         * in the other location and capacity == count.
         */
        reserve( count);
        uninitializedDefaultConstruct( data_ + size_, data_ + count);
    }

    void resize( size_t count,
                 const T& value)
    {
        if ( size_ >= count )
        {
            destroy( data_ + count, data_ + size_);
            size_ = count;
            return;
        }

        /* No strong exception safety (see resize(count)). */
        reserve( count);
        for ( size_t i = size_; i != count; ++i )
        {
            ::new( static_cast<void*>( data_ + i)) T{ value};
        }
    }

    ~Vector()
    {
        delete[] reinterpret_cast<char*>( data_);
        data_ = nullptr;
        capacity_ = 0;
        size_ = 0;
    }

private:

    template<class ForwardIt>
    void destroy( ForwardIt start, ForwardIt end)
    {
        using T = typename std::iterator_traits<ForwardIt>::value_type;
        for ( ; start != end ; ++start )
        {
            (*start).~T();
        }
    }

    template<class InputIt, class ForwardIt>
    ForwardIt uninitializedCopy( InputIt src_start,
                                 InputIt src_end,
                                 ForwardIt dest_start)
    {
        using T = typename std::iterator_traits<ForwardIt>::value_type;

        ForwardIt dest_end = dest_start;

        try
        {
            for ( ; src_start != src_end ; ++src_start, ++dest_end )
            {
                /**
                 * We must use std::addressof because operator& can be overloaded.
                 * Also voidify type to perform original (not overloaded) placement new operator.
                 */
                ::new( static_cast<void*>( std::addressof( *dest_end))) T{ *src_start};
            }
        } catch ( ... )
        {
            /**
             * To provide string exception safety need to cleanup all constructed objects.
             * Also it bans perfect forwarding.
             */
            for ( ; dest_start != dest_end; ++dest_start )
            {
                (*dest_start).~T();
            }

            throw;
        }

        return dest_end;
    }

    template<class ForwardIt>
    ForwardIt uninitializedDefaultConstruct( ForwardIt start,
                                             ForwardIt end)
    {
        using T = typename std::iterator_traits<ForwardIt>::value_type;

        ForwardIt cur = start;

        try
        {
            for ( ; cur != end ; ++cur )
            {
                /* See uninitializedCopy member function. */
                ::new( static_cast<void*>( std::addressof( *cur))) T{};
            }
        } catch ( ... )
        {
            for ( ; start != cur; ++start )
            {
                (*start).~T();
            }

            throw;
        }

        return end;
    }

private:
    T* data_;
    size_t size_;
    size_t capacity_;
};

}

int
main( int argc,
      const char* argv[])
{
    stl::Vector arr {10U};
    arr[0] = 123;

    arr.clear();
    std::printf( "Hello world\n");
    std::printf("elem[0] = %f \n", arr[0]);
    return 0;
}