/* dtkInspectorToolBar.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sun Feb 13 22:48:21 2011 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Apr 20 12:52:02 2011 (+0200)
 *           By: Julien Wintz
 *     Update #: 76
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "dtkInspectorToolBar.h"

#include <QtGui>

class dtkInspectorToolBarPrivate
{
public:
    QSizeGrip *grip;
    QPixmap pixmap;

public:
    QHash<QAction *, QRect> actions;
};

dtkInspectorToolBar::dtkInspectorToolBar(QWidget *parent) : QWidget(parent), d(new dtkInspectorToolBarPrivate)
{
    d->grip = new QSizeGrip(this);
    d->grip->setFixedWidth(16);

    d->pixmap = QPixmap(":dtkGui/pixmaps/dtkInspectorToolBar-bg.png");

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

dtkInspectorToolBar::~dtkInspectorToolBar(void)
{
    delete d;

    d = NULL;
}

QSize dtkInspectorToolBar::sizeHint(void) const
{
    return QSize(100, 32);
}

void dtkInspectorToolBar::addAction(QAction *action)
{
    d->actions.insert(action, QRect());
}

void dtkInspectorToolBar::mouseReleaseEvent(QMouseEvent *event)
{
    foreach(QRect rect, d->actions)
        if(rect.contains(event->pos()))
            d->actions.key(rect)->trigger();

    this->update();
}

void dtkInspectorToolBar::paintEvent(QPaintEvent *event)
{
    int x = 5;
    int y = 5;

    QPainter painter(this);
    painter.drawTiledPixmap(event->rect(), d->pixmap);

    QFont font = painter.font();
#if defined(Q_WS_MAC)
    font.setPointSize(11);
#else
    font.setPointSize(9);
#endif

    QFontMetrics metrics(font);
    painter.setFont(font);

    foreach(QAction *action, d->actions.keys()) {

        painter.setPen(Qt::black);
        painter.drawText(event->rect().left() + x, metrics.height() + 3 + y, action->text());
        if(action->isEnabled())
            painter.setPen("#bbbbbb");
        else
            painter.setPen("#00a2fd");
        painter.drawText(event->rect().left() + x, metrics.height() + 2 + y, action->text());

        d->actions[action] = QRect(x, 0, metrics.width(action->text()), 32);

        x += metrics.width(action->text()) + 10;
    }
}

void dtkInspectorToolBar::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    
    d->grip->move(this->width()-d->grip->width(), this->height()-d->grip->height());
}
