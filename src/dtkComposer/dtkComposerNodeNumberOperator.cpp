/* dtkComposerNodeNumberOperator.cpp --- 
 * 
 * Author: Thibaud Kloczko
 * Copyright (C) 2011 - Thibaud Kloczko, Inria.
 * Created: Tue Mar  1 10:18:08 2011 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Nov 17 16:17:54 2011 (+0100)
 *           By: Thibaud Kloczko
 *     Update #: 168
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "dtkComposerEdge.h"
#include "dtkComposerNodeNumber.h"
#include "dtkComposerNodeNumberOperator.h"
#include "dtkComposerNodeProperty.h"

#include <dtkCore/dtkGlobal.h>

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorLabel
// /////////////////////////////////////////////////////////////////

class dtkComposerNodeNumberOperatorLabel : public QGraphicsItem
{
public:
     dtkComposerNodeNumberOperatorLabel(dtkComposerNodeNumberOperator *parent);
    ~dtkComposerNodeNumberOperatorLabel(void);

public:
    QRectF boundingRect(void) const;

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

public:
    dtkComposerNodeNumberOperator *parent_node;
    QPainterPath path;
    QString text;
};

dtkComposerNodeNumberOperatorLabel::dtkComposerNodeNumberOperatorLabel(dtkComposerNodeNumberOperator *parent) : QGraphicsItem(parent)
{
    int margin = 10;
    int length = 50;
    int height = 15;
    int radius =  5;
    int origin_x = parent->boundingRect().width() / 2 - length - margin;
    int origin_y = -parent->boundingRect().height() / 2;

    QPainterPath b; b.addRoundedRect(origin_x,              origin_y, margin,          height,     radius, radius);
    QPainterPath c; c.addRoundedRect(origin_x + margin,     origin_y, length - margin, height,     radius, radius);
    QPainterPath d; d.addRoundedRect(origin_x + length,     origin_y, margin,          height,     radius, radius);
    QPainterPath e; e.addRoundedRect(origin_x + margin / 2, origin_y, length,          height / 2,      0,      0);

    path = c.united(e.subtracted(b.united(c.united(d))));

    parent_node = parent;

    text = "+";
}

dtkComposerNodeNumberOperatorLabel::~dtkComposerNodeNumberOperatorLabel(void)
{

}

QRectF dtkComposerNodeNumberOperatorLabel::boundingRect(void) const
{
    return path.boundingRect();
}

void dtkComposerNodeNumberOperatorLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QLinearGradient gradient(option->rect.left(), option->rect.top(), option->rect.left(), option->rect.bottom());
    gradient.setColorAt(0.0, QColor("#bbbbbb"));
    gradient.setColorAt(0.3, QColor("#333333"));
    gradient.setColorAt(1.0, QColor("#222222"));

    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(gradient);
    painter->drawPath(path);
    
#if defined(Q_WS_MAC)
    QFont font("Lucida Grande", 11);
#else
    QFont font("Lucida Grande", 9);
#endif
    font.setBold(true);

    QFontMetrics metrics(font);

    painter->setFont(font);
    painter->setPen(Qt::white);
    painter->drawText(this->boundingRect(), Qt::AlignCenter, text);
}

void dtkComposerNodeNumberOperatorLabel::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    if (text == "+") {

        text = "-";
        parent_node->setOperation(dtkComposerNodeNumberOperator::Substraction);

    } else if (text == "-") {

        text = "x";
        parent_node->setOperation(dtkComposerNodeNumberOperator::Multiplication);

    } else if (text == "x") {

        text = "/";
        parent_node->setOperation(dtkComposerNodeNumberOperator::Division);

    } else if (text == "/") {

        text = "++";
        parent_node->setOperation(dtkComposerNodeNumberOperator::Increment);

    } else if (text == "++") {

        text = "--";
        parent_node->setOperation(dtkComposerNodeNumberOperator::Decrement);

    } else if (text == "--") {

        text = "%";
        parent_node->setOperation(dtkComposerNodeNumberOperator::Modulo);

    } else if (text == "%") {

        text = "MIN";
        parent_node->setOperation(dtkComposerNodeNumberOperator::Min);

    } else if (text == "MIN") {

        text = "MAX";
        parent_node->setOperation(dtkComposerNodeNumberOperator::Max);

    } else if (text == "MAX") {

        text = "+";
        parent_node->setOperation(dtkComposerNodeNumberOperator::Addition);

    }

    this->update();
}

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperatorPrivate declaration
// /////////////////////////////////////////////////////////////////

class dtkComposerNodeNumberOperatorPrivate
{
public:
    dtkComposerNodeProperty *property_input_value_op1;
    dtkComposerNodeProperty *property_input_value_op2;
    dtkComposerNodeProperty *property_output_value;

public:
    dtkComposerNodeNumberOperatorLabel *label;

public:
    dtkComposerNodeNumberOperator::Operation operation;

public:
    QVariant value_op1;
    QVariant value_op2;
    QVariant value;
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeNumberOperator implementation
// /////////////////////////////////////////////////////////////////

//! Constructs number operator node.
/*! 
 *  + operation is set by default.
 */
