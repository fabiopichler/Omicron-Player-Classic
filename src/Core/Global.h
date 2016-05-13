/*******************************************************************************
  Tchê Media Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "../Version.h"
#include <qstring.h>
#include <qobject.h>
#include <QApplication>
#include <qstandardpaths.h>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#define Information(parent, text) QMessageBox::information(parent, "Informação", text)
#define Warning(parent, text) QMessageBox::warning(parent, "Aviso", text)

#ifdef Q_OS_WIN
    #define strncasecmp _strnicmp
    #define stdCout(str) std::wcout << QString(str).toStdWString() << std::endl
#else
    #define _strdup strdup
    #define stdCout(str) std::cout << QString(str).toStdString() << std::endl
#endif

enum
{
    DIALOG
};

const QString AppName = APP_NAME,
              CurrentVersion = PRODUCT_VERSION_STR,
              FileRadiolist = FILE_RADIOLIST,
              FileRadiolistDate = FILE_RADIOLIST_DATE,
              PathAudioPlugins = "plugins/audio/",
              AppConfigDir = "Tche-Media-Player",
              AppNameId = "Tche-Media-Player",
              PlaylistExt = "tmpp", // Tchê Media Player Playlist
              RadioPlaylistExt = "tmpr", // Tchê Media Player Radio list

#ifdef Q_OS_WIN
              FileNameFilter = "*.mp3 *.wma *.ogg *.wav *.aac *.mp2 *.mp1 *.ac3 *.m4a *.m4r *.aiff *.aif *.flac *.ape *.wv *.opus *.mo3 *.xm *.mod *.s3m *.it *.mtm *.umx",
              bass_aac = "bass_aac.dll",
              bass_ac3 = "bass_ac3.dll",
              bass_ape = "bass_ape.dll",
              bassflac = "bassflac.dll",
              basswma  = "basswma.dll",
              basswv   = "basswv.dll",
              bassopus = "bassopus.dll",
#elif defined(Q_OS_LINUX)
              FileNameFilter = "*.mp3 *.ogg *.wav *.aac *.mp2 *.mp1 *.ac3 *.m4a *.m4r *.aiff *.aif *.flac *.ape *.wv *.opus *.mo3 *.xm *.mod *.s3m *.it *.mtm *.umx",
              bass_aac = "libbass_aac.so",
              bass_ac3 = "libbass_ac3.so",
              bass_ape = "libbass_ape.so",
              bassflac = "libbassflac.so",
              basswma  = "",
              basswv   = "libbasswv.so",
              bassopus = "libbassopus.so",
#else
              FileNameFilter = "",
              bass_aac = "",
              bass_ac3 = "",
              bass_ape = "",
              bassflac = "",
              basswma  = "",
              basswv   = "",
              bassopus = "",
#endif // Q_OS_WIN

              CheckUpdate = "http://software.fabiopichler.net/updates/tche-media-player/check.php",
              OfficialSite = OFFICIAL_WEBSITE,
              DownloadUpdate = "http://fabiopichler.net/download/tche-media-player/";

class Global
{
public:
    static bool init(const int &);
    static void setupSupportedFiles();

    static QString getConfigPath(const QString &arg = QString());
    static QString getAppPath(const QString &arg = QString());
    static QString getQrcPath(const QString &arg = QString());
    static QString getRadiolistPath();
    static QString getThemePath(const QString &arg = QString());
    static QString getErrorHtml(const QString &arg = QString(), const QString &arg2 = QString());
    static QString getErrorText(const QString &arg = QString(), const QString &arg2 = QString());

    static QString regExp(const QString &, const QString &);
    static QString cStrToQString(const char *);

    static void setRadiolistName(const QString &);
    static void setThemePath(const QString &);

    static int argc;
    static bool inDevelopment;
    static QList<int> equalizerValues;
    static QList<QStringList> supportedFiles;

private:
    static QString configPath, appPath, qrcPath, themePath, radiolistPath;
};