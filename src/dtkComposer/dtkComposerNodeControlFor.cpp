/* dtkComposerNodeControlFor.cpp --- 
 * 
 * Author: tkloczko
 * Copyright (C) 2011 - Thibaud Kloczko, Inria.
 * Created: Wed Feb 15 09:14:22 2012 (+0100)
 * Version: $Id$
 * Last-Updated: Sat Aug  4 00:34:52 2012 (+0200)
 *           By: tkloczko
 *     Update #: 133
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "dtkComposerMetatype.h"

#include "dtkComposerNodeControlFor.h"

#include "dtkComposerNodeComposite.h"
#include "dtkComposerNodeProxy.h"

#include "dtkComposerTransmitter.h"
#include "dtkComposerTransmitterReceiver.h"
#include "dtkComposerTransmitterProxy.h"
#include "dtkComposerTransmitterVariant.h"

#include <dtkCore/dtkGlobal.h>

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeControlForPrivate definition
// /////////////////////////////////////////////////////////////////

class dtkComposerNodeControlForPrivate
{
public:    
    dtkComposerNodeProxy header;
    dtkComposerNodeProxy footer;

    dtkComposerNodeComposite cond_block;
    dtkComposerNodeComposite body_block;
    dtkComposerNodeComposite incr_block;

public:
    dtkComposerTransmitterReceiver<bool> cond;
    dtkComposerTransmitterProxy          cond_prx;

    dtkComposerTransmitterVariant block_rcv;

    dtkComposerTransmitterProxy   incr_rcv;
    dtkComposerTransmitterVariant incr_emt;
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeControlFor implementation
// /////////////////////////////////////////////////////////////////

dtkComposerNodeControlFor::dtkComposerNodeControlFor(void) : dtkComposerNodeControl(), d(new dtkComposerNodeControlForPrivate)
{
    d->header.removeEmitter(0);
    d->header.setInputLabelHint("i_begin", 0); 
    d->header.setAsHeader(true);

    d->footer.removeReceiver(0);
    d->footer.setOutputLabelHint("i_end", 0);
    d->footer.setAsFooter(true);

    d->cond_block.setTitleHint("Conditional");
    d->cond_block.appendEmitter(&(d->cond));
    d->cond_block.setOutputLabelHint("cond", 0);

    d->body_block.setTitleHint("Body");

    d->incr_block.setTitleHint("Increment");

    d->cond_block.appendReceiver(&(d->cond_prx));
    d->cond_block.setInputLabelHint("i", 0);
    
    d->body_block.appendReceiver(&(d->block_rcv));
    d->body_block.setInputLabelHint("i", 0);
    this->appendInputTwin(dynamic_cast<dtkComposerTransmitterVariant *>(d->body_block.receivers().first()));  

    d->incr_block.appendReceiver(&(d->incr_rcv));
    d->incr_block.setInputLabelHint("i", 0);

    d->incr_block.appendEmitter(&(d->incr_emt));
    d->incr_block.setOutputLabelHint("i_next", 0);
    this->appendOutputTwin(dynamic_cast<dtkComposerTransmitterVariant *>(d->incr_block.emitters().first()));

    d->body_block.receivers().first()->appendPrevious(d->header.receivers().first());
    d->header.receivers().first()->appendNext(d->body_block.receivers().first());

    d->cond_block.receivers().first()->appendPrevious(d->body_block.receivers().first());
    d->body_block.receivers().first()->appendNext(d->cond_block.receivers().first());

    d->incr_block.receivers().first()->appendPrevious(d->body_block.receivers().first());
    d->body_block.receivers().first()->appendNext(d->incr_block.receivers().first());

    d->incr_block.emitters().first()->appendNext(d->footer.emitters().first());
    d->footer.emitters().first()->appendPrevious(d->incr_block.emitters().first());

    this->outputTwins().first()->setTwin(this->inputTwins().first());
}

dtkComposerNodeControlFor::~dtkComposerNodeControlFor(void)
{
    delete d;

    d = NULL;
}

int dtkComposerNodeControlFor::blockCount(void)
{
    return 3;
}

dtkComposerNodeLeaf *dtkComposerNodeControlFor::header(void)
{
    return &(d->header);
}

dtkComposerNodeLeaf *dtkComposerNodeControlFor::footer(void)
{
    return &(d->footer);
}

dtkComposerNodeComposite *dtkComposerNodeControlFor::block(int id)
{
    if(id == 0)
        return &(d->cond_block);

    if(id == 1)
        return &(d->body_block);

    if(id == 2)
        return &(d->incr_block);

    return NULL;
}

void dtkComposerNodeControlFor::setInputs(void)
{
    // foreach(dtkComposerTransmitterVariant *v, this->inputTwins()) {
    //     v->setTwinned(false);
    //     if (v->container().isReset()) {
    //         v->setData(v->data());
    //     } else {
    //         v->setData(v->container());
    //     }
    //     v->setTwinned(true);
    // }
}

void dtkComposerNodeControlFor::setOutputs(void)
{
    // for (int i = 1; i < this->outputTwins().count(); i++) {
    //     if (this->outputTwins().at(i)->container().isReset()) {
    //         this->outputTwins().at(i)->twin()->setData(this->outputTwins().at(i)->data());
    //     } else {
    //         this->outputTwins().at(i)->twin()->setData(this->outputTwins().at(i)->container());
    //     }
    // }
}

void dtkComposerNodeControlFor::setVariables(void)
{
    // if (this->outputTwins().first()->container().isReset()) {
    //     this->outputTwins().first()->twin()->setData(this->outputTwins().first()->data());
    // } else {
    //     this->outputTwins().first()->twin()->setData(this->outputTwins().first()->container());
    // }
}

int dtkComposerNodeControlFor::selectBranch(void)
{
    return (int)(!*d->cond.data());
}

void dtkComposerNodeControlFor::begin(void)
{
}

void dtkComposerNodeControlFor::end(void)
{
}

QString dtkComposerNodeControlFor::type(void)
{
    return "for";
}

QString dtkComposerNodeControlFor::titleHint(void)
{
    return "For";
}
