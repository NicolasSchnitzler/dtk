/* dtkComposerScene.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Sep  7 15:06:06 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu May  5 13:27:05 2011 (+0200)
 *           By: Thibaud Kloczko
 *     Update #: 2661
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "dtkComposerEdge.h"
#include "dtkComposerNode.h"
#include "dtkComposerNodeControl.h"
#include "dtkComposerNodeControlBlock.h"
#include "dtkComposerNodeFactory.h"
#include "dtkComposerNodeProperty.h"
#include "dtkComposerNote.h"
#include "dtkComposerScene.h"
#include "dtkComposerScene_p.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// dtkComposerScenePrivate
// /////////////////////////////////////////////////////////////////

dtkComposerEdge *dtkComposerScenePrivate::edge(dtkComposerEdge *edge)
{
    foreach(dtkComposerEdge *e, this->edges)
        if (edge->source() == e->source() && edge->destination() == e->destination())
            return e;

    return NULL;
}

bool dtkComposerScenePrivate::isChildOf(QGraphicsItem *item, QGraphicsItem *parent)
{
    if (item == parent)
        return true;

    QGraphicsItem *p = item->parentItem();

    while (p)
        if (p == parent)
            return true;
        else
            p = p->parentItem();
    
    return false;
}

// /////////////////////////////////////////////////////////////////
// dtkComposerScene
// /////////////////////////////////////////////////////////////////

dtkComposerScene::dtkComposerScene(QObject *parent) : QGraphicsScene(parent), d(new dtkComposerScenePrivate)
{
    d->grabber_node_origin = QPointF(0,0);
    d->grabber_node_has_edges = false;

    d->grabber_node = NULL;
    d->current_edge = NULL;
    d->current_node = NULL;

    d->factory = new dtkComposerNodeFactory;
    
    d->modified = false;

    connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

dtkComposerScene::~dtkComposerScene(void)
{
    delete d->factory;
    delete d;

    d = NULL;
}

QList<dtkComposerNote *> dtkComposerScene::notes(void)
{
    return d->notes;
}

QList<dtkComposerEdge *> dtkComposerScene::edges(void)
{
    return d->edges;
}

QList<dtkComposerNode *> dtkComposerScene::nodes(void)
{
    return d->nodes;
}

QList<dtkComposerNode *> dtkComposerScene::nodes(QString name)
{
    QList<dtkComposerNode *> list;

    foreach(dtkComposerNode *node, d->nodes)
        if (node->object()->name() == name)
            list << node;

    return list;
}

QList<dtkComposerNodeProperty *> dtkComposerScene::properties(void)
{
    QList<dtkComposerNodeProperty *> list;

    foreach(QGraphicsItem *item, this->items())
        if (dtkComposerNodeProperty *property = dynamic_cast<dtkComposerNodeProperty *>(item))
            list << property;

    return list;
}

QList<dtkComposerNodeProperty *> dtkComposerScene::properties(QString name)
{
    QList<dtkComposerNodeProperty *> list;

    foreach(QGraphicsItem *item, this->items())
        if (dtkComposerNodeProperty *property = dynamic_cast<dtkComposerNodeProperty *>(item))
            if (property->name() == name)
                list << property;

    return list;
}

//! Start nodes
/*! 
 *  Returns the list of nodes that are not within a composite nor
 *  within a control block, with no input edges
 */

QList<dtkComposerNode *> dtkComposerScene::startNodes(void)
{
    QList<dtkComposerNode *> list;

    dtkComposerNode *p = NULL;

    foreach(dtkComposerNode *node, d->nodes) {
        if (!node->inputEdges().count() && node->outputEdges().count() && node->kind() != dtkComposerNode::Composite) {

            p = node->parentNode();
            while (p) {
                if (p->kind() == dtkComposerNode::Control)
                    break;
                else
                    p = p->parentNode();
            }
            if (!p)
                list << node;
        }
    }

    return list;
}

QList<dtkComposerNode *> dtkComposerScene::endNodes(void)
{
    QList<dtkComposerNode *> list;

    dtkComposerNode *p = NULL;

    foreach(dtkComposerNode *node, d->nodes) {
        if (node->inputEdges().count() && !node->outputEdges().count() && node->kind() != dtkComposerNode::Composite) {

            p = node->parentNode();
            while (p) {
                if (p->kind() == dtkComposerNode::Control)
                    break;
                else
                    p = p->parentNode();
            }
            if (!p)
                list << node;
        }
    }

    return list;
}

void dtkComposerScene::touch(void)
{
    this->updateEdgesVisibility();
}

void dtkComposerScene::clear(void)
{
    foreach(dtkComposerNode *node, this->nodes())
        delete node;

    foreach(dtkComposerNote *note, this->notes())
        delete note;

    d->nodes.clear();
    d->notes.clear();
}

bool dtkComposerScene::isModified(void)
{
    return d->modified;
}

void dtkComposerScene::setModified(bool modified)
{
    d->modified = modified;           

    if (d->modified)
        emit compositionChanged();
}

