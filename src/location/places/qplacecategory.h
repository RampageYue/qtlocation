/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QPLACECATEGORY_H
#define QPLACECATEGORY_H

#include <QSharedDataPointer>
#include <QString>
#include <QVariant>
#include <QList>
#include "qplacealternativevalue.h"

QT_BEGIN_NAMESPACE

class QPlaceManagerEngine;

class QPlaceCategoryPrivate;
class Q_LOCATION_EXPORT QPlaceCategory
{
public:
    QPlaceCategory();
    QPlaceCategory(const QPlaceCategory &other);

    virtual ~QPlaceCategory();

    QPlaceCategory &operator=(const QPlaceCategory &other);

    bool operator==(const QPlaceCategory &other) const;
    bool operator!=(const QPlaceCategory &other) const {
        return !(other == *this);
    }

    QList<QPlaceAlternativeValue> alternativeNames() const;
    void setAlternativeNames(const QList<QPlaceAlternativeValue> &names);
    QString categoryId() const;
    void setCategoryId(const QString &catID);
    QString description() const;
    void setDescription(const QString &description);
    QString name() const;
    void setName(const QString &name);

private:
    QSharedDataPointer<QPlaceCategoryPrivate> d;
};

QT_END_NAMESPACE

#endif // QPLACECATEGORY_H
