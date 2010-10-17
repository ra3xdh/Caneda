/***************************************************************************
 * Copyright (C) 2009 by Pablo Daniel Pareja Obregon                       *
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

#include "settingspages.h"

#include "global.h"
#include "mainwindow.h"
#include "settings.h"

#include <QApplication>
#include <QColorDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QDateEdit>
#include <QDir>
#include <QFontDialog>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>
#include <QSpinBox>

namespace Caneda
{

    //*!**************************************************
    // General configuration pages
    //*!**************************************************

    /*!
     * Constructor
     * @param QWidget *parent The parent of the dialog.
     */
    SettingsPage::SettingsPage(QWidget *parent) : QWidget(parent)
    {
    }

    void SettingsPage::setForegroundColor(QPushButton *b, QColor col)
    {
        QPalette palette(b->palette());
        palette.setColor(b->foregroundRole(), col);
        b->setPalette(palette);
    }

    QColor SettingsPage::getForegroundColor(QPushButton *b)
    {
        QPalette palette(b->palette());
        return palette.color(b->foregroundRole());
    }

    void SettingsPage::setBackgroundColor(QPushButton *b, QColor col)
    {
        QPalette palette(b->palette());
        palette.setColor(b->backgroundRole(), col);
        b->setPalette(palette);
    }

    QColor SettingsPage::getBackgroundColor(QPushButton *b)
    {
        QPalette palette(b->palette());
        return palette.color(b->backgroundRole());
    }

    //! Destructor
    SettingsPage::~SettingsPage()
    {
    }

    //*!**************************************************
    // General configuration pages
    //*!**************************************************

    /*!
     * Constructor
     * @param QWidget *parent The parent of the dialog.
     */
    GeneralConfigurationPage::GeneralConfigurationPage(QWidget *parent) :
        SettingsPage(parent)
    {
        Settings *settings = Settings::instance();

        //First we set the appereance settings group of options *******************
        checkShowGrid = new QCheckBox();
        checkShowGrid->setChecked(settings->currentValue("gui/gridVisible").value<bool>());

        buttonFont = new QPushButton;
        font = settings->currentValue("gui/font").value<QFont>();
        buttonFont->setText(font.family());

        buttonBackground = new QPushButton;
        const QColor currentBackgroundColor =
            settings->currentValue("gui/backgroundColor").value<QColor>();
        setBackgroundColor(buttonBackground, currentBackgroundColor);

        buttonForeground = new QPushButton;
        const QColor currentForegroundColor =
            settings->currentValue("gui/foregroundColor").value<QColor>();
        setBackgroundColor(buttonForeground, currentForegroundColor);

        spinIcons = new QSpinBox;
        spinIcons->setValue(settings->currentValue("gui/iconSize").toSize().height());
        spinIcons->setMinimum(10);
        spinIcons->setMaximum(48);

        connect(buttonFont, SIGNAL(clicked()), SLOT(slotFontDialog()));
        connect(buttonBackground, SIGNAL(clicked()), SLOT(slotBGColorDialog()));
        connect(buttonForeground, SIGNAL(clicked()), SLOT(slotFGColorDialog()));

        QGroupBox *appereance = new QGroupBox(tr("Appereance"), this);
        QFormLayout *appereanceLayout = new QFormLayout(appereance);
        appereanceLayout->addRow(tr("Show grid:"), checkShowGrid);
        appereanceLayout->addRow(tr("Fonts:"), buttonFont);
        appereanceLayout->addRow(tr("Document background color:"), buttonBackground);
        appereanceLayout->addRow(tr("Document foreground color:"), buttonForeground);
        appereanceLayout->addRow(tr("Icons size:"), spinIcons);


        //Now we set the misc group of options ************************************
        spinUndoNum = new QSpinBox;
        spinUndoNum->setValue(settings->currentValue("gui/maxUndo").toInt());
        spinUndoNum->setMinimum(0);
        spinUndoNum->setMaximum(200);

        editLibrary = new QLineEdit;
        editLibrary->setText(settings->currentValue("sidebarLibrary").toString());

        QGroupBox *misc = new QGroupBox(tr("Misc"), this);
        QFormLayout *miscLayout = new QFormLayout(misc);
        miscLayout->addRow(tr("Maximum undo operations:"), spinUndoNum);
        miscLayout->addRow(tr("Components library:"), editLibrary);


        //Finally we set the general layout of all groups *************************
        QVBoxLayout *vlayout1 = new QVBoxLayout();
        QLabel *title_label_ = new QLabel(title());
        vlayout1->addWidget(title_label_);

        QFrame *horiz_line_ = new QFrame();
        horiz_line_->setFrameShape(QFrame::HLine);
        vlayout1->addWidget(horiz_line_);

        vlayout1->addWidget(appereance);
        vlayout1->addWidget(misc);

        vlayout1->addStretch();

        setLayout(vlayout1);
    }

    //! Destructor
    GeneralConfigurationPage::~GeneralConfigurationPage()
    {
    }

    void GeneralConfigurationPage::slotFontDialog()
    {
        bool ok;
        QFont tmpFont = QFontDialog::getFont(&ok, font, this);
        if(ok) {
            font = tmpFont;
            buttonFont->setText(font.family());
        }
    }

    void GeneralConfigurationPage::slotBGColorDialog()
    {
        QColor c = QColorDialog::getColor(
                getBackgroundColor(buttonBackground), this);
        if(c.isValid()) {
            setBackgroundColor(buttonBackground,c);
        }
    }

    void GeneralConfigurationPage::slotFGColorDialog()
    {
        QColor c = QColorDialog::getColor(
                getBackgroundColor(buttonForeground), this);
        if(c.isValid()) {
            setBackgroundColor(buttonForeground,c);
        }
    }

    //! Applies the configuration of this page
    void GeneralConfigurationPage::applyConf()
    {
        bool changed = false;

        Settings *settings = Settings::instance();

        const bool currentGridVisible =
            settings->currentValue("gui/gridVisible").value<bool>();
        const bool newGridVisible = checkShowGrid->isChecked();
        if (currentGridVisible != newGridVisible) {
            settings->setCurrentValue("gui/gridVisible", newGridVisible);
            changed = true;
        }

        const QColor currentBackgroundColor =
            settings->currentValue("gui/backgroundColor").value<QColor>();
        const QColor newBackgroundColor = getBackgroundColor(buttonBackground);
        if (currentBackgroundColor != newBackgroundColor) {
            settings->setCurrentValue("gui/backgroundColor", newBackgroundColor);
            changed = true;
        }

        const QColor currentForegroundColor =
            settings->currentValue("gui/foregroundColor").value<QColor>();
        const QColor newForegroundColor = getBackgroundColor(buttonForeground);
        if (currentForegroundColor != newForegroundColor) {
            settings->setCurrentValue("gui/foregroundColor", newForegroundColor);
            changed = true;
        }

        const QFont currentFont = settings->currentValue("gui/font").value<QFont>();
        const QFont newFont = font;
        if(currentFont != newFont) {
            settings->setCurrentValue("gui/font", newFont);
            qApp->setFont(newFont);
            changed = true;
        }

        const QSize currentIconSize = settings->currentValue("gui/iconSize").toSize();
        const QSize newIconSize(spinIcons->value(), spinIcons->value());
        if (currentIconSize != newIconSize) {
            settings->setCurrentValue("gui/iconSize", newIconSize);
            MainWindow::instance()->setIconSize(newIconSize);
        }

        const int currentMaxUndo = settings->currentValue("gui/maxUndo").toInt();
        const int newMaxUndo = spinUndoNum->value();
        if (currentMaxUndo != newMaxUndo) {
            settings->setCurrentValue("gui/maxUndo", newMaxUndo);
            //TODO: Also update all undostacks
        }

        const QString currentLibrary = settings->currentValue("sidebarLibrary").toString();
        QString newLibrary = editLibrary->text();
        if (currentLibrary != newLibrary) {
            if (newLibrary.endsWith(QDir::separator()) == false) {
                newLibrary.append(QDir::separator());
            }
            settings->setCurrentValue("sidebarLibrary", newLibrary);
        }

        QSettings qSettings;
        settings->save(qSettings);

        if(changed) {
            MainWindow::instance()->slotUpdateSettingsChanges();
            MainWindow::instance()->repaint();
        }
    }

    //! @return Icon of this page
    QIcon GeneralConfigurationPage::icon() const {
        return(Caneda::icon("preferences-other"));
    }

    //! @return Title of this page
    QString GeneralConfigurationPage::title() const {
        return(tr("General", "configuration page title"));
    }


    //*!**************************************************
    // Simulation configuration pages
    //*!**************************************************
    /*!
     * Constructor
     * @param QWidget *parent The parent of the dialog.
     */
    SimulationConfigurationPage::SimulationConfigurationPage(QWidget *parent) :
        SettingsPage(parent)
    {
        //First we set the simulator group of options ***************************************
        QGroupBox *groupSimulator = new QGroupBox(tr("Simulator Engine"), this);
        QRadioButton *qucsatorMode = new QRadioButton(tr("Use qucs engine"), groupSimulator);
        QRadioButton *ngspiceMode = new QRadioButton(tr("Use ngspice engine"), groupSimulator);

        qucsatorMode->setChecked(true);

        QVBoxLayout *simulatorLayout = new QVBoxLayout();
        simulatorLayout->addWidget(qucsatorMode);
        simulatorLayout->addWidget(ngspiceMode);
        groupSimulator->setLayout(simulatorLayout);


        //Now we set the simulation tab group of options ************************************
        QGroupBox *groupSimTabMode = new QGroupBox(tr("Simulation Display Mode"), this);
        QRadioButton *specialTabMode = new QRadioButton(tr("Use a special tab"),
                groupSimTabMode);
        QRadioButton *sameTabMode = new QRadioButton(tr("Use same tab as schematic"),
                groupSimTabMode);

        specialTabMode->setChecked(true);

        QVBoxLayout *simTabModeLayout = new QVBoxLayout();
        simTabModeLayout->addWidget(specialTabMode);
        simTabModeLayout->addWidget(sameTabMode);
        groupSimTabMode->setLayout(simTabModeLayout);


        //Now we set the misc group of options **********************************************
        QCheckBox *checkOpenDataDisplay = new QCheckBox(tr("Open data display after simulation:"));
        checkOpenDataDisplay->setChecked(true);

        QLineEdit *editDataset = new QLineEdit;

        QGroupBox *misc = new QGroupBox(tr("Misc"), this);
        QFormLayout *miscLayout = new QFormLayout(misc);
        miscLayout->addRow(checkOpenDataDisplay);
        miscLayout->addRow(tr("Dataset:"), editDataset);


        //Finally we set the general layout of all groups ***********************************
        QVBoxLayout *vlayout1 = new QVBoxLayout();
        QLabel *title_label_ = new QLabel(title());
        vlayout1->addWidget(title_label_);

        QFrame *horiz_line_ = new QFrame();
        horiz_line_->setFrameShape(QFrame::HLine);
        vlayout1->addWidget(horiz_line_);

        vlayout1->addWidget(groupSimulator);
        vlayout1->addWidget(groupSimTabMode);
        vlayout1->addWidget(misc);

        vlayout1->addStretch();

        setLayout(vlayout1);
    }

    //! Destructor
    SimulationConfigurationPage::~SimulationConfigurationPage()
    {
    }

    //! Applies the configuration of this page
    void SimulationConfigurationPage::applyConf()
    {
        // TODO Implement this
    }

    //! @return Icon of this page
    QIcon SimulationConfigurationPage::icon() const
    {
        return(Caneda::icon("media-playback-start"));
    }

    //! @return Title of this page
    QString SimulationConfigurationPage::title() const
    {
        return(tr("Simulation", "simulation page title"));
    }


    //*!**************************************************
    // Document configuration pages
    //*!**************************************************

    /*!
     * Constructor
     * @param QWidget *parent The parent of the dialog.
     */
    SchematicDocumentConfigurationPage::SchematicDocumentConfigurationPage(CGraphicsScene *scene,
            QWidget *parent) : SettingsPage(parent)
    {

        m_scene = scene;

        //We set the frame group of options **********************************
        checkShowFrame = new QCheckBox;
        checkShowFrame->setChecked(m_scene->isFrameVisible());

        spinSchemaX = new QSpinBox;
        spinSchemaX->setMinimum(500);
        spinSchemaX->setMaximum(10000);
        spinSchemaX->setValue(m_scene->frameWidth());

        spinSchemaY = new QSpinBox;
        spinSchemaY->setMinimum(300);
        spinSchemaY->setMaximum(10000);
        spinSchemaY->setValue(m_scene->frameHeight());

        spinFrameY = new QSpinBox;
        spinFrameY->setMinimum(1);
        spinFrameY->setMaximum(50);
        spinFrameY->setValue(m_scene->frameRows());

        spinFrameX = new QSpinBox;
        spinFrameX->setMinimum(1);
        spinFrameX->setMaximum(50);
        spinFrameX->setValue(m_scene->frameColumns());

        QGroupBox *frame = new QGroupBox(tr("Frame"), this);
        QFormLayout *frameLayout = new QFormLayout(frame);
        frameLayout->addRow(tr("Show frame:"), checkShowFrame);
        frameLayout->addRow(tr("Frame Width:"), spinSchemaX);
        frameLayout->addRow(tr("Frame Height:"), spinSchemaY);
        frameLayout->addRow(tr("Frame Rows:"), spinFrameY);
        frameLayout->addRow(tr("Frame Columns:"), spinFrameX);


        //Next we set the document group of options *******************************
        editTitle = new QLineEdit;
        editName = new QLineEdit;
        editRevision = new QLineEdit;
        editDate = new QDateEdit;
        foreach(QString frame_text, m_scene->frameTexts()){
            if(frame_text.contains("Title: ")) {
                editTitle->setText(frame_text.remove("Title: "));
            }
            else if(frame_text.contains("Drawn By: ")) {
                editName->setText(frame_text.remove("Drawn By: "));
            }
            else if(frame_text.contains("Revision: ")) {
                editRevision->setText(frame_text.remove("Revision: "));
            }
            else if(frame_text.contains("Date: ")) {
                editDate->setDate(QDate::fromString(frame_text.remove("Date: ")));
            }
        }

        QGroupBox *document = new QGroupBox(tr("Document"), this);
        QFormLayout *documentLayout = new QFormLayout(document);
        documentLayout->addRow(tr("Title:"), editTitle);
        documentLayout->addRow(tr("Name:"), editName);
        documentLayout->addRow(tr("Revision:"), editRevision);
        documentLayout->addRow(tr("Date:"), editDate);


        //Finally we set the general layout of all groups *************************
        QVBoxLayout *vlayout1 = new QVBoxLayout();
        QLabel *title_label_ = new QLabel(title());
        vlayout1->addWidget(title_label_);

        QFrame *horiz_line_ = new QFrame();
        horiz_line_->setFrameShape(QFrame::HLine);
        vlayout1->addWidget(horiz_line_);

        vlayout1->addWidget(frame);
        vlayout1->addWidget(document);

        vlayout1->addStretch();

        setLayout(vlayout1);
    }

    //! Destructor
    SchematicDocumentConfigurationPage::~SchematicDocumentConfigurationPage()
    {
    }

    /*!
      Applies the configuration of this page
      */
    void SchematicDocumentConfigurationPage::applyConf()
    {
        bool changed = false;

        if(m_scene->isFrameVisible() != checkShowFrame->isChecked()) {
            m_scene->undoStack()->push(new ScenePropertyChangeCmd("frame visibility",
                        checkShowFrame->isChecked(), m_scene->isFrameVisible(), m_scene));
            m_scene->setFrameVisible(checkShowFrame->isChecked());
            changed = true;
        }
        if(m_scene->frameWidth() != spinSchemaX->value()) {
            m_scene->undoStack()->push(new ScenePropertyChangeCmd("frame width",
                        spinSchemaX->value(), m_scene->frameWidth(), m_scene));
            m_scene->setFrameSize(spinSchemaX->value(), m_scene->frameHeight());
            changed = true;
        }
        if(m_scene->frameHeight() != spinSchemaY->value()) {
            m_scene->undoStack()->push(new ScenePropertyChangeCmd("frame height",
                        spinSchemaY->value(), m_scene->frameHeight(), m_scene));
            m_scene->setFrameSize(m_scene->frameWidth(), spinSchemaY->value());
            changed = true;
        }
        if(m_scene->frameRows() != spinFrameY->value()) {
            m_scene->undoStack()->push(new ScenePropertyChangeCmd("frame rows",
                        spinFrameY->value(), m_scene->frameRows(), m_scene));
            m_scene->setFrameGeometry(spinFrameY->value(), m_scene->frameColumns());
            changed = true;
        }
        if(m_scene->frameColumns() != spinFrameX->value()) {
            m_scene->undoStack()->push(new ScenePropertyChangeCmd("frame columns",
                        spinFrameX->value(), m_scene->frameColumns(), m_scene));
            m_scene->setFrameGeometry(m_scene->frameRows(), spinFrameX->value());
            changed = true;
        }


        bool modified = false;
        if(!m_scene->frameTexts().contains(tr("Title: ")+editTitle->text())) {
            modified = true;
            changed = true;
        }
        else if(!m_scene->frameTexts().contains(tr("Drawn By: ")+editName->text())) {
            modified = true;
            changed = true;
        }
        else if(!m_scene->frameTexts().contains(tr("Date: ")+editDate->date().toString())) {
            modified = true;
            changed = true;
        }
        else if(!m_scene->frameTexts().contains(tr("Revision: ")+editRevision->text())) {
            modified = true;
            changed = true;
        }

        if(modified) {
            QStringList documentProperties = QStringList() <<
                tr("Title: ")+editTitle->text() << tr("Drawn By: ")+editName->text() <<
                tr("Date: ")+editDate->date().toString() <<
                tr("Revision: ")+editRevision->text();
            m_scene->undoStack()->push(new ScenePropertyChangeCmd("document properties",
                        documentProperties, m_scene->frameTexts(), m_scene));
            m_scene->setFrameTexts(documentProperties);
        }

        if(changed) {
            MainWindow::instance()->slotUpdateSettingsChanges();
            MainWindow::instance()->repaint();
        }
    }

    //! @return Icon of this page
    QIcon SchematicDocumentConfigurationPage::icon() const
    {
        return(Caneda::icon("document-properties"));
    }

    //! @return Title of this page
    QString SchematicDocumentConfigurationPage::title() const
    {
        return(tr("General", "document page title"));
    }

    //*!**************************************************
    // HDL configuration pages
    //*!**************************************************
    /*!
     * Constructor
     * @param QWidget *parent The parent of the dialog.
     */
    HdlConfigurationPage::HdlConfigurationPage(QWidget *parent) : SettingsPage(parent) {

        //First we set the color settings group of options **********************************
        QGroupBox *colorsHighlighting = new QGroupBox(tr("Colors for Syntax Highlighting"),
                this);
        QGridLayout *generalLayout = new QGridLayout(colorsHighlighting);

        keywordButton = new QPushButton(tr("Keyword"), colorsHighlighting);
        typeButton = new QPushButton(tr("Type"), colorsHighlighting);
        attributeButton = new QPushButton(tr("Attribute"), colorsHighlighting);
        blockButton = new QPushButton(tr("Block"), colorsHighlighting);
        classButton = new QPushButton(tr("Class"), colorsHighlighting);
        dataButton = new QPushButton(tr("Data"), colorsHighlighting);
        commentButton = new QPushButton(tr("Comment"), colorsHighlighting);
        systemButton = new QPushButton(tr("System"), colorsHighlighting);

        Settings *settings = Settings::instance();

        const QColor currentKeywordColor =
            settings->currentValue("gui/hdl/keyword").value<QColor>();
        const QColor currentTypeColor =
            settings->currentValue("gui/hdl/type").value<QColor>();
        const QColor currentAttributeColor =
            settings->currentValue("gui/hdl/attribute").value<QColor>();
        const QColor currentBlockColor =
            settings->currentValue("gui/hdl/block").value<QColor>();
        const QColor currentClassColor =
            settings->currentValue("gui/hdl/class").value<QColor>();
        const QColor currentDataColor =
            settings->currentValue("gui/hdl/data").value<QColor>();
        const QColor currentCommentColor =
            settings->currentValue("gui/hdl/comment").value<QColor>();
        const QColor currentSystemColor =
            settings->currentValue("gui/hdl/system").value<QColor>();

        const QColor currentBackgroundColor =
            settings->currentValue("gui/backgroundColor").value<QColor>();

        setBackgroundColor(keywordButton, currentBackgroundColor);
        setForegroundColor(keywordButton, currentKeywordColor);

        setBackgroundColor(typeButton, currentBackgroundColor);
        setForegroundColor(typeButton, currentTypeColor);

        setBackgroundColor(attributeButton, currentBackgroundColor);
        setForegroundColor(attributeButton, currentAttributeColor);

        setBackgroundColor(blockButton, currentBackgroundColor);
        setForegroundColor(blockButton, currentBlockColor);

        setBackgroundColor(classButton, currentBackgroundColor);
        setForegroundColor(classButton, currentClassColor);

        setBackgroundColor(dataButton, currentBackgroundColor);
        setForegroundColor(dataButton, currentDataColor);

        setBackgroundColor(commentButton, currentBackgroundColor);
        setForegroundColor(commentButton, currentCommentColor);

        setBackgroundColor(systemButton, currentBackgroundColor);
        setForegroundColor(systemButton, currentSystemColor);

        connect(keywordButton, SIGNAL(clicked()), SLOT(slotColorKeyword()));
        connect(typeButton, SIGNAL(clicked()), SLOT(slotColorType()));
        connect(attributeButton, SIGNAL(clicked()), SLOT(slotColorAttribute()));
        connect(blockButton, SIGNAL(clicked()), SLOT(slotColorBlock()));
        connect(classButton, SIGNAL(clicked()), SLOT(slotColorClass()));
        connect(dataButton, SIGNAL(clicked()), SLOT(slotColorData()));
        connect(commentButton, SIGNAL(clicked()), SLOT(slotColorComment()));
        connect(systemButton, SIGNAL(clicked()), SLOT(slotColorSystem()));

        generalLayout->addWidget(keywordButton, 0, 0);
        generalLayout->addWidget(typeButton, 0, 1);
        generalLayout->addWidget(attributeButton, 1, 0);
        generalLayout->addWidget(blockButton, 1, 1);
        generalLayout->addWidget(classButton, 2, 0);
        generalLayout->addWidget(dataButton, 2, 1);
        generalLayout->addWidget(commentButton, 3, 0);
        generalLayout->addWidget(systemButton, 3, 1);


        //Finally we set the general layout of all groups ***********************************
        QVBoxLayout *vlayout1 = new QVBoxLayout();
        QLabel *title_label_ = new QLabel(title());
        vlayout1->addWidget(title_label_);

        QFrame *horiz_line_ = new QFrame();
        horiz_line_->setFrameShape(QFrame::HLine);
        vlayout1->addWidget(horiz_line_);

        vlayout1->addWidget(colorsHighlighting);

        vlayout1->addStretch();

        setLayout(vlayout1);
    }

    //! Destructor
    HdlConfigurationPage::~HdlConfigurationPage() {
    }

    //! Applies the configuration of this page
    void HdlConfigurationPage::applyConf() {

        bool changed = false;
        Settings *settings = Settings::instance();

        const QColor currentKeywordColor =
            settings->currentValue("gui/hdl/keyword").value<QColor>();
        const QColor newKeywordColor = getForegroundColor(keywordButton);
        if (newKeywordColor != currentKeywordColor) {
            settings->setCurrentValue("gui/hdl/keyword", newKeywordColor);
            changed = true;
        }

        const QColor currentTypeColor =
            settings->currentValue("gui/hdl/type").value<QColor>();
        const QColor newTypeColor = getForegroundColor(typeButton);
        if (newTypeColor != currentTypeColor) {
            settings->setCurrentValue("gui/hdl/type", newTypeColor);
            changed = true;
        }

        const QColor currentAttributeColor =
            settings->currentValue("gui/hdl/attribute").value<QColor>();
        const QColor newAttributeColor = getForegroundColor(attributeButton);
        if (newAttributeColor != currentAttributeColor) {
            settings->setCurrentValue("gui/hdl/attribute", newAttributeColor);
            changed = true;
        }

        const QColor currentBlockColor =
            settings->currentValue("gui/hdl/block").value<QColor>();
        const QColor newBlockColor = getForegroundColor(blockButton);
        if (newBlockColor != currentBlockColor) {
            settings->setCurrentValue("gui/hdl/block", newBlockColor);
            changed = true;
        }

        const QColor currentClassColor =
            settings->currentValue("gui/hdl/class").value<QColor>();
        const QColor newClassColor = getForegroundColor(classButton);
        if (newClassColor != currentClassColor) {
            settings->setCurrentValue("gui/hdl/class", newClassColor);
            changed = true;
        }

        const QColor currentDataColor =
            settings->currentValue("gui/hdl/data").value<QColor>();
        const QColor newDataColor = getForegroundColor(dataButton);
        if (newDataColor != currentDataColor) {
            settings->setCurrentValue("gui/hdl/data", newDataColor);
            changed = true;
        }

        const QColor currentCommentColor =
            settings->currentValue("gui/hdl/comment").value<QColor>();
        const QColor newCommentColor = getForegroundColor(commentButton);
        if (newCommentColor != currentCommentColor) {
            settings->setCurrentValue("gui/hdl/comment", newCommentColor);
            changed = true;
        }

        const QColor currentSystemColor =
            settings->currentValue("gui/hdl/system").value<QColor>();
        const QColor newSystemColor = getForegroundColor(systemButton);
        if (newSystemColor != currentSystemColor) {
            settings->setCurrentValue("gui/hdl/system", newSystemColor);
            changed = true;
        }

        QSettings qSettings;
        settings->save(qSettings);
        if(changed) {
            MainWindow::instance()->slotUpdateSettingsChanges();
            MainWindow::instance()->repaint();
        }
    }

    //! @return Icon of this page
    QIcon HdlConfigurationPage::icon() const
    {
        return(Caneda::icon("code-context"));
    }

    //! @return Title of this page
    QString HdlConfigurationPage::title() const
    {
        return(tr("HDL", "hdl page title"));
    }

    void HdlConfigurationPage::slotColorKeyword()
    {
        QColor c = QColorDialog::getColor(
                getForegroundColor(keywordButton), this);
        if(c.isValid()) {
            setForegroundColor(keywordButton, c);
        }
    }

    void HdlConfigurationPage::slotColorType()
    {
        QColor c = QColorDialog::getColor(
                getForegroundColor(typeButton), this);
        if(c.isValid()) {
            setForegroundColor(typeButton, c);
        }
    }

    void HdlConfigurationPage::slotColorAttribute()
    {
        QColor c = QColorDialog::getColor(
                getForegroundColor(attributeButton), this);
        if(c.isValid()) {
            setForegroundColor(attributeButton, c);
        }
    }

    void HdlConfigurationPage::slotColorBlock()
    {
        QColor c = QColorDialog::getColor(
                getForegroundColor(blockButton), this);
        if(c.isValid()) {
            setForegroundColor(blockButton, c);
        }
    }

    void HdlConfigurationPage::slotColorClass()
    {
        QColor c = QColorDialog::getColor(
                getForegroundColor(classButton), this);
        if(c.isValid()) {
            setForegroundColor(classButton, c);
        }
    }

    void HdlConfigurationPage::slotColorData()
    {
        QColor c = QColorDialog::getColor(
                getForegroundColor(dataButton), this);
        if(c.isValid()) {
            setForegroundColor(dataButton, c);
        }
    }

    void HdlConfigurationPage::slotColorComment()
    {
        QColor c = QColorDialog::getColor(
                getForegroundColor(commentButton), this);
        if(c.isValid()) {
            setForegroundColor(commentButton, c);
        }
    }

    void HdlConfigurationPage::slotColorSystem()
    {
        QColor c = QColorDialog::getColor(
                getForegroundColor(systemButton), this);
        if(c.isValid()) {
            setForegroundColor(systemButton, c);
        }
    }

    //*!**************************************************
    // Layout configuration pages
    //*!**************************************************
    /*!
     * Constructor
     * @param QWidget *parent The parent of the dialog.
     */
    LayoutConfigurationPage::LayoutConfigurationPage(QWidget *parent) : SettingsPage(parent) {

        //First we set the color settings group of options **********************************
        QGroupBox *colorsLayers = new QGroupBox(tr("Layer Colors"),
                this);
        QGridLayout *generalLayout = new QGridLayout(colorsLayers);

        metal1Button = new QPushButton(tr("Metal 1"), colorsLayers);
        metal2Button = new QPushButton(tr("Metal 2"), colorsLayers);
        poly1Button = new QPushButton(tr("Poly 1"), colorsLayers);
        poly2Button = new QPushButton(tr("Poly 2"), colorsLayers);
        activeButton = new QPushButton(tr("Active"), colorsLayers);
        contactButton = new QPushButton(tr("Contact"), colorsLayers);
        nwellButton = new QPushButton(tr("N Well"), colorsLayers);
        pwellButton = new QPushButton(tr("P Well"), colorsLayers);

        Settings *settings = Settings::instance();

        const QColor currentMetal1Color =
            settings->currentValue("gui/layout/metal1").value<QColor>();
        const QColor currentMeta2Color =
            settings->currentValue("gui/layout/metal2").value<QColor>();
        const QColor currentPoly1Color =
            settings->currentValue("gui/layout/poly1").value<QColor>();
        const QColor currentPoly2Color =
            settings->currentValue("gui/layout/poly2").value<QColor>();
        const QColor currentActiveColor =
            settings->currentValue("gui/layout/active").value<QColor>();
        const QColor currentContactColor =
            settings->currentValue("gui/layout/contact").value<QColor>();
        const QColor currentNwellColor =
            settings->currentValue("gui/layout/nwell").value<QColor>();
        const QColor currentPwellColor =
            settings->currentValue("gui/layout/pwell").value<QColor>();

        const QColor currentForegroundColor =
            settings->currentValue("gui/foregroundColor").value<QColor>();

        setBackgroundColor(metal1Button, currentMetal1Color);
        setForegroundColor(metal1Button, currentForegroundColor);

        setBackgroundColor(metal2Button, currentMeta2Color);
        setForegroundColor(metal2Button, currentForegroundColor);

        setBackgroundColor(poly1Button, currentPoly1Color);
        setForegroundColor(poly1Button, currentForegroundColor);

        setBackgroundColor(poly2Button, currentPoly2Color);
        setForegroundColor(poly2Button, currentForegroundColor);

        setBackgroundColor(activeButton, currentActiveColor);
        setForegroundColor(activeButton, currentForegroundColor);

        setBackgroundColor(contactButton, currentContactColor);
        setForegroundColor(contactButton, currentForegroundColor);

        setBackgroundColor(nwellButton, currentNwellColor);
        setForegroundColor(nwellButton, currentForegroundColor);

        setBackgroundColor(pwellButton, currentPwellColor);
        setForegroundColor(pwellButton, currentForegroundColor);

        connect(metal1Button, SIGNAL(clicked()), SLOT(slotColorMetal1()));
        connect(metal2Button, SIGNAL(clicked()), SLOT(slotColorMetal2()));
        connect(poly1Button, SIGNAL(clicked()), SLOT(slotColorPoly1()));
        connect(poly2Button, SIGNAL(clicked()), SLOT(slotColorPoly2()));
        connect(activeButton, SIGNAL(clicked()), SLOT(slotColorActive()));
        connect(contactButton, SIGNAL(clicked()), SLOT(slotColorContact()));
        connect(nwellButton, SIGNAL(clicked()), SLOT(slotColorNwell()));
        connect(pwellButton, SIGNAL(clicked()), SLOT(slotColorPwell()));

        generalLayout->addWidget(metal1Button, 0, 0);
        generalLayout->addWidget(metal2Button, 0, 1);
        generalLayout->addWidget(poly1Button, 1, 0);
        generalLayout->addWidget(poly2Button, 1, 1);
        generalLayout->addWidget(activeButton, 2, 0);
        generalLayout->addWidget(contactButton, 2, 1);
        generalLayout->addWidget(nwellButton, 3, 0);
        generalLayout->addWidget(pwellButton, 3, 1);


        //Finally we set the general layout of all groups ***********************************
        QVBoxLayout *vlayout1 = new QVBoxLayout();
        QLabel *title_label_ = new QLabel(title());
        vlayout1->addWidget(title_label_);

        QFrame *horiz_line_ = new QFrame();
        horiz_line_->setFrameShape(QFrame::HLine);
        vlayout1->addWidget(horiz_line_);

        vlayout1->addWidget(colorsLayers);

        vlayout1->addStretch();

        setLayout(vlayout1);
    }

    //! Destructor
    LayoutConfigurationPage::~LayoutConfigurationPage() {
    }

    //! Applies the configuration of this page
    void LayoutConfigurationPage::applyConf() {

        bool changed = false;
        Settings *settings = Settings::instance();

        const QColor currentMetal1Color =
            settings->currentValue("gui/layout/metal1").value<QColor>();
        const QColor newMetal1Color = getBackgroundColor(metal1Button);
        if (newMetal1Color != currentMetal1Color) {
            settings->setCurrentValue("gui/layout/metal1", newMetal1Color);
            changed = true;
        }

        const QColor currentMeta2Color =
            settings->currentValue("gui/layout/metal2").value<QColor>();
        const QColor newMeta2Color = getBackgroundColor(metal2Button);
        if (newMeta2Color != currentMeta2Color) {
            settings->setCurrentValue("gui/layout/metal2", newMeta2Color);
            changed = true;
        }

        const QColor currentPoly1Color =
            settings->currentValue("gui/layout/poly1").value<QColor>();
        const QColor newPoly1Color = getBackgroundColor(poly1Button);
        if (newPoly1Color != currentPoly1Color) {
            settings->setCurrentValue("gui/layout/poly1", newPoly1Color);
            changed = true;
        }

        const QColor currentPoly2Color =
            settings->currentValue("gui/layout/poly2").value<QColor>();
        const QColor newPoly2Color = getBackgroundColor(poly2Button);
        if (newPoly2Color != currentPoly2Color) {
            settings->setCurrentValue("gui/layout/poly2", newPoly2Color);
            changed = true;
        }

        const QColor currentActiveColor =
            settings->currentValue("gui/layout/active").value<QColor>();
        const QColor newActiveColor = getBackgroundColor(activeButton);
        if (newActiveColor != currentActiveColor) {
            settings->setCurrentValue("gui/layout/active", newActiveColor);
            changed = true;
        }

        const QColor currentContactColor =
            settings->currentValue("gui/layout/contact").value<QColor>();
        const QColor newContactColor = getBackgroundColor(contactButton);
        if (newContactColor != currentContactColor) {
            settings->setCurrentValue("gui/layout/contact", newContactColor);
            changed = true;
        }

        const QColor currentNwellColor =
            settings->currentValue("gui/layout/nwell").value<QColor>();
        const QColor newNwellColor = getBackgroundColor(nwellButton);
        if (newNwellColor != currentNwellColor) {
            settings->setCurrentValue("gui/layout/nwell", newNwellColor);
            changed = true;
        }

        const QColor currentPwellColor =
            settings->currentValue("gui/layout/pwell").value<QColor>();
        const QColor newPwellColor = getBackgroundColor(pwellButton);
        if (newPwellColor != currentPwellColor) {
            settings->setCurrentValue("gui/layout/pwell", newPwellColor);
            changed = true;
        }

        QSettings qSettings;
        settings->save(qSettings);
        if(changed) {
            MainWindow::instance()->slotUpdateSettingsChanges();
            MainWindow::instance()->repaint();
        }
    }

    //! @return Icon of this page
    QIcon LayoutConfigurationPage::icon() const
    {
        return(Caneda::icon("view-grid"));
    }

    //! @return Title of this page
    QString LayoutConfigurationPage::title() const
    {
        return(tr("Layout", "layout page title"));
    }

    void LayoutConfigurationPage::slotColorMetal1()
    {
        QColor c = QColorDialog::getColor(
                getBackgroundColor(metal1Button), this);
        if(c.isValid()) {
            setBackgroundColor(metal1Button, c);
        }
    }

    void LayoutConfigurationPage::slotColorMetal2()
    {
        QColor c = QColorDialog::getColor(
                getBackgroundColor(metal2Button), this);
        if(c.isValid()) {
            setBackgroundColor(metal2Button, c);
        }
    }

    void LayoutConfigurationPage::slotColorPoly1()
    {
        QColor c = QColorDialog::getColor(
                getBackgroundColor(poly1Button), this);
        if(c.isValid()) {
            setBackgroundColor(poly1Button, c);
        }
    }

    void LayoutConfigurationPage::slotColorPoly2()
    {
        QColor c = QColorDialog::getColor(
                getBackgroundColor(poly2Button), this);
        if(c.isValid()) {
            setBackgroundColor(poly2Button, c);
        }
    }

    void LayoutConfigurationPage::slotColorActive()
    {
        QColor c = QColorDialog::getColor(
                getBackgroundColor(activeButton), this);
        if(c.isValid()) {
            setBackgroundColor(activeButton, c);
        }
    }

    void LayoutConfigurationPage::slotColorContact()
    {
        QColor c = QColorDialog::getColor(
                getBackgroundColor(contactButton), this);
        if(c.isValid()) {
            setBackgroundColor(contactButton, c);
        }
    }

    void LayoutConfigurationPage::slotColorNwell()
    {
        QColor c = QColorDialog::getColor(
                getBackgroundColor(nwellButton), this);
        if(c.isValid()) {
            setBackgroundColor(nwellButton, c);
        }
    }

    void LayoutConfigurationPage::slotColorPwell()
    {
        QColor c = QColorDialog::getColor(
                getBackgroundColor(pwellButton), this);
        if(c.isValid()) {
            setBackgroundColor(pwellButton, c);
        }
    }

} // namespace Caneda