void dtkComposerScene::addEdge(dtkComposerEdge *edge)
{
    this->addItem(edge);

    d->edges << edge;

    //this->setModified(true);
}

void dtkComposerScene::addNode(dtkComposerNode *node)
{
    d->nodes << node;

    emit nodeAdded(node);

    //this->setModified(true);
}

void dtkComposerScene::addNote(dtkComposerNote *note)
{
    this->addItem(note);

    d->notes << note;

    //this->setModified(true);
}

void dtkComposerScene::removeEdge(dtkComposerEdge *edge)
{
    edge->unlink();

    d->edges.removeAll(edge);

    delete edge;
    edge = NULL;
}

void dtkComposerScene::removeNode(dtkComposerNode *node)
{
    foreach(dtkComposerEdge *edge, node->inputEdges())
        this->removeEdge(edge);
    
    foreach(dtkComposerEdge *edge, node->outputEdges())
        this->removeEdge(edge);

    foreach(dtkComposerEdge *edge, node->inputRelayEdges())
        this->removeEdge(edge);
    
    foreach(dtkComposerEdge *edge, node->outputRelayEdges())
        this->removeEdge(edge);

    foreach(dtkComposerEdge *edge, node->inputGhostEdges())
        this->removeEdge(edge);
    
    foreach(dtkComposerEdge *edge, node->outputGhostEdges())
        this->removeEdge(edge);

    
    dtkComposerNode *parent = node->parentNode();
    if (parent)
        parent->removeChildNode(node); 

    if (parent && parent->kind() == dtkComposerNode::Control)        
        foreach(dtkComposerNodeControlBlock *parent_block, (dynamic_cast<dtkComposerNodeControl *>(parent))->blocks())
            if (parent_block->nodes().contains(node))
                parent_block->removeNode(node);

    dtkComposerNode *n = node;

    while(n->parentNode()) {

        parent = n->parentNode();

        foreach(dtkComposerNodeProperty *property, parent->inputProperties()) {
            if (property->clonedFrom() == node) {

                foreach(dtkComposerEdge *edge, parent->inputEdges())
                    if (edge->destination() == property)
                        this->removeEdge(edge);

                foreach(dtkComposerEdge *edge, parent->inputGhostEdges())
                    if (edge->source() == property)
                        this->removeEdge(edge);

                parent->removeInputProperty(property);
                delete property;
                property = NULL;
            }
        }

        foreach(dtkComposerNodeProperty *property, parent->outputProperties()) {
            if (property->clonedFrom() == node) {

                foreach(dtkComposerEdge *edge, parent->outputEdges())
                    if (edge->source() == property)
                        this->removeEdge(edge);

                foreach(dtkComposerEdge *edge, parent->outputGhostEdges()) 
                    if (edge->destination() == property) 
                        this->removeEdge(edge);

                parent->removeOutputProperty(property);
                delete property;
                property = NULL;
            }
        }

        n = parent;
    }

    QList<dtkComposerNode *> selected_nodes;
    foreach(QGraphicsItem *item, this->selectedItems())
        if (dtkComposerNode *sn = dynamic_cast<dtkComposerNode *>(item))
            selected_nodes << sn;

    foreach(dtkComposerNode *child, node->childNodes()) {
        node->removeChildNode(child);
        if (!selected_nodes.contains(child))
            this->removeNode(child);
    }

    d->nodes.removeAll(node);
    
    delete node;
    node = NULL;
}

void dtkComposerScene::removeNote(dtkComposerNote *note)
{
    d->notes.removeAll(note);

    delete note;
}

//! Group creation. Creates a composite node.
/*! 
 * \param nodes The list of nodes to be grouped together. Beware not
 * to group a not together with a composite node the node is a child
 * of.
 * 
 * \return The composite node that form the group.
 */

