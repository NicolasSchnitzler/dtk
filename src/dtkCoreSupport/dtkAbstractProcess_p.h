/* dtkAbstractProcess_p.h --- 
 * 
 * Author: tkloczko
 * Copyright (C) 2011 - Thibaud Kloczko, Inria.
 * Created: Mon Apr 23 16:39:35 2012 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Apr 24 14:46:03 2012 (+0200)
 *           By: tkloczko
 *     Update #: 4
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKABSTRACTPROCESS_P_H
#define DTKABSTRACTPROCESS_P_H

#include "dtkCoreSupportExport.h"
#include "dtkAbstractObject_p.h"

class dtkAbstractProcess;

////////////////////////////////////////////////////
// dtkAbstractProcessPrivate interface
////////////////////////////////////////////////////

class DTKCORESUPPORT_EXPORT dtkAbstractProcessPrivate : public dtkAbstractObjectPrivate
{
public:
    dtkAbstractProcessPrivate(dtkAbstractProcess *q = 0) : dtkAbstractObjectPrivate(q) {}
    dtkAbstractProcessPrivate(const dtkAbstractProcessPrivate& other) : dtkAbstractObjectPrivate(other) {}

public:
    virtual ~dtkAbstractProcessPrivate(void) {}
};

////////////////////////////////////////////////////
// dtkAbstractProcess protected constructors
////////////////////////////////////////////////////

DTK_IMPLEMENT_PRIVATE(dtkAbstractProcess, dtkAbstractObject);

#endif
