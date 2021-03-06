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

#include "images/image.hxx"
#include "images/imageimpex.hxx"

namespace graipe {

/**
 * @addtogroup graipe_images
 * @{
 *     @file
 *     @brief Implementation file for image classes
 * @}
 */
 
template<class T>
Image<T>::Image(Workspace* wsp)
 :  RasteredModel(wsp),
    m_numBands(new IntParameter("Number of bands:",0,1000, 0)),
    m_timestamp(new DateTimeParameter("Timestamp:", QDateTime::currentDateTime())),
    m_scale(new DoubleParameter("Scale (1 px = X m):", 0, 1000000000, 1)),
    m_comment(new LongStringParameter("Comment:", "")),
    m_units(new StringParameter("Units:", "m"))
{
    m_name->setValue(QString("New ") + typeName());
    m_description->setValue(QString("This new ") + typeName() + " has been created on " + QDateTime::currentDateTime().toString());
    
    appendParameters();
}

template<class T>
Image<T>::Image(const Image<T> & img)
 :	RasteredModel(img),
    m_numBands(new IntParameter("Number of bands:",0,1000, img.numBands())),
    m_timestamp(new DateTimeParameter("Timestamp:", img.timestamp())),
    m_scale(new DoubleParameter("Scale (1 px = X m):", 0, 1000000000, img.scale())),
    m_comment(new LongStringParameter("Comment:", img.comment())),
    m_units(new StringParameter("Units:", "m"))
{
    appendParameters();

    //Get tags from other image
	img.copyMetadata(*this);
	
	//Get Bands from other image
	for (unsigned int i=0; i< img.numBands(); ++i) 
    {
        //Copy bands from other image
        m_imagebands[i] = img.band(i);
    }
}

template<class T>
Image<T>::Image(Size_Type size, 
                unsigned int numBands,
                Workspace* wsp)