dtkComposerNodeNumberOperator::dtkComposerNodeNumberOperator(dtkComposerNode *parent) : dtkComposerNode(parent), d(new dtkComposerNodeNumberOperatorPrivate)
{
    d->property_input_value_op1 = new dtkComposerNodeProperty("left operand", dtkComposerNodeProperty::Left, dtkComposerNodeProperty::AsInput, dtkComposerNodeProperty::Multiple, this);
    d->property_input_value_op2 = new dtkComposerNodeProperty("right operand", dtkComposerNodeProperty::Left, dtkComposerNodeProperty::AsInput, dtkComposerNodeProperty::Multiple, this);

    d->property_output_value = new dtkComposerNodeProperty("result", dtkComposerNodeProperty::Right, dtkComposerNodeProperty::AsOutput, dtkComposerNodeProperty::Multiple, this);

    d->label = new dtkComposerNodeNumberOperatorLabel(this);
    d->label->setPos(0, 0);

    d->operation = dtkComposerNodeNumberOperator::Addition;

    this->setTitle("NumberOperator");
    this->setKind(dtkComposerNode::Process);
    this->setType("dtkComposerNumberOperator");

    this->g->appendLeftProperty(d->property_input_value_op1);
    this->g->appendLeftProperty(d->property_input_value_op2);
    this->g->appendRightProperty(d->property_output_value);
}

//! Destroys number operator node.
/*! 
 *  
 */
dtkComposerNodeNumberOperator::~dtkComposerNodeNumberOperator(void)
{
    delete d;

    d = NULL;
}

//! Sets current operator type.
/*! 
 *  Following types are avalaible:
 *
 *  -   + operator
 *  -   - operator
 *  -   x operator
 *  -   / operator
 *  -  ++ operator
 *  -  -- operator
 *  -   % operator
 *  - MIN operator
 *  - MAX operator
 */
void dtkComposerNodeNumberOperator::setOperation(dtkComposerNodeNumberOperator::Operation operation)
{
    d->operation = operation;

    switch(operation) {
    case dtkComposerNodeNumberOperator::Addition:
        d->label->text = "+";
        break;
    case dtkComposerNodeNumberOperator::Substraction:
        d->label->text = "-";
        break;
    case dtkComposerNodeNumberOperator::Multiplication:
        d->label->text = "x";
        break;
    case dtkComposerNodeNumberOperator::Division:
        d->label->text = "/";
        break;
    case dtkComposerNodeNumberOperator::Increment:
        d->label->text = "++";
        break;
    case dtkComposerNodeNumberOperator::Decrement:
        d->label->text = "--";
        break;
    case dtkComposerNodeNumberOperator::Modulo:
        d->label->text = "%";
        break;
    case dtkComposerNodeNumberOperator::Min:
        d->label->text = "MIN";
        break;
    case dtkComposerNodeNumberOperator::Max:
        d->label->text = "MAX";
        break;
    default:
        break;
    }
}

//! Returns current operation type.
/*! 
 *  
 */
dtkComposerNodeNumberOperator::Operation dtkComposerNodeNumberOperator::operation(void)
{
    return d->operation;
}

//! 
/*! 
 *  Reimplemented from dtkComposerNode.
 */
void dtkComposerNodeNumberOperator::pull(dtkComposerEdge *edge, dtkComposerNodeProperty *property)
{
    if(property == d->property_input_value_op1)
        d->value_op1 = edge->source()->node()->value(edge->source());

    if(property == d->property_input_value_op2)
        d->value_op2 = edge->source()->node()->value(edge->source());
}

//! 
/*! 
 *  Reimplemented from dtkComposerNode.
 */
