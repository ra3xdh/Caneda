/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Thu Aug 28 18:17:41 CEST 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <qapplication.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qfont.h>

#include "qucsedit.h"


struct tQucsSettings {
  int x, y, dx, dy;    // position and size of main window
  QFont font;
};

tQucsSettings QucsSettings
     = {200, 100, 400, 400,    // position and size
	QFont("Helvetica", 12)};

// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings()
{
  QFile file(QDir::homeDirPath()+"/.qucs/editrc");
  if(!file.open(IO_ReadOnly)) return false; // settings file doesn't exist

  QTextStream stream(&file);
  QString Line, Setting;

  while(!stream.atEnd()) {
    Line = stream.readLine();
    Setting = Line.section('=',0,0);
    Line    = Line.section('=',1,1);
    if(Setting == "EditWindow") {
	QucsSettings.x  = Line.section(",",0,0).toInt();
	QucsSettings.y  = Line.section(",",1,1).toInt();
	QucsSettings.dx = Line.section(",",2,2).toInt();
	QucsSettings.dy = Line.section(",",3,3).toInt();
	break; }
  }
  file.close();

  file.setName(QDir::homeDirPath()+"/.qucs/qucsrc");
  if(!file.open(IO_ReadOnly)) return true; // qucs settings not necessary

  while(!stream.atEnd()) {
    Line = stream.readLine();
    Setting = Line.section('=',0,0);
    Line    = Line.section('=',1,1);
    if(Setting == "Font") {     // get font from "qucsrc"
	QucsSettings.font.fromString(Line);
	break;
    }
  }
  file.close();

  return true;
}

// #########################################################################
// Saves the settings in the settings file.
bool saveApplSettings(QucsEdit *qucs)
{
  if(qucs->x() == QucsSettings.x)
    if(qucs->y() == QucsSettings.y)
      if(qucs->width() == QucsSettings.dx)
	if(qucs->height() == QucsSettings.dy)
	  return true;   // nothing has changed


  QFile file(QDir::homeDirPath()+"/.qucs/editrc");
  if(!file.open(IO_WriteOnly)) {
    QMessageBox::warning(0, QObject::tr("Warning"),
			QObject::tr("Cannot save settings !"));
    return false;
  }

  QString Line;
  QTextStream stream(&file);

  stream << "Settings file, Qucs Editor " PACKAGE_VERSION "\n"
	 << "EditWindow=" << qucs->x() << ',' << qucs->y() << ','
	 << qucs->width() << ',' << qucs->height() << '\n';

  file.close();
  return true;
}


// #########################################################################
// ##########                                                     ##########
// ##########                  Program Start                      ##########
// ##########                                                     ##########
// #########################################################################

int main(int argc, char *argv[])
{
  loadSettings();

  QApplication a(argc, argv);
  a.setFont(QucsSettings.font);

  bool readOnly = false;
  QString FileName;
  if(argc > 0)  FileName = argv[1];

  QTranslator tor( 0 );
  tor.load( QString("qucs_") + QTextCodec::locale(), LANGUAGEDIR );
  a.installTranslator( &tor );

  QucsEdit *qucs = new QucsEdit(FileName, readOnly);
  a.setMainWidget(qucs);
  qucs->resize(QucsSettings.dx, QucsSettings.dy); // size and position ...
  qucs->move(QucsSettings.x, QucsSettings.y);     // ... before "show" !!!
  qucs->show();
  int result = a.exec();
  saveApplSettings(qucs);
  return result;
}