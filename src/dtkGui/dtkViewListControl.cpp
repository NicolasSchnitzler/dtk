/* dtkViewListControl.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008-2011 - Julien Wintz, Inria.
 * Created: Wed Oct 31 12:51:29 2012 (+0100)
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "dtkViewLayout.h"
#include "dtkViewLayoutItem.h"
#include "dtkViewList.h"
#include "dtkViewListControl.h"

#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

class dtkViewListControlPrivate
{
public:
    dtkViewLayout *layout;
    dtkViewList *list;

public:
    QPushButton *hor;
    QPushButton *ver;
    QPushButton *grd;
    QPushButton *cls;
};

dtkViewListControl::dtkViewListControl(QWidget *parent) : QFrame(parent), d(new dtkViewListControlPrivate)
{
    d->layout = NULL;
    d->list = NULL;

    d->hor = new QPushButton("Horizontal", this);
    d->ver = new QPushButton("Vertical", this);
    d->grd = new QPushButton("Grid", this);
    d->cls = new QPushButton("Close All", this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(d->hor);
    layout->addWidget(d->ver);
    layout->addWidget(d->grd);
    layout->addWidget(d->cls);

    connect(d->hor, SIGNAL(clicked()), this, SLOT(onLayoutHorizontally()));
    connect(d->ver, SIGNAL(clicked()), this, SLOT(onLayoutVertically()));
    connect(d->grd, SIGNAL(clicked()), this, SLOT(onLayoutGrid()));
    connect(d->cls, SIGNAL(clicked()), this, SLOT(onLayoutCloseAll()));
}

dtkViewListControl::~dtkViewListControl(void)
{
    delete d;

    d = NULL;
}

void dtkViewListControl::setLayout(dtkViewLayout *layout)
{
    d->layout = layout;
}

void dtkViewListControl::setList(dtkViewList *list)
{
    d->list = list;
}

void dtkViewListControl::onLayoutHorizontally(void)
{
    if(!d->list)
        return;

    if(!d->list->count())
        return;

    if(!d->layout)
        return;

    d->layout->clear();
    d->layout->setCurrent(d->layout->root());

    int w = d->layout->current()->width();
    int n = d->list->count();
    int s = d->layout->current()->handleWidth();
    int v = (w-(n-1)*s)/n;

    for(int i = 1; i <= n; i++) {

        dtkViewLayoutItem *current = d->layout->current();
        current->setOrientation(Qt::Horizontal);
        current->proxy()->setView(dtkAbstractViewFactory::instance()->view(d->list->item(i-1)->text()));
        if(i != n) {

            QList<int> sizes = QList<int>() << v << current->width()-s-v;
            current->split();
            current->setSizes(sizes);
        }

        d->layout->setCurrent(current->second());
    }
}

void dtkViewListControl::onLayoutVertically(void)
{
    if(!d->list)
        return;

    if(!d->list->count())
        return;

    if(!d->layout)
        return;

    d->layout->clear();
    d->layout->setCurrent(d->layout->root());

    int h = d->layout->current()->height();
    int f = d->layout->current()->footerHeight();
    int n = d->list->count();
    int s = d->layout->current()->handleHeight();
    int v = (h-n*f-(n-1)*s)/n;

    for(int i = 1; i <= n; i++) {

        dtkViewLayoutItem *current = d->layout->current();
        current->setOrientation(Qt::Vertical);
        current->proxy()->setView(dtkAbstractViewFactory::instance()->view(d->list->item(i-1)->text()));
        if(i != n) {

            QList<int> sizes = QList<int>() << v+f << current->height()-s-v-f;
            current->split();
            current->setSizes(sizes);
        }

        d->layout->setCurrent(current->second());
    }
}

void dtkViewListControl::onActorStarted(QString view_name)
{
    dtkAbstractView *view = dtkAbstractViewFactory::instance()->view(view_name);
    if (view &&  !d->layout->current()->proxy()->view())  {
        d->layout->current()->proxy()->setView(view);
    }
}

void dtkViewListControl::onLayoutGrid(void)
{
    if(!d->list)
        return;

    if(!d->list->count())
        return;

    if(!d->layout)
        return;

    int n = d->list->count();
    int i = 0;

    typedef QPair<dtkViewLayoutItem *, Qt::Orientation> item_t;

    d->layout->clear();
    d->layout->setCurrent(d->layout->root());
    d->layout->current()->proxy()->setView(dtkAbstractViewFactory::instance()->view(d->list->item(i)->text()));

    QList<item_t> items; items << qMakePair(d->layout->current(), Qt::Horizontal);

    for(int i = 1; i < n; i++) {

        item_t item = items.takeFirst();

        d->layout->setCurrent(item.first);

        dtkViewLayoutItem *current = item.first;
        current->setOrientation(item.second);
        current->split();
        current->second()->proxy()->setView(dtkAbstractViewFactory::instance()->view(d->list->item(i)->text()));

        items << qMakePair(current->first(), item.second == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal);
        items << qMakePair(current->second(), item.second == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal);
    }
}

void dtkViewListControl::onLayoutCloseAll(void)
{
    if(!d->list)
        return;
    if(!d->list->count())
        return;
    if(!d->layout)
        return;

    d->layout->clear();
    d->layout->setCurrent(d->layout->root());
}
