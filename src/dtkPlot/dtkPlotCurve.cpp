/* dtkPlotCurve.cpp --- 
 *
 * Author: Julien Wintz
 * Copyright (C) 2008-2011 - Julien Wintz, Inria.
 * Created: Tue Jun  7 16:09:17 2011 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Sep 25 16:18:42 2012 (+0200)
 *           By: tkloczko
 *     Update #: 51
 */

/* Commentary: 
 *
 */

/* Change log:
 *
 */

#include "dtkPlotCurve.h"

#include <qwt_plot_curve.h>

// /////////////////////////////////////////////////////////////////
// dtkPlotCurvePrivate
// /////////////////////////////////////////////////////////////////

class dtkPlotCurvePrivate : public QwtPlotCurve
{
public:
    dtkPlotCurvePrivate(const QString& title = QString());

public:
    QVector<QPointF> data;
};

dtkPlotCurvePrivate::dtkPlotCurvePrivate(const QString& title) : QwtPlotCurve(title)
{

}

// /////////////////////////////////////////////////////////////////
// dtkPlotCurve
// /////////////////////////////////////////////////////////////////

dtkPlotCurve::dtkPlotCurve(const QString& title) : QObject(), d(new dtkPlotCurvePrivate(title))
{

}

dtkPlotCurve::dtkPlotCurve(const dtkPlotCurve& other) : QObject(), d(new dtkPlotCurvePrivate(other.d->title().text()))
{
    d->data = other.d->data;
    
    d->setSamples(d->data);
}

dtkPlotCurve::~dtkPlotCurve(void)
{
    delete d;

    d = NULL;
}

dtkPlotCurve& dtkPlotCurve::operator=(const dtkPlotCurve& other)
{
    d->setTitle(other.d->title().text()); 

    d->data = other.d->data;
    
    d->setSamples(d->data);

    return *this;
}

void dtkPlotCurve::append(const QPointF& data)
{
    d->data << data;

    d->setSamples(d->data);

    emit updated();
}

void dtkPlotCurve::setAntialiased(bool antiliased)
{
    d->setRenderHint(QwtPlotItem::RenderAntialiased, antiliased);
}

void dtkPlotCurve::setColor(const QColor& color)
{
    d->setPen(color);
}

QColor dtkPlotCurve::color(void)
{
   return  d->pen().color();
}

void dtkPlotCurve::setData(const QVector<QPointF>& data)
{
    d->data = data;

    d->setSamples(d->data);

    emit updated();
}
