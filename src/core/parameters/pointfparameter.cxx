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

#include "core/parameters/pointfparameter.hxx"

#include <QLabel>
#include <QtDebug>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implementation file for the PointFParameter class
 * @}
 */

PointFParameter::PointFParameter(const QString& name, QPointF low, QPointF upp, QPointF value, Parameter* parent, bool invert_parent)
:   Parameter(name, parent, invert_parent),
    m_value(value),
    m_min_value(low),
    m_max_value(upp),
    m_delegate(NULL),
    m_dsbXDelegate(NULL),
    m_dsbYDelegate(NULL)
{
}

PointFParameter::~PointFParameter()
{
    //Also deletes other widget, since they are owned
    //by the assigned layout.
    if(m_delegate != NULL)
        delete m_delegate;
}

QPointF PointFParameter::lowerBound() const
{
    return m_min_value;//QPointF(m_dsbXDelegate->minimum(), m_dsbYDelegate->minimum());
}

void PointFParameter::setLowerBound(const QPointF& value)
{
    m_min_value = value;
    
    if(m_delegate)
    {
        m_dsbXDelegate->setMinimum(value.x());
        m_dsbYDelegate->setMinimum(value.y());
    }
}

QPointF PointFParameter::upperBound() const
{
    return m_max_value;//QPointF(m_dsbXDelegate->maximum(), m_dsbYDelegate->maximum());
}

void PointFParameter::setUpperBound(const QPointF& value)
{
    m_max_value = value;
    
    if(m_delegate)
    {
        m_dsbXDelegate->setMaximum(value.x());
        m_dsbYDelegate->setMaximum(value.y());
    }
}

void PointFParameter::setRange(const QPointF& min_value, const QPointF& max_value)
{
    setLowerBound(min_value);
    setUpperBound(max_value);
}

QPointF PointFParameter::value() const
{
    return m_value;//QPointF(m_dsbXDelegate->value(), m_dsbYDelegate->value());
}

void PointFParameter::setValue(const QPointF& value)
{
    m_value=value;
    
    if(m_delegate)
    {
        m_dsbXDelegate->setValue(value.x());
        m_dsbYDelegate->setValue(value.y());
        Parameter::updateValue();
    }
}

QString  PointFParameter::toString() const
{
    return QString("(") + QString::number(value().x(),'g', 10) + "x" + QString::number(value().y(),'g', 10) + ")";
}

void PointFParameter::serialize(QXmlStreamWriter& xmlWriter) const
{
    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartElement(typeName());
    xmlWriter.writeAttribute("ID", id());
    xmlWriter.writeTextElement("Name", name());
    xmlWriter.writeTextElement("x", QString::number(value().x(),'g', 10));
    xmlWriter.writeTextElement("y", QString::number(value().y(),'g', 10));
    xmlWriter.writeEndElement();
}

bool PointFParameter::deserialize(QXmlStreamReader& xmlReader)
{
    try
    {
        if(     xmlReader.name() == typeName()
            &&  xmlReader.attributes().hasAttribute("ID"))
        {
            setID(xmlReader.attributes().value("ID").toString());
            
            QPointF p;
            
            for(int i=0; i!=3; ++i)
            {
                xmlReader.readNextStartElement();
            
                if(xmlReader.name() == "Name")
                {
                    setName(xmlReader.readElementText());
                }
                if(xmlReader.name() == "x")
                {
                   p.setX(xmlReader.readElementText().toFloat());
                }
                if(xmlReader.name() == "y")
                {
                   p.setY(xmlReader.readElementText().toFloat());
                }
            }
            //Read until </PointFParameter> comes....
            while(true)
            {
                if(!xmlReader.readNext())
                {
                    return false;
                }
                
                if(xmlReader.isEndElement() && xmlReader.name() == typeName())
                {
                    break;
                }
            }
            setValue(p);
            return true;
        }
        else
        {
            throw std::runtime_error("Did not find typeName() or id() in XML tree");
        }
    }
    catch(std::runtime_error & e)
    {
        qCritical() << "PointFParameter::deserialize failed! Was looking for typeName(): " << typeName() << "Error: " << e.what();
        return false;
    }
}

bool PointFParameter::isValid() const
{
    return m_value.x() >= m_min_value.x() && m_value.y() >= m_min_value.y()
        && m_value.x() <= m_max_value.x() && m_value.y() <= m_max_value.y();
}

QWidget*  PointFParameter::delegate()
{
    if(m_delegate == NULL)
    {
        m_delegate = new QWidget;
        m_dsbXDelegate = new QDoubleSpinBox;
        m_dsbYDelegate = new QDoubleSpinBox;
  
        m_dsbXDelegate->setMaximumSize(9999,9999);
        m_dsbXDelegate->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
        m_dsbXDelegate->setRange(m_min_value.x(), m_max_value.x());
        m_dsbXDelegate->setValue(m_value.x());
        
        m_dsbYDelegate->setMaximumSize(9999,9999);
        m_dsbYDelegate->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
        m_dsbYDelegate->setRange(m_min_value.y(), m_max_value.y());
        m_dsbYDelegate->setValue(m_value.y());
  
        QHBoxLayout * layout = new QHBoxLayout(m_delegate);
        
        layout->setContentsMargins(0,0,0,0);
        layout->addWidget(new QLabel("x:"));
        layout->addWidget(m_dsbXDelegate);
        layout->addWidget(new QLabel("y:"));
        layout->addWidget(m_dsbYDelegate);
    
        
        connect(m_dsbXDelegate, SIGNAL(valueChanged(double)), this, SLOT(updateValue()));
        connect(m_dsbYDelegate, SIGNAL(valueChanged(double)), this, SLOT(updateValue()));
        Parameter::initConnections();
    }
    return m_delegate;
}

void PointFParameter::updateValue()
{
    //Should not happen - otherwise, better safe than sorry:
    if(m_delegate != NULL)
    {
        m_value.setX(m_dsbXDelegate->value());
        m_value.setY(m_dsbYDelegate->value());
        Parameter::updateValue();
    }
}

} //end of namespace graipe
