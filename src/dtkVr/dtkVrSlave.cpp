/* dtkVrSlave.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 12 10:03:10 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Sun Feb 21 16:41:39 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 228
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "dtkVrScreen.h"
#include "dtkVrSlave.h"

#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>
#include <dtkCore/dtkVec3.h>
#include <dtkCore/dtkQuat.h>

class dtkVrSlavePrivate
{
public:
    dtkVrScreen *screen;

    dtkAbstractView *view;
};

dtkVrSlave::dtkVrSlave(dtkDistributedCommunicator *communicator) : dtkVrProcess(communicator), d(new dtkVrSlavePrivate)
{
    qDebug() << "Creating slave with rank" << this->rank();

    d->screen = new dtkVrScreen(
        dtkVrScreen::screens[this->rank()-1][0],
        dtkVrScreen::screens[this->rank()-1][1],
        dtkVrScreen::screens[this->rank()-1][2]
    );

    d->view = NULL;
}

dtkVrSlave::~dtkVrSlave(void)
{
    delete d->screen;
    delete d;
    
    d = NULL;
}

void dtkVrSlave::initialize(void)
{
    if (d->view)
        d->view->initialize();
}

void dtkVrSlave::uninitialize(void)
{
    if (d->view)
        d->view->uninitialize();
}

void dtkVrSlave::show(void)
{
    if (d->view) {
        d->view->showNormal();
        d->view->widget()->setWindowTitle(d->view->widget()->windowTitle() + " - Rank " + QString::number(this->rank()));
    }
}

void dtkVrSlave::showFullScreen(void)
{
    if (d->view)
        d->view->showFullScreen();
}

void dtkVrSlave::resize(int w, int h)
{
    if (d->view)
        d->view->widget()->resize(w, h);
}

void dtkVrSlave::move(int x, int y)
{
    if (d->view)
        d->view->widget()->move(x, y);
}

void dtkVrSlave::setView(dtkAbstractView *view)
{
    d->view = view;
}

void dtkVrSlave::process(void)
{
    dtkVec3 eye(0, 0, 1.72);
    
    double x0   = (eye - d->screen->lowerLeft()) * d->screen->right();
    double y0   = (eye - d->screen->lowerLeft()) * d->screen->up();
    dtkVec3 center = d->screen->lowerLeft() + d->screen->right() * x0 + d->screen->up() * y0;
    double dist = (eye - center).length();
    double focusDist = 5.0;
    dtkVec3 focusPoint = eye + (center-eye)/dist*focusDist;
    
    this->setupCameraLookAt(eye, focusPoint, d->screen->up());
    
    double near   = 1e-2;
    double far    = 1e2;
    double left   = -x0 * near / dist;
    double bottom = -y0 * near / dist;
    double right  =  (d->screen->width() - x0) * near / dist;
    double top    = (d->screen->height() - y0) * near / dist;
    
    this->setupCameraFrustum(left, right, bottom, top, near, far);
    
    d->view->update();
}

void dtkVrSlave::setupCameraLookAt(const dtkVec3& eye, const dtkVec3& center, const dtkVec3& up)
{
    // qDebug() << rank() << "eye" << eye.x() << eye.y() << eye.z();
    // qDebug() << rank() << "center" << center.x() << center.y() << center.z();
    // qDebug() << rank() << "up" << up.x() << up.y() << up.z();

    if (d->view)
        d->view->setupCameraLookAt(eye, center, up);
}

void dtkVrSlave::setupCameraFrustum(double left, double right, double bottom, double top, double near, double far)
{
    if (d->view)
        d->view->setupCameraFrustum(left, right, bottom, top, near, far);
}
