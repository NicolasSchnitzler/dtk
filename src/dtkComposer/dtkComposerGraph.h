/* dtkComposerGraph.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008-2011 - Julien Wintz, Inria.
 * Created: Thu Feb  9 14:42:13 2012 (+0100)
 * Version: $Id$
 * Last-Updated: mer. févr. 22 08:57:11 2012 (+0100)
 *           By: Nicolas Niclausse
 *     Update #: 52
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKCOMPOSERGRAPH_H
#define DTKCOMPOSERGRAPH_H

#include <QtGui>

class dtkComposerGraphEdge;
class dtkComposerGraphEdgeList;
class dtkComposerGraphNode;
class dtkComposerGraphNodeList;
class dtkComposerGraphPrivate;
class dtkComposerSceneEdge;
class dtkComposerSceneNode;

class dtkComposerGraph : public QGraphicsScene
{
    Q_OBJECT

public:
     dtkComposerGraph(void);
     dtkComposerGraph(const dtkComposerGraph& other);
    ~dtkComposerGraph(void);

public:
    void    addNode(dtkComposerSceneNode *node);
    void removeNode(dtkComposerSceneNode *node);

    void    addEdge(dtkComposerSceneEdge *edge);
    void removeEdge(dtkComposerSceneEdge *edge);

    void createGroup(dtkComposerSceneNode *node);
    void destroyGroup(dtkComposerSceneNode *node);
    void removeGroup(dtkComposerSceneNode *node);

public:
    dtkComposerGraphEdgeList edges(void);
    dtkComposerGraphNodeList nodes(void);

// --

public:
    void removeNode(dtkComposerGraphNode *node);
    void removeEdge(dtkComposerGraphEdge *edge);
// --

public:
    void layout(void);

protected slots:
    void onSelectionChanged(void);

private:
    dtkComposerGraphPrivate *d;
};

#endif
