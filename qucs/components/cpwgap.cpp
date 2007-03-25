/***************************************************************************
 * Copyright (C) 2007 by Gopala Krishna A <krishna.ggk@gmail.com>          *
 *                                                                         *
 * This is free software; you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2, or (at your option)     *
 * any later version.                                                      *
 *                                                                         *
 * This software is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this package; see the file COPYING.  If not, write to        *
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,   *
 * Boston, MA 02110-1301, USA.                                             *
 ***************************************************************************/

#include "cpwgap.h"
#include "shapes.h"

CPWgap::CPWgap(SchematicScene *s) : Component(s)
{
   initConstants();
   initPorts();
   initProperties();
}

void CPWgap::initConstants()
{
   qreal pw = 0.5;
   m_boundingRect = QRectF( -30, -24, 60, 48).adjusted(-pw, -pw, pw, pw);

   model = "CGAP";
   name = "CL";
   description =  QObject::tr("coplanar gap");

   m_shapes.append(new Line(-30,  0,-18,  0, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line( 18,  0, 30,  0, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line(-13, -8,  2, -8, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line(  8, -8, 23, -8, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line(-23,  8, -8,  8, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line( -2,  8, 13,  8, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line(-13, -8,-23,  8, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line(  2, -8, -8,  8, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line(  8, -8, -2,  8, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line( 23, -8, 13,  8, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line(-25,-13, 25,-13, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line( 16,-21, 24,-13, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line(  8,-21, 16,-13, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line(  0,-21,  8,-13, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line( -8,-21,  0,-13, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line(-16,-21, -8,-13, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line(-24,-21,-16,-13, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line(-25, 13, 25, 13, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line(-24, 13,-16, 21, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line(-16, 13, -8, 21, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line( -8, 13,  0, 21, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line(  0, 13,  8, 21, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line(  8, 13, 16, 21, Component::getPen(Qt::darkBlue,2)));
   m_shapes.append(new Line( 16, 13, 24, 21, Component::getPen(Qt::darkBlue,2)));
}

void CPWgap::initPorts()
{
   addPort(QPointF(-30,0));
   addPort(QPointF(30,0));
}

void CPWgap::initProperties()
{
   addProperty("Subst","Subst1",QObject::tr("name of substrate definition"),true);
   addProperty("W","1 mm",QObject::tr("width of the line"),true);
   addProperty("S","1 mm",QObject::tr("width of a gap"),true);
   addProperty("G","0.5 mm",QObject::tr("width of gap between the two lines"),true);
}

