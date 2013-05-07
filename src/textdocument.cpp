/***************************************************************************
 * Copyright (C) 2010 by Gopala Krishna A <krishna.ggk@gmail.com>          *
 * Copyright (C) 2010-2013 by Pablo Daniel Pareja Obregon                  *
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

#include "textdocument.h"

#include "documentviewmanager.h"
#include "syntaxhighlighters.h"
#include "textcontext.h"
#include "textedit.h"
#include "textview.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QTextCodec>
#include <QTextDocument>
#include <QTextStream>
#include <QUndoStack>

namespace Caneda
{
    //! \brief Constructor.
    TextDocument::TextDocument()
    {
        m_textDocument = new QTextDocument;
        m_textDocument->setModified(false);

        connect(m_textDocument, SIGNAL(undoAvailable(bool)),
                this, SLOT(emitDocumentChanged()));
        connect(m_textDocument, SIGNAL(redoAvailable(bool)),
                this, SLOT(emitDocumentChanged()));
        connect(m_textDocument, SIGNAL(modificationChanged(bool)),
                this, SLOT(emitDocumentChanged()));
        connect(m_textDocument, SIGNAL(contentsChanged()),
                this, SLOT(onContentsChanged()));
    }

    //! \brief Destructor.
    TextDocument::~TextDocument()
    {
        delete m_textDocument;
    }

    IContext* TextDocument::context()
    {
        return TextContext::instance();
    }

    bool TextDocument::isModified() const
    {
        return m_textDocument->isModified();
    }

    bool TextDocument::canUndo() const
    {
        return m_textDocument->isUndoAvailable();
    }

    bool TextDocument::canRedo() const
    {
        return m_textDocument->isRedoAvailable();
    }

    void TextDocument::undo()
    {
        m_textDocument->undo();
    }

    void TextDocument::redo()
    {
        m_textDocument->redo();
    }

    QUndoStack* TextDocument::undoStack()
    {
        QUndoStack *stack = new QUndoStack(this);
        return stack;
    }

    void TextDocument::cut()
    {
        TextEdit *te = activeTextEdit();
        if (!te) {
            return;
        }
        te->cut();
    }

    void TextDocument::copy()
    {
        TextEdit *te = activeTextEdit();
        if (!te) {
            return;
        }
        te->copy();
    }

    void TextDocument::paste()
    {
        TextEdit *te = activeTextEdit();
        if (!te) {
            return;
        }
        te->paste();
    }

    void TextDocument::selectAll()
    {
        TextEdit *te = activeTextEdit();
        if (!te) {
            return;
        }
        te->selectAll();
    }

    void TextDocument::intoHierarchy()
    {
        //! \todo Implement this. This should open currently selected file.
    }

    void TextDocument::popHierarchy()
    {
        //! \todo Implement this. This should return to previously opened file.
    }

    /*!
     * \brief Start a simulation
     *
     * Start a simulation, invoking the correct simulator depending on the
     * file extension, and then open the waveform viewer (could be internal
     * or external acording to the user settings).
     */
    void TextDocument::simulate()
    {
        simulationErrorStatus = false;

        QFileInfo info(fileName());
        QString baseName = info.completeBaseName();
        QString suffix = info.suffix();
        QString path = info.path();

        QProcess *simulationProcess = new QProcess(this);
        simulationProcess->setWorkingDirectory(path);
        simulationProcess->setProcessChannelMode(QProcess::MergedChannels);  // Output std:error and std:output together into the same file
        simulationProcess->setStandardOutputFile(path + "/" + baseName + ".log", QIODevice::Append);  // Create a log file

        connect(simulationProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(simulationLog(int)));


        if (suffix == "net" || suffix == "cir" || suffix == "spc" || suffix == "sp") {
            // It is a netlist file, we should invoke a spice simulator in batch mode

            // Set the environment variable to get an ascii raw file instead of a binary one
            //! \todo Add an option to generate binary raw files to save disk space.
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            env.insert("SPICE_ASCIIRAWFILE", "1"); // Add an environment variable
            simulationProcess->setProcessEnvironment(env);

            // If using ngspice simulator, the command to simulate is:
            // ngspice -b -r output.raw input.net
            simulationProcess->start(QString("ngspice -b -r ") + baseName + ".raw "
                                     + fileName());  // Analize the file
        }
        else if (suffix == "vhd" || suffix == "vhdl") {
            // It is a vhdl file, we should invoke ghdl simulator
            /*! \todo Here we should analize (ghdl -a) all included files of the vhdl project.
             *  This could be done in a recursive way (although that is not optimal). That is,
             *  try to compile all files in the directory. At least of the files will compile,
             *  as it does not depend on the others. Then add that file to a list, and compile
             *  the rest. Again add the files that compile and go on until no file is left to
             *  compile.
             */

            simulationProcess->start(QString("ghdl -a ") + fileName());  // Analize the files
            simulationProcess->waitForFinished();
            simulationProcess->start(QString("ghdl -e ") + baseName);  // Create the simulation
            simulationProcess->waitForFinished();
            simulationProcess->start(QString("./") + baseName + " --wave=waveforms.ghw");  // Run the simulation
        }
        else if (suffix == "v") {
            // Is is a verilog file, we should invoke iverilog
            simulationProcess->start(QString("iverilog ") + fileName());  // Analize the files
            simulationProcess->waitForFinished();
            simulationProcess->start(QString("./a.out"));  // Run the simulation
        }

        // The simulation results are opened in the simulationReady slot, to achieve non-modal simulations
        connect(simulationProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(simulationReady(int)));
    }

    void TextDocument::print(QPrinter *printer, bool fitInView)
    {
        Q_UNUSED(fitInView);

        m_textDocument->print(printer);
    }

    QSizeF TextDocument::documentSize()
    {
        return m_textDocument->size();
    }

    bool TextDocument::load(QString *errorMessage)
    {
        if (fileName().isEmpty()) {
            if (errorMessage) {
                *errorMessage = tr("Empty filename");
            }
            return false;
        }

        QFile file(fileName());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            if (errorMessage) {
                *errorMessage = tr("Could not open file for reading");
            }
            return false;
        }

        QTextStream stream(&file);
        stream.setCodec(QTextCodec::codecForName("UTF-8"));

        QString content = stream.readAll();
        m_textDocument->setPlainText(content);

        file.close();

        QFileInfo fileInfo(fileName());
        if ( fileInfo.suffix() == "vhdl" || fileInfo.suffix() == "vhd" ) {
            VhdlHighlighter *highlighter = new VhdlHighlighter(m_textDocument);
        }
        else if ( fileInfo.suffix() == "v" ) {
            VerilogHighlighter *highlighter = new VerilogHighlighter(m_textDocument);
        }
        else if ( fileInfo.suffix() == "net" ||
                  fileInfo.suffix() == "cir" ||
                  fileInfo.suffix() == "spc" ) {
            SpiceHighlighter *highlighter = new SpiceHighlighter(m_textDocument);
        }

        m_textDocument->setModified(false);
        return true;
    }

    bool TextDocument::save(QString *errorMessage)
    {
        if (fileName().isEmpty()) {
            if (errorMessage) {
                *errorMessage = tr("Empty filename");
            }
            return false;
        }

        QFile file(fileName());
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            if (errorMessage) {
                *errorMessage = tr("Could not open file for writing");
            }
            return false;
        }

        QTextStream stream(&file);
        stream.setCodec(QTextCodec::codecForName("UTF-8"));

        stream << m_textDocument->toPlainText();

        file.close();

        m_textDocument->setModified(false);
        return true;
    }

    IView* TextDocument::createView()
    {
        return new TextView(this);
    }

    void TextDocument::pasteTemplate(const QString& text)
    {
        TextEdit *te = activeTextEdit();
        if (!te) {
            return;
        }
        te->insertPlainText(text);
    }

    TextEdit* TextDocument::activeTextEdit()
    {
        IView *view = DocumentViewManager::instance()->currentView();
        TextView *active = qobject_cast<TextView*>(view);

        if (active) {
            return active->textEdit();
        }

        return 0;
    }

    void TextDocument::onContentsChanged()
    {
        if (!m_textDocument->isModified()) {
            m_textDocument->setModified(true);
        }
    }

    /*!
     * \brief Open simulation results.
     *
     * Once a simulation is started, the simulation process is connected to
     * this slot, to achive non-modal simulations (ie, the gui is responsive
     * during simulations).
     *
     * Once the simulation has finished, this slot is invoked and if no error
     * ocurred, simulation results are shown to the user. The waveform viewer
     * can be internal or external acording to the user settings.
     */
    void TextDocument::simulationReady(int error)
    {
        // If there was any error during the process, do not display the waveforms
        if(error | simulationErrorStatus) {
            return;
        }

        QFileInfo info(fileName());
        QString path = info.path();
        QString baseName = info.completeBaseName();
        QString suffix = info.suffix();

        QProcess *simulationProcess = new QProcess(this);
        simulationProcess->setWorkingDirectory(path);
        simulationProcess->setProcessChannelMode(QProcess::MergedChannels);  // Output std:error and std:output together into the same file
        simulationProcess->setStandardOutputFile(path + "/" + baseName + ".log", QIODevice::Append);  // Create a log file
        connect(simulationProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(simulationLog(int)));

        // Open the resulting waveforms
        if (suffix == "net" || suffix == "cir" || suffix == "spc" || suffix == "sp") {
            DocumentViewManager *manager = DocumentViewManager::instance();
            manager->openFile(QDir::toNativeSeparators(path + "/" + baseName + ".raw"));
        }
        else if (suffix == "vhd" || suffix == "vhdl") {
            simulationProcess->start(QString("gtkwave waveforms.ghw"));  // Open the waveforms
        }
        else if (suffix == "v") {
            simulationProcess->start(QString("gtkwave ") + baseName + ".vcd");  // Open the waveforms
        }
    }

    /*!
     * \brief Test for errors, and open log file (in case something went
     * wrong).
     *
     * This method is called whenever simulationProcess emits the finished
     * signal, to keep track of the different logs available.
     */
    void TextDocument::simulationLog(int error)
    {
        QFileInfo info(fileName());
        QString path = info.path();
        QString baseName = info.completeBaseName();

        // If there was any error during the process, open the log
        if(error) {
            simulationErrorStatus = true;  // This variable is used in multistep simulations (eg. verilog) to avoid opening previously generated waveforms

            DocumentViewManager *manager = DocumentViewManager::instance();
            manager->openFile(QDir::toNativeSeparators(path + "/" + baseName + ".log"));
        }

    }

} // namespace Caneda
