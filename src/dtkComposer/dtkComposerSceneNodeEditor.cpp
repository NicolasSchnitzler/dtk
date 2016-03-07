// Version: $Id$
//
//

// Commentary:
//
//

// Change Log:
//
//

// Code:

#include "dtkComposerGraph.h"
#include "dtkComposerScene.h"
#include "dtkComposerSceneNode.h"
#include "dtkComposerSceneNodeComposite.h"
#include "dtkComposerSceneNodeControl.h"
#include "dtkComposerSceneNodeEditor.h"
#include "dtkComposerSceneNodeEditor_p.h"
#include "dtkComposerScenePort.h"
#include "dtkComposerStack.h"
#include "dtkComposerStackCommand.h"

#include "dtkComposerNodeLeaf.h"
#include "dtkComposerNodeObject.h"
#include "dtkComposerNodeMetaData.h"
#include "dtkComposerNodeBoolean.h"
#include "dtkComposerNodeControlCase.h"
#include "dtkComposerNodeControlIf.h"
#include "dtkComposerNodeControlFor.h"
#include "dtkComposerNodeControlForEach.h"
#include "dtkComposerNodeControlMap.h"
#include "dtkComposerNodeFile.h"
#include "dtkComposerNodeInteger.h"

#include "dtkComposerNodeReal.h"
#include "dtkComposerNodeString.h"
#include "dtkComposerViewWidget.h"

#include <dtkConfig>

#if defined(DTK_BUILD_DISTRIBUTED)
#include "dtkComposerNodeRemote.h"
#endif

#include "dtkComposerTransmitterProxyLoop.h"

#include <dtkWidgets/dtkToolBox.h>

// /////////////////////////////////////////////////////////////////
// dtkComposerSceneNodeEditorList
// /////////////////////////////////////////////////////////////////

dtkComposerSceneNodeEditorList::dtkComposerSceneNodeEditorList(QWidget *parent) : QListWidget(parent)
{
    this->setAttribute(Qt::WA_MacShowFocusRect, false);
    this->setFrameShape(QFrame::NoFrame);

    connect(this, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(onItemClicked(QListWidgetItem *)));
    connect(this, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(onItemChanged(QListWidgetItem *)));
}

dtkComposerSceneNodeEditorList::~dtkComposerSceneNodeEditorList(void)
{

}

QSize dtkComposerSceneNodeEditorList::sizeHint(void) const
{
    return QSize(300, 50);
}

void dtkComposerSceneNodeEditorList::addInputPort(dtkComposerScenePort *port)
{
    dtkComposerSceneNodeEditorListItem *item = new dtkComposerSceneNodeEditorListItem(port, this);
    port->isVisible() ? item->setCheckState(Qt::Checked) : item->setCheckState(Qt::Unchecked);

    this->addItem(item);
}

void dtkComposerSceneNodeEditorList::addOutputPort(dtkComposerScenePort *port)
{
    dtkComposerSceneNodeEditorListItem *item = new dtkComposerSceneNodeEditorListItem(port, this);
    port->isVisible() ? item->setCheckState(Qt::Checked) : item->setCheckState(Qt::Unchecked);

    this->addItem(item);
}

void dtkComposerSceneNodeEditorList::onItemClicked(QListWidgetItem *item)
{
    if (dtkComposerSceneNodeEditorListItem *i = dynamic_cast<dtkComposerSceneNodeEditorListItem *>(item)) {

        if (i->checkState() == Qt::Checked)
            i->port()->show();

        if (i->checkState() == Qt::Unchecked)
            i->port()->hide();

        if (!(i->port()->node()->parent()))
            return;

        int loop = i->port()->loop();

        if (!loop)
            return;

        dtkComposerSceneNodeControl *control = dynamic_cast<dtkComposerSceneNodeControl *>(i->port()->node()->parent());

        foreach(dtkComposerSceneNodeComposite *block, control->blocks()) {

            foreach(dtkComposerScenePort *port, block->inputPorts()) {
                if (port->loop() == loop) {
                    if (i->checkState() == Qt::Checked)
                        port->show();
                    if (i->checkState() == Qt::Unchecked)
                        port->hide();
                }
            }

            foreach(dtkComposerScenePort *port, block->outputPorts()) {
                if (port->loop() == loop) {
                    if (i->checkState() == Qt::Checked)
                        port->show();
                    if (i->checkState() == Qt::Unchecked)
                        port->hide();
                }
            }
        }
    }
}

void dtkComposerSceneNodeEditorList::onItemChanged(QListWidgetItem *item)
{
    if (item->text().isEmpty())
        return;

    if (dtkComposerSceneNodeEditorListItem *i = dynamic_cast<dtkComposerSceneNodeEditorListItem *>(item)) {

        i->port()->setLabel(item->text());
        i->port()->update();


        if (!(i->port()->node()->parent()))
            return;

        int loop = i->port()->loop();

        if (!loop)
            return;

        dtkComposerSceneNodeControl *control = dynamic_cast<dtkComposerSceneNodeControl *>(i->port()->node()->parent());

        foreach(dtkComposerSceneNodeComposite *block, control->blocks()) {

            foreach(dtkComposerScenePort *port, block->inputPorts()) {
                if (port->loop() == loop) {
                    port->setLabel(item->text());
                    port->update();
                }
            }

            foreach(dtkComposerScenePort *port, block->outputPorts()) {
                if (port->loop() == loop) {
                    port->setLabel(item->text());
                    port->update();
                }
            }
        }
    }
}

// /////////////////////////////////////////////////////////////////
// dtkComposerSceneNodeEditorListItem
// /////////////////////////////////////////////////////////////////

