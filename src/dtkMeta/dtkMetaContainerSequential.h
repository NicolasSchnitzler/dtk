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

#include "dtkMeta.h"

namespace dtkMetaContainerSequentialPrivate
{
// /////////////////////////////////////////////////////////////////
// dtkMetaContainerSequentialPrivate::iterator definition
// /////////////////////////////////////////////////////////////////

template<typename I, typename V> struct iterator
{
    static       void  assign(void *& ptr, const I& it);
    static       void  assign(void *& ptr, void *const& src);
    static       void  advance(void *& it, int step);
    static const void *data(void *const& it);
    static const void *data(I& it);
    static       int   distance(I& from, void *const& to);
    static       void  destroy(void *& it);
    static       bool  equal(void *it, void *other);

    static       void  setData(void *& it, const void *value);
    static       void  setData(I& it, const void *value);

    static       void  add(void *& it, const void *value);
    static       void  sub(void *& it, const void *value);
    static       void  mul(void *& it, const void *value);
    static       void  div(void *& it, const void *value);
};

// /////////////////////////////////////////////////////////////////
// dtkMetaContainerSequentialPrivate::iterator specialization for pointers as iterators
// /////////////////////////////////////////////////////////////////

template<typename V> struct iterator<V *, V>
{
    static       void  assign(void *& ptr, V *const it);
    static       void  assign(void *& ptr, void *const& src);
    static       void  advance(void *& it, int step);
    static const void *data(void *const& it);
    static const void *data(V *it);
    static       void  destroy(void *& it);
    static       int   distance(V *from, void *const& to);
    static       bool  equal(void *it, void *other);

    static       void  setData(void *& it, const void *value);
    static       void  setData(V *it, const void *value);

    static       void  add(void *& it, const void *value);
    static       void  sub(void *& it, const void *value);
    static       void  mul(void *& it, const void *value);
    static       void  div(void *& it, const void *value);
};

// /////////////////////////////////////////////////////////////////
// dtkMetaContainerSequentialPrivate::iteratorCapabilities definition
// /////////////////////////////////////////////////////////////////

enum IteratorCapability
    {
        ForwardCapability       = 1,
        BiDirectionalCapability = 2,
        RandomAccessCapability  = 4
    };

template<typename T, typename Category = typename std::iterator_traits<typename T::const_iterator>::iterator_category> struct iteratorCapabilities;

template<typename T> struct iteratorCapabilities<T, std::forward_iterator_tag>
{ 
    enum { 
        IteratorCapabilities = ForwardCapability 
    }; 
};

template<typename T> struct iteratorCapabilities<T, std::bidirectional_iterator_tag>
{ 
    enum { 
        IteratorCapabilities = BiDirectionalCapability | ForwardCapability 
    }; 
};

template<typename T> struct iteratorCapabilities<T, std::random_access_iterator_tag>
{ 
    enum { 
        IteratorCapabilities = RandomAccessCapability | BiDirectionalCapability | ForwardCapability 
    }; 
};
}

// /////////////////////////////////////////////////////////////////
// dtkMetaContainerSequentialPrivate::containerAPI definition
// /////////////////////////////////////////////////////////////////

template<typename T> struct containerAPI : iteratorCapabilities<T>
{
    static bool empty(const T *c); 
    static int   size(const T *c);
    static void  insertData(T *c, int idx, const void *value);
};

// /////////////////////////////////////////////////////////////////
// dtkMetaContainerSequentialPrivate::handler definition
// /////////////////////////////////////////////////////////////////

typedef QtMetaTypePrivate::VariantData Data;

class handler
{

public:
    void *m_container;
    void *m_iterator;
    int   m_metaType_id;
    uint  m_metaType_flags;
    uint  m_iterator_capabilities;

public:
    typedef       bool (*emptyFunc)     (void *c);
    typedef       int  (*sizeFunc)      (void *c);
    typedef       void (*setAtFunc)     (void *c, int idx, const void *value);
    typedef       void (*insertFunc)    (void *c, int idx, const void *value);
    typedef       void (*advanceFunc)   (void *& it, int step);
    typedef       Data (*getDataFunc)   (void *const& it, int metaTypeId, uint flags);
    typedef const void *(*atFunc)       (const void *c, int);
    typedef       void (*moveItFunc)    (void *c, void *& it);
    typedef       void (*destroyItFunc) (void *& it);
    typedef       bool (*equalItFunc)   (void *it, void *o);
    typedef       void (*copyItFunc)    (void *& it, void *const& src);
    typedef const void *(*itValFunc)    (void *const& it);
    typedef       void (*opItFunc)      (void *& it, const void *value);
    typedef       int  (*indexOfItFunc) (void *c, void *const& it);

public:
    emptyFunc     m_empty;
    sizeFunc      m_size;
    setAtFunc     m_setAt;
    insertFunc    m_insert;
    atFunc        m_at;
    advanceFunc   m_advance;
    advanceFunc   m_advanceConst;
    getDataFunc   m_getData;
    getDataFunc   m_getConstData;
    moveItFunc    m_moveToBegin;
    moveItFunc    m_moveToCBegin;
    moveItFunc    m_moveToEnd;
    moveItFunc    m_moveToCEnd;
    destroyItFunc m_destroyIterator;
    destroyItFunc m_destroyConstIterator;
    equalItFunc   m_equalIterator;
    equalItFunc   m_equalConstIterator;
    copyItFunc    m_copyIterator;
    copyItFunc    m_copyConstIterator;
    itValFunc     m_iteratorValue;
    opItFunc      m_setValueToIterator;
    opItFunc      m_addValueTotIterator;
    opItFunc      m_subValueTotIterator;
    opItFunc      m_mulValueTotIterator;
    opItFunc      m_divValueTotIterator;
    indexOfItFunc m_indexOfIterator;
    indexOfItFunc m_indexOfConstIterator;

public:
    template<typename T> static       bool  emptyPrivate(void *c);
    template<typename T> static       int   sizePrivate(void *c);
    template<typename T> static       void  setAtPrivate(void *c, int idx, const void *val);
    template<typename T> static       void  insertPrivate(void *c, int idx, const void *val);
    template<typename T> static const void *atPrivate(const void *c, int idx);
    template<typename T> static       void  moveToBeginPrivate(void *c, void *& it);
    template<typename T> static       void  moveToCBeginPrivate(void *c, void *& it);
    template<typename T> static       void  moveToEndPrivate(void *c, void *& it);
    template<typename T> static       void  moveToCEndPrivate(void *c, void *& it);
    template<typename T> static       void  destroyIteratorPrivate(void *& it);
    template<typename T> static       void  destroyConstIteratorPrivate(void *& it);
    template<typename T> static       bool  equalIteratorPrivate(void *it, void *o);
    template<typename T> static       bool  equalConstIteratorPrivate(void *it, void *o);
    template<typename T> static       void  copyIteratorPrivate(void *& it, void *const& src);
    template<typename T> static       void  copyConstIteratorPrivate(void *& it, void *const& src);
    template<typename T> static       void  advancePrivate(void *& it, int step);
    template<typename T> static       void  advanceConstPrivate(void *& it, int step);
    template<typename T> static       Data  getDataPrivate(void *const& it, int metaTypeId, uint flags);
    template<typename T> static       Data  getConstDataPrivate(void *const& it, int metaTypeId, uint flags);
    template<typename T> static const void  *iteratorValuePrivate(void *const& it);
    template<typename T> static       void  setValueToIteratorPrivate(void *& it, const void *val);
    template<typename T> static       void  addValueToIteratorPrivate(void *& it, const void *val);
    template<typename T> static       void  subValueToIteratorPrivate(void *& it, const void *val);
    template<typename T> static       void  mulValueToIteratorPrivate(void *& it, const void *val);
    template<typename T> static       void  divValueToIteratorPrivate(void *& it, const void *val);
    template<typename T> static       int   indexOfIteratorPrivate(void *c, void *const& it);
    template<typename T> static       int   indexOfConstIteratorPrivate(void *c, void *const& it);

public:
    template<typename T> handler(T *c);
                         handler(void);

public:
    bool empty(void);
    int   size(void);

    void   setAt(int idx, const void *val);
    void  insert(int idx, const void *val);

    Data at(int idx) const;

    Data      currentData(void) const;
    Data currentConstData(void) const;

    handler&      advance(int step);
    handler& advanceConst(int step);

    void  moveToBegin(void);
    void moveToCBegin(void);
    void    moveToEnd(void);
    void   moveToCEnd(void);

    void      destroyIterator(void);
    void destroyConstIterator(void);

    bool      equal(const handler& o) const;
    bool equalConst(const handler& o) const;

    void  copy(const handler& o);
    void  copyConst(const handler& o);

public:
    void setValueToIterator(const void *value);
    void addValueToIterator(const void *value);
    void subValueToIterator(const void *value);
    void mulValueToIterator(const void *value);
    void divValueToIterator(const void *value);

    const void *iteratorValue(void) const;

    int      indexOfIterator(void) const;
    int indexOfConstIterator(void) const;
};

// /////////////////////////////////////////////////////////////////
// dtkMetaContainerSequentialPrivate::convertFunctor for QMetatype system
// /////////////////////////////////////////////////////////////////

template<typename From> struct convertFunctor
{
};

template<typename From> struct convertFunctor<From *>
{
    handler operator () (From *f) const;
};

