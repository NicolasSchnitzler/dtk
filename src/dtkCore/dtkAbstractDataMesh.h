/* dtkAbstractDataMesh.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Feb  2 09:08:09 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Jul 31 23:22:54 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 23
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKABSTRACTMESHDATA_H
#define DTKABSTRACTMESHDATA_H

#include <dtkCore/dtkAbstractData.h>

class dtkAbstractDataMeshPrivate;

class DTKCORE_EXPORT dtkAbstractDataMesh : public dtkAbstractData
{
    Q_OBJECT

public:
             dtkAbstractDataMesh(void);
             dtkAbstractDataMesh(const  dtkAbstractDataMesh& other);
    virtual ~dtkAbstractDataMesh(void);

    void *mesh(void);

private:
    dtkAbstractDataMeshPrivate *d;
};

#endif
