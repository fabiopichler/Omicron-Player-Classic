/*******************************************************************************

  Omicron Player Classic

  Author: Fábio Pichler
  Website: http://fabiopichler.net
  License: BSD 3-Clause License

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of Omicron Player Classic nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "Database.h"

#include <iostream>

QSqlDatabase *Database::db = nullptr;

bool Database::init(const QString &name)
{
    bool _default = !QFile::exists(name);

    if ((db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"))))
    {
        db->setDatabaseName(name);

        if (db->open())
        {
            db->exec("PRAGMA journal_mode = OFF");
            db->exec("PRAGMA synchronous = OFF");

            if (_default)
                defaultConfig();

            return true;
        }
        else
        {
            free();
        }
    }

    stdCout("Error! Data base failed");
    return false;
}

void Database::free()
{
    if (db)
    {
        if (db->isOpen())
            db->close();

        delete db;
    }

    db = nullptr;
}

//================================================================================================================
// Values
//================================================================================================================

bool Database::add(const QString &table, const QString &id, const QVariant &value)
{
    if (!db) return false;

    QSqlQuery query(*db);

    query.exec(QString("CREATE TABLE IF NOT EXISTS %1 (id TEXT PRIMARY KEY, value TEXT)").arg(table));
    query.prepare(QString("INSERT INTO %1 (id, value) VALUES (:id, :value)").arg(table));

    query.bindValue(":id", id);
    query.bindValue(":value", value.toString());

    return query.exec();
}

bool Database::setValue(const QString &table, const QString &id, const QVariant &value)
{
    if (!db) return false;

    QSqlQuery query(*db);

    query.prepare(QString("UPDATE %1 SET value = :value WHERE id = :id").arg(table));

    query.bindValue(":value", value.toString());
    query.bindValue(":id", id);

    return query.exec();
}

bool Database::setValues(const QString &table, const QVariantList &ids, const QVariantList &values)
{
    if (!db) return false;

    QSqlQuery query(*db);
    query.prepare(QString("UPDATE %1 SET value = :value WHERE id = :id").arg(table));

    query.bindValue(":value", values);
    query.bindValue(":id", ids);

    if (!query.execBatch())
        qDebug() << query.lastError();

    return true;
}

QVariant Database::value(const QString &table, const QString &id, const QVariant &defValue)
{
    QVariant value;
    if (!db) return value;

    QSqlQuery query(*db);
    query.exec(QString("SELECT value FROM %1 WHERE id = '%2'").arg(table, id));

    if (query.next())
        value = query.value(0);

    return (!value.toString().isEmpty() ? value : defValue);
}

bool Database::exist(const QString &table, const QString &id)
{
    if (!db) return false;

    QSqlQuery query(*db);
    query.exec(QString("SELECT id FROM %1 WHERE id = '%2'").arg(table, id));

    if (query.next())
        return !query.value(0).toString().isEmpty();

    return false;
}

//================================================================================================================
// Remove
//================================================================================================================

bool Database::clear(const QString &table, const QString &id)
{
    return setValue(table, id, QVariant());
}

bool Database::remove(const QString &table, const QString &col, const QString &row)
{
    if (!db) return false;
    return QSqlQuery(*db).exec("DELETE FROM " + table + " WHERE " + col + " = '" + row + "'");
}

bool Database::removeRows(const QString &table)
{
    if (!db) return false;
    return QSqlQuery(*db).exec("DELETE FROM " + table);
}

bool Database::removeTable(const QString &table)
{
    if (!db) return false;
    return QSqlQuery(*db).exec("DROP TABLE " + table);
}

bool Database::removeDatabase()
{
    if (!db) return false;

    QString dbName = db->databaseName();
    free();

    if (QFile::exists(dbName))
        return QFile::remove(dbName);

    return true;
}

//================================================================================================================
// Favorites
//================================================================================================================

bool Database::addMusicFavorite(QString value)
{
    if (!db) return false;

    QSqlQuery query(*db);

    query.prepare("INSERT INTO MusicFavorites (path) VALUES (:path)");
    query.bindValue(":path", value.replace("'","&#39;"));

    return query.exec();
}

QStringList Database::getMusicFavorites()
{
    if (!db) return QStringList();

    QSqlQuery query(*db);
    QStringList list;

    query.exec("SELECT path FROM MusicFavorites");

    while (query.next())
        list << query.value(0).toString().replace("&#39;","'");

    return list;
}

bool Database::addRadioFavorite(QStringList list)
{
    if (!db) return false;

    QSqlQuery query(*db);

    query.prepare("INSERT INTO RadioFavorites (title, genre, contry, url1, url2, url3, url4, url5) VALUES "
                          "(:title, :genre, :contry, :url1, :url2, :url3, :url4, :url5)");

    query.bindValue(":title", list[0].replace("'","&#39;"));
    query.bindValue(":genre", list[1].replace("'","&#39;"));
    query.bindValue(":contry", list[2].replace("'","&#39;"));
    query.bindValue(":url1", list[3].replace("'","&#39;"));
    query.bindValue(":url2", list[4].replace("'","&#39;"));
    query.bindValue(":url3", list[5].replace("'","&#39;"));
    query.bindValue(":url4", list[6].replace("'","&#39;"));
    query.bindValue(":url5", list[7].replace("'","&#39;"));

    return query.exec();
}

QList<QStringList> Database::getRadioFavorites()
{
    if (!db) return QList<QStringList>();

    QSqlQuery query(*db);
    QList<QStringList> list;

    query.exec("SELECT * FROM RadioFavorites");

    while (query.next())
    {
        QStringList stringList;

        for (int i = 0; i < 8; i++)
            stringList << query.value(i).toString().replace("&#39;","'");

        list << stringList;
    }

    return list;
}

//================================================================================================================
// Equalizer
//================================================================================================================

bool Database::addEqualizerPreset(const QString &title, const std::vector<int> &values)
{
    if (!db) return false;

    QSqlQuery query(*db);

    query.prepare("INSERT INTO EqualizerPreset "
                  "(title, eq0, eq1, eq2, eq3, eq4, eq5, eq6, eq7, eq8, eq9, eq10, eq11, eq12, eq13, "
                  "eq14, eq15) VALUES (:title, :eq0, :eq1, :eq2, :eq3, :eq4, :eq5, :eq6, :eq7, :eq8, :eq9, "
                  ":eq10, :eq11, :eq12, :eq13, :eq14, :eq15)");

    query.bindValue(":title", title);

    for (int i = 0; i < 16; i++)
        query.bindValue(QString(":eq%1").arg(i), values[i]);

    return query.exec();
}

bool Database::setEqualizerValues(const QString &values)
{
    if (!db) return false;

    QSqlQuery query(*db);
    query.exec("DELETE FROM CurrentEqualizer");
    return query.exec("INSERT INTO CurrentEqualizer VALUES " + values);
}

bool Database::setEqualizerPreset(const int &id, const QString &title, const std::vector<int> &values)
{
    if (!db) return false;

    QSqlQuery query(*db);

    query.prepare(QString("UPDATE EqualizerPreset SET title = :title, eq0 = :eq0, eq1 = :eq1, eq2 = :eq2, eq3 = :eq3, "
                          "eq4 = :eq4, eq5 = :eq5, eq6 = :eq6, eq7 = :eq7, eq8 = :eq8, eq9 = :eq9, eq10 = :eq10, "
                          "eq11 = :eq11, eq12 = :eq12, eq13 = :eq13, eq14 = :eq14, eq15 = :eq15 "
                          "WHERE id = %1").arg(id));

    query.bindValue(":title", title);

    for (int i = 0; i < 16; i++)
        query.bindValue(QString(":eq%1").arg(i), values[i]);

    return query.exec();
}

std::vector<int> Database::getEqualizerPreset(const int &id)
{
    if (!db) return std::vector<int>();

    QSqlQuery query(*db);
    std::vector<int> list;

    query.exec(QString("SELECT * FROM EqualizerPreset WHERE id = %1").arg(id));

    if (query.next())
    {
        for (int i = 2; i < 18; i++)
            list.push_back(query.value(i).toInt());
    }

    return list;
}

int Database::getEqualizerPresets(std::vector<int> &ids, std::vector<std::string> &titles)
{
    if (!db) return 0;

    QSqlQuery query(*db);
    int count = 0;

    query.exec(QString("SELECT id, title FROM EqualizerPreset"));

    while (query.next())
    {
        ids.push_back(query.value(0).toInt());
        titles.push_back(query.value(1).toString().toStdString());
        count++;
    }

    return count;
}

//================================================================================================================
// Others
//================================================================================================================

void Database::defaultConfig()
{
    if (!db) return;

    QSqlQuery query(*db);

    query.exec("CREATE TABLE Version (id TEXT PRIMARY KEY, value TEXT)");
    query.exec("INSERT INTO Version VALUES "
               "('current', '0'), "
               "('updates_check', '1'), "
               "('updates_lastCheck', '0000-00-00')");

    query.exec("CREATE TABLE Config (id TEXT PRIMARY KEY, value TEXT)");
    query.exec("INSERT INTO Config VALUES "
               "('theme', ''), "
               "('style', 'default'), "
               "('device', '-1'), "
               "('autoDlRadioList', 'true'), "
               "('autoPlay', 'false'), "
               "('errorNotification', 'dialog')");

    query.exec("CREATE TABLE MusicConfig (id TEXT PRIMARY KEY, value TEXT)");
    query.exec("INSERT INTO MusicConfig VALUES "
               "('fadeIn', '2'), "
               "('fadeOut', '2'), "
               "('notifiSysTray', 'false'), "
               "('continuePlaying', 'false'), "
               "('continuePlayingTab', 'false'), "
               "('automaticGainControl', 'true'), "
               "('allowAnyFile', 'false')");

    query.exec("CREATE TABLE RadioConfig (id TEXT PRIMARY KEY, value TEXT)");
    query.exec("INSERT INTO RadioConfig VALUES "
               "('fadeIn', '3'), "
               "('fadeOut', '3'), "
               "('notifiSysTray', 'true'), "
               "('recordPath', ''), "
               "('recordSubDir', 'true'), "
               "('reconnectionMode', '1'), "
               "('automaticGainControl', 'true'), "
               "('net_readtimeout', '20000'), "
               "('net_timeout', '20000'), "
               "('net_proxy', '0')");

    query.exec("CREATE TABLE Current (id TEXT PRIMARY KEY, value TEXT)");
    query.exec("INSERT INTO Current VALUES "
               "('mode', 'Music'), "
               "('windowPosition', ''), "
               "('fileDialog', ''), "
               "('fileDialogDir', ''), "
               "('fileDialogPl', ''), "
               "('volume', '100')");

    query.exec("CREATE TABLE MusicMode (id TEXT PRIMARY KEY, value TEXT)");
    query.exec("INSERT INTO MusicMode VALUES "
               "('playlist', ''), "
               "('playlistMode', '1'), "
               "('indexPlaylist', '0'), "
               "('indexMusics', '0'), "
               "('indexFavorites', '0'), "
               "('soundPosition', '0'), "
               "('repeat', '0'), "
               "('random', '0')");

    query.exec("CREATE TABLE RadioMode (id TEXT PRIMARY KEY, value TEXT)");
    query.exec("INSERT INTO RadioMode VALUES "
               "('playlist', ''), "
               "('playlistMode', '0'), "
               "('indexAll', '0'), "
               "('indexCustom', '0'), "
               "('indexFavorites', '0'), "
               "('quick-link', '')");

    query.exec("CREATE TABLE RecorderMode (id TEXT PRIMARY KEY, value TEXT)");
    query.exec("INSERT INTO RecorderMode VALUES "
               "('device', '-1'), "
               "('encoder', '0'), "
               "('aac', '8'), "
               "('ogg', '6'), "
               "('mp3', '8')");

    query.exec("CREATE TABLE MusicFavorites (path TEXT PRIMARY KEY)");

    query.exec("CREATE TABLE RadioFavorites (title TEXT PRIMARY KEY, genre TEXT, contry TEXT, "
               "url1 TEXT, url2 TEXT, url3 TEXT, url4 TEXT, url5 TEXT)");

    query.exec("CREATE TABLE IF NOT EXISTS EqualizerConfig (id TEXT PRIMARY KEY, value TEXT)");
    query.exec("INSERT INTO EqualizerConfig VALUES "
               "('EqualizerPreset', '0')");

    query.exec("CREATE TABLE IF NOT EXISTS EqualizerPreset (id INTEGER PRIMARY KEY AUTOINCREMENT, title TEXT, "
               "eq0 INTEGER, eq1 INTEGER, eq2 INTEGER, eq3 INTEGER, eq4 INTEGER, eq5 INTEGER, eq6 INTEGER, eq7 INTEGER, "
               "eq8 INTEGER, eq9 INTEGER, eq10 INTEGER, eq11 INTEGER, eq12 INTEGER, eq13 INTEGER, eq14 INTEGER, eq15 INTEGER)");

    query.exec("CREATE TABLE CurrentEqualizer (id INTEGER PRIMARY KEY, value INTEGER)");
    query.exec("INSERT INTO CurrentEqualizer VALUES "
               "(0, 0), "
               "(1, 0), "
               "(2, 0), "
               "(3, 0), "
               "(4, 0), "
               "(5, 0), "
               "(6, 0), "
               "(7, 0), "
               "(8, 0), "
               "(9, 0), "
               "(10, 0), "
               "(11, 0), "
               "(12, 0), "
               "(13, 0), "
               "(14, 0), "
               "(15, 0), "
               "(16, 0), "
               "(17, 0), "
               "(18, 0)");
}

void Database::upgrade()
{
}

