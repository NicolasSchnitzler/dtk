/* dtkComposerTransmitterVariant.tpp --- 
 * 
 * Author: tkloczko
 * Copyright (C) 2011 - Thibaud Kloczko, Inria.
 * Created: Fri Aug  3 14:31:25 2012 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Sep 25 09:14:47 2012 (+0200)
 *           By: tkloczko
 *     Update #: 90
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKCOMPOSERTRANSMITTERVARIANT_TPP
#define DTKCOMPOSERTRANSMITTERVARIANT_TPP

#include "dtkComposerTransmitter.h"
#include "dtkComposerTransmitter_p.h"
#include "dtkComposerTransmitterVariant_p.h"

#include <dtkContainer/dtkContainerVectorWrapper.h>

// /////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////


template <typename T> inline void dtkComposerTransmitterVariant::setData(T *data)
{
    d->data_type = qMetaTypeId<T>(reinterpret_cast<T*>(0));

    d->variant.setValue(data);
    if (d->container) {
        delete d->container;
        d->container = NULL;
    }

    if (dtkTypeInfo<T*>::dtkAbstractObjectPointer)
        d->object = reinterpret_cast<dtkAbstractObject*>(data);
    else
        d->object = NULL;
}

template <typename T> inline void dtkComposerTransmitterVariant::setData(dtkContainerVector<T> *data)
{
    d->data_type = qMetaTypeId<dtkAbstractContainerWrapper>(reinterpret_cast<dtkAbstractContainerWrapper*>(0));

    if (!d->container)
        d->container = new dtkContainerVectorWrapper<T>(data);
    else
        d->container->setVector(data);

    d->variant.setValue(d->container);
    
    d->object = d->container;
}

template <typename T> T *dtkComposerTransmitterVariant::data(void)
{
    T *data = this->variant().value<T*>();

    if (e->twinned)
        return data;

    if (!data)
        return NULL;

    T *m_data;
    if (!e->m_variant.isValid())
        m_data = NULL;
    else
        m_data = e->m_variant.value<T*>();

    switch(this->dataTransmission()) {
    case dtkComposerTransmitter::CopyOnWrite:
        if (this->enableCopy()) {
            if (!m_data) {
                if (dtkTypeInfo<T*>::dtkAbstractObjectPointer)
                    m_data = reinterpret_cast<T*>(reinterpret_cast<dtkAbstractObject*>(data)->clone());
                else
                    m_data = new T(*data);
                e->m_variant.setValue(m_data);
            } else {
                *m_data = *data;
            }
            return m_data;
        } else {
            return data;
        }
        break;
    case dtkComposerTransmitter::Copy:
        if (!m_data) {
            if (dtkTypeInfo<T*>::dtkAbstractObjectPointer)
                m_data = reinterpret_cast<T*>(reinterpret_cast<dtkAbstractObject*>(data)->clone());
            else
                m_data = new T(*data);
            e->m_variant.setValue(m_data);
        } else {
            *m_data = *data;
        }
        return m_data;
        break;
    case dtkComposerTransmitter::Reference:
        return data;
        break;
    default:
        break;
    };

    return NULL;
}

#endif
