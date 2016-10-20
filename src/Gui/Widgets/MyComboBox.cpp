/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "MyComboBox.h"
#include <QListView>

MyComboBox::MyComboBox(QWidget *parent) : QComboBox(parent)
{
    QListView *listView = new QListView;
    listView->setObjectName("ComboBox");
    setView(listView);
}