dtkComposerSceneNodeEditorListItem::dtkComposerSceneNodeEditorListItem(dtkComposerScenePort *port, dtkComposerSceneNodeEditorList *parent) : QListWidgetItem(parent, QListWidgetItem::UserType+1)
{
    this->m_port = port;

    this->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
    this->setText(port->label());
}

dtkComposerSceneNodeEditorListItem::~dtkComposerSceneNodeEditorListItem(void)
{

}

int dtkComposerSceneNodeEditorListItem::type(void) const
{
    return QListWidgetItem::UserType+1;
}

dtkComposerScenePort *dtkComposerSceneNodeEditorListItem::port(void)
{
    return m_port;
}

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

dtkComposerSceneNodeEditor::dtkComposerSceneNodeEditor(QWidget *parent) : QWidget(parent), d(new dtkComposerSceneNodeEditorPrivate)
{
    d->node = NULL;
    d->scene = NULL;

    d->loop_ports = new dtkComposerSceneNodeEditorList(this);
    d->loop_ports->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    d->loop_ports->setVisible(false);

    d->add_loop_port = new QPushButton("+", this);
    d->add_loop_port->setEnabled(false);
    d->add_loop_port->setVisible(false);

    d->rem_loop_port = new QPushButton("-", this);
    d->rem_loop_port->setEnabled(false);
    d->rem_loop_port->setVisible(false);

    d->blocks = new dtkComposerSceneNodeEditorList(this);
    d->blocks->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    d->blocks->setVisible(false);

    d->add_block = new QPushButton("+", this);
    d->add_block->setEnabled(false);
    d->add_block->setVisible(false);

    d->rem_block = new QPushButton("-", this);
    d->rem_block->setEnabled(false);
    d->rem_block->setVisible(false);

    d->input_ports = new dtkComposerSceneNodeEditorList(this);
    d->input_ports->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    d->input_ports->setVisible(false);

    d->add_input_port = new QPushButton("+", this);
    d->add_input_port->setEnabled(false);
    d->add_input_port->setVisible(false);

    d->rem_input_port = new QPushButton("-", this);
    d->rem_input_port->setEnabled(false);
    d->rem_input_port->setVisible(false);

    d->output_ports = new dtkComposerSceneNodeEditorList(this);
    d->output_ports->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    d->output_ports->setVisible(false);

    d->add_output_port = new QPushButton("+", this);
    d->add_output_port->setEnabled(false);
    d->add_output_port->setVisible(false);

    d->rem_output_port = new QPushButton("-", this);
    d->rem_output_port->setEnabled(false);
    d->rem_output_port->setVisible(false);

    d->edit = new QLineEdit(this);
    d->edit->setEnabled(false);

    d->butn_f = new QToolButton(this);
    d->butn_f->setText("Browse");
    d->butn_f->blockSignals(true);
    d->butn_f->setEnabled(false);
    d->butn_f->setVisible(false);

    d->butn_d = new QToolButton(this);
    d->butn_d->setText("Browse");
    d->butn_d->blockSignals(true);
    d->butn_d->setEnabled(false);
    d->butn_d->setVisible(false);

    d->spin_d = new QSpinBox(this);
    d->spin_d->setMinimum(-9999999);
    d->spin_d->setMaximum(+9999999);
    d->spin_d->setSingleStep(1);
    d->spin_d->setEnabled(false);
    d->spin_d->setVisible(false);
    d->spin_d->blockSignals(true);

    d->edit_s = new QLineEdit(this);
    d->edit_s->setEnabled(false);
    d->edit_s->setVisible(false);
    d->edit_s->blockSignals(true);

    // --

    d->selector = new QComboBox(this);
    d->selector->setEnabled(false);
    d->selector->setVisible(false);
    d->selector->blockSignals(true);

    // --

    d->select_implementation = new QComboBox(this);
    d->select_implementation->setEnabled(false);
    d->select_implementation->setVisible(false);
    d->select_implementation->blockSignals(true);

    // --

    d->node_toolbox = new dtkToolBox(this);
    d->node_toolbox->setDisplayMode(dtkToolBox::AllItemExpanded);
    d->node_toolbox->setEnabled(false);
    d->node_toolbox->setVisible(false);
    d->node_toolbox->blockSignals(true);
    d->node_toolbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);


    d->monitoring_box = new QCheckBox("Monitor");
       d->monitoring_box->setEnabled(true);

       // --

       d->current_widget = NULL;

    // --

    QHBoxLayout *l_layout = new QHBoxLayout;
    l_layout->setContentsMargins(0, 0, 0, 0);
    l_layout->addWidget(d->add_loop_port);
    l_layout->addWidget(d->rem_loop_port);

    QHBoxLayout *b_layout = new QHBoxLayout;
    b_layout->setContentsMargins(0, 0, 0, 0);
    b_layout->addWidget(d->add_block);
    b_layout->addWidget(d->rem_block);

    QFrame *l_frame = new QFrame(this);
    l_frame->setLayout(l_layout);
    l_frame->setObjectName("dtkComposerSceneNodeEditorButtons");
    l_frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    QFrame *b_frame = new QFrame(this);
    b_frame->setLayout(b_layout);
    b_frame->setObjectName("dtkComposerSceneNodeEditorButtons");
    b_frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    QHBoxLayout *t_layout = new QHBoxLayout;
    t_layout->setContentsMargins(0, 0, 0, 0);
    t_layout->setSpacing(5);
    t_layout->addWidget(new QLabel("Node", this));
    t_layout->addWidget(d->edit);
    t_layout->addWidget(d->monitoring_box);
    t_layout->addWidget(d->selector);

    QFrame *top = new QFrame(this);
    top->setObjectName("dtkComposerSceneNodeEditorTitle");
    top->setLayout(t_layout);
    top->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    QHBoxLayout *i_layout = new QHBoxLayout;
    i_layout->setContentsMargins(0, 0, 0, 0);
    i_layout->addWidget(d->add_input_port);
    i_layout->addWidget(d->rem_input_port);

    QHBoxLayout *o_layout = new QHBoxLayout;
    o_layout->setContentsMargins(0, 0, 0, 0);
    o_layout->addWidget(d->add_output_port);
    o_layout->addWidget(d->rem_output_port);

    QFrame *i_frame = new QFrame(this);
    i_frame->setLayout(i_layout);
    i_frame->setObjectName("dtkComposerSceneNodeEditorButtons");
    i_frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    QFrame *o_frame = new QFrame(this);
    o_frame->setLayout(o_layout);
    o_frame->setObjectName("dtkComposerSceneNodeEditorButtons");
    o_frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    QVBoxLayout *ip_layout = new QVBoxLayout;
    ip_layout->addWidget(d->input_ports);
    ip_layout->addWidget(i_frame);

    QVBoxLayout *op_layout = new QVBoxLayout;
    op_layout->addWidget(d->output_ports);
    op_layout->addWidget(o_frame);

    QVBoxLayout *lv_layout = new QVBoxLayout;
    lv_layout->addWidget(d->loop_ports);
    lv_layout->addWidget(l_frame);

    QVBoxLayout *bv_layout = new QVBoxLayout;
    bv_layout->addWidget(d->blocks);
    bv_layout->addWidget(b_frame);

    QHBoxLayout *p_layout = new QHBoxLayout;
    p_layout->addLayout(ip_layout);
    p_layout->addLayout(op_layout);

    QHBoxLayout *si_layout = new QHBoxLayout;
    si_layout->setContentsMargins(0, 0, 0, 0);
    si_layout->setSpacing(0);
    si_layout->addWidget(d->select_implementation);

    d->layout_widg_edit = new QHBoxLayout;
    d->layout_widg_edit->setContentsMargins(0, 0, 0, 0);
    d->layout_widg_edit->setSpacing(0);
    d->layout_widg_edit->addWidget(d->spin_d);
    d->layout_widg_edit->addWidget(d->edit_s);
    d->layout_widg_edit->addWidget(d->butn_f);
    d->layout_widg_edit->addWidget(d->butn_d);

    d->layout = new QVBoxLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);
    d->layout->addWidget(top);
    d->layout->addLayout(si_layout);
    d->layout->addWidget(d->node_toolbox);
    d->layout->addLayout(lv_layout);
    d->layout->addLayout(bv_layout);
    d->layout->addLayout(p_layout);
    d->layout->addSpacing(10);
    d->layout->addLayout(d->layout_widg_edit);

    connect(d->add_loop_port, SIGNAL(clicked()), this, SLOT(addLoopPort()));
    connect(d->rem_loop_port, SIGNAL(clicked()), this, SLOT(removeLoopPort()));

    connect(d->add_block, SIGNAL(clicked()), this, SLOT(addBlock()));
    connect(d->rem_block, SIGNAL(clicked()), this, SLOT(removeBlock()));

    connect(d->add_input_port, SIGNAL(clicked()), this, SLOT(addInputPort()));
    connect(d->rem_input_port, SIGNAL(clicked()), this, SLOT(removeInputPort()));

    connect(d->add_output_port, SIGNAL(clicked()), this, SLOT(addOutputPort()));
    connect(d->rem_output_port, SIGNAL(clicked()), this, SLOT(removeOutputPort()));

    connect(d->edit, SIGNAL(textChanged(const QString&)), this, SLOT(onTitleChanged(const QString&)));

    connect(d->selector, SIGNAL(currentIndexChanged(int)), this, SLOT(onBlockChanged(int)));

    connect(d->spin_d, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
    connect(d->edit_s, SIGNAL(textChanged(const QString&)), this, SLOT(onValueChanged(const QString &)));
    connect(d->butn_f, SIGNAL(clicked()), this, SLOT(onBrowse()));
    connect(d->butn_d, SIGNAL(clicked()), this, SLOT(onBrowseDirectory()));
    connect(d->monitoring_box, SIGNAL(toggled(bool)), this, SLOT(onMonitoringChanged(bool)));

    connect(d->select_implementation, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onImplementationChanged(const QString&)));
}


void dtkComposerSceneNodeEditor::onMonitoringChanged(bool checked)
{
    if (d->node) {
        d->node->wrapee()->setMonitorable(checked);
        d->node->update();
    }
}

dtkComposerSceneNodeEditor::~dtkComposerSceneNodeEditor(void)
{
    delete d;

    d = NULL;
}

void dtkComposerSceneNodeEditor::setNode(dtkComposerSceneNode *node)
{
    this->clear();

    d->node = node;

    d->input_ports->setVisible(true);
    d->output_ports->setVisible(true);

    if (dynamic_cast<dtkComposerSceneNodeComposite *>(node)) {

        foreach(dtkComposerScenePort *port, node->inputPorts())
            d->input_ports->addInputPort(port);

        foreach(dtkComposerScenePort *port, node->outputPorts())
            d->output_ports->addOutputPort(port);

        d->add_input_port->setEnabled(true);
        d->add_input_port->setVisible(true);
        d->rem_input_port->setEnabled(true);
        d->rem_input_port->setVisible(true);

        d->add_output_port->setEnabled(true);
        d->add_output_port->setVisible(true);
        d->rem_output_port->setEnabled(true);
        d->rem_output_port->setVisible(true);

    } else if (dtkComposerSceneNodeControl *c = dynamic_cast<dtkComposerSceneNodeControl *>(node)) {

        QList<int> loop_ports;

        foreach(dtkComposerSceneNodeComposite *block, c->blocks()) {

            foreach(dtkComposerScenePort *port, block->inputPorts()) {
                if (port->loop() && !loop_ports.contains(port->loop())) {
                    d->loop_ports->addInputPort(port);
                    loop_ports << port->loop();
                } else {
                    d->input_ports->addInputPort(port);
                }
            }

            foreach(dtkComposerScenePort *port, block->outputPorts()) {
                if (port->loop() && !loop_ports.contains(port->loop())) {
                    d->loop_ports->addOutputPort(port);
                    loop_ports << port->loop();
                } else {
                    d->output_ports->addOutputPort(port);
                }
            }
        }

        if (!dynamic_cast<dtkComposerNodeControlIf *>(node->wrapee()) && !dynamic_cast<dtkComposerNodeControlCase *>(node->wrapee())) {
            d->loop_ports->setVisible(true);

            d->add_loop_port->setEnabled(true);
            d->add_loop_port->setVisible(true);
            d->rem_loop_port->setEnabled(true);
            d->rem_loop_port->setVisible(true);
        }

        if (dynamic_cast<dtkComposerNodeControlCase *>(node->wrapee())) {
            d->blocks->setVisible(true);

            d->add_block->setEnabled(true);
            d->add_block->setVisible(true);
            d->rem_block->setEnabled(true);
            d->rem_block->setVisible(true);
        }

        d->add_input_port->setEnabled(true);
        d->add_input_port->setVisible(true);
        d->rem_input_port->setEnabled(true);
        d->rem_input_port->setVisible(true);

        d->add_output_port->setEnabled(true);
        d->add_output_port->setVisible(true);
        d->rem_output_port->setEnabled(true);
        d->rem_output_port->setVisible(true);

        foreach(dtkComposerSceneNodeComposite *block, c->blocks())
            d->selector->addItem(block->title());

        d->selector->blockSignals(false);
        d->selector->setVisible(true);
        d->selector->setEnabled(true);
        d->selector->setCurrentIndex(0);

    } else {

        foreach(dtkComposerScenePort *port, node->inputPorts())
            d->input_ports->addInputPort(port);

        foreach(dtkComposerScenePort *port, node->outputPorts())
            d->output_ports->addOutputPort(port);

        if (dtkComposerNodeBoolean *b_node = dynamic_cast<dtkComposerNodeBoolean *>(node->wrapee())) {

            // if (b_node->value()) {
            //     d->t_b->setChecked(true);
            //     d->f_b->setChecked(false);
            // } else {
            //     d->t_b->setChecked(false);
            //     d->f_b->setChecked(true);
            // }

            // d->t_b->blockSignals(false);
            // d->bool_widget->setVisible(true);
            // d->bool_widget->setEnabled(true);

        } else if (dtkComposerNodeInteger *i_node = dynamic_cast<dtkComposerNodeInteger *>(node->wrapee())) {

            d->spin_d->blockSignals(false);
            d->spin_d->setVisible(true);
            d->spin_d->setEnabled(true);
            d->spin_d->setValue(i_node->value());

        } else if (dtkComposerNodeReal *r_node = dynamic_cast<dtkComposerNodeReal *>(node->wrapee())) {

            // d->spin_f->blockSignals(false);
            // d->spin_f->setVisible(true);
            // d->spin_f->setEnabled(true);
            // d->spin_f->setValue(r_node->value());

        } else if (dtkComposerNodeString *s_node = dynamic_cast<dtkComposerNodeString *>(node->wrapee())) {

            d->edit_s->blockSignals(false);
            d->edit_s->setVisible(true);
            d->edit_s->setEnabled(true);
            d->edit_s->setText(s_node->value());
            d->edit_s->setMaximumWidth(this->sizeHint().width());

        } else if (dtkComposerNodeFile *f_node = dynamic_cast<dtkComposerNodeFile *>(node->wrapee())) {

            d->butn_f->blockSignals(false);
            d->butn_f->setEnabled(true);
            d->butn_f->setVisible(true);

            d->edit_s->blockSignals(false);
            d->edit_s->setVisible(true);
            d->edit_s->setEnabled(true);
            d->edit_s->setText(f_node->value());

            d->edit_s->setMaximumWidth(this->sizeHint().width() - d->butn_f->sizeHint().width());

        } else if (dtkComposerNodeDirectory *d_node = dynamic_cast<dtkComposerNodeDirectory *>(node->wrapee())) {

            d->butn_d->blockSignals(false);
            d->butn_d->setEnabled(true);
            d->butn_d->setVisible(true);

            d->edit_s->blockSignals(false);
            d->edit_s->setVisible(true);
            d->edit_s->setEnabled(true);
            d->edit_s->setText(d_node->value());

            d->edit_s->setMaximumWidth(this->sizeHint().width() - d->butn_d->sizeHint().width());

        } else {

            QObject *object = NULL;
            dtkComposerNodeLeafObject *object_node = dynamic_cast<dtkComposerNodeLeafObject *>(node->wrapee());

            if (object_node) {

                object = object_node->variant().value<QObject*>();

            // if (dtkComposerNodeLeafData *data_node = dynamic_cast<dtkComposerNodeLeafData *>(node->wrapee())) {

            //     if (data_node->isAbstractData()) {

                int current_index = 0;
                d->select_implementation->addItem("Choose implementation");

                for(int i = 0; i < object_node->implementations().count(); ++i) {
                    if (object_node->implementations().at(i) == object_node->currentImplementation())
                        current_index = i+1;
                    d->select_implementation->addItem(object_node->implementations().at(i));
                }

                d->select_implementation->setMaximumWidth(this->size().width() - 10);
                d->select_implementation->setCurrentIndex(current_index);

                d->select_implementation->blockSignals(false);
                d->select_implementation->setVisible(true);
                d->select_implementation->setEnabled(true);

            //         object = reinterpret_cast<QObject *>(data_node->data());

            //     }
            }

            // else if (dtkComposerNodeLeafProcess *process_node = dynamic_cast<dtkComposerNodeLeafProcess *>(node->wrapee())) {

            //     if (process_node->isAbstractProcess()) {

            //         int current_index = 0;
            //         d->select_implementation->addItem("Choose implementation");

            //         for(int i = 0; i < process_node->implementations().count(); ++i) {
            //             if (process_node->implementations().at(i) == process_node->currentImplementation())
            //                 current_index = i+1;
            //             d->select_implementation->addItem(process_node->implementations().at(i));
            //         }

            //        d->select_implementation->setMaximumWidth(this->size().width() - 10);
            //         d->select_implementation->setCurrentIndex(current_index);

            //         d->select_implementation->blockSignals(false);
            //         d->select_implementation->setVisible(true);
            //         d->select_implementation->setEnabled(true);

            //         object = reinterpret_cast<QObject *>(process_node->process());

            //     }
            // }

            // else if (dtkComposerNodeLeafView *view_node = dynamic_cast<dtkComposerNodeLeafView *>(node->wrapee())) {

            //     if (view_node->isAbstractView()) {

            //         int current_index = 0;
            //         d->select_implementation->addItem("Choose implementation");

            //         for(int i = 0; i < view_node->implementations().count(); ++i) {
            //             if (view_node->implementations().at(i) == view_node->currentImplementation())
            //                 current_index = i+1;
            //             d->select_implementation->addItem(view_node->implementations().at(i));
            //         }

            //         d->select_implementation->setMaximumWidth(this->size().width() - 10);
            //         d->select_implementation->setCurrentIndex(current_index);

            //         d->select_implementation->blockSignals(false);
            //         d->select_implementation->setVisible(true);
            //         d->select_implementation->setEnabled(true);

            //         object = reinterpret_cast<QObject *>(view_node->view());

            //     }
            // }

            if (object) {
                dtkToolBoxItem *item = dtkToolBoxItem::fromObject(object, 0);
                if (item) {
                    item->setContentsMargins(0, 0, 0, 0);
                    item->hideButton();
                    qreal height = item->size().height();
                    d->node_toolbox->resize(this->size().width(), height);
                    d->node_toolbox->setMinimumHeight(height);
                    d->node_toolbox->setMaximumHeight(height);
                    d->node_toolbox->addItem(item);
                    d->node_toolbox->blockSignals(false);
                    d->node_toolbox->setVisible(true);
                    d->node_toolbox->setEnabled(true);
                }
            }
        }
    }

    d->edit->setText(d->node->title());
    d->edit->setEnabled(true);
}

