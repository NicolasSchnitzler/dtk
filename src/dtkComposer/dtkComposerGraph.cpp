/* dtkComposerGraph.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008-2011 - Julien Wintz, Inria.
 * Created: Thu Feb  9 14:43:33 2012 (+0100)
 * Version: $Id$
 * Last-Updated: mar. févr. 21 15:43:30 2012 (+0100)
 *           By: Nicolas Niclausse
 *     Update #: 959
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "dtkComposerGraph.h"
#include "dtkComposerGraphEdge.h"
#include "dtkComposerGraphLayouter.h"
#include "dtkComposerGraphNode.h"
#include "dtkComposerGraphNodeBegin.h"
#include "dtkComposerGraphNodeEnd.h"
#include "dtkComposerGraphNodeLeaf.h"
#include "dtkComposerGraphNodeSelectBranch.h"
#include "dtkComposerGraphNodeSetConditions.h"
#include "dtkComposerGraphNodeSetInputs.h"
#include "dtkComposerGraphNodeSetOutputs.h"
#include "dtkComposerGraphNodeSetVariables.h"
#include "dtkComposerNode.h"
#include "dtkComposerNodeComposite.h"
#include "dtkComposerNodeControl.h"
#include "dtkComposerNodeFor.h"
#include "dtkComposerNodeLeaf.h"
#include "dtkComposerSceneEdge.h"
#include "dtkComposerSceneNode.h"
#include "dtkComposerSceneNodeComposite.h"
#include "dtkComposerSceneNodeControl.h"
#include "dtkComposerScenePort.h"

// /////////////////////////////////////////////////////////////////
// dtkComposerGraphPrivate
// /////////////////////////////////////////////////////////////////

class dtkComposerGraphPrivate
{
public:
    bool exists(dtkComposerSceneEdge *edge);
    bool exists(dtkComposerGraphNode *src, dtkComposerGraphNode *destination);
    void addEdge(dtkComposerGraphNode *source, dtkComposerGraphNode *dest, dtkComposerSceneNode *node, int id = 0);

    dtkComposerGraphNode *getBeginNode(dtkComposerSceneNode *node);
    dtkComposerGraphNode *getEndNode(dtkComposerSceneNode *node);

public:
    QMultiHash<dtkComposerSceneEdge *, dtkComposerGraphEdge *> edges;
    // for each control node, we need to create several graph nodes ("statements")
    QMultiHash<dtkComposerSceneNode *, dtkComposerGraphNode *> nodes;
    // for each control node, we need to create several graph edges, not linked to any scene edge
    QMultiHash<dtkComposerSceneNode *, dtkComposerGraphEdge *> dummy_edges;

public:
    dtkComposerGraph *q;
};

bool dtkComposerGraphPrivate::exists(dtkComposerSceneEdge *edge)
{

    // for control nodes, the graph

    dtkComposerGraphNode *s = this->nodes.value(edge->source()->node());
    dtkComposerGraphNode *d = this->nodes.value(edge->destination()->node());

    foreach(dtkComposerGraphEdge *e, this->edges)
        if(e->source() == s && e->destination() == d)
            return true;

    return false;
}

bool dtkComposerGraphPrivate::exists(dtkComposerGraphNode *s, dtkComposerGraphNode *d)
{

    foreach(dtkComposerGraphEdge *e, this->edges)
        if(e->source() == s && e->destination() == d)
            return true;

    return false;
}


dtkComposerGraphNode *dtkComposerGraphPrivate::getBeginNode(dtkComposerSceneNode *node)
{
    if (dynamic_cast<dtkComposerSceneNodeComposite *>(node)) {
        foreach(dtkComposerGraphNode *n, this->nodes.values(node)) {
            if (dynamic_cast<dtkComposerGraphNodeBegin *>(n) )
                return n;
        }
    } else { //Leaf
        return  this->nodes.value(node);
    }
    return NULL;
}

dtkComposerGraphNode *dtkComposerGraphPrivate::getEndNode(dtkComposerSceneNode *node)
{
    if (dynamic_cast<dtkComposerSceneNodeComposite *>(node)) {
        foreach(dtkComposerGraphNode *n, this->nodes.values(node)) {
            if (dynamic_cast<dtkComposerGraphNodeEnd *>(n) )
                return n;
        }
    } else { //Leaf
        return  this->nodes.value(node);
    }
    return NULL;
}

// add graph edge not related to scene edge (for a scene control nodes)
void dtkComposerGraphPrivate::addEdge(dtkComposerGraphNode *source, dtkComposerGraphNode *destination, dtkComposerSceneNode *node, int id)
{
    dtkComposerGraphEdge *e = new dtkComposerGraphEdge;
    e->setSource(source);
    e->setDestination(destination);
    e->setId(id);
    qDebug() << "adding dummy edge";;
    dummy_edges.insertMulti(node, e);
    edges.insertMulti(0, e);
    q->addItem(e);
}

// /////////////////////////////////////////////////////////////////
// dtkComposerGraph
// /////////////////////////////////////////////////////////////////


dtkComposerGraph::dtkComposerGraph(void) : QGraphicsScene(), d(new dtkComposerGraphPrivate)
{
    d->q = this;

    this->connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

dtkComposerGraph::dtkComposerGraph(const dtkComposerGraph& other) : QGraphicsScene(), d(new dtkComposerGraphPrivate)
{
    d->edges = QMultiHash<dtkComposerSceneEdge *, dtkComposerGraphEdge *>(other.d->edges);
    d->nodes = QMultiHash<dtkComposerSceneNode *, dtkComposerGraphNode *>(other.d->nodes);

    this->connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

dtkComposerGraph::~dtkComposerGraph(void)
{
    // qDeleteAll(d->edges);
    // qDeleteAll(d->nodes);

    delete d;

    d = NULL;
}

void dtkComposerGraph::addNode(dtkComposerSceneNode *node)
{

    qDebug() << " add Node ";
    dtkComposerNode *wrapee = node->wrapee();

    if (dynamic_cast<dtkComposerNodeControl *>(wrapee)) {
        dtkComposerGraphNode *inputs    = new dtkComposerGraphNodeSetInputs(wrapee);
        dtkComposerGraphNode *outputs   = new dtkComposerGraphNodeSetOutputs(wrapee);
        dtkComposerGraphNode *begin     = new dtkComposerGraphNodeBegin(wrapee);
        dtkComposerGraphNode *end       = new dtkComposerGraphNodeEnd(wrapee);
        dtkComposerGraphNode *set_conds = new dtkComposerGraphNodeSetConditions(wrapee);
        dtkComposerGraphNode *select    = new dtkComposerGraphNodeSelectBranch(wrapee);

        d->nodes.insertMulti(node, inputs);
        d->nodes.insertMulti(node, outputs);
        d->nodes.insertMulti(node, begin);
        d->nodes.insertMulti(node, end);
        d->nodes.insertMulti(node, select);
        d->nodes.insertMulti(node, set_conds);

        if (dynamic_cast<dtkComposerNodeFor *>(wrapee)) {
            dtkComposerGraphNode *begin_cond = new dtkComposerGraphNodeBegin(wrapee,"Begin Cond");
            dtkComposerGraphNode *end_cond     = new dtkComposerGraphNodeEnd(wrapee,"End Cond");
            dtkComposerGraphNode *begin_block = new dtkComposerGraphNodeBegin(wrapee,"Begin Block");
            dtkComposerGraphNode *end_block     = new dtkComposerGraphNodeEnd(wrapee,"End Block");
            dtkComposerGraphNode *begin_inc   = new dtkComposerGraphNodeBegin(wrapee,"Begin Inc");
            dtkComposerGraphNode *end_inc       = new dtkComposerGraphNodeEnd(wrapee,"End Inc");
            dtkComposerGraphNode *vars = new dtkComposerGraphNodeSetVariables(wrapee);
            QList<dtkComposerSceneNodeComposite *> blocks  = dynamic_cast<dtkComposerSceneNodeControl *>(node)->blocks();

            d->nodes.insertMulti(blocks[0], begin_cond);
            d->nodes.insertMulti(blocks[0], end_cond);
            d->nodes.insertMulti(blocks[1], begin_block);
            d->nodes.insertMulti(blocks[1], end_block);
            d->nodes.insertMulti(blocks[2], begin_inc);
            d->nodes.insertMulti(blocks[2], end_inc);
            d->nodes.insertMulti(node, vars);

            foreach (dtkComposerSceneNodeComposite *b, blocks ) {
                foreach (dtkComposerGraphNode *n, d->nodes.values(b) ) {
                    this->addItem(n);
                }
            }

            d->addEdge(   begin,        inputs, node);
            d->addEdge(   inputs,   begin_cond, node);
            d->addEdge( end_cond,    set_conds, node);
            d->addEdge(set_conds,       select, node);
            d->addEdge(   select,  begin_block, node);
            d->addEdge(   select,          end, node, 1);
            d->addEdge(end_block,      outputs, node);
            d->addEdge(  outputs,    begin_inc, node);
            d->addEdge(  end_inc,         vars, node);
            d->addEdge(     vars,   begin_cond, node);
        }
        // } else if (dynamic_cast<dtkComposerNodeWhile *>(wrapee)) {
        //     dtkComposerGraphNode *begin_cond  = new dtkComposerGraphNodeBegin(wrapee,"Begin Cond");
        //     dtkComposerGraphNode *end_cond      = new dtkComposerGraphNodeEnd(wrapee,"End Cond");
        //     dtkComposerGraphNode *begin_block = new dtkComposerGraphNodeBegin(wrapee,"Begin Block");
        //     dtkComposerGraphNode *end_block     = new dtkComposerGraphNodeEnd(wrapee,"End Block");

        //     d->nodes.insertMulti(node, begin_cond);
        //     d->nodes.insertMulti(node, end_cond);
        //     d->nodes.insertMulti(node, begin_block);
        //     d->nodes.insertMulti(node, end_block);

        //     d->addEdge(   begin,       inputs, node);
        //     d->addEdge(   inputs,  begin_cond, node);
        //     d->addEdge( end_cond,   set_conds, node);
        //     d->addEdge(set_conds,      select, node);

        //     d->addEdge(   select, begin_block, node);
        //     d->addEdge(   select,         end, node, 1);
        //     d->addEdge(end_block,     outputs, node);
        //     d->addEdge(  outputs, begin_conds, node);
        // } else if (dynamic_cast<dtkComposerNodeIf *>(wrapee)) {
        //     dtkComposerGraphNode *begin_true  = new dtkComposerGraphNodeBegin(wrapee,"Begin True");
        //     dtkComposerGraphNode *end_true      = new dtkComposerGraphNodeEnd(wrapee,"End True");
        //     dtkComposerGraphNode *begin_false = new dtkComposerGraphNodeBegin(wrapee,"Begin False");
        //     dtkComposerGraphNode *end_false     = new dtkComposerGraphNodeEnd(wrapee,"End False");

        //     d->nodes.insertMulti(node, begin_true);
        //     d->nodes.insertMulti(node, end_true);
        //     d->nodes.insertMulti(node, begin_false);
        //     d->nodes.insertMulti(node, end_false);

        //     // FIXME: what about inputs ?
        //     d->addEdge(   begin,    set_conds, node);
        //     d->addEdge(set_conds,      select, node);

        //     d->addEdge(   select,  begin_true, node);
        //     d->addEdge(   select, begin_false, node, 1);
        //     d->addEdge( end_true,     outputs, node);
        //     d->addEdge(end_false,     outputs, node);
        //     d->addEdge(  outputs,         end, node);
        // }
    } else if (dynamic_cast<dtkComposerNodeComposite *>(wrapee)) {
        dtkComposerGraphNode *begin = new dtkComposerGraphNodeBegin(wrapee,"Begin");
        dtkComposerGraphNode *end   = new dtkComposerGraphNodeEnd(wrapee,"End");
        d->nodes.insertMulti(node, begin);
        d->nodes.insertMulti(node, end);

         dtkComposerSceneNodeComposite *composite = dynamic_cast<dtkComposerSceneNodeComposite *>(node);
         foreach(dtkComposerSceneNode *child, composite->nodes()) {
             foreach(dtkComposerGraphEdge *edge, d->edges.values()) {
                 if ((d->nodes.key(edge->source()) == child) && !(composite->nodes().contains(d->nodes.key(edge->destination())))) {
                     dtkComposerGraph::removeEdge(d->edges.key(edge));
                     d->addEdge(edge->source(), end, node);
                     d->addEdge(end, edge->destination(), node);
                 } else if ((d->nodes.key(edge->destination()) == child) && !(composite->nodes().contains(d->nodes.key(edge->source())))) {
                     dtkComposerGraph::removeEdge(d->edges.key(edge));
                     d->addEdge(begin, edge->destination(), node);
                     d->addEdge(edge->source(), begin, node);
                 }
             }
             d->addEdge( begin, d->nodes.value(child), node);
             d->addEdge( d->nodes.value(child),  end, node);
         }
    } else { // Leaf node
        dtkComposerGraphNode *leaf = new dtkComposerGraphNodeLeaf(wrapee);
        d->nodes.insertMulti(node, leaf);
        dtkComposerSceneNodeComposite *parent;
        if (parent = dynamic_cast<dtkComposerSceneNodeComposite *>(node->parent())) {
            if (!parent->root()) {
                    //leaf node is added in a composite (or block)
                    dtkComposerGraphNode *begin = d->getBeginNode(parent);
                    dtkComposerGraphNode *end   = d->getEndNode(parent);
                    d->addEdge( begin, leaf, node);
                    d->addEdge(  leaf,  end, node);
                }
        }
    }
    foreach (dtkComposerGraphNode *n, d->nodes.values(node) )
        this->addItem(n);
    this->layout();
}

void dtkComposerGraph::removeGroup(dtkComposerSceneNode *node) {
    qDebug() << " remove group, NOT IMPLEMENTED!!!";
}


void dtkComposerGraph::removeNode(dtkComposerSceneNode *node)
{
    qDebug() << " remove node";

    QList<dtkComposerGraphNode *> nodes = d->nodes.values(node);
    QList<dtkComposerGraphEdge *> edges = d->dummy_edges.values(node);

    foreach(dtkComposerGraphEdge *dummy, edges) {
        qDebug() << " remove real edge based on dummy edge";
        d->edges.remove(0, dummy);
    }

    d->nodes.remove(node);
    d->dummy_edges.remove(node);

    dtkComposerSceneNodeControl *control;
    // need to also remove blocks begin/end nodes for control nodes
    if (control = dynamic_cast<dtkComposerSceneNodeControl *>(node)) {
        qDebug() << "control node, remove blocks items";
        foreach(  dtkComposerSceneNodeComposite *block, control->blocks()) {
            foreach(dtkComposerGraphNode *n, d->nodes.values(block)) {
                this->removeItem(n);
            }
            foreach(dtkComposerGraphEdge *e, d->dummy_edges.values(block)) {
                this->removeItem(e);
            }
            d->nodes.remove(block);
        }
    } else if (dynamic_cast<dtkComposerSceneNodeComposite *>(node)) {
        qDebug() << "composite node, remove childs nodesx";
        foreach(dtkComposerSceneNode *n, dynamic_cast<dtkComposerSceneNodeComposite *>(node)->nodes()) {
            dtkComposerGraph::removeNode(n);
        }
    }

    foreach(dtkComposerGraphNode *n, nodes) {
        this->removeItem(n);
    }

    foreach(dtkComposerGraphEdge *e, edges) {
        qDebug() << "remove dummy edge";
        this->removeItem(e);
    }

    this->layout();
}

void dtkComposerGraph::addEdge(dtkComposerSceneEdge *edge)
{
    qDebug() << " addEdge ";
    if(!d->nodes.contains(edge->source()->node()))
        return;

    if(!d->nodes.contains(edge->destination()->node()))
        return;

    if(d->exists(edge))
        return;

    dtkComposerGraphEdge *e = new dtkComposerGraphEdge;
    // dtkComposerGraphNode *src = d->nodes.value(edge->source()->node());
    // dtkComposerGraphNode *dest = d->nodes.value(edge->destination()->node());
    dtkComposerSceneNode *scene_src = edge->source()->node();
    dtkComposerSceneNode *scene_dest = edge->destination()->node();
    dtkComposerGraphNode *src ;
    dtkComposerGraphNode *dest;

    if (scene_src->parent() == scene_dest) {
        src = d->getEndNode(scene_src);
        dest = d->getEndNode(scene_dest);
    } else if (scene_dest->parent() == scene_src) {
        src = d->getBeginNode(scene_src);
        dest = d->getBeginNode(scene_dest);
    } else {
        src = d->getEndNode(scene_src);
        dest = d->getBeginNode(scene_dest);
    }

    e->setSource(src);
    e->setDestination(dest);

    if(d->exists(src,dest)) {
        delete e;
        return;
    }

    d->edges.insertMulti(edge, e);

    this->addItem(e);

    this->layout();
}

void dtkComposerGraph::removeEdge(dtkComposerSceneEdge *edge)
{
    if(!d->edges.contains(edge))
        return;

    QList<dtkComposerGraphEdge *> edges = d->edges.values(edge);

    d->edges.remove(edge);

    foreach (dtkComposerGraphEdge *e, edges)
        this->removeItem(e);

    this->layout();
}

dtkComposerGraphEdgeList dtkComposerGraph::edges(void)
{
    return dtkComposerGraphEdgeList(d->edges.values());
}

dtkComposerGraphNodeList dtkComposerGraph::nodes(void)
{
    return dtkComposerGraphNodeList(d->nodes.values());
}

void dtkComposerGraph::removeNode(dtkComposerGraphNode *node)
{
    foreach(dtkComposerGraphNode *n, d->nodes.values())
        if(n = node)
            d->nodes.remove(d->nodes.key(n));
}

void dtkComposerGraph::removeEdge(dtkComposerGraphEdge *edge)
{
    foreach(dtkComposerGraphEdge *e, d->edges.values())
        if(e = edge)
            d->edges.remove(d->edges.key(e));
}

void dtkComposerGraph::layout(void)
{
    dtkComposerGraphLayouter layouter;
    layouter.setGraph(this);
    layouter.layout();

    this->update();
}

void dtkComposerGraph::onSelectionChanged(void)
{
    QList<QGraphicsItem *> selection = this->selectedItems();

    if(selection.count() != 1)
        return;

    dtkComposerGraphNode *node = dynamic_cast<dtkComposerGraphNode *>(selection.first());

    if(!node)
        return;

    dtkComposerSceneNode *n = d->nodes.key(node);

    if (n)
        n->setSelected(true);
}
