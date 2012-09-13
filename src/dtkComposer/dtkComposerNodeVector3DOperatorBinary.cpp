/* dtkComposerNodeVector3DOperatorBinary.cpp ---
 *
 * Author: tkloczko
 * Copyright (C) 2011 - Thibaud Kloczko, Inria.
 * Created: Thu Apr 26 16:15:40 2012 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Sep 13 15:54:09 2012 (+0200)
 *           By: tkloczko
 *     Update #: 67
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "dtkComposerNodeVector3D.h"
#include "dtkComposerNodeVector3DOperatorBinary.h"
#include "dtkComposerTransmitterEmitter.h"
#include "dtkComposerTransmitterReceiver.h"
#include "dtkComposerTransmitterVariant.h"

#include <dtkMath>

#include <dtkLog/dtkLog>

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeVector3DOperatorBinary
// /////////////////////////////////////////////////////////////////

class dtkComposerNodeVector3DOperatorBinaryPrivate
{
public:
    dtkComposerTransmitterReceiver<dtkVector3DReal> receiver_lhs;
    dtkComposerTransmitterReceiver<dtkVector3DReal> receiver_rhs;

public:
    dtkComposerTransmitterEmitter<dtkVector3DReal> emitter_vec;

public:
    dtkVector3DReal *vector;
};

dtkComposerNodeVector3DOperatorBinary::dtkComposerNodeVector3DOperatorBinary(void) : dtkComposerNodeLeaf(), d(new dtkComposerNodeVector3DOperatorBinaryPrivate)
{
    d->vector = NULL;

    this->appendReceiver(&d->receiver_lhs);
    this->appendReceiver(&d->receiver_rhs);
    this->appendEmitter(&d->emitter_vec);
}

dtkComposerNodeVector3DOperatorBinary::~dtkComposerNodeVector3DOperatorBinary(void)
{
    if (d->vector)
        delete d->vector;

    d->vector = NULL;

    delete d;

    d = NULL;
}

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeVector3DOperatorBinaryScalar
// /////////////////////////////////////////////////////////////////

class dtkComposerNodeVector3DOperatorBinaryScalarPrivate
{
public:
    dtkComposerTransmitterReceiver<dtkVector3DReal> receiver_lhs;
    dtkComposerTransmitterReceiver<dtkVector3DReal> receiver_rhs;

public:
    dtkComposerTransmitterEmitter<qreal> emitter_val;

public:
    qreal value;
};

dtkComposerNodeVector3DOperatorBinaryScalar::dtkComposerNodeVector3DOperatorBinaryScalar(void) : dtkComposerNodeLeaf(), d(new dtkComposerNodeVector3DOperatorBinaryScalarPrivate)
{
    this->appendReceiver(&d->receiver_lhs);
    this->appendReceiver(&d->receiver_rhs);

    d->value = 0.;
    d->emitter_val.setData(&d->value);
    this->appendEmitter(&d->emitter_val);
}

dtkComposerNodeVector3DOperatorBinaryScalar::~dtkComposerNodeVector3DOperatorBinaryScalar(void)
{
    delete d;

    d = NULL;
}

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeVector3DOperatorHomothetic
// /////////////////////////////////////////////////////////////////

class dtkComposerNodeVector3DOperatorHomotheticPrivate
{
public:
    dtkComposerTransmitterReceiver<dtkVector3DReal> receiver_vec;
    dtkComposerTransmitterReceiver<qreal>           receiver_val;

public:
    dtkComposerTransmitterEmitter<dtkVector3DReal>  emitter_vec;
};

dtkComposerNodeVector3DOperatorHomothetic::dtkComposerNodeVector3DOperatorHomothetic(void) : dtkComposerNodeLeaf(), d(new dtkComposerNodeVector3DOperatorHomotheticPrivate)
{
    this->appendReceiver(&d->receiver_vec);
    this->appendReceiver(&d->receiver_val);
    this->appendEmitter(&d->emitter_vec);
}

dtkComposerNodeVector3DOperatorHomothetic::~dtkComposerNodeVector3DOperatorHomothetic(void)
{
    delete d;

    d = NULL;
}

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeVector3DOperatorBinary - SUM
// /////////////////////////////////////////////////////////////////

void dtkComposerNodeVector3DOperatorBinarySum::run(void)
{
    if (d->receiver_lhs.isEmpty() || d->receiver_rhs.isEmpty()){
        dtkWarn() << "Inputs not specified. Nothing is done";

    } else {

        if (!d->vector) {
            d->vector = new dtkVector3DReal(0., 0., 0.);
            d->emitter_vec.setData(d->vector);
        }

        *(d->vector) = (*(d->receiver_lhs.data())) + (*(d->receiver_rhs.data()));
    }
}

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeVector3DOperatorBinary - SUBSTRACT
// /////////////////////////////////////////////////////////////////

void dtkComposerNodeVector3DOperatorBinarySubstract::run(void)
{
    if (d->receiver_lhs.isEmpty() || d->receiver_rhs.isEmpty()){
        dtkWarn() << "Inputs not specified. Nothing is done";

    } else {

        if (!d->vector) {
            d->vector = new dtkVector3DReal(0., 0., 0.);
            d->emitter_vec.setData(d->vector);
        }

        *(d->vector) = (*(d->receiver_lhs.data())) - (*(d->receiver_rhs.data()));

    }
}
// /////////////////////////////////////////////////////////////////
// dtkComposerNodeVector3DOperatorBinary - CROSS PROD
// /////////////////////////////////////////////////////////////////

void dtkComposerNodeVector3DOperatorBinaryCrossProd::run(void)
{
    if (d->receiver_lhs.isEmpty() || d->receiver_rhs.isEmpty()){
        dtkWarn() << "Inputs not specified. Nothing is done";

    } else {

        if (!d->vector) {
            d->vector = new dtkVector3DReal(0., 0., 0.);
            d->emitter_vec.setData(d->vector);
        }

        *(d->vector) = (*(d->receiver_lhs.data())) % (*(d->receiver_rhs.data()));        
    }
}

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeVector3DOperatorBinaryScalar - DOT PROD
// /////////////////////////////////////////////////////////////////

void dtkComposerNodeVector3DOperatorBinaryScalarDotProd::run(void)
{
    if (d->receiver_lhs.isEmpty() || d->receiver_rhs.isEmpty()){
        dtkWarn() << "Inputs not specified. Nothing is done";

    } else {

        d->value = (*(d->receiver_lhs.data())) * (*(d->receiver_rhs.data()));

    }
}

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeVector3DOperatorHomothetic - MULTIPLICATION
// /////////////////////////////////////////////////////////////////

void dtkComposerNodeVector3DOperatorHomotheticMult::run(void)
{
    if (d->receiver_vec.isEmpty() || d->receiver_val.isEmpty()) { 
        dtkWarn() << "Inputs not specified. Nothing is done";

    } else {
        
        dtkVector3DReal *vector = d->receiver_vec.data();
        qreal value = *d->receiver_val.data();

        *vector *= value;

        d->emitter_vec.setData(vector);
    }
}

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeVector3DOperatorHomothetic - DIVISION
// /////////////////////////////////////////////////////////////////

void dtkComposerNodeVector3DOperatorHomotheticDivision::run(void)
{
    if (d->receiver_vec.isEmpty() || d->receiver_val.isEmpty()) {
        dtkWarn() << "Inputs not specified. Nothing is done";

    } else {
        qreal value = *d->receiver_val.data();

        if (value != 0 ) {
            
            dtkVector3DReal *vector = d->receiver_vec.data();

            *vector /= value;

            d->emitter_vec.setData(vector);

        } else {
            dtkWarn() << "You divide by zero. Nothing is done" ;

        }
    }
}
