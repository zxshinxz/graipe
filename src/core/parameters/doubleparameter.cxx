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

#include "core/parameters/doubleparameter.hxx"

#include <QtDebug>

namespace graipe {

/**
 * Default constructor of the DoubleParameter class with a setting of the
 * most important values directly.
 *
 * \param name          The name (label) of this parameter.
 * \param low           The lowest allowed value of this parameter.
 * \param upp           The highest allowed value of this parameter.
 * \param value         The initial value of this parameter.
 * \param parent        If given (!= NULL), this parameter has a parent and will
 *                      be enabled/disabled, if the parent is a BoolParameter.
 * \param invert_parent If true, the enables/disabled dependency to the parent will be swapped.
 */
DoubleParameter::DoubleParameter(const QString& name, double low, double upp, double value, Parameter* parent, bool invert_parent)
:	Parameter(name, parent, invert_parent),
    m_dsbDelegate(new QDoubleSpinBox)
{
    setRange(low, upp);
    setValue(value);
    
    m_dsbDelegate->setDecimals(3);
    
    connect(m_dsbDelegate, SIGNAL(valueChanged(double)), this, SLOT(updateValue()));
    initConnections();
}

/**
 * Destructor of the DoubleParameter class.
 */
DoubleParameter::~DoubleParameter()
{
    delete m_dsbDelegate;
}

/**
 * The (immutable) type name of this parameter class.
 *
 * \return "DoubleParameter".
 */
QString  DoubleParameter::typeName() const
{
	return "DoubleParameter";
}

/**
 * The lowest possible value of this parameter.
 *
 * \return The minimal value of this parameter.
 */
double DoubleParameter::lowerBound() const
{
    return m_dsbDelegate->minimum();
}

/**
 * Writing accessor of the minimum value of this parameter.
 *
 * \param value The new minimum value of this parameter.
 */
void DoubleParameter::setLowerBound(double value)
{
    m_dsbDelegate->setMinimum(value);
}

/**
 * The highest possible value of this parameter.
 *
 * \return The maximal value of this parameter.
 */
double DoubleParameter::upperBound() const
{
    return m_dsbDelegate->maximum();;
}

/**
 * Writing accessor of the maximum value of this parameter.
 *
 * \param value The new maximum value of this parameter.
 */
void DoubleParameter::setUpperBound(double value)
{
    m_dsbDelegate->setMaximum(value);
}

/**
 * Writing accessor of the minimum and maximum value of this parameter.
 *
 * \param min_value The new minimum value of this parameter.
 * \param max_value The new maximum value of this parameter.
 */
void DoubleParameter::setRange(double min_value, double max_value)
{
    m_dsbDelegate->setRange(min_value, max_value);
}

/**
 * The current value of this parameter in the correct, most special type.
 *
 * \return The value of this parameter.
 */
double DoubleParameter::value() const
{
    return m_dsbDelegate->value();
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void DoubleParameter::setValue(double value)
{
    m_dsbDelegate->setValue(value);
    Parameter::updateValue();
}

/**
 * The value converted to a QString. Please note, that this can vary from the 
 * serialize() result, which also returns a QString. This is due to the fact,
 * that serialize also may perform encoding of QStrings to avoid special chars
 * inside the QString.
 *
 * \return The value of the parameter converted to an QString.
 */
QString DoubleParameter::valueText() const
{
	return QString::number(value(),'g', 10);
}

/**
 * Serialization of the parameter's state to a QString. Please note, that this can
 * vary from the valueText() result, which also returns a QString. This is due to the fact,
 * that serialize also may perform encoding of QStrings to avoid special chars.
 *
 * \return The serialization of the parameter's state.
 */
void DoubleParameter::serialize(QIODevice& out) const
{
    Parameter::serialize(out);
    write_on_device(", " + valueText(), out);
}

/**
 * Deserialization of a parameter's state from a QString.
 *
 * \param str The serialization of this parameter's state.
 * \return True, if the deserialization was successful, else false.
 */
bool DoubleParameter::deserialize(QIODevice& in)
{
    if(!Parameter::deserialize(in))
    {
        return false;
    }
    
    QString content(in.readLine().trimmed());

    try
    {
        double val = content.toDouble();
        setValue(val);
        
        return true;
    }
    catch (...)
    {
        qCritical() << "DoubleParameter deserialize: value could not be imported from: '" << content << "'";
    }
    return false;
}

/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
bool DoubleParameter::isValid() const
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
QWidget*  DoubleParameter::delegate()
{

    return m_dsbDelegate;
}

} //end of namespace graipe