void dtkComposerSceneNodeEditor::setScene(dtkComposerScene *scene)
{
    d->scene = scene;

    connect(d->scene, SIGNAL(selectedNode(dtkComposerSceneNode *)), this, SLOT(setNode(dtkComposerSceneNode *)));
    connect(d->scene, SIGNAL(selectionCleared()), this, SLOT(clear()));
}

void dtkComposerSceneNodeEditor::setStack(dtkComposerStack *stack)
{
    d->stack = stack;
}

void dtkComposerSceneNodeEditor::setGraph(dtkComposerGraph *graph)
{
    d->graph = graph;
}

void dtkComposerSceneNodeEditor::clear(void)
{
    d->node = NULL;

    d->loop_ports->clear();
    d->loop_ports->setVisible(false);

    d->add_loop_port->setEnabled(false);
    d->add_loop_port->setVisible(false);
    d->rem_loop_port->setEnabled(false);
    d->rem_loop_port->setVisible(false);

    d->blocks->clear();
    d->blocks->setVisible(false);

    d->add_block->setEnabled(false);
    d->add_block->setVisible(false);
    d->rem_block->setEnabled(false);
    d->rem_block->setVisible(false);

    d->input_ports->clear();
    d->input_ports->setVisible(false);

    d->add_input_port->setEnabled(false);
    d->add_input_port->setVisible(false);
    d->rem_input_port->setEnabled(false);
    d->rem_input_port->setVisible(false);

    d->output_ports->clear();
    d->output_ports->setVisible(false);

    d->add_output_port->setEnabled(false);
    d->add_output_port->setVisible(false);
    d->rem_output_port->setEnabled(false);
    d->rem_output_port->setVisible(false);

    d->edit->clear();
    d->edit->setEnabled(false);

    d->selector->clear();
    d->selector->setEnabled(false);
    d->selector->setVisible(false);

    // d->t_b->blockSignals(true);
    // d->bool_widget->setVisible(false);
    // d->bool_widget->setEnabled(false);

    d->spin_d->blockSignals(true);
    d->spin_d->setVisible(false);
    d->spin_d->setEnabled(false);

    // d->spin_f->blockSignals(true);
    // d->spin_f->setVisible(false);
    // d->spin_f->setEnabled(false);

    d->edit_s->blockSignals(true);
    d->edit_s->setVisible(false);
    d->edit_s->setEnabled(false);

    d->butn_f->blockSignals(true);
    d->butn_f->setVisible(false);
    d->butn_f->setEnabled(false);

    d->butn_d->blockSignals(true);
    d->butn_d->setVisible(false);
    d->butn_d->setEnabled(false);

    d->select_implementation->blockSignals(true);
    d->select_implementation->setVisible(false);
    d->select_implementation->setEnabled(false);
    d->select_implementation->clear();

    d->node_toolbox->blockSignals(true);
    d->node_toolbox->setVisible(false);
    d->node_toolbox->setEnabled(false);
    d->node_toolbox->clear();

}

