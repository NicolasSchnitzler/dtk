/* dtkComposerNodeOperator.h --- 
 * 
 * Author: David Rey
 * Copyright (C) 2008-2011 - David Rey, Inria.
 * Created: Mon Feb 27 14:19:28 2012 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Feb 27 16:15:09 2012 (+0100)
 *           By: David Rey
 *     Update #: 34
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKCOMPOSERNODENUMBEROPERATOR_H
#define DTKCOMPOSERNODENUMBEROPERATOR_H

#include "dtkComposerExport.h"
#include "dtkComposerNodeLeaf.h"

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary
// /////////////////////////////////////////////////////////////////

class dtkComposerNodeNumberOperatorUnaryPrivate;

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnary : public dtkComposerNodeLeaf
{
public:
     dtkComposerNodeNumberOperatorUnary(void);
    ~dtkComposerNodeNumberOperatorUnary(void);

    inline QString labelHint(int) {
        return "value";
    }

protected:
    dtkComposerNodeNumberOperatorUnaryPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorBinary
// /////////////////////////////////////////////////////////////////

class dtkComposerNodeNumberOperatorBinaryPrivate;

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorBinary : public dtkComposerNodeLeaf
{
public:
     dtkComposerNodeNumberOperatorBinary(void);
    ~dtkComposerNodeNumberOperatorBinary(void);

    inline QString labelHint(int port) {
        if (port == 0)
            return "lhs";
        else if (port == 1) 
            return "rhs";
        else
            return "value";
    }

protected:
    dtkComposerNodeNumberOperatorBinaryPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberComparator
// /////////////////////////////////////////////////////////////////

class dtkComposerNodeNumberComparatorPrivate;

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberComparator : public dtkComposerNodeLeaf
{
public:
     dtkComposerNodeNumberComparator(void);
    ~dtkComposerNodeNumberComparator(void);

    inline QString labelHint(int port) {
        if (port == 0)
            return "lhs";
        else if (port == 1) 
            return "rhs";
        else
            return "value";
    }

protected:
    dtkComposerNodeNumberComparatorPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - INCR
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnaryIncr : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "incr";
    }

    inline QString titleHint(void) {
        return "Incr";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - DECR
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnaryDecr : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "decr";
    }

    inline QString titleHint(void) {
        return "Decr";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - SQRT
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnarySqrt : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "sqrt";
    }

    inline QString titleHint(void) {
        return "Sqrt";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - SQUARE
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnarySquare : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "square";
    }

    inline QString titleHint(void) {
        return "Square";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - LN
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnaryLn : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "ln";
    }

    inline QString titleHint(void) {
        return "Ln";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - LOG10
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnaryLog10 : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "log10";
    }

    inline QString titleHint(void) {
        return "Log10";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - EXP
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnaryExp : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "exp";
    }

    inline QString titleHint(void) {
        return "Exp";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - COS
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnaryCos : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "cos";
    }

    inline QString titleHint(void) {
        return "Cos";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - SIN
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnarySin : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "sin";
    }

    inline QString titleHint(void) {
        return "Sin";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - TAN
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnaryTan : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "tan";
    }

    inline QString titleHint(void) {
        return "Tan";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - ACOS
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnaryAcos : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "acos";
    }

    inline QString titleHint(void) {
        return "Acos";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - ASIN
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnaryAsin : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "asin";
    }

    inline QString titleHint(void) {
        return "Asin";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - ATAN
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnaryAtan : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "atan";
    }

    inline QString titleHint(void) {
        return "Atan";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - DEG2RAD
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnaryDeg2Rad : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "deg2rad";
    }

    inline QString titleHint(void) {
        return "Deg2Rad";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - RAD2DEG
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnaryRad2Deg : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "rad2deg";
    }

    inline QString titleHint(void) {
        return "Rad2Deg";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - INV
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnaryInv : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "inv";
    }

    inline QString titleHint(void) {
        return "Inv";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - OPP
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnaryOpp : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "opp";
    }

    inline QString titleHint(void) {
        return "Opp";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - CEIL
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnaryCeil : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "ceil";
    }

    inline QString titleHint(void) {
        return "Ceil";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - FLOOR
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnaryFloor : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "floor";
    }

    inline QString titleHint(void) {
        return "Floor";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - ROUND
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnaryRound : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "round";
    }

    inline QString titleHint(void) {
        return "Round";
    }
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorUnary - ROUND
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeNumberOperatorUnaryAbs : public dtkComposerNodeNumberOperatorUnary
{
public:
    void run(void);

public:
    inline QString type(void) {
        return "abs";
    }

    inline QString titleHint(void) {
        return "Abs";
    }
};

#endif