// /////////////////////////////////////////////////////////////////

} // end of dtkMetaContainerSequentialPrivate namespace

Q_DECLARE_METATYPE(dtkMetaContainerSequentialPrivate::handler)

// /////////////////////////////////////////////////////////////////
// dtkMetaContainerSequential definition
// /////////////////////////////////////////////////////////////////

class dtkMetaContainerSequential
{
public:
    struct iterator;
    struct item
    {
    private:
        friend struct iterator;
        iterator *m_iterator;
        dtkMetaContainerSequentialPrivate::handler& m_h;
        QAtomicInt *m_ref;

    public:
        explicit item(iterator *iter);
                 item(const item& o) = delete;
                ~item(void);

    public:
        item& operator = (const item& o);

    public:
        const QVariant operator * (void) const;

    public:
        template<typename T> item& operator  = (const T& value);
        template<typename T> item& operator += (const T& value);
        template<typename T> item& operator -= (const T& value);
        template<typename T> item& operator *= (const T& value);
        template<typename T> item& operator /= (const T& value);
        
    };
    friend struct item;

public:
    struct iterator
    {
    private:
        friend struct const_iterator;
        friend class dtkMetaContainerSequential;
        dtkMetaContainerSequentialPrivate::handler m_h;
        friend struct item;
        item *m_item;
        QAtomicInt *m_ref;

    private:
        explicit iterator(const dtkMetaContainerSequential& container, QAtomicInt *ref);
        explicit iterator(const dtkMetaContainerSequentialPrivate::handler& h,  QAtomicInt *ref);

    public:
         iterator(const iterator& o);
        ~iterator(void);

    public:
        iterator& operator = (const iterator& o);

    public:
        item& operator *  (void)  const;
        item& operator [] (int j) const;

    public:
        bool operator == (const iterator& o) const;
        bool operator != (const iterator& o) const;

    public:
        iterator& operator ++ (void);
        iterator  operator ++ (int);
        iterator& operator -- (void);
        iterator  operator -- (int);
        iterator& operator += (int j);
        iterator& operator -= (int j);
        iterator  operator +  (int j) const;
        iterator  operator -  (int j) const;
    };
    friend struct iterator;

public:
    struct const_iterator
    {
    private:
        friend class dtkMetaContainerSequential;
        dtkMetaContainerSequentialPrivate::handler m_h;
        QAtomicInt *m_ref;

    private:
        explicit const_iterator(const dtkMetaContainerSequential& container, QAtomicInt *ref);
        explicit const_iterator(const dtkMetaContainerSequentialPrivate::handler& handler, QAtomicInt *ref);

    public:
                 const_iterator(const const_iterator& o);
        explicit const_iterator(const       iterator& o);
                ~const_iterator(void);

    public:
        const_iterator& operator = (const const_iterator& o);

    public:
        const QVariant operator *  (void)  const;
        const QVariant operator [] (int j) const;

    public:
        bool operator == (const const_iterator& o) const;
        bool operator != (const const_iterator& o) const;

    public:
        const_iterator& operator ++ (void);
        const_iterator  operator ++ (int);
        const_iterator& operator -- (void);
        const_iterator  operator -- (int);
        const_iterator& operator += (int j);
        const_iterator& operator -= (int j);
        const_iterator  operator +  (int j) const;
        const_iterator  operator -  (int j) const;
    };
    friend struct const_iterator;

public:
    explicit dtkMetaContainerSequential(dtkMetaContainerSequentialPrivate::handler h);

public:
          iterator  begin(void)      ;
    const_iterator  begin(void) const;
    const_iterator cbegin(void) const;
          iterator    end(void)      ;
    const_iterator    end(void) const;
    const_iterator   cend(void) const;

public:
    bool hasBiDirectionalIterator(void) const;
    bool  hasRandomAccessIterator(void) const;

public:
    bool empty(void) const;
    int   size(void) const;

public:
    template<typename T> void setAt(int idx, const        T& t);
                         void setAt(int idx, const QVariant& v);

    template<typename T> void append(const        T& t);
                         void append(const QVariant& v);

    template<typename T> void prepend(const        T& t);
                         void prepend(const QVariant& v);

    template<typename T> void insert(int idx, const        T& t);
                         void insert(int idx, const QVariant& v);

public:
    const QVariant at(int idx) const;

    const QVariant  first(void) const;
              item& first(void);

    const QVariant  last(void) const;
              item& last(void);

    const QVariant  operator [] (int idx) const;
              item& operator [] (int idx);

protected:
    dtkMetaContainerSequentialPrivate::handler m_handler;
};

// /////////////////////////////////////////////////////////////////

#include "dtkMetaContainerSequential.tpp"

//
// dtkMetaContainerSequential.h ends here