dtkComposerNode *dtkComposerScene::createGroup(QList<dtkComposerNode *> nodes, QPointF position)
{
    if (d->grabber_node)
        d->grabber_node = NULL;

    dtkComposerNodeControlBlock * parent_block = NULL;

    foreach(dtkComposerNode *node, nodes) {
        if (node->parentItem() != nodes.first()->parentItem()) {
            qDebug() << "Cannot create group of nodes that do not share the same parent item.";
            return NULL;
        }
    }        

    dtkComposerNode *group = new dtkComposerNode;
    group->setTitle("Composite node");
    group->setType("dtkComposerNodeComposite");
    group->setKind(dtkComposerNode::Composite);

    this->addNode(group);
    this->addItem(group);

    if (nodes.first()->parentNode() != d->current_node) {
        parent_block = dynamic_cast<dtkComposerNodeControlBlock *>(nodes.first()->parentItem());
        group->setParentNode(parent_block->parentNode());
        group->setParentItem(parent_block);
        parent_block->parentNode()->addChildNode(group);
        parent_block->addNode(group);
    } else if (d->current_node) {
        group->setParentNode(d->current_node);
        group->setParentItem(d->current_node);
        d->current_node->addChildNode(group);
    } else {
        group->setParentNode(NULL);
        group->setParentItem(NULL);
    }

    QPointF group_ave_pos(0., 0.);

    dtkComposerNodeProperty *source;
    dtkComposerNodeProperty *destin;
    dtkComposerNodeProperty *clone;

    foreach(dtkComposerNode *node, nodes) {

        group_ave_pos += node->pos();
       
        if (dtkComposerNode *parent = node->parentNode())
            parent->removeChildNode(node);
        if (parent_block = dynamic_cast<dtkComposerNodeControlBlock *>(node->parentItem()))
            parent_block->removeNode(node);

        node->setParentNode(group);
        group->addChildNode(node);

        foreach(dtkComposerNodeProperty *property, node->inputProperties())
            group->addInputProperty(property->clone(group));

        foreach(dtkComposerNodeProperty *property, node->outputProperties())
            group->addOutputProperty(property->clone(group));

        foreach(dtkComposerEdge *edge, node->inputEdges()) {
            
            if (!nodes.contains(edge->source()->node())) {
                    
                source = edge->source();
                destin = edge->destination();

                if (node->kind() == dtkComposerNode::Composite)
                    clone = group->inputProperty(destin->name(), destin->clonedFrom());
                else
                    clone = group->inputProperty(destin->name(), node);

                clone->show();

                this->removeEdge(edge);

                dtkComposerEdge *input = new dtkComposerEdge;
                input->setSource(source);
                input->setDestination(clone);
                input->link(true);
                this->addEdge(input);

                dtkComposerEdge *ghost = new dtkComposerEdge;
                ghost->setSource(clone);
                ghost->setDestination(destin);
                ghost->link(true);
                this->addEdge(ghost);
            }
        }
        
        foreach(dtkComposerEdge *edge, node->outputEdges()) {

            if (!nodes.contains(edge->destination()->node())) {
                    
                source = edge->source();
                destin = edge->destination();

                if (node->kind() == dtkComposerNode::Composite)
                    clone = group->outputProperty(source->name(), source->clonedFrom());
                else
                    clone = group->outputProperty(source->name(), node);

                clone->show();

                this->removeEdge(edge);

                dtkComposerEdge *output = new dtkComposerEdge;
                output->setSource(clone);
                output->setDestination(destin);
                output->link();
                this->addEdge(output);

                dtkComposerEdge *ghost = new dtkComposerEdge;
                ghost->setSource(source);
                ghost->setDestination(clone);
                ghost->link(true);
                this->addEdge(ghost);
            }

        }

        //node->removeAllEdges();
        node->hide();
    }
    group->setPos(group_ave_pos / nodes.count());
    group->setGhost(false);

    this->updateEdgesVisibility();
        
    return group;
}

dtkComposerNode *dtkComposerScene::createNode(QString type, QPointF position)
{
    if (dtkComposerNode *node = d->factory->create(type)) {
        node->setPos(position);
        node->setParentNode(d->current_node);
        node->setParentItem(d->current_node);

        if (d->current_node)
            d->current_node->addChildNode(node);

        dtkComposerNode *n = d->current_node;

        while (n) {
            
            foreach(dtkComposerNodeProperty *property, node->inputProperties())
                n->addInputProperty(property->clone(n));
            
            foreach(dtkComposerNodeProperty *property, node->outputProperties())
                n->addOutputProperty(property->clone(n));
            
            n = n->parentNode();

        }

        this->addNode(node);
        this->addItem(node);

        return node;

    } else {

        return NULL;
    }
}

dtkComposerNote *dtkComposerScene::createNote(QString text, QPointF position, QSizeF size)
{
    dtkComposerNote *note = new dtkComposerNote(d->current_node);
    
    if (!text.isNull())
        note->setText(text);

    if (!position.isNull())
        note->setPos(position);

    if (size.isValid() && !size.isNull())
        note->setSize(size);

    this->addNote(note);

    return note;
}

