/* dtkComposerNodeConditional.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Feb 28 13:03:58 2011 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Apr 28 09:55:33 2011 (+0200)
 *           By: Thibaud Kloczko
 *     Update #: 74
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "dtkComposerEdge.h"
#include "dtkComposerNodeConditional.h"
#include "dtkComposerNodeControlBlock.h"
#include "dtkComposerNodeLoop.h"
#include "dtkComposerNodeProperty.h"

#include <dtkCore/dtkGlobal.h>

// #define DTK_DEBUG_COMPOSER_INTERACTION 1
#define DTK_DEBUG_COMPOSER_EVALUATION 1

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeConditionalPrivate declaration
// /////////////////////////////////////////////////////////////////

class dtkComposerNodeConditionalPrivate
{
public:
    dtkComposerNodeControlBlock *block_then;
    dtkComposerNodeControlBlock *block_else;
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeConditional implementation
// /////////////////////////////////////////////////////////////////

dtkComposerNodeConditional::dtkComposerNodeConditional(dtkComposerNode *parent) : dtkComposerNodeControl(parent), d(new dtkComposerNodeConditionalPrivate)
{
    d->block_then = this->addBlock("then");
    d->block_then->setInteractive(true);

    d->block_else = this->addBlock("else");
    d->block_else->setInteractive(true);

    this->setColor(QColor("#aa7845"));
    this->setTitle("Conditional");
    this->setType("dtkComposerConditional");
}

dtkComposerNodeConditional::~dtkComposerNodeConditional(void)
{
    delete d;

    d = NULL;
}

void dtkComposerNodeConditional::layout(void)
{
    dtkComposerNodeControl::layout();

    int i = 0;
    int j;
    
    foreach(dtkComposerNodeControlBlock *block, this->blocks()) {

        block->setRect(QRectF(this->boundingRect().x(),
                              this->boundingRect().y() + 23 + i * ((this->boundingRect().height() - 46) / this->blocks().count()),
                              this->boundingRect().width(),
                              (this->boundingRect().height() - 46) / this->blocks().count()));

        j = 0;
        foreach(dtkComposerNodeProperty *property, block->inputProperties()) {

            property->setRect(QRectF(block->rect().left() + this->nodeRadius(),
                                     block->rect().top()  + this->nodeRadius() * (4*j + 1),
                                     2 * this->nodeRadius(),
                                     2 * this->nodeRadius() ));

            j++;
        }
        j = 0;
        foreach(dtkComposerNodeProperty *property, block->outputProperties()) {

            property->setRect(QRectF(block->rect().right() - this->nodeRadius() * 3,
                                     block->rect().top()   + this->nodeRadius() * (4*j + 1),
                                     2 * this->nodeRadius(),
                                     2 * this->nodeRadius() ));

            j++;
        }

        i++;
    }     
}

void dtkComposerNodeConditional::update(void)
{
#if defined(DTK_DEBUG_COMPOSER_EVALUATION)
        qDebug() << DTK_PRETTY_FUNCTION << this;
#endif

    if (!this->isRunning()) {

        if (!this->dirty())
            return;

        // -- Check dirty input value

        if (this->dirtyInputValue())
            return;

#if defined(DTK_DEBUG_COMPOSER_EVALUATION)
        qDebug() << DTK_COLOR_BG_GREEN << DTK_PRETTY_FUNCTION << "Dirty input value OK" << DTK_NO_COLOR;
#endif

        // -- Block selection

        if(this->condition())
            this->setCurrentBlock(d->block_then);
        else
            this->setCurrentBlock(d->block_else);

#if defined(DTK_DEBUG_COMPOSER_EVALUATION)
        qDebug() << DTK_COLOR_BG_GREEN << DTK_PRETTY_FUNCTION << "Selected block is" << this->currentBlock()->title() << DTK_NO_COLOR;
#endif

        // -- Check dirty inputs

        if (this->dirtyUpstreamNodes())
            return;

        // -- Mark dirty outputs

        this->markDirtyDownstreamNodes();

#if defined(DTK_DEBUG_COMPOSER_EVALUATION)
        qDebug() << DTK_COLOR_BG_GREEN << DTK_PRETTY_FUNCTION << "All output nodes are set to dirty" << DTK_NO_COLOR;
#endif

        // -- Clean active input routes

        this->cleanInputActiveRoutes();

#if defined(DTK_DEBUG_COMPOSER_EVALUATION)
        qDebug() << DTK_COLOR_BG_YELLOW << DTK_PRETTY_FUNCTION << "Input active routes cleaned" << DTK_NO_COLOR;
#endif

        // -- Pull

        foreach(dtkComposerEdge *i_route, this->inputRoutes())
            if (i_route->destination()->blockedFrom() == this->currentBlock()->title())
                this->pull(i_route, i_route->destination());

#if defined(DTK_DEBUG_COMPOSER_EVALUATION)
        qDebug() << DTK_COLOR_BG_YELLOW << DTK_PRETTY_FUNCTION << "Pull done" << DTK_NO_COLOR;
#endif
        
        // -- Running logics

#if defined(DTK_DEBUG_COMPOSER_EVALUATION)
    qDebug() << DTK_COLOR_BG_RED  << "Running node" << this->title() << "'s logics" << DTK_NO_COLOR;
#endif

        this->setRunning(true);
        this->run();

    } else {

        // -- Check Dirty end nodes

        if (this->dirtyBlockEndNodes())
            return;
        
        // foreach(dtkComposerEdge *o_route, this->outputRelayRoutes())
        //     if (o_route->destination()->blockedFrom() == this->currentBlock()->title())
        //         if (o_route->source()->node()->dirty())
        //             return;

#if defined(DTK_DEBUG_COMPOSER_EVALUATION)
        qDebug() << DTK_COLOR_BG_GREEN << DTK_PRETTY_FUNCTION << "All end block nodes have finished their work" << DTK_NO_COLOR;
#endif

        // -- Clean active output routes

        this->cleanOutputActiveRoutes();

#if defined(DTK_DEBUG_COMPOSER_EVALUATION)
        qDebug() << DTK_COLOR_BG_YELLOW << DTK_PRETTY_FUNCTION << "Output active routes cleaned" << DTK_NO_COLOR;
#endif

        // -- Push

        foreach(dtkComposerEdge *o_route, this->outputRoutes())
            if (o_route->source()->blockedFrom() == this->currentBlock()->title())
                this->push(o_route, o_route->source());

#if defined(DTK_DEBUG_COMPOSER_EVALUATION)
        qDebug() << DTK_COLOR_BG_YELLOW << DTK_PRETTY_FUNCTION << "Push done" << DTK_NO_COLOR;
#endif

        // -- Forward

        this->setDirty(false);
        this->setRunning(false);

#if defined(DTK_DEBUG_COMPOSER_EVALUATION)
        qDebug() << DTK_COLOR_BG_BLUE << DTK_PRETTY_FUNCTION << "Forward done" << DTK_NO_COLOR;
#endif

        foreach(dtkComposerEdge *o_route, this->outputRoutes())
            if (o_route->source()->blockedFrom() == this->currentBlock()->title())
                o_route->destination()->node()->update();

    }
}