void dtkComposerSceneNodeEditor::addBlock(void)
{
    dtkComposerSceneNodeControl *control = dynamic_cast<dtkComposerSceneNodeControl *>(d->node);

    if (!control)
        return;

    dtkComposerStackCommandCreateBlock *command;
    command = new dtkComposerStackCommandCreateBlock;
    command->setScene(d->scene);
    command->setGraph(d->graph);
    command->setNode(control);

    d->stack->push(command);

    this->setNode(d->node);
}

void dtkComposerSceneNodeEditor::removeBlock(void)
{
    dtkComposerSceneNodeControl *control = dynamic_cast<dtkComposerSceneNodeControl *>(d->node);

    if (!control)
        return;

    int i = d->selector->currentIndex();

    if (i < 1)
        return;

    dtkComposerStackCommandDestroyBlock *command;
    command = new dtkComposerStackCommandDestroyBlock;
    command->setScene(d->scene);
    command->setGraph(d->graph);
    command->setNode(control->blocks().at(i));
    command->setId(i);

    d->stack->push(command);

    this->setNode(d->node);

    d->selector->setCurrentIndex(i-1);
}

void dtkComposerSceneNodeEditor::addLoopPort(void)
{
    dtkComposerSceneNodeControl *control = dynamic_cast<dtkComposerSceneNodeControl *>(d->node);

    if (!control)
        return;

    dtkComposerStackCommandCreatePort *command_cond_i = NULL;
    dtkComposerStackCommandCreatePort *command_body_i = NULL;
    dtkComposerStackCommandCreatePort *command_body_o = NULL;
    dtkComposerStackCommandCreatePort *command_incr_i = NULL;

    command_body_i = new dtkComposerStackCommandCreatePort;
    command_body_i->setScene(d->scene);
    command_body_i->setNode(control->block("Body"));
    command_body_i->setType(dtkComposerScenePort::Input);
    command_body_i->setKind(dtkComposerTransmitter::ProxyLoop);

    command_body_o = new dtkComposerStackCommandCreatePort;
    command_body_o->setScene(d->scene);
    command_body_o->setNode(control->block("Body"));
    command_body_o->setType(dtkComposerScenePort::Output);
    command_body_o->setKind(dtkComposerTransmitter::ProxyLoop);

    dtkComposerNodeControl *wrapee = static_cast<dtkComposerNodeControl *>(control->wrapee());

    if (dynamic_cast<dtkComposerNodeControlForEach *>(wrapee) || dynamic_cast<dtkComposerNodeControlMap *>(wrapee)) {
        command_cond_i = NULL;
    } else {
        command_cond_i = new dtkComposerStackCommandCreatePort;
        command_cond_i->setScene(d->scene);
        command_cond_i->setNode(control->block("Conditional"));
        command_cond_i->setType(dtkComposerScenePort::Input);
        command_cond_i->setKind(dtkComposerTransmitter::Proxy);
    }

    if (dynamic_cast<dtkComposerNodeControlFor *>(wrapee)) {
        command_incr_i = new dtkComposerStackCommandCreatePort;
        command_incr_i->setScene(d->scene);
        command_incr_i->setNode(control->block("Increment"));
        command_incr_i->setType(dtkComposerScenePort::Input);
        command_incr_i->setKind(dtkComposerTransmitter::Proxy);
    } else {
        command_incr_i = NULL;
    }

// /////////////////////////////////////////////////////////////////
// Push to the stack
// /////////////////////////////////////////////////////////////////

    if (command_cond_i)
        d->stack->push(command_cond_i);

    d->stack->push(command_body_i);
    d->stack->push(command_body_o);

    if (command_incr_i)
        d->stack->push(command_incr_i);

// /////////////////////////////////////////////////////////////////
// Loop them together
// /////////////////////////////////////////////////////////////////

    static QHash<dtkComposerSceneNodeControl *, int> loop_ids;

    if (!loop_ids.contains(control)) {
        int loop_count = 0;
        foreach (dtkComposerScenePort *port, control->block("Body")->inputPorts() )
            loop_count = qMax(port->loop(), loop_count);
        loop_ids[control] = loop_count;
    }

    loop_ids[control] = loop_ids[control] + 1;

    if (command_cond_i)
        command_cond_i->port()->setLoop(loop_ids[control]);

    command_body_i->port()->setLoop(loop_ids[control]);
    command_body_o->port()->setLoop(loop_ids[control]);

    if (command_incr_i)
        command_incr_i->port()->setLoop(loop_ids[control]);

// /////////////////////////////////////////////////////////////////
// Connect proxy and variant transmitters
// /////////////////////////////////////////////////////////////////

    if (command_cond_i) {
        control->block("Body")->wrapee()->receivers().last()->appendNext(control->block("Conditional")->wrapee()->receivers().last());
        control->block("Conditional")->wrapee()->receivers().last()->appendPrevious(control->block("Body")->wrapee()->receivers().last());
    }

    if (command_incr_i) {
        control->block("Body")->wrapee()->emitters().last()->appendNext(control->block("Increment")->wrapee()->receivers().last());
        control->block("Increment")->wrapee()->receivers().last()->appendPrevious(control->block("Body")->wrapee()->emitters().last());
    }

    dtkComposerTransmitterProxyLoop *emitter  = dynamic_cast<dtkComposerTransmitterProxyLoop *>(control->block("Body")->wrapee()->emitters().last());
    dtkComposerTransmitterProxyLoop *receiver = dynamic_cast<dtkComposerTransmitterProxyLoop *>(control->block("Body")->wrapee()->receivers().last());

    emitter->setTwin(receiver);
    receiver->setTwin(emitter);

    wrapee->appendInputTwin(receiver);
    wrapee->appendOutputTwin(emitter);

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

    int index = -1;

    if (d->selector->isVisible())
        index = d->selector->currentIndex();

    this->setNode(d->node);

    if (index > 0)
        d->selector->setCurrentIndex(index);
}

