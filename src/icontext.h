/***************************************************************************
 * Copyright (C) 2010 by Gopala Krishna A <krishna.ggk@gmail.com>          *
 * Copyright (C) 2013 by Pablo Daniel Pareja Obregon                       *
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

#ifndef CANEDA_ICONTEXT_H
#define CANEDA_ICONTEXT_H

#include "global.h"

#include <QObject>

// Forward declaration
class QFileInfo;
class QToolBar;
class QWidget;

namespace Caneda
{
    // Forward declarations.
    class IDocument;
    class IView;

    /*!
     * \brief This class provides an interface for a context which is
     * used by IDocument and IView. Only one instance of this class per
     * document type is used during the whole life span of the program.
     * This class answers the general questions about each document type,
     * like which file suffixes it can handle, points to the appropiate
     * methods to create new documents of its type, etc. This class also
     * provides objects like the toolbar, statusbar, etc, which are specific
     * to the particular context. The context class can also be used to host
     * functionalites shared by all views and documents of same type.
     *
     * Each inherited class must be a singleton class and thier only static
     * instance (returned by instance()) must be used.
     *
     * \sa IDocument, IView, \ref DocumentViewFramework
     */
    class IContext : public QObject
    {
        Q_OBJECT

    public:
        virtual ~IContext();

        virtual void init();

        virtual QToolBar* toolBar() { return 0; }
        virtual QWidget* sideBarWidget()  { return 0; }

        virtual bool canOpen(const QFileInfo& info) const = 0;
        virtual QStringList fileNameFilters() const = 0;
        virtual QString defaultSuffix() const = 0;

        virtual IDocument* newDocument() = 0;
        virtual IDocument* open(const QString& filename, QString *errorMessage = 0) = 0;

    protected:
        IContext(QObject *parent = 0);
    };

} // namespace Caneda

#endif //CANEDA_ICONTEXT_H
