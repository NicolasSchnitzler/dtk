/* dtkComposerTransmitterReceiver.h --- 
 * 
 * Author: tkloczko
 * Copyright (C) 2011 - Thibaud Kloczko, Inria.
 * Created: Tue Feb 14 11:39:15 2012 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Sep 26 16:38:08 2012 (+0200)
 *           By: Julien Wintz
 *     Update #: 114
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKCOMPOSERTRANSMITTERRECEIVER_H
#define DTKCOMPOSERTRANSMITTERRECEIVER_H

#include "dtkComposerExport.h"
#include "dtkComposerTransmitter.h"

#include <dtkCore/dtkGlobal>

#include <dtkContainer/dtkContainerVector.h>

#include <QtCore>

template <typename T> class dtkComposerTransmitterEmitter;

class dtkComposerTransmitterVariant;

// /////////////////////////////////////////////////////////////////
// dtkComposerTransmitterReceiver interface
// /////////////////////////////////////////////////////////////////

template <typename T> class dtkComposerTransmitterReceiver : public dtkComposerTransmitter
{
public:
     dtkComposerTransmitterReceiver(dtkComposerNode *parent = 0);
    ~dtkComposerTransmitterReceiver(void);

public:
          T& data(void);
    const T& data(void) const;

    QVector<T> allData(void);

public:
    bool isEmpty(void) const;

public:
    Kind kind(void) const;

    QString kindName(void) const;

public:
    bool    connect(dtkComposerTransmitter *transmitter);
    bool disconnect(dtkComposerTransmitter *transmitter);

public:
    void setActiveEmitter(dtkComposerTransmitter *emitter);

public: 
    LinkMap rightLinks(dtkComposerTransmitter *transmitter, dtkComposerTransmitterLinkList list);

private:
    QList<dtkComposerTransmitterEmitter<T> *> emitters;
    QList<dtkComposerTransmitterVariant *>    variants;

private:
    dtkComposerTransmitterEmitter<T> *active_emitter;
    dtkComposerTransmitterVariant    *active_variant;

private:
    T m_data;
};

// /////////////////////////////////////////////////////////////////
// dtkComposerTransmitterReceiverVector interface
// /////////////////////////////////////////////////////////////////

template <typename T> class dtkComposerTransmitterEmitterVector;

class dtkComposerTransmitterVariantContainer;

template <typename T> class dtkComposerTransmitterReceiverVector : public dtkComposerTransmitterReceiver<T>
{
public:
     dtkComposerTransmitterReceiverVector(dtkComposerNode *parent = 0);
    ~dtkComposerTransmitterReceiverVector(void);

public:
          dtkAbstractContainerWrapper& container(void);
    const dtkAbstractContainerWrapper& container(void) const;

          dtkContainerVector<T>& data(void);
    const dtkContainerVector<T>& data(void) const;

public:
    bool    connect(dtkComposerTransmitter *transmitter);
    bool disconnect(dtkComposerTransmitter *transmitter);

public:
    bool isEmpty(void) const;

public:
    void setActiveEmitter(dtkComposerTransmitter *emitter);

private:
    QList<dtkComposerTransmitterEmitterVector<T> *> emitters;
    QList<dtkComposerTransmitterVariant *>          variants;

private:
    dtkComposerTransmitterEmitterVector<T> *active_emitter;
    dtkComposerTransmitterVariant          *active_variant;

private:
    dtkContainerVector<T> m_vector;

    using dtkComposerTransmitterReceiver<T>::d;
};

// /////////////////////////////////////////////////////////////////
// dtkComposerTransmitterReceiver implementation
// /////////////////////////////////////////////////////////////////

#include "dtkComposerTransmitterReceiver.tpp"

#endif
