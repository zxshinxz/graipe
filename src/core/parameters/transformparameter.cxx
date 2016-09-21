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

#include "core/parameters/transformparameter.hxx"

#include <QtDebug>

namespace graipe {

/**
 * Default constructor of the TransformParameter class with a setting of the
 * most important values directly.
 *
 * \param name          The name (label) of this parameter.
 * \param value         The initial value of this parameter.
 * \param parent        If given (!= NULL), this parameter has a parent and will
 *                      be enabled/disabled, if the parent is a BoolParameter.
 * \param invert_parent If true, the enables/disabled dependency to the parent will be swapped.
 */
TransformParameter::TransformParameter(const QString& name, QTransform value, Parameter* parent, bool invert_parent)
:   Parameter(name, parent, invert_parent),
    m_delegate(NULL),
    m_lne11(NULL), m_lne12(NULL), m_lne13(NULL),
    m_lne21(NULL), m_lne22(NULL), m_lne23(NULL),
    m_lne31(NULL), m_lne32(NULL), m_lne33(NULL),
    m_value(value)
{
}

/**
 * Destructor of the TransformParameter class.
 */
TransformParameter::~TransformParameter()
{
    if(m_delegate != NULL)
    {
        delete m_lne11; m_lne11=NULL;    delete m_lne12; m_lne12=NULL;    delete m_lne13; m_lne13=NULL;
        delete m_lne21; m_lne21=NULL;    delete m_lne22; m_lne22=NULL;    delete m_lne23; m_lne23=NULL;
        delete m_lne31; m_lne31=NULL;    delete m_lne32; m_lne32=NULL;    delete m_lne33; m_lne33=NULL;
        
        delete m_delegate;
        m_delegate=NULL;
    }
}

/**
 * The (immutable) type name of this parameter class.
 *
 * \return "TransformParameter".
 */
QString  TransformParameter::typeName() const
{
    return "TransformParameter";
}

/**
 * The current value of this parameter in the correct, most special type.
 *
 * \return The value of this parameter.
 */
const QTransform& TransformParameter::value() const
{
    return m_value;
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void TransformParameter::setValue(const QTransform& value)
{
    m_value = value;
    
    if(m_delegate != NULL)
    {
        m_lne11->setText(QString::number(m_value.m11())); m_lne12->setText(QString::number(m_value.m12())), m_lne13->setText(QString::number(m_value.m13()));
        m_lne21->setText(QString::number(m_value.m21())); m_lne22->setText(QString::number(m_value.m22())), m_lne23->setText(QString::number(m_value.m23()));
        m_lne31->setText(QString::number(m_value.m31())); m_lne32->setText(QString::number(m_value.m32())), m_lne33->setText(QString::number(m_value.m33()));
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
QString TransformParameter::valueText() const
{
    return valueText(m_value);
}

QString TransformParameter::valueText(const QTransform & trans)
{

    return   QString::number(trans.m11(), 'g', 10) + ", "
           + QString::number(trans.m12(), 'g', 10) + ", "
           + QString::number(trans.m13(), 'g', 10) + ", "
           + QString::number(trans.m21(), 'g', 10) + ", "
           + QString::number(trans.m22(), 'g', 10) + ", "
           + QString::number(trans.m23(), 'g', 10) + ", "
           + QString::number(trans.m31(), 'g', 10) + ", "
           + QString::number(trans.m32(), 'g', 10) + ", "
           + QString::number(trans.m33(), 'g', 10);
}

/**
 * Serialization of the parameter's state to a QString. Please note, that this can 
 * vary from the valueText() result, which also returns a QString. This is due to the fact,
 * that serialize also may perform encoding of QStrings to avoid special chars.
 *
 * \return The serialization of the parameter's state.
 */
void TransformParameter::serialize(QIODevice& out) const
{
    Parameter::serialize(out);
    write_on_device(", "+ valueText(), out);
}

/**
 * Deserialization of a parameter's state from a QString.
 *
 * \param str The serialization of this parameter's state.
 * \return True, if the deserialization was successful, else false.
 */
bool TransformParameter::deserialize(QIODevice& in)
{
    if(!Parameter::deserialize(in))
    {
        return false;
    }
    
    using namespace ::std;

    QTransform trans;
    
    QString content(in.readLine().trimmed());
    
    QStringList values = content.split(", ");
    
    if(values.size() == 9)
    {
        try
        {
            trans.setMatrix(values[0].toDouble(), values[1].toDouble(), values[2].toDouble(),
                            values[3].toDouble(), values[4].toDouble(), values[5].toDouble(),
                            values[6].toDouble(), values[7].toDouble(), values[8].toDouble());
            setValue(trans);
            return true;
        }
        catch(...)
        {
        }
    }
    
    qDebug("TransformParameter deserialize: date could not be imported from file using format 'dd.MM.yyyy hh:mm:ss'");
    return false;
}

/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
bool TransformParameter::isValid() const
{
    return true;
}

/**
 * The delegate widget of this parameter. 
 * Each parameter generates such a widget on demand, which refers to the
 * first call of this function. This is needed due to the executability of
 * classes using parameters (like the Algorithm class) in different threads.
 *
 * \return The delegate widget to control the values of this parameter.
 */
QWidget*  TransformParameter::delegate()
{
    if(m_delegate == NULL)
    {
        m_delegate = new QWidget;
        
        m_lne11 = new QLineEdit(QString::number(m_value.m11()), m_delegate); m_lne12 = new QLineEdit(QString::number(m_value.m12()), m_delegate); m_lne13 = new QLineEdit(QString::number(m_value.m13()), m_delegate);
        m_lne21 = new QLineEdit(QString::number(m_value.m21()), m_delegate); m_lne22 = new QLineEdit(QString::number(m_value.m22()), m_delegate); m_lne23 = new QLineEdit(QString::number(m_value.m23()), m_delegate);
        m_lne31 = new QLineEdit(QString::number(m_value.m31()), m_delegate); m_lne32 = new QLineEdit(QString::number(m_value.m32()), m_delegate); m_lne33 = new QLineEdit(QString::number(m_value.m33()), m_delegate);
        
        QGridLayout * layout = new QGridLayout(m_delegate);
        
        layout->addWidget(m_lne11, 0,0); layout->addWidget(m_lne12, 0,1); layout->addWidget(m_lne13, 0,2);
        layout->addWidget(m_lne21, 1,0); layout->addWidget(m_lne22, 1,1); layout->addWidget(m_lne23, 1,2);
        layout->addWidget(m_lne31, 2,0); layout->addWidget(m_lne32, 2,1); layout->addWidget(m_lne33, 2,2);
        
        initConnections();
    }
    return m_delegate;
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void TransformParameter::updateValue()
{
    if(m_delegate != NULL)
    {
        m_value.setMatrix(  m_lne11->text().toDouble() , m_lne12->text().toDouble(), m_lne13->text().toDouble(),
                            m_lne21->text().toDouble() , m_lne22->text().toDouble(), m_lne23->text().toDouble(),
                            m_lne31->text().toDouble() , m_lne32->text().toDouble(), m_lne33->text().toDouble());
        Parameter::updateValue();
    }
}

/**
 * Initializes the connections (signal<->slot) between the parameter class and
 * the delegate widget. This will be done after the first call of the delegate()
 * function, since the delegate is NULL until then.
 */
void TransformParameter::initConnections()
{
    connect(m_lne11,  SIGNAL(textChanged(QString)),  this, SLOT(updateValue()));
    connect(m_lne12,  SIGNAL(textChanged(QString)),  this, SLOT(updateValue()));
    connect(m_lne13,  SIGNAL(textChanged(QString)),  this, SLOT(updateValue()));
    connect(m_lne21,  SIGNAL(textChanged(QString)),  this, SLOT(updateValue()));
    connect(m_lne22,  SIGNAL(textChanged(QString)),  this, SLOT(updateValue()));
    connect(m_lne23,  SIGNAL(textChanged(QString)),  this, SLOT(updateValue()));
    connect(m_lne31,  SIGNAL(textChanged(QString)),  this, SLOT(updateValue()));
    connect(m_lne32,  SIGNAL(textChanged(QString)),  this, SLOT(updateValue()));
    connect(m_lne33,  SIGNAL(textChanged(QString)),  this, SLOT(updateValue()));
    Parameter::initConnections();
}

} //end of namespace graipe
