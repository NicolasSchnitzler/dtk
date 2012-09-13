/* dtkComposerNodeQuaternion.cpp --- 
 * 
 * Author: tkloczko
 * Copyright (C) 2011 - Thibaud Kloczko, Inria.
 * Created: Thu Apr 26 13:06:02 2012 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Sep 13 14:33:56 2012 (+0200)
 *           By: tkloczko
 *     Update #: 35
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "dtkComposerNodeQuaternion.h"
#include "dtkComposerTransmitterEmitter.h"
#include "dtkComposerTransmitterReceiver.h"

#include <dtkMath>

// /////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////

class dtkComposerNodeQuaternionPrivate
{
public:
    dtkComposerTransmitterReceiver<dtkQuaternionReal> receiver_quat;

    dtkComposerTransmitterReceiver<qreal> receiver_q0;
    dtkComposerTransmitterReceiver<qreal> receiver_q1;
    dtkComposerTransmitterReceiver<qreal> receiver_q2;
    dtkComposerTransmitterReceiver<qreal> receiver_q3;

public:
    dtkComposerTransmitterEmitter<dtkQuaternionReal> emitter_quat;

    dtkComposerTransmitterEmitter<qreal> emitter_q0;
    dtkComposerTransmitterEmitter<qreal> emitter_q1;
    dtkComposerTransmitterEmitter<qreal> emitter_q2;
    dtkComposerTransmitterEmitter<qreal> emitter_q3;

public:
    dtkQuaternionReal *quat;
    qreal q0;
    qreal q1;
    qreal q2;
    qreal q3;
};

// /////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////

dtkComposerNodeQuaternion::dtkComposerNodeQuaternion(void) : dtkComposerNodeLeaf(), d(new dtkComposerNodeQuaternionPrivate)
{
    d->quat = NULL;
    d->q0 = 0.;
    d->q1 = 0.;
    d->q2 = 0.;
    d->q3 = 0.;

    this->appendReceiver(&d->receiver_quat);

    this->appendReceiver(&d->receiver_q0);
    this->appendReceiver(&d->receiver_q1);
    this->appendReceiver(&d->receiver_q2);
    this->appendReceiver(&d->receiver_q3);

    this->appendEmitter(&d->emitter_quat);

    d->emitter_q0.setData(&d->q0);
    this->appendEmitter(&d->emitter_q0);
    d->emitter_q1.setData(&d->q1);
    this->appendEmitter(&d->emitter_q1);
    d->emitter_q2.setData(&d->q2);
    this->appendEmitter(&d->emitter_q2);
    d->emitter_q3.setData(&d->q3);
    this->appendEmitter(&d->emitter_q3);
}

dtkComposerNodeQuaternion::~dtkComposerNodeQuaternion(void)
{
    delete d;
    
    d = NULL;
}

QString dtkComposerNodeQuaternion::inputLabelHint(int port) 
{
    switch(port) {
    case 0:
        return "quat";
        break;
    case 1:
        return "q0";
        break;
    case 2:
        return "q1";
        break;
    case 3:
        return "q2";
        break;
    case 4:
        return "q3";
        break;
    default:
        break;
    }

    return dtkComposerNodeLeaf::inputLabelHint(port);
}

QString dtkComposerNodeQuaternion::outputLabelHint(int port)
{    
    switch(port) {
    case 0:
        return "quat";
        break;
    case 1:
        return "q0";
        break;
    case 2:
        return "q1";
        break;
    case 3:
        return "q2";
        break;
    case 4:
        return "q3";
        break;
    default:
        break;
    }

    return dtkComposerNodeLeaf::inputLabelHint(port);
}

void dtkComposerNodeQuaternion::run(void)
{
    if (!d->receiver_quat.isEmpty()) {

        d->quat = d->receiver_quat.data();
        d->q0 = (*d->quat)[0];
        d->q1 = (*d->quat)[1];
        d->q2 = (*d->quat)[2];
        d->q3 = (*d->quat)[3];

    } else {

        if (!d->quat)
            d->quat = new dtkQuaternionReal();  

        if (!d->receiver_q0.isEmpty())
            d->q0 = *d->receiver_q0.data();

        if (!d->receiver_q1.isEmpty())
            d->q1 = *d->receiver_q1.data();

        if (!d->receiver_q2.isEmpty())
            d->q2 = *d->receiver_q2.data();

        if (!d->receiver_q3.isEmpty())
            d->q3 = *d->receiver_q3.data();
        
        (*d->quat)[0] = d->q0;
        (*d->quat)[1] = d->q1;
        (*d->quat)[2] = d->q2;
        (*d->quat)[3] = d->q3;
    }

    d->emitter_quat.setData(d->quat);
}
