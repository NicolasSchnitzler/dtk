// Version: $Id$
//
//

// Commentary:
//
//

// Change Log:
//
//

// Code:

#pragma once

#include <cstring>

// ///////////////////////////////////////////////////////////////////
// dtkStaticArrayData interface
// ///////////////////////////////////////////////////////////////////

template < typename T, size_t Alignment, qlonglong Size, bool = std::is_pod<T>::value > struct dtkStaticArrayData
{
    T _data[Size];

    const T *data(void) const { return _data; }
          T *data(void)       { return _data; }
};

template < typename T, size_t Alignment > struct dtkStaticArrayData <T, Alignment, 0, false>
{
    const T *data(void) const { return NULL; }
          T *data(void)       { return NULL; }
};

template < typename T, size_t Alignment, qlonglong Size > struct dtkStaticArrayData<T, Alignment, Size, true>
{
#ifndef Q_CC_MSVC
    alignas(Alignment) T _data[Size];
#else
    Q_DECL_ALIGN(Alignment) T _data[Size];
#endif
    const T *data(void) const { return _data; }
          T *data(void)       { return _data; }
};

template < typename T, size_t Alignment > struct dtkStaticArrayData <T, Alignment, 0, true>
{
    const T *data(void) const { return NULL; }
          T *data(void)       { return NULL; }
};

// ///////////////////////////////////////////////////////////////////
// dtkStaticArray interface
// ///////////////////////////////////////////////////////////////////

template < typename T, qlonglong Size, size_t Alignment = std::alignment_of<T>::value > class dtkStaticArray
{
public:
    dtkStaticArrayData<T, Alignment, Size> d;

public:
    typedef T                 value_type;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;
    typedef value_type&       reference;
    typedef const value_type& const_reference;
    typedef qptrdiff          difference_type;
    typedef qlonglong         size_type;

public:
    typedef value_type*                                         iterator;
    typedef const value_type*                             const_iterator;
    typedef iterator                                            Iterator;
    typedef const_iterator                                 ConstIterator;
    typedef std::reverse_iterator<iterator>	        reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

public:
    void swap(dtkStaticArray& rhs) { std::swap_ranges(begin(), end(), rhs.begin()); }

public:
    bool       empty(void) const { return !Size; }
    qlonglong   size(void) const { return  Size; }
    qlonglong  count(void) const { return  Size; }

public:
    void fill(const T& value) { std::fill_n(begin(), Size, value); }

    void setAt(qlonglong index, const T& value) { data()[index] = value; }

public:
    const T&          at (qlonglong index) const { return *(data() + index); }
    const T& operator [] (qlonglong index) const { return *(data() + index); }
          T& operator [] (qlonglong index)       { return *(data() + index); }

          T& first(void)       { return *data(); }
    const T& first(void) const { return *data(); }
          T&  last(void)       { return *(data() + Size - 1); }
    const T&  last(void) const { return *(data() + Size - 1); }

public:
          reference front(void)       { return first(); }
    const_reference front(void) const { return first(); }
          reference  back(void)       { return  last(); }
    const_reference  back(void) const { return  last(); }

public:
          iterator  begin(void)       { return       iterator(data()); }
    const_iterator  begin(void) const { return const_iterator(data()); }
    const_iterator cbegin(void) const { return const_iterator(data()); }

          iterator  end(void)       { return       iterator(data() + Size); }
    const_iterator  end(void) const { return const_iterator(data() + Size); }
    const_iterator cend(void) const { return const_iterator(data() + Size); }

          reverse_iterator  rbegin(void)       { return       reverse_iterator(end()); }
    const_reverse_iterator  rbegin(void) const { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin(void) const { return const_reverse_iterator(end()); }

          reverse_iterator  rend(void)       { return       reverse_iterator(begin()); }
    const_reverse_iterator  rend(void) const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend(void) const { return const_reverse_iterator(begin()); }

public:
    const T *data(void) const { return d.data(); }
          T *data(void)       { return d.data(); }

    const T *constData(void) const { return d.data(); }
};

// ///////////////////////////////////////////////////////////////////
// Compararison operators
// ///////////////////////////////////////////////////////////////////

template < typename T, qlonglong Size> inline bool operator == (const dtkStaticArray<T, Size>& lhs, const dtkStaticArray<T, Size>& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template < typename T, qlonglong Size> inline bool operator != (const dtkStaticArray<T, Size>& lhs, const dtkStaticArray<T, Size>& rhs)
{
    return !(lhs == rhs);
}

template < typename T, qlonglong Size> inline bool operator < (const dtkStaticArray<T, Size>& lhs, const dtkStaticArray<T, Size>& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template < typename T, qlonglong Size> inline bool operator > (const dtkStaticArray<T, Size>& lhs, const dtkStaticArray<T, Size>& rhs)
{
    return rhs < lhs;
}

template < typename T, qlonglong Size> inline bool operator <= (const dtkStaticArray<T, Size>& lhs, const dtkStaticArray<T, Size>& rhs)
{
    return !(lhs > rhs);
}

template < typename T, qlonglong Size> inline bool operator >= (const dtkStaticArray<T, Size>& lhs, const dtkStaticArray<T, Size>& rhs)
{
    return !(lhs < rhs);
}


//
// dtkStaticArray.h ends here
