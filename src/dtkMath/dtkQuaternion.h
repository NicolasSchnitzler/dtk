/* dtkQuaternion.h --- 
 * 
 * Author: Thibaud Kloczko
 * Copyright (C) 2008 - Thibaud Kloczko, Inria.
 * Created: Tue Jun  8 15:00:12 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Apr 30 16:59:07 2012 (+0200)
 *           By: tkloczko
 *     Update #: 49
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKQUATERNION_H
#define DTKQUATERNION_H

#include "dtkMathExport.h"
#include "dtkVector.h"
#include "dtkVector3D.h"

template <class T = double> class DTKMATH_EXPORT dtkQuaternion : public dtkVector<T>
{
public:	
     dtkQuaternion(void): dtkVector<T>(4U) {};
     dtkQuaternion(const dtkMatrix<T>& mat): dtkVector<T>(mat) {};
     dtkQuaternion(const dtkVector<T>& vec): dtkVector<T>(vec) {};
     dtkQuaternion(const dtkQuaternion& qtn): dtkVector<T>(qtn) {};
     dtkQuaternion(const dtkVector3D<T>&, const T& = dtkZero<T>());
     dtkQuaternion(const T&, const T&, const T&, const T&);
     dtkQuaternion(const T&);
     dtkQuaternion(const dtkMatrix<T>&, unsigned, unsigned);
     dtkQuaternion(const dtkVector<T>& vec, unsigned irowStart);
     dtkQuaternion(const T& r, const dtkVector3D<T>& vec, const T& angle);
    ~dtkQuaternion(void) {};

    void allocate(void) { dtkVector<T>::allocate(4); };

    void mapInto(const dtkMatrix<T>& mat,
		 unsigned irowStart,
		 unsigned icolStart);
    void mapInto(const dtkVector<T>& vec, unsigned irowStart);

    dtkQuaternion operator +(const dtkQuaternion&) const;
    dtkQuaternion operator -(const dtkQuaternion&) const;
    dtkQuaternion operator -(void) const;
    dtkQuaternion operator *(const T&) const;
    dtkQuaternion operator *(const dtkQuaternion&) const;
    dtkQuaternion operator /(const T& value) const {
        T tTmp = dtkUnity<T>();
        tTmp /= value;
        return (*this)*tTmp;
    }
    dtkQuaternion operator /(const dtkQuaternion&) const;

    dtkQuaternion& operator  =(const dtkQuaternion&);
    dtkQuaternion& operator  =(const dtkVector3D<T>&);
    dtkQuaternion& operator +=(const dtkQuaternion&);
    dtkQuaternion& operator -=(const dtkQuaternion&);
    dtkQuaternion& operator *=(const T&);
    dtkQuaternion& operator *=(const dtkQuaternion&);
    dtkQuaternion& operator /=(const T&);
    dtkQuaternion& operator /=(const dtkQuaternion&);

    void storeProduct(const dtkQuaternion&, const dtkQuaternion&);
    void storeConjugate(const dtkQuaternion&);

    void makeConjugate(void);

    dtkQuaternion unit(void) const { return (*this)/(this->norm()); }
};

template <class T> class dtkUnity< dtkQuaternion<T> >: public dtkQuaternion<T>
{
public:
    dtkUnity(void) {
        (*this)[0] = (*this)[1] = (*this)[2] = dtkZero<T>();
        (*this)[3] = dtkUnity<T>();
    }
};

template <class T> class dtkZero< dtkQuaternion<T> >: public dtkQuaternion<T>
{
public:
    dtkZero(void) { fill(dtkZero<T>()); }
};

// /////////////////////////////////////////////////////////////////
// Implementation of the template class dtkQuaternion's methods
// /////////////////////////////////////////////////////////////////

#include "dtkQuaternion.tpp"

// /////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////

#include <QtCore>

typedef dtkQuaternion<qreal> dtkQuaternionReal;

Q_DECLARE_METATYPE(dtkQuaternionReal);
Q_DECLARE_METATYPE(dtkQuaternionReal *);

#endif

// /////////////////////////////////////////////////////////////////
// Credits
// /////////////////////////////////////////////////////////////////

// $Id: quatern.h 184 2008-05-10 19:43:17Z hkuiper $

// CwMtx matrix and vector math library
// Copyright (C) 1999-2001  Harry Kuiper
// Copyright (C) 2000  Will DeVore (template conversion)
// Copyright (C) 2000  Jiri Ecer (constructor from exponential form)

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA

