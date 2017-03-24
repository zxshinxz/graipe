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

#ifndef GRAIPE_CORE_PARAMETERS_FILENAMEPARAMETER_HXX
#define GRAIPE_CORE_PARAMETERS_FILENAMEPARAMETER_HXX

#include "core/parameters/parameter.hxx"

#include <QPointer>
#include <QLineEdit>
#include <QPushButton>


namespace graipe {

/**
 * This is the FilenameParameter class.
 * It inherits from the Parameter base class to:
 * - hold a QString value as the filename, and
 * - provide editing facilities by means of a QLineEdit, which holds the
 *   filename and a QPushButton to show the file selection dialog.
 */
class GRAIPE_CORE_EXPORT FilenameParameter
:   public Parameter
{
    Q_OBJECT
    
    public:
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
        FilenameParameter(const QString& name, QString value="", Parameter* parent=NULL, bool invert_parent=false);
    
        /**
         * Destructor of the FilenameParameter class.
         */
        ~FilenameParameter();
    
        /**
         * The (immutable) type name of this parameter class.
         *
         * \return "FilenameParameter".
         */
        QString typeName() const;
        
        /** 
         * The current value of this parameter in the correct, most special type.
         *
         * \return The value of this parameter (For filename: QString).
         */
        QString value() const;
    
        /**
         * Writing accessor of the current value of this parameter.
         *
         * \param value The new value of this parameter.
         */
        void setValue(const QString& value);
            
        /**
         * The value converted to a QString. Please note, that this can vary from the 
         * serialize() result, which also returns a QString. This is due to the fact,
         * that serialize also may perform encoding of QStrings to avoid special chars
         * inside the QString.
         *
         * \return The value of the parameter converted to an QString.
         */
        QString valueText() const;
            
        /**
         * Serialization of the parameter's state to an output device.
         * Basically, just: "FilenameParameter, " + encode_string(value())
         *
         * \param out The output device on which we serialize the parameter's state.
         */
        void serialize(QIODevice& out) const;
    
        /**
         * Deserialization of a parameter's state from an input device.
         *
         * \param in the input device.
         * \return True, if the deserialization was successful, else false.
         */
        bool deserialize(QIODevice& in);
    
        /**
         * This function indicates whether the value of a parameter is valid or not.
         *
         * \return True, if the parameter's value is valid.
         */
        bool isValid() const;
        
        /**
         * The delegate widget of this parameter. 
         * Each parameter generates such a widget on demand, which refers to the
         * first call of this function. This is needed due to the executability of
         * classes using parameters (like the Algorithm class) in different threads.
         *
         * \return The delegate widget to control the values of this parameter.
         */
        QWidget * delegate();
        
    protected slots:
        /**
         * This slot is called if the user pressed the file-selection button. It is mainly
         * used to present the file selection dialog to the user.
         */
        void updateValue();
        
   protected:    
        /** The storage for the value of this parameter **/
        QString m_value;
    
        /** The parent delegate widget **/
        QPointer<QWidget> m_delegate;
    
        /** The children delegates inside the parent **/
        QPointer<QLineEdit> m_lneDelegate;
        /** The children delegates inside the parent **/
        QPointer<QPushButton> m_btnDelegate;
};

} //end of namespace graipe

#endif //GRAIPE_CORE_PARAMETERS_FILENAMEPARAMETER_HXX
