/* dtkComposerNodeInteger.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008-2011 - Julien Wintz, Inria.
 * Created: Mon Feb 27 12:38:46 2012 (+0100)
 * Version: $Id$
 * Last-Updated: Sat Aug  4 00:42:25 2012 (+0200)
 *           By: tkloczko
 *     Update #: 56
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "dtkComposerMetatype.h"

#include "dtkComposerNodeInteger.h"
#include "dtkComposerTransmitterEmitter.h"
#include "dtkComposerTransmitterReceiver.h"
#include "dtkComposerTransmitterUtils.h"

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeIntegerPrivate interface
// /////////////////////////////////////////////////////////////////

class dtkComposerNodeIntegerPrivate
{
public:
    dtkComposerTransmitterVariant receiver;

public:
    dtkComposerTransmitterEmitter<qlonglong> emitter;

public:
    qlonglong value;
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeInteger implementation
// /////////////////////////////////////////////////////////////////

dtkComposerNodeInteger::dtkComposerNodeInteger(void) : dtkComposerNodeLeaf(), d(new dtkComposerNodeIntegerPrivate)
{
    QList<QVariant::Type> variant_list;
    variant_list << QVariant::Int << QVariant::UInt << QVariant::LongLong << QVariant::ULongLong << QVariant::Double;

    d->receiver.setTypes(variant_list);
    this->appendReceiver(&(d->receiver));

    d->value = 0;
    d->emitter.setData(&d->value);
    this->appendEmitter(&(d->emitter));
}

dtkComposerNodeInteger::~dtkComposerNodeInteger(void)
{
    delete d;

    d = NULL;
}

void dtkComposerNodeInteger::run(void)
{
    if (!d->receiver.isEmpty())
        d->value = *dtkComposerTransmitterData<qlonglong>(d->receiver);
}

qlonglong dtkComposerNodeInteger::value(void)
{
    return d->value;
}

void dtkComposerNodeInteger::setValue(qlonglong value)
{
    d->value = value;
}
