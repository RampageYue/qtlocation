/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPLACEMANAGER_P_H
#define QPLACEMANAGER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmobilityglobal.h"
#include "qplacemanager.h"
#include <QHash>
#include <QString>
#include <QMap>

QT_BEGIN_NAMESPACE

class QPlaceManagerEngine;
class QPlaceManagerEngineFactory;

class QPlaceManagerPrivate
{
public:
    QPlaceManagerPrivate();
    void createEngine(const QString &managerName, const QMap<QString,QString> &parameters = (QMap<QString, QString>()));

    QPlaceManagerEngine *engine;
    mutable QPlaceManager::Error errorCode;
    mutable QString errorString;
    mutable QMap<int, QPlaceManager::Error> errorMap;
    static QPlaceManagerEngine *getEngine(const QPlaceManager* manager);
    static void loadFactories();
    static QHash<QString, QPlaceManagerEngineFactory*> factories(bool reload = false);
    static  void loadDynamicFactories (QHash<QString, QPlaceManagerEngineFactory*> *factories);
    static  void loadStaticFactories (QHash<QString, QPlaceManagerEngineFactory*> *factories);
    bool isConnected;//identifies whether connections have been made to the notification signals

    QPlaceManager *q_ptr;
    Q_DECLARE_PUBLIC(QPlaceManager)
};

QT_END_NAMESPACE

#endif // QPLACEMANAGER_P_H
