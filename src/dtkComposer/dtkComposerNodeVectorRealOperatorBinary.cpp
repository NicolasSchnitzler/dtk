/* dtkComposerNodeVectorRealOperatorBinary.cpp ---
 *
 * Author: sblekout
 * Copyright (C) 2011 - Babette Lekouta, Inria.
 * Created: Thu Apr 26 16:15:40 2012 (+0200)
 * Version: $Id$
 * Last-Updated:
 *           By:
 *     Update #:
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "dtkComposerNodeVectorRealOperatorBinary.h"
#include "dtkComposerTransmitterEmitter.h"
#include "dtkComposerTransmitterReceiver.h"
#include "dtkComposerTransmitterVariant.h"

#include <dtkMath>

#include <dtkLog/dtkLog>

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeVectorRealOperatorBinary
// /////////////////////////////////////////////////////////////////

class dtkComposerNodeVectorRealOperatorBinaryPrivate
{
public:
    dtkComposerTransmitterReceiver<dtkVectorReal> receiver_lhs;
    dtkComposerTransmitterReceiver<dtkVectorReal> receiver_rhs;

public:
    dtkComposerTransmitterEmitter<dtkVectorReal> emitter_vec;
};

dtkComposerNodeVectorRealOperatorBinary::dtkComposerNodeVectorRealOperatorBinary(void) : dtkComposerNodeLeaf(), d(new dtkComposerNodeVectorRealOperatorBinaryPrivate)
{
    this->appendReceiver(&d->receiver_lhs);
    this->appendReceiver(&d->receiver_rhs);
    this->appendEmitter(&d->emitter_vec);
}

dtkComposerNodeVectorRealOperatorBinary::~dtkComposerNodeVectorRealOperatorBinary(void)
{
    delete d;

    d = NULL;
}

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeVectorRealOperatorBinaryScalar
// /////////////////////////////////////////////////////////////////

class dtkComposerNodeVectorRealOperatorBinaryScalarPrivate
{
public:
    dtkComposerTransmitterReceiver<dtkVectorReal> receiver_lhs;
    dtkComposerTransmitterReceiver<dtkVectorReal> receiver_rhs;

public:
    dtkComposerTransmitterEmitter<qreal> emitter_val;
};

dtkComposerNodeVectorRealOperatorBinaryScalar::dtkComposerNodeVectorRealOperatorBinaryScalar(void) : dtkComposerNodeLeaf(), d(new dtkComposerNodeVectorRealOperatorBinaryScalarPrivate)
{
    this->appendReceiver(&d->receiver_lhs);
    this->appendReceiver(&d->receiver_rhs);
    this->appendEmitter(&d->emitter_val);
}

dtkComposerNodeVectorRealOperatorBinaryScalar::~dtkComposerNodeVectorRealOperatorBinaryScalar(void)
{
    delete d;

    d = NULL;
}

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeVectorRealOperatorHomothetic
// /////////////////////////////////////////////////////////////////

class dtkComposerNodeVectorRealOperatorHomotheticPrivate
{
public:
    dtkComposerTransmitterReceiver<dtkVectorReal> receiver_vec;
    dtkComposerTransmitterVariant                 receiver_val;

public:
    dtkComposerTransmitterEmitter<dtkVectorReal> emitter_vec;
};

dtkComposerNodeVectorRealOperatorHomothetic::dtkComposerNodeVectorRealOperatorHomothetic(void) : dtkComposerNodeLeaf(), d(new dtkComposerNodeVectorRealOperatorHomotheticPrivate)
{
    this->appendReceiver(&d->receiver_vec);
    this->appendReceiver(&d->receiver_val);
    this->appendEmitter(&d->emitter_vec);
}

dtkComposerNodeVectorRealOperatorHomothetic::~dtkComposerNodeVectorRealOperatorHomothetic(void)
{
    delete d;

    d = NULL;
}

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeVectorRealOperatorBinary - SUM
// /////////////////////////////////////////////////////////////////

void dtkComposerNodeVectorRealOperatorBinarySum::run(void)
{
    int size = qMin(d->receiver_lhs.data().getRows(), d->receiver_rhs.data().getRows());

    for(int i = 0 ; i < size; i++)
        d->emitter_vec.setData(d->receiver_lhs.data() + d->receiver_rhs.data());

}

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeVectorRealOperatorBinary - SUBSTRACT
// /////////////////////////////////////////////////////////////////

void dtkComposerNodeVectorRealOperatorBinarySubstract::run(void)
{
    int size = qMin(d->receiver_lhs.data().getRows(), d->receiver_rhs.data().getRows());

    for(int i = 0 ; i < size; i++)
        d->emitter_vec.setData(d->receiver_lhs.data() - d->receiver_rhs.data());

}

void dtkComposerNodeVectorRealOperatorBinaryScalarDotProd::run(void)
{
    int size = qMin(d->receiver_lhs.data().getRows(), d->receiver_rhs.data().getRows());

    for(int i = 0 ; i < size; i++)
        d->emitter_val.setData(d->receiver_lhs.data() * d->receiver_rhs.data());

}

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeVectorRealOperatorHomothetic - MULTIPLICATION
// /////////////////////////////////////////////////////////////////

void dtkComposerNodeVectorRealOperatorHomotheticMult::run(void)
{
    d->emitter_vec.setData(d->receiver_vec.data() * d->receiver_val.data().toReal());
}

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeVectorRealOperatorHomothetic - DIVISION
// /////////////////////////////////////////////////////////////////

void dtkComposerNodeVectorRealOperatorHomotheticDivision::run(void)
{
    d->emitter_vec.setData(d->receiver_vec.data() / d->receiver_val.data().toReal());
}