void dtkComposerSceneNodeEditor::removeLoopPort(void)
{
    dtkComposerSceneNodeEditorListItem *item = dynamic_cast<dtkComposerSceneNodeEditorListItem *>(d->loop_ports->currentItem());

    if (!item)
        return;

    int loop = item->port()->loop();

    dtkComposerSceneNodeControl *control = dynamic_cast<dtkComposerSceneNodeControl *>(d->node);

    foreach(dtkComposerSceneNodeComposite *block, control->blocks()) {

        foreach(dtkComposerScenePort *port, block->inputPorts()) {
            if (port->loop() == loop) {
                dtkComposerStackCommandDestroyPort *command = new dtkComposerStackCommandDestroyPort;
                command->setScene(d->scene);
                command->setGraph(d->graph);
                command->setNode(block);
                command->setPort(port);
                d->stack->push(command);
            }
        }

        foreach(dtkComposerScenePort *port, block->outputPorts()) {
            if (port->loop() == loop) {
                dtkComposerStackCommandDestroyPort *command = new dtkComposerStackCommandDestroyPort;
                command->setScene(d->scene);
                command->setGraph(d->graph);
                command->setNode(block);
                command->setPort(port);
                d->stack->push(command);
            }
        }
    }

    int index = -1;

    if (d->selector->isVisible())
        index = d->selector->currentIndex();

    this->setNode(d->node);

    if (index > 0)
        d->selector->setCurrentIndex(index);
}