void dtkComposerNodeNumberOperator::run(void)
{
    QVariant a = d->value_op1;
    QVariant b = d->value_op2;
    QVariant r;

    dtkComposerNodeNumber::Genre genre = dtkComposerNodeNumber::genre(a, b);

    if (genre != dtkComposerNodeNumber::Invalid) {

        switch (d->operation) {

        case dtkComposerNodeNumberOperator::Addition:
            switch (genre) {
            case dtkComposerNodeNumber::Int:
                r = qVariantFromValue(a.toInt() + b.toInt());
                break;
            case dtkComposerNodeNumber::UInt:
                r = qVariantFromValue(a.toUInt() + b.toUInt());
                break;
            case dtkComposerNodeNumber::Long:
                r = qVariantFromValue(qVariantValue<long>(a) + qVariantValue<long>(b));
                break;
            case dtkComposerNodeNumber::ULong:
                r = qVariantFromValue(qVariantValue<ulong>(a) + qVariantValue<ulong>(b));
                break;
            case dtkComposerNodeNumber::LongLong:
                r = qVariantFromValue(a.toLongLong() + b.toLongLong());
                break;
            case dtkComposerNodeNumber::ULongLong:
                r = qVariantFromValue(a.toULongLong() + b.toULongLong());
                break;
            case dtkComposerNodeNumber::Float:
                r = qVariantFromValue(qVariantValue<float>(a) + qVariantValue<float>(b));
                break;
            case dtkComposerNodeNumber::Double:
                r = qVariantFromValue(a.toDouble() + b.toDouble());
                break;
            case dtkComposerNodeNumber::Invalid:
            default:
                break;
            }
            break;

        case dtkComposerNodeNumberOperator::Substraction:
            switch (genre) {
            case dtkComposerNodeNumber::Int:
                r = qVariantFromValue(a.toInt() - b.toInt());
                break;
            case dtkComposerNodeNumber::UInt:
                r = qVariantFromValue(a.toUInt() - b.toUInt());
                break;
            case dtkComposerNodeNumber::Long:
                r = qVariantFromValue(qVariantValue<long>(a) - qVariantValue<long>(b));
                break;
            case dtkComposerNodeNumber::ULong:
                r = qVariantFromValue(qVariantValue<ulong>(a) - qVariantValue<ulong>(b));
                break;
            case dtkComposerNodeNumber::LongLong:
                r = qVariantFromValue(a.toLongLong() - b.toLongLong());
                break;
            case dtkComposerNodeNumber::ULongLong:
                r = qVariantFromValue(a.toULongLong() - b.toULongLong());
                break;
            case dtkComposerNodeNumber::Float:
                r = qVariantFromValue(qVariantValue<float>(a) - qVariantValue<float>(b));
                break;
            case dtkComposerNodeNumber::Double:
                r = qVariantFromValue(a.toDouble() - b.toDouble());
                break;
            case dtkComposerNodeNumber::Invalid:
            default:
                break;
            }
            break;

        case dtkComposerNodeNumberOperator::Multiplication:
            switch (genre) {
            case dtkComposerNodeNumber::Int:
                r = qVariantFromValue(a.toInt() * b.toInt());
                break;
            case dtkComposerNodeNumber::UInt:
                r = qVariantFromValue(a.toUInt() * b.toUInt());
                break;
            case dtkComposerNodeNumber::Long:
                r = qVariantFromValue(qVariantValue<long>(a) * qVariantValue<long>(b));
                break;
            case dtkComposerNodeNumber::ULong:
                r = qVariantFromValue(qVariantValue<ulong>(a) * qVariantValue<ulong>(b));
                break;
            case dtkComposerNodeNumber::LongLong:
                r = qVariantFromValue(a.toLongLong() * b.toLongLong());
                break;
            case dtkComposerNodeNumber::ULongLong:
                r = qVariantFromValue(a.toULongLong() * b.toULongLong());
                break;
            case dtkComposerNodeNumber::Float:
                r = qVariantFromValue(qVariantValue<float>(a) * qVariantValue<float>(b));
                break;
            case dtkComposerNodeNumber::Double:
                r = qVariantFromValue(a.toDouble() * b.toDouble());
                break;
            case dtkComposerNodeNumber::Invalid:
            default:
                break;
            }
            break;

        case dtkComposerNodeNumberOperator::Division:
            switch (genre) {
            case dtkComposerNodeNumber::Int:
                r = qVariantFromValue(a.toInt() / b.toInt());
                break;
            case dtkComposerNodeNumber::UInt:
                r = qVariantFromValue(a.toUInt() / b.toUInt());
                break;
            case dtkComposerNodeNumber::Long:
                r = qVariantFromValue(qVariantValue<long>(a) / qVariantValue<long>(b));
                break;
            case dtkComposerNodeNumber::ULong:
                r = qVariantFromValue(qVariantValue<ulong>(a) / qVariantValue<ulong>(b));
                break;
            case dtkComposerNodeNumber::LongLong:
                r = qVariantFromValue(a.toLongLong() / b.toLongLong());
                break;
            case dtkComposerNodeNumber::ULongLong:
                r = qVariantFromValue(a.toULongLong() / b.toULongLong());
                break;
            case dtkComposerNodeNumber::Float:
                r = qVariantFromValue(qVariantValue<float>(a) / qVariantValue<float>(b));
                break;
            case dtkComposerNodeNumber::Double:
                r = qVariantFromValue(a.toDouble() / b.toDouble());
                break;
            case dtkComposerNodeNumber::Invalid:
            default:
                break;
            }
            break;
        case dtkComposerNodeNumberOperator::Increment:
            switch (genre) {
            case dtkComposerNodeNumber::Int:
                r = qVariantFromValue(a.toInt() + 1);
                break;
            case dtkComposerNodeNumber::UInt:
                r = qVariantFromValue(a.toUInt() + 1);
                break;
            case dtkComposerNodeNumber::Long:
                r = qVariantFromValue(qVariantValue<long>(a) + 1);
                break;
            case dtkComposerNodeNumber::ULong:
                r = qVariantFromValue(qVariantValue<ulong>(a) + 1);
                break;
            case dtkComposerNodeNumber::LongLong:
                r = qVariantFromValue(a.toLongLong() + 1);
                break;
            case dtkComposerNodeNumber::ULongLong:
                r = qVariantFromValue(a.toULongLong() + 1);
                break;
            case dtkComposerNodeNumber::Float:
                r = qVariantFromValue(qVariantValue<float>(a) + 1);
                break;
            case dtkComposerNodeNumber::Double:
                r = qVariantFromValue(a.toDouble() + 1);
                break;
            case dtkComposerNodeNumber::Invalid:
            default:
                break;
            }
            break;
        case dtkComposerNodeNumberOperator::Decrement:
            switch (genre) {
            case dtkComposerNodeNumber::Int:
                r = qVariantFromValue(a.toInt() - 1);
                break;
            case dtkComposerNodeNumber::UInt:
                r = qVariantFromValue(a.toUInt() - 1);
                break;
            case dtkComposerNodeNumber::Long:
                r = qVariantFromValue(qVariantValue<long>(a) - 1);
                break;
            case dtkComposerNodeNumber::ULong:
                r = qVariantFromValue(qVariantValue<ulong>(a) - 1);
                break;
            case dtkComposerNodeNumber::LongLong:
                r = qVariantFromValue(a.toLongLong() - 1);
                break;
            case dtkComposerNodeNumber::ULongLong:
                r = qVariantFromValue(a.toULongLong() - 1);
                break;
            case dtkComposerNodeNumber::Float:
                r = qVariantFromValue(qVariantValue<float>(a) - 1);
                break;
            case dtkComposerNodeNumber::Double:
                r = qVariantFromValue(a.toDouble() - 1);
                break;
            case dtkComposerNodeNumber::Invalid:
            default:
                break;
            }
            break;

        case dtkComposerNodeNumberOperator::Modulo:
            switch (genre) {
            case dtkComposerNodeNumber::Int:
                r = qVariantFromValue(a.toInt() % b.toInt());
                break;
            case dtkComposerNodeNumber::UInt:
                r = qVariantFromValue(a.toUInt() % b.toUInt());
                break;
            case dtkComposerNodeNumber::Long:
            case dtkComposerNodeNumber::LongLong:
                r = qVariantFromValue(a.toLongLong() % b.toLongLong());
                break;
            case dtkComposerNodeNumber::ULong:
            case dtkComposerNodeNumber::ULongLong:
                r = qVariantFromValue(a.toULongLong() % b.toULongLong());
                break;
            case dtkComposerNodeNumber::Float:
            case dtkComposerNodeNumber::Double:
                r = qVariantFromValue(a.toLongLong() % b.toLongLong());
                break;
            case dtkComposerNodeNumber::Invalid:
            default:
                break;
            }
            break;

        case dtkComposerNodeNumberOperator::Min:
            switch (genre) {
            case dtkComposerNodeNumber::Int:
                a.toInt() < b.toInt() ? r = a : r = b;
                break;
            case dtkComposerNodeNumber::UInt:
                a.toUInt() < b.toUInt() ? r = a : r = b;
                break;
            case dtkComposerNodeNumber::Long:
            case dtkComposerNodeNumber::LongLong:
                a.toLongLong() < b.toLongLong() ? r = a : r = b;
                break;
            case dtkComposerNodeNumber::ULong:
            case dtkComposerNodeNumber::ULongLong:
                a.toULongLong() < b.toULongLong() ? r = a : r = b;
                break;
            case dtkComposerNodeNumber::Float:
            case dtkComposerNodeNumber::Double:
                a.toDouble() < b.toDouble() ? r = a : r = b;
                break;
            case dtkComposerNodeNumber::Invalid:
            default:
                break;
            }
            break;

        case dtkComposerNodeNumberOperator::Max:
            switch (genre) {
            case dtkComposerNodeNumber::Int:
                a.toInt() > b.toInt() ? r = a : r = b;
                break;
            case dtkComposerNodeNumber::UInt:
                a.toUInt() > b.toUInt() ? r = a : r = b;
                break;
            case dtkComposerNodeNumber::Long:
            case dtkComposerNodeNumber::LongLong:
                a.toLongLong() > b.toLongLong() ? r = a : r = b;
                break;
            case dtkComposerNodeNumber::ULong:
            case dtkComposerNodeNumber::ULongLong:
                a.toULongLong() > b.toULongLong() ? r = a : r = b;
                break;
            case dtkComposerNodeNumber::Float:
            case dtkComposerNodeNumber::Double:
                a.toDouble() > b.toDouble() ? r = a : r = b;
                break;
            case dtkComposerNodeNumber::Invalid:
            default:
                break;
            }
            break;

        default:
            break;
        }
    }

    d->value = r;
}