void dtkComposerScene::explodeGroup(dtkComposerNode *node)
{
    if (!node)
        return;
    
    if (node->kind() != dtkComposerNode::Composite) {
        qDebug() << "Only composite nodes can be ungrouped.";
        return;
    }

    if (d->grabber_node)
        d->grabber_node = NULL;

    dtkComposerNode *parent = node->parentNode();

    // -- Looking for parent block if parent node is of control kind

    dtkComposerNodeControlBlock *parent_block = NULL;
    if (parent && parent->kind() == dtkComposerNode::Control)
        foreach(dtkComposerNodeControlBlock *block, (dynamic_cast<dtkComposerNodeControl *>(parent))->blocks())
            if (block->nodes().contains(node))
                parent_block = block;

    // -- Looking for a composite parent node through the parent ancestors.

    dtkComposerNode *ghost_parent = NULL;        
    ghost_parent = node->parentNode();
    while (ghost_parent) {
        if (ghost_parent->kind() == dtkComposerNode::Composite)  
            break; 
        else
            ghost_parent = ghost_parent->parentNode();
    }
    
    // -- 

    bool node_was_ghost = node->isGhost();
    QPointF scene_center = node->mapRectToScene(node->boundingRect()).center();

    // -- For each child nodes, the parenthood is reset and the edge logic is saved.

    foreach(dtkComposerNode *child, node->childNodes()) {

        if (parent)
            parent->addChildNode(child);

        child->setParentNode(parent);

        if (parent_block) {
            parent_block->addNode(child);
            child->setParentItem(parent_block);
        } else {
            child->setParentItem(node->parentItem());
        }

        foreach(dtkComposerEdge *ghost, node->inputGhostEdges()) {
            if (ghost->destination()->node() == child) {
                foreach(dtkComposerEdge *input, node->inputEdges()) {
                    if (input->destination() == ghost->source()) {
                        dtkComposerEdge *e = new dtkComposerEdge;
                        e->setSource(input->source());
                        e->setDestination(ghost->destination());
                        e->link(true);
                        this->addEdge(e);
                    }
                 }
            }
        } 

        foreach(dtkComposerEdge *ghost, node->outputGhostEdges()) {
            if (ghost->source()->node() == child) {
                foreach(dtkComposerEdge *output, node->outputEdges()) {
                    if (output->source() == ghost->destination()) {
                        dtkComposerEdge *e = new dtkComposerEdge;
                        e->setSource(ghost->source());
                        e->setDestination(output->destination());
                        e->link(true);
                        this->addEdge(e);
                    }
                }
            }
        }

        node->removeChildNode(child);
        
    }        
    
    this->removeNode(node);

    if (node_was_ghost && ghost_parent) {
    
        ghost_parent->setGhost(true);
        ghost_parent->setPos(ghost_parent->ghostPosition());
        this->showChildNodes(ghost_parent);
        this->setCurrentNode(ghost_parent);
            
        emit centerOn(ghost_parent->mapRectToScene(parent->boundingRect()).center());
        emit pathChanged(d->current_node);

    } else if (ghost_parent) {

        this->showChildNodes(ghost_parent);
        
    } else {

        this->showAllNodes();
        this->setCurrentNode(NULL);
            
        emit centerOn(scene_center);
        emit pathChanged(d->current_node);
    }

    if (parent_block)
        parent_block->parentNode()->resize();
        
    this->updateEdgesVisibility();
}

void dtkComposerScene::setFactory(dtkComposerNodeFactory *factory)
{
    d->factory = factory;
}

void dtkComposerScene::startEvaluation(void)
{
    s_evaluate = true;

    emit evaluationStarted();

    // foreach(dtkComposerNode *node, this->nodes())
    //     if (node->kind() == dtkComposerNode::Data)
    //         node->setDirty(false);
    //     else
    //         node->setDirty(true);

    // foreach(dtkComposerNode *node, this->nodes()) {
    //     node->setActive(false);
    //     node->setDirty(true);
    // }

    if (this->selectedItems().count()) {
        foreach(QGraphicsItem *item, this->selectedItems()) {
            if (dtkComposerNode *node = dynamic_cast<dtkComposerNode *>(item)) { 
                node->setActive(false);        
                node->setDirty(true);
                node->update();
            }
        }
    } else {
        foreach(dtkComposerNode *node, this->startNodes()) {
            node->setActive(false);
            node->setDirty(true);
            node->update();
        }
    }
}

void dtkComposerScene::stopEvaluation(void)
{
    s_evaluate = false;

    emit evaluationStopped();
}

void dtkComposerScene::copy(void)
{
    d->clipboard.nodes.clear();
    d->clipboard.edges.clear();

    foreach(QGraphicsItem *item, this->selectedItems())
        if (dtkComposerNode *node = dynamic_cast<dtkComposerNode *>(item))
            d->clipboard.nodes << node;

    foreach(dtkComposerEdge *edge, d->edges)
        if (d->clipboard.nodes.contains(edge->source()->node()) && d->clipboard.nodes.contains(edge->destination()->node()))
            d->clipboard.edges << edge;

    // -- Forbid copy/paste of composite nodes for the moment
    
    foreach(dtkComposerNode *node, d->clipboard.nodes) {
        if (node->kind() == dtkComposerNode::Composite) {
            d->clipboard.nodes.clear();
            d->clipboard.edges.clear();
        }
    }
}

void dtkComposerScene::paste(void)
{
    QMap<dtkComposerNode *, dtkComposerNode *> node_map;

    foreach(dtkComposerNode *node, d->clipboard.nodes)
        node_map.insert(node, this->createNode(node->type(), node->pos() + QPointF(200, 200)));

    foreach(dtkComposerEdge *edge, d->clipboard.edges) {

        dtkComposerEdge *e = new dtkComposerEdge;
        e->setSource(node_map.value(edge->source()->node())->outputProperty(edge->source()->name()));
        e->setDestination(node_map.value(edge->destination()->node())->inputProperty(edge->destination()->name()));

        node_map.value(edge->source()->node())->addOutputEdge(e, node_map.value(edge->source()->node())->outputProperty(edge->source()->name()));
        node_map.value(edge->destination()->node())->addInputEdge(e, node_map.value(edge->destination()->node())->inputProperty(edge->destination()->name()));

        this->addEdge(e);
    }
}