void dtkComposerSceneNodeEditor::addInputPort(void)
{
    dtkComposerStackCommandCreatePort *command;

    if (dtkComposerSceneNodeControl *control = dynamic_cast<dtkComposerSceneNodeControl *>(d->node)) {

        command = new dtkComposerStackCommandCreatePort;
        command->setScene(d->scene);
        command->setNode(dynamic_cast<dtkComposerSceneNodeComposite *>(control->blocks().at(d->selector->currentIndex())));
        command->setType(dtkComposerScenePort::Input);

    } else {

        command = new dtkComposerStackCommandCreatePort;
        command->setScene(d->scene);
        command->setNode(dynamic_cast<dtkComposerSceneNodeComposite *>(d->node));
        command->setType(dtkComposerScenePort::Input);

#if defined(DTK_BUILD_DISTRIBUTED)
        if (dynamic_cast<dtkComposerNodeRemote *>(d->node->wrapee()))
             command->setKind(dtkComposerTransmitter::ProxyVariant);
#endif
    }

    d->stack->push(command);

    int index = -1;

    if (d->selector->isVisible())
        index = d->selector->currentIndex();

    this->setNode(d->node);

    if (index > 0)
        d->selector->setCurrentIndex(index);
}

void dtkComposerSceneNodeEditor::removeInputPort(void)
{
    dtkComposerSceneNodeEditorListItem *item = dynamic_cast<dtkComposerSceneNodeEditorListItem *>(d->input_ports->currentItem());

    if (!item)
        return;

    dtkComposerStackCommandDestroyPort *command;

    if (dtkComposerSceneNodeControl *control = dynamic_cast<dtkComposerSceneNodeControl *>(d->node)) {

        command = new dtkComposerStackCommandDestroyPort;
        command->setScene(d->scene);
        command->setGraph(d->graph);
        command->setNode(dynamic_cast<dtkComposerSceneNodeComposite *>(control->blocks().at(d->selector->currentIndex())));
        command->setPort(item->port());

    } else {

        command = new dtkComposerStackCommandDestroyPort;
        command->setScene(d->scene);
        command->setGraph(d->graph);
        command->setNode(dynamic_cast<dtkComposerSceneNodeComposite *>(d->node));
        command->setPort(item->port());
    }

    d->stack->push(command);

    int index = -1;

    if (d->selector->isVisible())
        index = d->selector->currentIndex();

    this->setNode(d->node);

    if (index > 0)
        d->selector->setCurrentIndex(index);
}

void dtkComposerSceneNodeEditor::addOutputPort(void)
{
    dtkComposerStackCommandCreatePort *command;

    if (dtkComposerSceneNodeControl *control = dynamic_cast<dtkComposerSceneNodeControl *>(d->node)) {

        command = new dtkComposerStackCommandCreatePort;
        command->setScene(d->scene);
        command->setNode(dynamic_cast<dtkComposerSceneNodeComposite *>(control->blocks().at(d->selector->currentIndex())));
        command->setType(dtkComposerScenePort::Output);

    } else {

        command = new dtkComposerStackCommandCreatePort;
        command->setScene(d->scene);
        command->setNode(dynamic_cast<dtkComposerSceneNodeComposite *>(d->node));
        command->setType(dtkComposerScenePort::Output);

#if defined(DTK_BUILD_DISTRIBUTED)
        if (dynamic_cast<dtkComposerNodeRemote *>(d->node->wrapee()))
            command->setKind(dtkComposerTransmitter::ProxyVariant);
#endif
    }

    d->stack->push(command);

    int index = -1;

    if (d->selector->isVisible())
        index = d->selector->currentIndex();

    this->setNode(d->node);

    if (index > 0)
        d->selector->setCurrentIndex(index);
}

