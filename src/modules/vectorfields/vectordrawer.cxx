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

#include "vectorfields/vectordrawer.hxx"

namespace graipe {

/**
 * @addtogroup graipe_vectorfields
 * @{
 *     @file
 *     @brief Implementation file for drawing single vectors
 * @}
 */

VectorDrawer::VectorDrawer(float line_width, float head_size, QVector<QRgb> colorTable)
: m_arrow_brush(colorTable[0])
{
    setLineWidth(line_width);
    setHeadSize(head_size);
    setColorTable(colorTable);
}

void VectorDrawer::setLineWidth(float new_line_width)
{
    m_line_pen.setWidthF(new_line_width);
}

float VectorDrawer::lineWidth() const
{
    return m_line_pen.widthF();
}

void VectorDrawer::setHeadSize(float new_head_size)
{
    m_head_size = new_head_size;
    updateHeadTriangle();
}

float VectorDrawer::headSize() const
{
    return m_head_size;
}

void VectorDrawer::setColorTable(QVector<QRgb> colorTable)
{
    m_colorTable = colorTable;
}

QVector<QRgb> VectorDrawer::colorTable() const
{
    return m_colorTable;
}

void VectorDrawer::paint(QPainter * painter, const QPointFX& origin, const QPointFX& target, float normalized_weight)
{
    QColor current_color =  QColor(m_colorTable[normalized_weight*255]);
    
    painter->setPen(QPen());
    painter->setBrush(QBrush());
    
    m_line_pen.setColor(current_color);
    m_arrow_brush.setColor(current_color);
    
    QPointFX direction = target-origin;
    
    QTransform t;
    t.translate(target.x(), target.y());
    t.rotate(direction.angle());
            
    float line_length = direction.length() - 2*m_head_size;
                    
    if(line_length > 0)
    {
        painter->setPen(m_line_pen);
        painter->setBrush(QBrush());
        painter->drawLine(origin, origin + direction/direction.length()*line_length);
    }
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_arrow_brush);
    painter->drawConvexPolygon(t.map(m_triangle));
}

void VectorDrawer::updateHeadTriangle()
{
   QPolygonF new_polygon;
        
   new_polygon << QPointF( 0,  0)
                 << QPointF(-2*m_head_size, -m_head_size*0.6)
                 << QPointF(-2*m_head_size, m_head_size*0.6)
                 << QPointF( 0,  0);
    
    m_triangle = new_polygon;
}

} //end of namespace graipe