 :  RasteredModel(wsp),
    m_numBands(new IntParameter("Number of bands:",0,1000, numBands)),
    m_timestamp(new DateTimeParameter("Timestamp:", QDateTime::currentDateTime())),
    m_scale(new DoubleParameter("Scale (1 px = X m):", 0, 1000000000, 1)),
    m_comment(new LongStringParameter("Comment:", "")),
    m_units(new StringParameter("Units:", "m"))
{
    appendParameters();
    setWidth((unsigned int)size[0]);
    setHeight((unsigned int)size[1]);
    setNumBands(numBands);
}

template<class T>
Image<T>::~Image()
{
    //The command "delete m_parameters;" inside the base class
    //will also delete all other (newly introduced) parameters
    //No need to do anything here!
}

template<>
QString Image<float>::typeName() const
{
    return "Image";
}

template<>
QString Image<int>::typeName() const
{
    return "IntImage";
}

template<>
QString Image<unsigned char>::typeName() const
{
    return "ByteImage";
}

template<class T>
bool Image<T>::isEmpty() const
{
    return RasteredModel::isEmpty() || numBands()==0;
}

template<class T>
typename Image<T>::Size_Type Image<T>::size() const
{
    return typename Image<T>::Size_Type(width(), height());
}

template<class T>
const vigra::MultiArrayView<2,T> & Image<T>::band(unsigned int band_id) const
{
    return m_imagebands[band_id];
}

template<class T>
void Image<T>::setBand(unsigned int band_id, const vigra::MultiArrayView<2,T>& band)
{
    if(locked())
        return;
    
    m_imagebands[band_id] = band;
}

template <class T>
unsigned int Image<T>::numBands() const
{
	return m_numBands->value();
}

template <class T>
void Image<T>::setNumBands( unsigned int bands)
{
    if(locked())
        return;
    
    m_numBands->setValue(bands);
    updateModel();
}

template<class T>
typename Image<T>::DateTime_Type Image<T>::timestamp() const
{
	return m_timestamp->value();
}

template<class T>
typename Image<T>::Comment_Type Image<T>::comment() const
{
	return m_comment->value();
}

template<class T>
typename Image<T>::Units_Type Image<T>::units() const
{
	return m_units->value();
}

template<class T>
typename Image<T>::Scale_Type Image<T>::scale() const
{
	return m_scale->value();
}

template<class T>
void Image<T>::setTimestamp(const DateTime_Type& timestamp)
{
    if(locked())
        return;
    
    m_timestamp->setValue(timestamp);
    updateModel();
}

template<class T>
void Image<T>::setComment(const Comment_Type& comment)
{
    if(locked())
        return;
    
    m_comment->setValue(comment);
    updateModel();
}

template<class T>
void Image<T>::setUnits(const Units_Type& units)
{
    if(locked())
        return;
        
	m_units->setValue(units);
        updateModel();
}

template<class T>
void Image<T>::setScale(Scale_Type scale)
{
    if(locked())
        return;
        
	m_scale->setValue(scale);
    updateModel();
}

template<class T>
void Image<T>::copyMetadata(Model & other) const
{
    RasteredModel::copyMetadata(other);
    
	if(this != &other && other.typeName() == typeName())
	{
        Image<T>& image_model = static_cast<Image<T>&>(other);
        
		image_model.setTimestamp(timestamp());
		image_model.setComment(comment());
		image_model.setUnits(units());
        image_model.setScale(scale());
        image_model.setNumBands(numBands());
	}
}

template<class T>
void Image<T>::copyData(Model& other) const
{
    RasteredModel::copyData(other);
    
    copyMetadata(other);
    
	if(this != &other && other.typeName() == typeName())
	{
        Image<T>& image_model = static_cast<Image<T>&>(other);
        
        for (unsigned int i=0; i< numBands(); ++i)
        {
            image_model.setBand(i, band(i));
        }
    }    
}

template<class T>
void Image<T>::serialize_content(QXmlStreamWriter& xmlWriter) const
{
    try
    {
    
        xmlWriter.writeTextElement("Width",    QString::number(this->width()));
        xmlWriter.writeTextElement("Height",   QString::number(this->height()));
        xmlWriter.writeTextElement("Channels", QString::number(this->numBands()));
        xmlWriter.writeTextElement("Order",   "Row-major");
        xmlWriter.writeTextElement("Encoding", "Base64");
        
        qint64 channel_size = this->width()*this->height()*sizeof(T);

        for(unsigned int c=0; c<m_imagebands.size(); ++c)
        {
            QByteArray block((const char*)m_imagebands[c].data(),channel_size);
            
            xmlWriter.writeStartElement("Channel");
            xmlWriter.writeAttribute("ID", QString::number(c));
                xmlWriter.writeCharacters(block.toBase64());
            xmlWriter.writeEndElement();
        }
    }
    catch(...)
    {
        qCritical() << "Image<T>::serialize_content failed!";
    }
}

template<class T>
bool Image<T>::deserialize_content(QXmlStreamReader& xmlReader)
{

    if(this->width() == 0 || this->height()==0 || this->numBands() ==0)
    {
        qCritical("Image<T>::deserialize_content: Image has zero size!");
        return false;
    }
    
    qint64 channel_size = this->width()*this->height()*sizeof(T);
    
    m_imagebands.clear();
    m_imagebands.resize(numBands());
        
    //Prepare all bands:
    for(unsigned int c=0; c<m_imagebands.size(); ++c)
    {
        m_imagebands[c] = vigra::MultiArray<2,T>(width(),height());
    }
    
    try
    {
        while(xmlReader.readNextStartElement())
        {            
            if (xmlReader.name() == "Width")
            {
                int w = xmlReader.readElementText().toInt();
                
                if(w != width())
                {
                    throw std::runtime_error("Width does not match Header info.");
                }
            }            
            if (xmlReader.name() == "Height")
            {
                int h = xmlReader.readElementText().toInt();
                
                if(h != height())
                {
                    throw std::runtime_error("Height does not match Header info.");
                }
            }
            
            if (xmlReader.name() == "Channels")
            {
                int c = xmlReader.readElementText().toInt();
                
                if(c != numBands())
                {
                    throw std::runtime_error("Number of Channels does not match Header info.");
                }
            }
            
            if (xmlReader.name() == "Order" && xmlReader.readElementText() != "Row-major")
            {
                throw std::runtime_error("Order of data has to be 'Row-major'.");
            }
            
            
            if (xmlReader.name() == "Encoding" && xmlReader.readElementText() != "Base64")
            {
                throw std::runtime_error("Encoding of data has to be 'Base64'.");
            }
            
            if(xmlReader.name() == "Channel" && xmlReader.attributes().hasAttribute("ID"))
            {
                int id = xmlReader.attributes().value("ID").toInt();
                
                if (id < 0 || (unsigned int)id >= numBands())
                {
                    throw std::runtime_error("Channel id not found in image");
                }
                
                QByteArray block;
                block.append(xmlReader.readElementText());
                block = QByteArray::fromBase64(block);
                
                if(block.size() == channel_size)
                {
                    memcpy((char*)m_imagebands[id].data(), block.data(), channel_size);
                }
                else
                {
                    throw std::runtime_error("Channel serialization was of wrong size in XML after Base64 decoding.");
                }
            }
        }
    }
    catch(std::runtime_error & e)
    {
        qCritical() << "Image<T>::deserialize_content failed! Error: " << e.what();
        return false;
    }
    return true;
}

template <class T>
void Image<T>::updateModel()
{
    //qDebug() << QString("Inside Image<T>::updateModel() - numBands=%1, size=(%2x%3) -locked=%4").arg(numBands()).arg(width()).arg(height()).arg(locked());
    
    //remove existing image bands
    if (numBands() < m_imagebands.size())
    {
        //qDebug("remove existing image bands");
        
        while (m_imagebands.size() != numBands())
        {
            m_imagebands.pop_back();
        }
    }
    else if(width()!=0 && height()!=0)
    {
        //Add a new image band
        if (numBands() > m_imagebands.size())
        {
        
            vigra::MultiArray<2,T> band(width(), height());
            band.init(vigra::NumericTraits<T>::zero());
            
            while (m_imagebands.size() != numBands())
            {
                //qDebug() << QString("Add a new image band of size: (%1x%2)").arg(width()).arg(height());
                m_imagebands.push_back(band);
            }
        }
        //Dimensions have changed
        else if(   m_imagebands.size()!=0
                && ((unsigned int)m_imagebands[0].width()!= width() || (unsigned int)m_imagebands[0].height()!= height()))
        {
            //qDebug() << QString("Dimensions have changed from (%1x%2) to (%3x%4)").arg(m_imagebands[0].width()).arg(m_imagebands[0].height()).arg(width()).arg(height());
            for(vigra::MultiArray<2,T> & band: m_imagebands)
            {
                band.reshape(vigra::Shape2(width(),height()));
                band.init(vigra::NumericTraits<T>::zero());
            }
        }
        
        RasteredModel::updateModel();
    }
}

template <class T>
void Image<T>::appendParameters()
{
    m_parameters->addParameter("numbands", m_numBands);
    m_parameters->addParameter("timestamp", m_timestamp);
    m_parameters->addParameter("scale", m_scale);
    m_parameters->addParameter("comment", m_comment);
    m_parameters->addParameter("units", m_units);
}

//Promote the following three temple instances for further use:
template class Image<float>;
template class Image<int>;
template class Image<unsigned char>;


} //end of namespace graipe