void dtkComposerSceneNodeEditor::removeOutputPort(void)
{
    dtkComposerSceneNodeEditorListItem *item = dynamic_cast<dtkComposerSceneNodeEditorListItem *>(d->output_ports->currentItem());

    if (!item)
        return;

    dtkComposerStackCommandDestroyPort *command;

    if (dtkComposerSceneNodeControl *control = dynamic_cast<dtkComposerSceneNodeControl *>(d->node)) {

        command = new dtkComposerStackCommandDestroyPort;
        command->setScene(d->scene);
        command->setGraph(d->graph);
        command->setNode(dynamic_cast<dtkComposerSceneNodeComposite *>(control->blocks().at(d->selector->currentIndex())));
        command->setPort(item->port());

    } else {

        command = new dtkComposerStackCommandDestroyPort;
        command->setScene(d->scene);
        command->setGraph(d->graph);
        command->setNode(dynamic_cast<dtkComposerSceneNodeComposite *>(d->node));
        command->setPort(item->port());
    }

    d->stack->push(command);

    int index = -1;

    if (d->selector->isVisible())
        index = d->selector->currentIndex();

    this->setNode(d->node);

    if (index > 0)
        d->selector->setCurrentIndex(index);
}

void dtkComposerSceneNodeEditor::onBlockChanged(int index)
{
    if (index < 0)
        return;

    dtkComposerSceneNodeControl *c = dynamic_cast<dtkComposerSceneNodeControl *>(d->node);

    if (!c)
        return;

    d->input_ports->clear();

    foreach(dtkComposerScenePort *port, c->blocks().at(index)->inputPorts())
        if (!port->loop())
            d->input_ports->addInputPort(port);

    d->output_ports->clear();

    foreach(dtkComposerScenePort *port, c->blocks().at(index)->outputPorts())
        if (!port->loop())
            d->output_ports->addOutputPort(port);
}

void dtkComposerSceneNodeEditor::onTitleChanged(const QString& text)
{
    if (d->node) {
        d->node->setTitle(d->edit->text());
        d->node->update();
    }
}

void dtkComposerSceneNodeEditor::onBrowse(void)
{
    QSettings settings("inria", "dtk");
    settings.beginGroup("composer");
    QString path = settings.value("last_open_file_node", QDir::homePath()).toString();
    settings.endGroup();

    QString file = QFileDialog::getOpenFileName(0, "Open composition");

    QFileInfo info(file);

    settings.beginGroup("composer");
    settings.setValue("last_open_file_node", info.absolutePath());
    settings.endGroup();

    d->edit_s->setText(file);
}

void dtkComposerSceneNodeEditor::onBrowseDirectory(void)
{
    QSettings settings("inria", "dtk");
    settings.beginGroup("composer");
    QString path = settings.value("last_open_directory_node", QDir::homePath()).toString();
    settings.endGroup();

    QString dir = QFileDialog::getExistingDirectory(0, "Open directory");

    QDir info(dir);

    settings.beginGroup("composer");
    settings.setValue("last_open_directory_node", info.absolutePath());
    settings.endGroup();

    d->edit_s->setText(dir);
}

void dtkComposerSceneNodeEditor::onValueChanged(bool value)
{
    if (dtkComposerNodeBoolean *b_node = dynamic_cast<dtkComposerNodeBoolean *>(d->node->wrapee()))
        b_node->setValue(value);
}

void dtkComposerSceneNodeEditor::onValueChanged(int value)
{
    if (dtkComposerNodeInteger *i_node = dynamic_cast<dtkComposerNodeInteger *>(d->node->wrapee()))
        i_node->setValue(value);
}

void dtkComposerSceneNodeEditor::onValueChanged(double value)
{
    if (dtkComposerNodeReal *r_node = dynamic_cast<dtkComposerNodeReal *>(d->node->wrapee()))
        r_node->setValue(value);
}

void dtkComposerSceneNodeEditor::onValueChanged(const QString& value)
{
    if (dtkComposerNodeString    *s_node = dynamic_cast<dtkComposerNodeString *>(d->node->wrapee()))
        s_node->setValue(value);

    if (dtkComposerNodeFile      *f_node = dynamic_cast<dtkComposerNodeFile *>(d->node->wrapee()))
        f_node->setValue(value);

    if (dtkComposerNodeDirectory *d_node = dynamic_cast<dtkComposerNodeDirectory *>(d->node->wrapee()))
        d_node->setValue(value);

}

void dtkComposerSceneNodeEditor::onImplementationChanged(const QString& implementation)
{
    d->node_toolbox->clear();

    QObject *object = NULL;

    dtkComposerNodeLeafObject *object_node = dynamic_cast<dtkComposerNodeLeafObject *>(d->node->wrapee());

    if (object_node) {
        object_node->createObject(implementation);
        object = object_node->variant().value<QObject*>();
    }

    // if (dtkComposerNodeLeafData *data_node = dynamic_cast<dtkComposerNodeLeafData *>(d->node->wrapee())) {
    //     data_node->createData(implementation);
    //     object = reinterpret_cast<QObject *>(data_node->data());

    // } else if (dtkComposerNodeLeafProcess *process_node = dynamic_cast<dtkComposerNodeLeafProcess *>(d->node->wrapee())) {
    //     process_node->createProcess(implementation);
    //     object = reinterpret_cast<QObject *>(process_node->process());

    // } else if (dtkComposerNodeLeafView *view_node = dynamic_cast<dtkComposerNodeLeafView *>(d->node->wrapee())) {
    //     view_node->createView(implementation);
    //     object = reinterpret_cast<QObject *>(view_node->view());

    // }

    if (object) {
        dtkToolBoxItem *item = dtkToolBoxItem::fromObject(object, 0);
        item->setContentsMargins(0, 0, 0, 0);
        item->hideButton();
        qreal height = item->size().height();
        d->node_toolbox->resize(this->size().width(), height);
        d->node_toolbox->setMinimumHeight(height);
        d->node_toolbox->setMaximumHeight(height);
        d->node_toolbox->addItem(item);
        d->node_toolbox->blockSignals(false);
        d->node_toolbox->setVisible(true);
        d->node_toolbox->setEnabled(true);
    }

}

//
// dtkComposerSceneNodeEditor.cpp ends here