//! 
/*! 
 *  Reimplemented from dtkComposerNode.
 */
void dtkComposerNodeNumberOperator::push(dtkComposerEdge *edge, dtkComposerNodeProperty *property)
{
    Q_UNUSED(edge);
    Q_UNUSED(property);
}

//! 
/*! 
 *  Reimplemented from dtkComposerNode.
 */
dtkComposerNodeAbstractTransmitter *dtkComposerNodeNumberOperator::emitter(dtkComposerNodeProperty *property)
{
    // if (property == d->property_right_value) {
    //     if (!d->transmitter)
    //         return d->default_emitter;
    //     else
    //         return d->transmitter;
    // }
    
    return NULL;
}

//! Sets the receiver from the emitter of the node at the source of \a
//! route.
/*! 
 *  When the source emitter can be casted into current receiver type,
 *  true is returned. Else it returns false.
 *
 *  It makes also the node non-interactive nad clears the text of
 *  edition aera.
 *
 *  Reimplemented from dtkComposerNode.
 */
bool dtkComposerNodeNumberOperator::setReceiver(dtkComposerEdge *route, dtkComposerNodeProperty *destination)
{
    Q_UNUSED(destination);

    // d->transmitter = route->source()->node()->emitter(route->source());

    // if (d->transmitter) {

    //     if (!(dynamic_cast<dtkComposerNodeTransmitter<int>        *>(d->transmitter)) &&
    //         !(dynamic_cast<dtkComposerNodeTransmitter<uint>       *>(d->transmitter)) &&
    //         !(dynamic_cast<dtkComposerNodeTransmitter<long>       *>(d->transmitter)) &&
    //         !(dynamic_cast<dtkComposerNodeTransmitter<ulong>      *>(d->transmitter)) &&
    //         !(dynamic_cast<dtkComposerNodeTransmitter<qlonglong>  *>(d->transmitter)) &&
    //         !(dynamic_cast<dtkComposerNodeTransmitter<qulonglong> *>(d->transmitter)) &&
    //         !(dynamic_cast<dtkComposerNodeTransmitter<float>      *>(d->transmitter)) &&
    //         !(dynamic_cast<dtkComposerNodeTransmitter<double>     *>(d->transmitter))) {

    //         qDebug() << DTK_PRETTY_FUNCTION << ": Type from node" << route->source()->node() << "is not handled by node number.";
    //         return false;
            
    //     }

    //     d->interactive = false;
    //     d->editor->setTextInteractionFlags(Qt::NoTextInteraction);
    //     d->editor->setPlainText("");    
        
    //     return true;

    // }

    // qDebug() << DTK_PRETTY_FUNCTION << "Transmitter of source node" << route->source()->node() << "has not been instanciated. Connection failed.";
    return false;
}

//! 
/*! 
 *  Reimplemented from dtkComposerNode.
 */
bool dtkComposerNodeNumberOperator::onRightRouteConnected(dtkComposerEdge *route, dtkComposerNodeProperty *property)
{
    return true;
}
