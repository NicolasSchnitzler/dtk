/* dtkDistributedCommunicator.h ---
 *
 * Author: Nicolas Niclausse
 * Copyright (C) 2013 - Nicolas Niclausse, Inria.
 * Created: 2013/02/07 11:38:59
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#pragma once

#include <QtCore>

// class dtkDistributedCommunicatorPrivate;

class dtkDistributedCommunicator : public QObject
{
    Q_OBJECT

public:
             dtkDistributedCommunicator(void);
    virtual ~dtkDistributedCommunicator(void);

public:
    static const qint32 ANY_TAG    = -1;
    static const qint32 ANY_SOURCE = -1;

public:
    dtkDistributedCommunicator(const dtkDistributedCommunicator& other);
    dtkDistributedCommunicator& operator = (const dtkDistributedCommunicator& other);

public:
    virtual void  initialize(void);
    virtual bool  initialized(void);
    virtual void uninitialize(void);

public:
    virtual qlonglong allocate(qlonglong count, qlonglong size, void *buffer);

public:
    virtual void       get(qint32 from, qlonglong position,    void *array, qlonglong buffer_id);
    virtual QByteArray get(qint32 from, qlonglong position, qlonglong size, qlonglong buffer_id);

public:
    virtual void put(qint32 dest, qlonglong position, qlonglong size, const QByteArray &data, qlonglong buffer_id);

// public:
//     template<class T> T * get(qint32 from, qlonglong position, qlonglong size, qlonglong buffer_id);
//     template<class T> void put(qint32 dest, qlonglong position, qlonglong size, const T &data, qlonglong buffer_id);

public:
    virtual qint32 rank(void);
    virtual qint32 size(void);

};