dtkComposerNode *dtkComposerScene::nodeAt(const QPointF& point) const
{
    QList<QGraphicsItem *> items = this->items(point.x(), point.y(), 1, 1, Qt::IntersectsItemBoundingRect);

    foreach(QGraphicsItem *item, items)
        if (dtkComposerNode *node = dynamic_cast<dtkComposerNode *>(item))
            return node;

    return NULL;
}

dtkComposerNodeProperty *dtkComposerScene::propertyAt(const QPointF& point) const
{
    QList<QGraphicsItem *> items = this->items(point.x(), point.y(), 1, 1, Qt::IntersectsItemBoundingRect);

    foreach(QGraphicsItem *item, items)
        if (dtkComposerNodeProperty *property = dynamic_cast<dtkComposerNodeProperty *>(item))
            return property;         

    return NULL;
}

void dtkComposerScene::setCurrentNode(dtkComposerNode *node)
{
    d->current_node = node;
}

void dtkComposerScene::hideAllNodes(void)
{
    foreach(QGraphicsItem *item, this->items()) {
        if (dtkComposerNode *node = dynamic_cast<dtkComposerNode *>(item)) {
            if (node->isVisible()) {
                node->hide();
            }
        }
    }
}

void dtkComposerScene::showAllNodes(void)
{
    foreach(dtkComposerNode *node, d->nodes) {
        if (!node->parentNode() || node->parentNode()->kind() != dtkComposerNode::Composite)
            node->show();
    }
}

void dtkComposerScene::hideChildNodes(dtkComposerNode *node)
{
    foreach(dtkComposerNode *child, node->childNodes())
        child->hide();

    node->hide();
}

void dtkComposerScene::showChildNodes(dtkComposerNode *node)
{
    node->show();

    foreach(dtkComposerNode *child, node->childNodes())
        if (child->kind() == dtkComposerNode::Control)
            this->showChildNodes(child);
        else
            child->show();
}

void dtkComposerScene::updateEdgesVisibility(void)
{
    foreach(dtkComposerEdge *edge, d->edges) {
        
        if (edge->source()->node()->isVisible() && edge->destination()->node()->isVisible() && !edge->isVisible())
            edge->show();

        if (!edge->source()->node()->isVisible() && edge->destination()->node()->isVisible() && edge->isVisible())
            edge->hide();

        if (edge->source()->node()->isVisible() && !edge->destination()->node()->isVisible() && edge->isVisible())
            edge->hide();

        if (!edge->source()->node()->isVisible() && !edge->destination()->node()->isVisible() && edge->isVisible())
            edge->hide();

        edge->adjust();
    }
}

QList<dtkComposerNodeControlBlock *> dtkComposerScene::hoveredControlBlocks(dtkComposerNode *node)
{
    QList<dtkComposerNodeControlBlock *> blocks;
    QList<dtkComposerNodeControlBlock *> hovered_child_blocks;

    QRectF node_rec = node->mapRectToScene(node->boundingRect());

    foreach(QGraphicsItem *item, this->items(node_rec)) {

        if (dtkComposerNodeControlBlock *block = dynamic_cast<dtkComposerNodeControlBlock *>(item)) {

            if (!d->isChildOf(block, node)) {

                hovered_child_blocks = this->hoveredControlBlocks(node, block->nodes());

                if (block->mapRectToScene(block->boundingRect()).contains(node_rec) && hovered_child_blocks.isEmpty()) {

                    blocks.clear();
                    blocks << block;
                    return blocks;

                } else if (hovered_child_blocks.isEmpty()) {

                    blocks << block;

                } else {

                    bool node_is_shared = true;

                    foreach(dtkComposerNodeControlBlock *block_of_childs, hovered_child_blocks) {
                        if (block_of_childs->mapRectToScene(block_of_childs->boundingRect()).contains(node_rec) || 
                            block_of_childs->parentNode()->mapRectToScene(block_of_childs->parentNode()->boundingRect()).contains(node_rec)) {
                            node_is_shared = false;
                            break;
                        }
                    }
                                
                    if (node_is_shared)
                        blocks << block;

                }
            }
        }
    }

    if (blocks.count() == 1 && !blocks.first()->mapRectToScene(blocks.first()->boundingRect()).contains(node_rec))
        blocks << blocks.first();
    
    return blocks;
}

