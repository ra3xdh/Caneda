/***************************************************************************
 * Copyright (C) 2006 by Gopala Krishna A <krishna.ggk@gmail.com>          *
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

#include "schematicview.h"

#include "item.h"
#include "qucsmainwindow.h"
#include "schematicscene.h"
#include "xmlformat.h"

#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QScrollBar>
#include <QTimer>
#include <QWheelEvent>

const qreal SchematicView::zoomFactor = 1.2f;

SchematicView::SchematicView(SchematicScene *sc, QucsMainWindow *parent) :
    QGraphicsView(sc,parent),
    QucsView(parent),
    m_horizontalScroll(0), m_verticalScroll(0)
{
    if(sc == 0) {
        sc = new SchematicScene(0, 0, 1024, 768);
        setScene(sc);
        DragMode dragMode = (sc->currentMouseAction() == SchematicScene::Normal) ?
            RubberBandDrag : NoDrag;
        setDragMode(dragMode);
    }

    setAcceptDrops(true);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setViewportUpdateMode(SmartViewportUpdate);
    viewport()->setMouseTracking(true);
    ensureVisible(0, 0, 5, 5);

    connect(sc, SIGNAL(modificationChanged(bool)), SIGNAL(modificationChanged(bool)));
    connect(sc, SIGNAL(fileNameChanged(const QString&)),
            SIGNAL(fileNameChanged(const QString&)));
    connect(sc, SIGNAL(titleToBeUpdated()), SIGNAL(titleToBeUpdated()));

    connect(this, SIGNAL(titleToBeUpdated()), SLOT(updateTabs()));
}

SchematicView::~SchematicView()
{
}

void SchematicView::test()
{
    schematicScene()->test();
}

SchematicScene* SchematicView::schematicScene() const
{
    SchematicScene* s = qobject_cast<SchematicScene*>(scene());
    Q_ASSERT(s);// This should never fail!
    return s;
}

void SchematicView::setFileName(const QString& name)
{
    schematicScene()->setFileName(name);
}

QString SchematicView::fileName() const
{
    return schematicScene()->fileName();
}

bool SchematicView::load()
{
    //Assumes file name is set
    FileFormatHandler *format =
        FileFormatHandler::handlerFromSuffix(QFileInfo(fileName()).suffix(), this);

    if(!format) {
        QMessageBox::critical(0, tr("Error"), tr("Unknown file format!"));
        return false;
    }

    if(!format->load()) {
        return false;
    }
    return true;
}

bool SchematicView::save()
{
    //Assumes filename is set before the call
    QFileInfo info(fileName());

    if(QString(info.suffix()).isEmpty()) {
        setFileName(fileName()+".xsch");
        info = QFileInfo(fileName());
    }

    FileFormatHandler *format =
        FileFormatHandler::handlerFromSuffix(info.suffix(), this);

    if(!format) {
        QMessageBox::critical(0, tr("Error"), tr("Unknown file format!"));
        return false;
    }

    if(!format->save()) {
        return false;
    }
    else {
        schematicScene()->undoStack()->clear();
    }

    return true;
}

void SchematicView::zoomIn()
{
    scale(zoomFactor, zoomFactor);
    repaintWires();
}

void SchematicView::zoomOut()
{
    qreal zf = 1.0/zoomFactor;
    scale(zf, zf);
    repaintWires();
}

void SchematicView::showAll()
{
    fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
    repaintWires();
}

void SchematicView::showNoZoom()
{
    resetMatrix();
    repaintWires();
}

QWidget* SchematicView::toWidget() const
{
    SchematicView* self = const_cast<SchematicView*>(this);
    QGraphicsView* view = qobject_cast<QGraphicsView*>(self);
    return static_cast<QWidget*>(view);
}

SchematicView* SchematicView::toSchematicView() const
{
    SchematicView* self = const_cast<SchematicView*>(this);
    QGraphicsView* view = qobject_cast<QGraphicsView*>(self);
    return qobject_cast<SchematicView*>(view);
}

bool SchematicView::isModified() const
{
    return schematicScene()->isModified();
}

void SchematicView::copy() const
{
    QList<QGraphicsItem*> items = scene()->selectedItems();
    QList<QucsItem*> qItems = filterItems<QucsItem>(items);
    schematicScene()->copyItems(qItems);
}

void SchematicView::cut()
{
    QList<QGraphicsItem*> items = scene()->selectedItems();
    QList<QucsItem*> qItems = filterItems<QucsItem>(items);

    if(!qItems.isEmpty()) {
        schematicScene()->cutItems(qItems);
    }
}

void SchematicView::paste()
{
    schematicScene()->paste();
}

void SchematicView::resetState()
{
    schematicScene()->resetState();
}

void SchematicView::saveScrollState()
{
    m_horizontalScroll = horizontalScrollBar()->value();
    m_verticalScroll  = verticalScrollBar()->value();
}

void SchematicView::restoreScrollState()
{
    horizontalScrollBar()->setValue(m_horizontalScroll);
    verticalScrollBar()->setValue(m_verticalScroll);
}

void SchematicView::setModified(bool m)
{
    schematicScene()->setModified(m);
}

void SchematicView::updateTabs()
{
    QTabWidget *tw = mainWindow->tabWidget();
    int index = tabIndex();
    if(index != -1) {
        tw->setTabText(index, tabText());
        tw->setTabIcon(index, isModified() ? modifiedTabIcon() : unmodifiedTabIcon());
    }
}

void SchematicView::repaintWires()
{
    bool fixSchematicViewrepaintWires;
}

void SchematicView::addTestComponents()
{
}
