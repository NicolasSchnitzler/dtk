/* dtkAbstractDataComposite.h --- 
 * 
 * Author: Thibaud Kloczko
 * Copyright (C) 2011 - Thibaud Kloczko, Inria.
 * Created: Wed Sep 21 13:46:31 2011 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Apr 16 10:41:51 2012 (+0200)
 *           By: Julien Wintz
 *     Update #: 265
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKABSTRACTDATACOMPOSITE_H
#define DTKABSTRACTDATACOMPOSITE_H

#include "dtkAbstractData.h"
#include "dtkCoreExport.h"
#include "dtkGlobal.h"

#include <dtkLog/dtkLog.h>

class dtkAbstractDataCompositePrivate;

class DTKCORE_EXPORT dtkAbstractDataComposite : public dtkAbstractData
{
    Q_OBJECT

public:
    enum Type {
               None = 0,
        ConstVector = 1,
             Vector = 2,
          ConstList = 3,
               List = 4
    };

public:
             dtkAbstractDataComposite(      QVector<dtkAbstractData *>& vector, dtkAbstractData *parent = 0);
             dtkAbstractDataComposite(const QVector<dtkAbstractData *>& vector, dtkAbstractData *parent = 0);
             dtkAbstractDataComposite(        QList<dtkAbstractData *>&   list, dtkAbstractData *parent = 0);
             dtkAbstractDataComposite(const   QList<dtkAbstractData *>&   list, dtkAbstractData *parent = 0);
    virtual ~dtkAbstractDataComposite(void);

public:     
    void append(dtkAbstractData *data);
    void remove(dtkAbstractData *data);
     
    bool has(dtkAbstractData *data) const;

    qlonglong count(void);
    
    qlonglong indexOf(dtkAbstractData *data, qlonglong from = 0);

    Type type(void);
     
    const dtkAbstractData *at(qlonglong index) const;
          dtkAbstractData *at(qlonglong index);
     
    const QVector<dtkAbstractData *>& vector(void) const;  
          QVector<dtkAbstractData *>& vector(void);

    const QList<dtkAbstractData *>& list(void) const;
          QList<dtkAbstractData *>& list(void);

private:
    dtkAbstractDataCompositePrivate *d;

private:
    dtkAbstractDataComposite(QVector<dtkAbstractData *> *vector);
    dtkAbstractDataComposite(  QList<dtkAbstractData *>   *list);

public:    
    static dtkAbstractDataComposite *createFromVectorCopy(const QVector<dtkAbstractData *>& vector);
    static dtkAbstractDataComposite *createFromVectorCopy(      QVector<dtkAbstractData *>& vector);

    static dtkAbstractDataComposite *createFromListCopy(const QList<dtkAbstractData *>& list);
    static dtkAbstractDataComposite *createFromListCopy(      QList<dtkAbstractData *>& list);
};

// /////////////////////////////////////////////////////////////////
// Related non-member templated functions
// /////////////////////////////////////////////////////////////////

template <typename T> const QVector<dtkAbstractData *>& dtkUpCastIntoDataVector(const QVector<T>& vector);
template <typename T>       QVector<dtkAbstractData *>& dtkUpCastIntoDataVector(      QVector<T>& vector);

template <typename T> const QList<dtkAbstractData *>& dtkUpCastIntoDataList(const QList<T>& list);
template <typename T>       QList<dtkAbstractData *>& dtkUpCastIntoDataList(      QList<T>& list);

template <typename T> const QVector<T>& dtkDownCastFromDataVector(const QVector<dtkAbstractData *>& vector);
template <typename T>       QVector<T>& dtkDownCastFromDataVector(      QVector<dtkAbstractData *>& vector);

template <typename T> const QList<T>& dtkDownCastFromDataList(const QList<dtkAbstractData *>& list);
template <typename T>       QList<T>& dtkDownCastFromDataList(      QList<dtkAbstractData *>& list);

template <typename T> dtkAbstractDataComposite *dtkAbstractDataCompositeFromVectorCopy(const QVector<T>& vector);
template <typename T> dtkAbstractDataComposite *dtkAbstractDataCompositeFromVectorCopy(      QVector<T>& vector);

template <typename T> dtkAbstractDataComposite *dtkAbstractDataCompositeFromListCopy(const QList<T>& list);
template <typename T> dtkAbstractDataComposite *dtkAbstractDataCompositeFromListCopy(      QList<T>& list);

// /////////////////////////////////////////////////////////////////

#include <dtkCore/dtkAbstractDataComposite.tpp>

#endif
