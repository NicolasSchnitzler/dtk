/* dtkAbstractView.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Nov  7 16:01:09 2008 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Jan 27 15:30:50 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 144
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include <QtGui/QWidget>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewAnimator.h>
#include <dtkCore/dtkAbstractViewNavigator.h>
#include <dtkCore/dtkAbstractViewInteractor.h>
#include <dtkCore/dtkLog.h>

class dtkAbstractViewPrivate
{
public:
    QMap<QString, dtkAbstractViewAnimator   *>   animators;
    QMap<QString, dtkAbstractViewNavigator  *>  navigators;
    QMap<QString, dtkAbstractViewInteractor *> interactors;
};

dtkAbstractView::dtkAbstractView(dtkAbstractView *parent) : dtkAbstractObject(parent), d(new dtkAbstractViewPrivate)
{

}

dtkAbstractView::dtkAbstractView(const dtkAbstractView& view) : dtkAbstractObject(), d(new dtkAbstractViewPrivate)
{
    d->animators   = view.d->animators;
    d->navigators  = view.d->navigators;
    d->interactors = view.d->interactors;
}

dtkAbstractView::~dtkAbstractView(void)
{
    delete d;
    
    d = NULL;
}

void dtkAbstractView::setView(void *)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void dtkAbstractView::setData(dtkAbstractData *data)
{
    DTK_DEFAULT_IMPLEMENTATION;

    foreach(dtkAbstractViewAnimator *animator, d->animators)
        if (animator->enabled())
            animator->setData(data);

    foreach(dtkAbstractViewInteractor *interactor, d->interactors)
      if (interactor->enabled())
	interactor->setData(data);
}

void dtkAbstractView::setData(dtkAbstractData *data, int channel)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(data);
    DTK_UNUSED(channel);
}

void *dtkAbstractView::data(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return NULL;
}

void *dtkAbstractView::data(int channel)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return NULL;
}

void dtkAbstractView::updatePosition(double x, double y, double z)
{
    DTK_DEFAULT_IMPLEMENTATION;

    foreach(dtkAbstractViewInteractor *interactor, d->interactors)
      interactor->updatePosition (x, y, z);
}

void dtkAbstractView::showFullScreen(void)
{
    if(QWidget *widget = this->widget())
        widget->showFullScreen();
}

void dtkAbstractView::showMinimized(void)
{
    if(QWidget *widget = this->widget())
        widget->showMinimized();
}

void dtkAbstractView::showMaximized(void)
{
    if(QWidget *widget = this->widget())
        widget->showMaximized();
}

void dtkAbstractView::showNormal(void)
{
    if(QWidget *widget = this->widget())
        widget->showNormal();
}

void dtkAbstractView::addAnimator(dtkAbstractViewAnimator *animator)
{
    d->animators.insert(animator->description(), animator);
}

void dtkAbstractView::addNavigator(dtkAbstractViewNavigator *navigator)
{
    d->navigators.insert(navigator->description(), navigator);
}

void dtkAbstractView::addInteractor(dtkAbstractViewInteractor *interactor)
{
    if(interactor)
        d->interactors.insert(interactor->description(), interactor);
}

void dtkAbstractView::enableAnimator(QString animator)
{
    if (d->animators.contains(animator)) {
        d->animators.value(animator)->setView(this);
        d->animators.value(animator)->enable();
    } else
        dtkDebug() << description() << "has no such animator:" << animator;
}

void dtkAbstractView::disableAnimator(QString animator)
{
    if (d->animators.contains(animator))
        d->animators.value(animator)->disable();
}

void dtkAbstractView::enableNavigator(QString navigator)
{
    if (d->navigators.contains(navigator)) {
//      d->navigators.value(navigator)->setView(this);
        d->navigators.value(navigator)->enable();
    } else
        dtkDebug() << description() << "has no such navigator:" << navigator;
}

void dtkAbstractView::disableNavigator(QString navigator)
{
    if (d->navigators.contains(navigator))
        d->navigators.value(navigator)->disable();
}

void dtkAbstractView::enableInteractor(QString interactor)
{
    if (d->interactors.contains(interactor)) {
        d->interactors.value(interactor)->setView(this);
        d->interactors.value(interactor)->enable();
    } else
        dtkDebug() << description() << "has no such interactor:" << interactor;
}

void dtkAbstractView::disableInteractor(QString interactor)
{
    if (d->interactors.contains(interactor))
        d->interactors.value(interactor)->disable();
}

dtkAbstractViewAnimator *dtkAbstractView::animator(QString type)
{
    if (d->animators.contains(type))
        return d->animators.value(type);

    return NULL;
}

dtkAbstractViewNavigator *dtkAbstractView::navigator(QString type)
{
    if (d->navigators.contains(type))
        return d->navigators.value(type);

    return NULL;
}

dtkAbstractViewInteractor *dtkAbstractView::interactor(QString type)
{
    if (d->interactors.contains(type))
        return d->interactors.value(type);

    return NULL;
}

void dtkAbstractView::start(unsigned int iterations)
{
    foreach(dtkAbstractViewAnimator *animator, d->animators)
        if (animator->enabled())
            animator->start(iterations);
}

void dtkAbstractView::stop(void)
{
    foreach(dtkAbstractViewAnimator *animator, d->animators)
        if (animator->enabled())
            animator->stop();
}

QList<dtkAbstractViewAnimator *> dtkAbstractView::animators(void)
{
    return d->animators.values();
}

QList<dtkAbstractViewNavigator *> dtkAbstractView::navigators(void)
{
    return d->navigators.values();
}

QList<dtkAbstractViewInteractor *> dtkAbstractView::interactors(void)
{
    return d->interactors.values();
}

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

QDebug operator<<(QDebug debug, const dtkAbstractView& viewer)
{
    debug.nospace() << viewer.description();

    return debug.space();
}

QDebug operator<<(QDebug debug, dtkAbstractView *viewer)
{
    debug.nospace() << viewer->description();

    return debug.space();
}
