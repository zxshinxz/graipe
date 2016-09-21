/************************************************************************/
/*                                                                      */
/*               Copyright 2008-2016 by Benjamin Seppke                 */
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

#include "core/parameters/filenameparameter.hxx"

#include <QFileDialog>
#include <QHBoxLayout>

#include <QtDebug>

namespace graipe {

/**
 * Default constructor of the FilenameParameter class with a setting of the
 * most important values directly.
 *
 * \param name          The name (label) of this parameter.
 * \param value         The initial value of this parameter.
 * \param parent        If given (!= NULL), this parameter has a parent and will
 *                      be enabled/disabled, if the parent is a BoolParameter.
 * \param invert_parent If true, the enables/disabled dependency to the parent will be swapped.
 */
FilenameParameter::FilenameParameter(const QString& name, QString value, Parameter* parent, bool invert_parent)
:	Parameter(name, parent, invert_parent),
    m_delegate(NULL),
    m_lneDelegate(NULL),
    m_btnDelegate(NULL),
    m_value(value)
{
}

/**
 * Destructor of the FilenameParameter class.
 */
FilenameParameter::~FilenameParameter()
{
    if(m_delegate != NULL)
    {
        delete m_lneDelegate;
        m_lneDelegate=NULL;
        
        delete m_btnDelegate;
        m_btnDelegate=NULL;
        
        delete m_delegate;
        m_delegate=NULL;
    }
}

/**
 * The (immutable) type name of this parameter class.
 *
 * \return "FilenameParameter".
 */
QString  FilenameParameter::typeName() const
{
    return "FilenameParameter";
}

/**
 * The current value of this parameter in the correct, most special type.
 *
 * \return The value of this parameter (For filename: QString).
 */
const QString& FilenameParameter::value()  const
{
    return m_value;
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void FilenameParameter::setValue(const QString & value)
{
    m_value = value;
    
    if(m_delegate != NULL)
    {
        m_lneDelegate->setText(value);
    }
}

/**
 * The value converted to a QString. Please note, that this can vary from the 
 * serialize() result, which also returns a QString. This is due to the fact,
 * that serialize also may perform encoding of QStrings to avoid special chars
 * inside the QString.
 *
 * \return The value of the parameter converted to an QString.
 */
QString  FilenameParameter::valueText() const
{
    return value();
}

/**
 * Serialization of the parameter's state to a QString. Please note, that this can 
 * vary from the valueText() result, which also returns a QString. This is due to the fact,
 * that serialize also may perform encoding of QStrings to avoid special chars.
 *
 * \return The serialization of the parameter's state.
 */
void FilenameParameter::serialize(QIODevice& out) const
{
    Parameter::serialize(out);
    write_on_device(", " + encode_string(value()), out);
}

/**
 * Deserialization of a parameter's state from a QString.
 *
 * \param str The serialization of this parameter's state.
 * \return True, if the deserialization was successful, else false.
 */
bool FilenameParameter::deserialize(QIODevice& in)
{
    if(!Parameter::deserialize(in))
    {
        return false;
    }
    
    QString content(in.readLine().trimmed());
    setValue(decode_string(content));
    
    return true;
}

/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
bool FilenameParameter::isValid() const
{
    QFile file(m_value);
    
    return file.exists();
}

/**
 * The delegate widget of this parameter. 
 * Each parameter generates such a widget on demand, which refers to the
 * first call of this function. This is needed due to the executability of
 * classes using parameters (like the Algorithm class) in different threads.
 *
 * \return The delegate widget to control the values of this parameter.
 */
QWidget*  FilenameParameter::delegate()
{
    if(m_delegate == NULL)
    {
        m_delegate = new QWidget;
        m_lneDelegate = new QLineEdit(m_delegate);
        m_btnDelegate = new QPushButton("Browse", m_delegate);
        
        QHBoxLayout * buttonsLayout = new QHBoxLayout;
        m_delegate->setLayout(buttonsLayout);
    
        buttonsLayout->addWidget(m_lneDelegate);
        buttonsLayout->addWidget(m_btnDelegate);
    
        initConnections();
    }
    
    return m_delegate;
}

/**
 * This slot is called if the user pressed the file-selection button. It is mainly
 * used to present the file selection dialog to the user.
 */
void FilenameParameter::selectFilename()
{
    if(m_delegate != NULL)
    {
        QString file = QFileDialog::getOpenFileName(m_delegate, name());
    
        if(file.size())
        {
            m_lneDelegate->setText(file);
            updateValue();
        }
    }
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void FilenameParameter::updateValue()
{
    if(m_lneDelegate != NULL)
    {
        m_value = m_lneDelegate->text();
        Parameter::updateValue();
    }
}

/**
 * Initializes the connections (signal<->slot) between the parameter class and
 * the delegate widget. This will be done after the first call of the delegate()
 * function, since the delegate is NULL until then.
 */
void FilenameParameter::initConnections()
{
    connect(m_btnDelegate,  SIGNAL(clicked()),             this, SLOT(selectFilename()));
    connect(m_lneDelegate,  SIGNAL(textChanged(QString)),  this, SLOT(updateValue()));
    Parameter::initConnections();
}

} //end of namespace graipe