QList<dtkComposerNodeControlBlock *> dtkComposerScene::hoveredControlBlocks(dtkComposerNode *node, QList<dtkComposerNode *> parents)
{
    QList<dtkComposerNodeControlBlock *> blocks;
    QList<dtkComposerNodeControlBlock *> hovered_child_blocks;
    QRectF node_rec = node->mapRectToScene(node->boundingRect());

    foreach(dtkComposerNode *parent_node, parents) {

        if (dtkComposerNodeControl *parent = dynamic_cast<dtkComposerNodeControl *>(parent_node)) {

            foreach(dtkComposerNodeControlBlock *block, parent->blocks()) {
                    
                if (!d->isChildOf(block, node)) {
                        
                    hovered_child_blocks = this->hoveredControlBlocks(node, block->nodes());
                        
                    if (block->mapRectToScene(block->boundingRect()).contains(node_rec) && hovered_child_blocks.isEmpty()) {
                            
                        blocks.clear();
                        blocks << block;
                        return blocks;
                            
                    } else if (hovered_child_blocks.isEmpty()) {
                            
                        blocks << block;
                            
                    } else {
                            
                        bool node_is_shared = true;
                            
                        foreach(dtkComposerNodeControlBlock *block_of_childs, hovered_child_blocks) {
                            if (block_of_childs->mapRectToScene(block_of_childs->boundingRect()).contains(node_rec) || 
                                block_of_childs->parentNode()->mapRectToScene(block_of_childs->parentNode()->boundingRect()).contains(node_rec)) {
                                node_is_shared = false;
                                break;
                            }
                        }
                            
                        if (node_is_shared)
                            blocks << block;
                            
                    }
                }
            }
        }
    }
    
    return blocks;
}

void dtkComposerScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
    else
        event->ignore();
}

void dtkComposerScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->accept();
}

void dtkComposerScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
    else
        event->ignore();
}

void dtkComposerScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    QUrl url = event->mimeData()->urls().first();

    if (url.scheme() == "note") {
        this->createNote(url.path(), event->scenePos());
        event->acceptProposedAction();
        return;
    }

    if (url.scheme() != "type") {
        event->ignore();
        return;
    }

    this->createNode(url.path(), event->scenePos());

    event->acceptProposedAction();
}

void dtkComposerScene::keyPressEvent(QKeyEvent *event)
{
    // Item deletion - Delete | Backspace

    if ((event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete) && (event->modifiers() & Qt::ControlModifier))
        foreach(QGraphicsItem *item, this->selectedItems())
            if (dtkComposerNode *node = dynamic_cast<dtkComposerNode *>(item))
                this->removeNode(node);

    // Item Copy - Ctrl + C

    if (event->key() == Qt::Key_C && event->modifiers() & Qt::ControlModifier) {
        this->copy();
    }

    // Item Copy - Ctrl + V

    if (event->key() == Qt::Key_V && event->modifiers() & Qt::ControlModifier) {
        this->paste();
    }

    QGraphicsScene::keyPressEvent(event);
}

void dtkComposerScene::keyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void dtkComposerScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);

    if (d->current_edge)
        d->current_edge->adjust(d->current_edge->start(), mouseEvent->scenePos());

    this->updateEdgesVisibility();

    // -- Control nodes handling

    d->grabber_node = NULL;
    d->grabber_node = dynamic_cast<dtkComposerNode *>(this->mouseGrabberItem());

    if (!d->grabber_node || d->grabber_node->isGhost())
        return;

    QList<dtkComposerNodeControlBlock *> hovered_control_blocks = this->hoveredControlBlocks(d->grabber_node);

    if (hovered_control_blocks.count() > 1) {

        foreach(dtkComposerNodeControlBlock *block, hovered_control_blocks)
            block->highlight(false);

        d->grabber_node->highlight(false);

    } else if (hovered_control_blocks.count() == 1) {

        if (d->grabber_node_has_edges && d->grabber_node->parentItem() != hovered_control_blocks.first()) {

            hovered_control_blocks.first()->highlight(false);
            d->grabber_node->highlight(false);

        } else {

            hovered_control_blocks.first()->highlight(true);
            d->grabber_node->highlight(true);

        }

    } else {

        if (d->grabber_node_has_edges && d->grabber_node->parentItem() != d->current_node) {

            d->grabber_node->highlight(false);

        } else {

            d->grabber_node->highlight(true);
            
        }
    }
}

void dtkComposerScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mousePressEvent(mouseEvent);

    // --- Pulling edge from property

    dtkComposerNodeProperty *property = propertyAt(mouseEvent->scenePos());

     if (property && property->node() == dynamic_cast<dtkComposerNode *>(this->mouseGrabberItem())) {

        if (!property->node()->isGhost() && property->type() == dtkComposerNodeProperty::Output) {

            if (d->current_edge) {
                d->current_edge->hide();
                delete d->current_edge;
                d->current_edge = NULL;
            }
            d->current_edge = new dtkComposerEdge;
            this->addItem(d->current_edge);
            d->current_edge->setSource(property);
            d->current_edge->show();

        } else if (!property->node()->isGhost() && property->type() == dtkComposerNodeProperty::Input) {

            d->current_edge = property->edge();
            if (d->current_edge) {
                d->current_edge->unlink();
                d->edges.removeAll(d->edge(d->current_edge));
            }

        } else if (!property->node()->isGhost() && (property->type() == dtkComposerNodeProperty::HybridOutput || property->type() == dtkComposerNodeProperty::PassThroughOutput)) {
        
            if (property->contains(mouseEvent->pos())) {
        
                property->setBehavior(dtkComposerNodeProperty::AsOutput);
                if (d->current_edge) {
                    d->current_edge->hide();
                    delete d->current_edge;
                    d->current_edge = NULL;
                }
                d->current_edge = new dtkComposerEdge;
                this->addItem(d->current_edge);
                d->current_edge->setSource(property);
                d->current_edge->show();

            } else {
            
                property->setBehavior(dtkComposerNodeProperty::AsRelay);
                d->current_edge = property->edge();
                if (d->current_edge) {
                    d->current_edge->unlink();
                    d->edges.removeAll(d->edge(d->current_edge));
                }
            }

        } else if (!property->node()->isGhost() && (property->type() == dtkComposerNodeProperty::HybridInput || property->type() == dtkComposerNodeProperty::PassThroughInput)) {
        
            if (property->contains(mouseEvent->pos())) {

                property->setBehavior(dtkComposerNodeProperty::AsRelay);
                if (d->current_edge) {
                    d->current_edge->hide();
                    delete d->current_edge;
                    d->current_edge = NULL;
                }
                d->current_edge = new dtkComposerEdge;
                this->addItem(d->current_edge);
                d->current_edge->setSource(property);
                d->current_edge->show();

            } else {

                property->setBehavior(dtkComposerNodeProperty::AsInput);
                d->current_edge = property->edge();
                if (d->current_edge) {
                    d->current_edge->unlink();
                    d->edges.removeAll(d->edge(d->current_edge));
                }
            }

        } else if (property->node()->isGhost() && property->type() == dtkComposerNodeProperty::Input) {

            if (d->current_edge) {
                d->current_edge->hide();
                delete d->current_edge;
                d->current_edge = NULL;
            }
            d->current_edge = new dtkComposerEdge;
            this->addItem(d->current_edge);
            d->current_edge->setSource(property);
            d->current_edge->show();

        } else if (property->node()->isGhost() && property->type() == dtkComposerNodeProperty::Output) {

            d->current_edge = property->edge();
            if (d->current_edge) {
                d->current_edge->unlink();
                d->edges.removeAll(d->edge(d->current_edge));
            }
        }

        mouseEvent->accept();
        return;

    }
    
    // --- Node grabbing

    if (d->grabber_node = dynamic_cast<dtkComposerNode *>(this->mouseGrabberItem())) {

        d->grabber_node_origin = d->grabber_node->scenePos();
        d->grabber_node_zvalue = d->grabber_node->zValue();

        if (d->grabber_node->isGhost())
            return;

        // -- Rise z-value of the node   

        d->grabber_node->setZValue(100);

        // -- Search for external edges
        d->grabber_node_has_edges = false;

        QRectF node_rec = d->grabber_node->mapRectToScene(d->grabber_node->boundingRect());

        foreach(QGraphicsItem *item, this->items(node_rec)) {
            if (dtkComposerEdge *edge = dynamic_cast<dtkComposerEdge *>(item)) {
                if (d->isChildOf(edge->source()->node(), d->grabber_node) && d->isChildOf(edge->destination()->node(), d->grabber_node)) {

                } else if (d->isChildOf(edge->source()->node(), d->grabber_node) || d->isChildOf(edge->destination()->node(), d->grabber_node)) {
                    d->grabber_node_has_edges = true;
                }
            }
        }
        
        mouseEvent->accept();
        return;
    }
    
}

void dtkComposerScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mouseReleaseEvent(mouseEvent);

    // -- Current edge handling

    if (d->current_edge) {

        if (dtkComposerNodeProperty *property = propertyAt(mouseEvent->scenePos())) {
            
            d->current_edge->setDestination(property);
            
            if (!d->current_edge->link()) {
                
                delete d->current_edge;
                
                d->current_edge = 0;
                
                this->setModified(true);
                
            } else {
                
                dtkComposerEdge *edge = d->current_edge;

                d->edges << edge;
                
                d->current_edge = 0;
            }
            
            this->setModified(true);

        } else {

            delete d->current_edge;

            d->current_edge = 0;
        }        
    }
        
    // -- Control nodes handling

    if (!d->grabber_node || d->grabber_node->isGhost())
        return;

    d->grabber_node->setZValue(d->grabber_node_zvalue);

    QRectF node_rec = d->grabber_node->mapRectToScene(d->grabber_node->boundingRect());
        
    QList<dtkComposerNodeControlBlock *> blocks = this->hoveredControlBlocks(d->grabber_node);

    QPointF w = QPointF(0, 0);

    dtkComposerNodeControlBlock *parent_block = NULL;
        
    if (blocks.count() > 1) {
            
        if (d->grabber_node->parentItem())
            w = d->grabber_node->parentItem()->scenePos();
        d->grabber_node->setPos(d->grabber_node_origin - w);

    } else if (blocks.count() == 1) {

        if (d->grabber_node_has_edges && d->grabber_node->parentItem() != blocks.first()) {

            if (d->grabber_node->parentItem()) 
                w = d->grabber_node->parentItem()->scenePos();
            d->grabber_node->setPos(d->grabber_node_origin - w);

        } else if (d->grabber_node->parentItem() != blocks.first()) {

            if (d->grabber_node->parentNode())
                d->grabber_node->parentNode()->removeChildNode(d->grabber_node);
                
            if(parent_block = dynamic_cast<dtkComposerNodeControlBlock *>(d->grabber_node->parentItem()))
                parent_block->removeNode(d->grabber_node);

            w = blocks.first()->scenePos(); 
            d->grabber_node->setPos(d->grabber_node->scenePos() - w);           
            d->grabber_node->setParentItem(blocks.first());
            d->grabber_node->setParentNode(blocks.first()->parentNode());

            blocks.first()->parentNode()->addChildNode(d->grabber_node);
            blocks.first()->addNode(d->grabber_node);

        }
            
    } else {

        parent_block = dynamic_cast<dtkComposerNodeControlBlock *>(d->grabber_node->parentItem());

        if (parent_block && d->grabber_node_has_edges) {
            
            w = parent_block->scenePos();
            d->grabber_node->setPos(d->grabber_node_origin - w);
                
        } else {

            if (d->grabber_node->parentNode() && d->grabber_node->parentNode() != d->current_node)
                d->grabber_node->parentNode()->removeChildNode(d->grabber_node);

            if (parent_block)
                parent_block->removeNode(d->grabber_node);
                
            if (d->current_node)
                w = d->current_node->scenePos();
            else if (d->grabber_node->parentItem()) 
                w = -1. * d->grabber_node->parentItem()->scenePos();
                
            d->grabber_node->setParentItem(d->current_node); 
            d->grabber_node->setParentNode(d->current_node);
            d->grabber_node->setPos(d->grabber_node->scenePos() - w);
                
        }
    }
        
    d->grabber_node = NULL;

}

void dtkComposerScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{

    dtkComposerNode *node = nodeAt(mouseEvent->scenePos());
    if (!node)
        return; 

    if (!(mouseEvent->buttons() & Qt::LeftButton))
        return;

    if (node->kind() != dtkComposerNode::Composite)
        return;

    // --- Browsing through composite levels

    dtkComposerNode *ghost_parent = NULL;

    this->hideAllNodes();

    if (!node->isGhost()) {

        node->setNonGhostPosition(node->pos());
        
        ghost_parent = node->parentNode();
        while (ghost_parent) {
            if (ghost_parent->isGhost()) {
                ghost_parent->setGhostPosition(ghost_parent->scenePos());
                ghost_parent->setGhost(false);  
                break; 
            } else {
                ghost_parent = ghost_parent->parentNode();
            }
        }

        node->setGhost(true);
        node->setPos(node->ghostPosition());

        this->setCurrentNode(node);

    } else if (node->isGhost()) {  

        node->setGhostPosition(node->scenePos());
        node->setGhost(false);      
        
        ghost_parent = node->parentNode();
        while (ghost_parent) {
            if (ghost_parent->kind() == dtkComposerNode::Composite) {
                ghost_parent->setGhost(true);
                ghost_parent->setPos(ghost_parent->ghostPosition());
                break; 
            } else {
                ghost_parent = ghost_parent->parentNode();
            }
        }

        node->setPos(node->nonGhostPosition());

        this->setCurrentNode(ghost_parent);
    }

    QPointF scene_center = node->mapRectToScene(node->boundingRect()).center();

    if (d->current_node)
        this->showChildNodes(d->current_node);
    else
        this->showAllNodes();

    this->updateEdgesVisibility();
    emit centerOn(scene_center);
    emit pathChanged(d->current_node);
}

void dtkComposerScene::onSelectionChanged(void)
{
    foreach(QGraphicsItem *item, this->selectedItems()) {

        if (dtkComposerNode *node = dynamic_cast<dtkComposerNode *>(item)) {

            emit nodeSelected(node);

            if (dtkAbstractData *data = dynamic_cast<dtkAbstractData *>(node->object()))
                emit dataSelected(data);

            if (dtkAbstractProcess *process = dynamic_cast<dtkAbstractProcess *>(node->object()))
                emit processSelected(process);

            if (dtkAbstractView *view = dynamic_cast<dtkAbstractView *>(node->object()))
                emit viewSelected(view);
        }
    }

    if (this->selectedItems().count() == 0)
        emit selectionCleared();
}

bool dtkComposerScene::s_evaluate = false;
