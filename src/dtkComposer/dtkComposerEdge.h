/* dtkComposerEdge.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Sep  7 14:26:53 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Oct 27 16:10:10 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 19
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKCOMPOSEREDGE_H
#define DTKCOMPOSEREDGE_H

#include "dtkComposerExport.h"

#include <QtCore>
#include <QtGui>

class dtkComposerEdgePrivate;
class dtkComposerNodeProperty;

class DTKCOMPOSER_EXPORT dtkComposerEdge : public QObject, public QGraphicsItem
{
    Q_OBJECT

    Q_INTERFACES(QGraphicsItem)

public:
     dtkComposerEdge(void);
    ~dtkComposerEdge(void);

    dtkComposerNodeProperty *source(void);
    dtkComposerNodeProperty *destination(void);

    void setSource(dtkComposerNodeProperty *property);
    void setDestination(dtkComposerNodeProperty *property);

public:
    QRectF boundingRect(void) const;

    QPointF start(void) const;
    QPointF end(void) const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void adjust(void);
    void adjust(const QPointF& start, const QPointF& end);

    bool link(void);
    bool unlink(void);

private:
    dtkComposerEdgePrivate *d;
};

#endif
