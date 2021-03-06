/************************************************************************/
/*                                                                      */
/*               Copyright 2008-2017 by Benjamin Seppke                 */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/*    This file is part of the GrAphical Image Processing Enviroment.   */
/*    The GRAIPE Website may be found at:                               */
/*        https://github.com/bseppke/graipe                             */
/*    Please direct questions, bug reports, and contributions to        */
/*    the GitHub page and use the methods provided there.               */
/*                                                                      */
/*    Permission is hereby granted, free of charge, to any person       */
/*    obtaining a copy of this software and associated documentation    */
/*    files (the "Software"), to deal in the Software without           */
/*    restriction, including without limitation the rights to use,      */
/*    copy, modify, merge, publish, distribute, sublicense, and/or      */
/*    sell copies of the Software, and to permit persons to whom the    */
/*    Software is furnished to do so, subject to the following          */
/*    conditions:                                                       */
/*                                                                      */
/*    The above copyright notice and this permission notice shall be    */
/*    included in all copies or substantial portions of the             */
/*    Software.                                                         */
/*                                                                      */
/*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND    */
/*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES   */
/*    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND          */
/*    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT       */
/*    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,      */
/*    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      */
/*    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR     */
/*    OTHER DEALINGS IN THE SOFTWARE.                                   */
/*                                                                      */
/************************************************************************/

#include "core/parameterselection.hxx"

#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implementation file for the ParameterSelection class
 * @}
 */

ParameterSelection::ParameterSelection(QWidget *parent, Parameter* param)
:	QDialog(parent),
    m_widget(param->delegate()),
    m_verticalLayout(NULL)
{
    this->setWindowTitle(QString("Selection for: ") + param->name());
    
    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->addWidget(m_widget);
    
    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    QSpacerItem* horizontalSpacer = new QSpacerItem(148, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    
    QPushButton* btnCancel = new QPushButton(this);
    btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
    btnCancel->setText(QString::fromUtf8("Cancel"));
    
    QPushButton* btnOk = new QPushButton(this);
    btnOk->setObjectName(QString::fromUtf8("btnOk"));
    btnOk->setText(QString::fromUtf8("Ok"));
    btnOk->setFocus();
    
    horizontalLayout->addItem(horizontalSpacer);
    horizontalLayout->addWidget(btnCancel);
    horizontalLayout->addWidget(btnOk);
    
    m_verticalLayout->addLayout(horizontalLayout);
    
    connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

ParameterSelection::~ParameterSelection()
{
    m_verticalLayout->removeWidget(m_widget);
}

ModelParameterSelection::ModelParameterSelection(QWidget *parent, Model* model)
:	QDialog(parent),
    m_radNewParameters(NULL),
    m_radCopyParameters(NULL),
    m_chkCloneOtherModel(NULL),
    m_scrParameters(NULL),
    m_otherModel(NULL)
{
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    
    m_radNewParameters = new QRadioButton("Select parameters by hand:", this);
    m_radNewParameters->setChecked(true);
    verticalLayout->addWidget(m_radNewParameters);
    
    m_scrParameters = new QScrollArea(this);
    m_scrParameters->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_scrParameters->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrParameters->setWidget(model->parameters()->delegate());
    m_scrParameters->setWidgetResizable(true);
    
    verticalLayout->addWidget(m_scrParameters);
    
    m_radCopyParameters = new QRadioButton("Copy parameters from other model:", this);
    verticalLayout->addWidget(m_radCopyParameters);
    
    connect(m_radNewParameters, SIGNAL(toggled(bool)), m_scrParameters, SLOT(setEnabled(bool)));
    connect(m_radCopyParameters, SIGNAL(toggled(bool)), m_scrParameters, SLOT(setDisabled(bool)));
    
    m_otherModel = new ModelParameter("Model:", model->typeName(), NULL, false, model->workspace());
    
    QHBoxLayout * layoutOtherModel = new QHBoxLayout(this);
    layoutOtherModel->addWidget(new QLabel(m_otherModel->name()));
    layoutOtherModel->addWidget(m_otherModel->delegate());
    verticalLayout->addLayout(layoutOtherModel);
    connect(m_radCopyParameters, SIGNAL(toggled(bool)), m_otherModel->delegate(), SLOT(setEnabled(bool)));
    
    m_chkCloneOtherModel = new QCheckBox("Completely clone other model?", this);
    verticalLayout->addWidget(m_chkCloneOtherModel);
    connect(m_radCopyParameters, SIGNAL(toggled(bool)), m_chkCloneOtherModel, SLOT(setEnabled(bool)));
    
    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    QSpacerItem* horizontalSpacer = new QSpacerItem(148, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    
    QPushButton* btnCancel = new QPushButton(this);
    btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
    btnCancel->setText(QString::fromUtf8("Cancel"));
    
    QPushButton* btnOk = new QPushButton(this);
    btnOk->setObjectName(QString::fromUtf8("btnOk"));
    btnOk->setText(QString::fromUtf8("Ok"));
    btnOk->setFocus();
    
    horizontalLayout->addItem(horizontalSpacer);
    
    horizontalLayout->addWidget(btnCancel);
    horizontalLayout->addWidget(btnOk);
    
    verticalLayout->addLayout(horizontalLayout);
    
    connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

ModelParameterSelection::~ModelParameterSelection()
{
    m_scrParameters->takeWidget();
}

Model* ModelParameterSelection::useOtherModel()
{
    if (m_radCopyParameters->isChecked())
    {
        return m_otherModel->value();
    }
    else
    {
        return NULL;
    }
}

bool ModelParameterSelection::cloneOtherModel() const
{
    return m_radCopyParameters->isChecked() && m_chkCloneOtherModel->isChecked();
}

AlgorithmParameterSelection::AlgorithmParameterSelection(QWidget *parent, Algorithm* alg)
:   QDialog(parent),
    m_lblResults(NULL),
    m_algorithm(alg)
{
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    verticalLayout->addWidget(alg->parameters()->delegate());
    
    //Add resulting types of the algorithm
    m_lblResults = new QLabel(this);
    verticalLayout->addWidget(m_lblResults);
    
    connect(alg->parameters(), SIGNAL(valueChanged()), this, SLOT(updateResults()));
    updateResults();
    
    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    QSpacerItem* horizontalSpacer = new QSpacerItem(148, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    
    QPushButton* btnCancel = new QPushButton(this);
    btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
    btnCancel->setText(QString::fromUtf8("Cancel"));
    
    QPushButton* btnOk = new QPushButton(this);
    btnOk->setObjectName(QString::fromUtf8("btnOk"));
    btnOk->setText(QString::fromUtf8("Ok"));
    btnOk->setFocus();
    
    horizontalLayout->addItem(horizontalSpacer);
    
    horizontalLayout->addWidget(btnCancel);
    horizontalLayout->addWidget(btnOk);
    
    verticalLayout->addLayout(horizontalLayout);
    
    connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

AlgorithmParameterSelection::~AlgorithmParameterSelection()
{
    m_algorithm->parameters()->delegate()->setParent(NULL);
}

void AlgorithmParameterSelection::updateResults()
{
    if(m_algorithm->results().size() != 0)
    {
        QString res_QString("<b>Algorithm results:</b>\n<ol>");
        
        for(Model* res_model : m_algorithm->results())
        {
            res_QString += "  <li>" + res_model->typeName() + "</li>\n";
        }
        
        res_QString +="</ol>";
        m_lblResults->setText(res_QString);
        m_lblResults->setVisible(true);
    }
    else
    {
        m_lblResults->setText("");
        m_lblResults->setVisible(true);
    }
}

} //end of namespace graipe
