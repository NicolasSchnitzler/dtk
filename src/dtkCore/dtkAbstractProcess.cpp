/* dtkAbstractProcess.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Nov  7 16:01:09 2008 (+0100)
 * Version: $Id$
 * Last-Updated: Thu May 28 15:15:06 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 31
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <dtkCore/dtkAbstractProcess.h>

dtkAbstractProcess::dtkAbstractProcess(dtkAbstractProcess *parent) : dtkAbstractObject(parent)
{

}

dtkAbstractProcess::dtkAbstractProcess(const dtkAbstractProcess& process)
{
    Q_UNUSED(process);
}

dtkAbstractProcess::~dtkAbstractProcess(void)
{

}

QDebug operator<<(QDebug debug, const dtkAbstractProcess& process)
{
    debug.nospace() << process.description();

    return debug.space();
}

QDebug operator<<(QDebug debug, dtkAbstractProcess *process)
{
    debug.nospace() << process->description();

    return debug.space();
}

void dtkAbstractProcess::run(void)
{
    this->update();
}

void dtkAbstractProcess::update(void)
{

}

void dtkAbstractProcess::setParameter(double  data)
{
    Q_UNUSED(data);
}

void dtkAbstractProcess::setParameter(double  data, int channel)
{
    Q_UNUSED(data);
    Q_UNUSED(channel);
}

void dtkAbstractProcess::setParameter(double  data, int channel, int frame)
{
    Q_UNUSED(data);
    Q_UNUSED(channel);
    Q_UNUSED(frame);
}

void dtkAbstractProcess::setParameter(double *data, int count)
{
    Q_UNUSED(data);
    Q_UNUSED(count);
}

void dtkAbstractProcess::setParameter(double *data, int count, int channel)
{
    Q_UNUSED(data);
    Q_UNUSED(count);
    Q_UNUSED(channel);
}

void dtkAbstractProcess::setParameter(double *data, int count, int channel, int frame)
{
    Q_UNUSED(data);
    Q_UNUSED(count);
    Q_UNUSED(channel);
    Q_UNUSED(frame);
}

void dtkAbstractProcess::setParameter(dtkAbstractObject*  data)
{
    Q_UNUSED(data);
}

void dtkAbstractProcess::setParameter(dtkAbstractObject*  data, int channel)
{
    Q_UNUSED(data);
    Q_UNUSED(channel);
}

void dtkAbstractProcess::setParameter(dtkAbstractObject*  data, int channel, int frame)
{
    Q_UNUSED(data);
    Q_UNUSED(channel);
    Q_UNUSED(frame);
}

void dtkAbstractProcess::setInput(dtkAbstractData *data)
{
    Q_UNUSED(data);
}

void dtkAbstractProcess::setInput(dtkAbstractData *data, int channel)
{
    Q_UNUSED(data);
    Q_UNUSED(channel);
}

void dtkAbstractProcess::setInput(dtkAbstractData *data, int channel, int frame)
{
    Q_UNUSED(data);
    Q_UNUSED(channel);
    Q_UNUSED(frame);
}

void dtkAbstractProcess::setData(void *data)
{
    Q_UNUSED(data);
}

void dtkAbstractProcess::setData(void *data, int channel)
{
    Q_UNUSED(data);
    Q_UNUSED(channel);
}

void dtkAbstractProcess::setData(void *data, int channel, int frame)
{
    Q_UNUSED(data);
    Q_UNUSED(channel);
    Q_UNUSED(frame);
}

dtkAbstractData *dtkAbstractProcess::output(void)
{
    return NULL;
}

dtkAbstractData *dtkAbstractProcess::output(int channel)
{
    Q_UNUSED (channel);
    return NULL;
}

dtkAbstractData *dtkAbstractProcess::output(int channel, int frame)
{
    Q_UNUSED (channel);
    Q_UNUSED (frame);
    return NULL;
}

void *dtkAbstractProcess::data(void)
{
    return NULL;
}

void *dtkAbstractProcess::data(int channel)
{
    Q_UNUSED (channel);
    return NULL;
}

void *dtkAbstractProcess::data(int channel, int frame)
{
    Q_UNUSED (channel);
    Q_UNUSED (frame);
    return NULL;
}
