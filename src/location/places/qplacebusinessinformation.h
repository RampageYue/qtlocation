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

#ifndef QPLACEBUSINESSINFORMATION_H
#define QPLACEBUSINESSINFORMATION_H

#include <QSharedDataPointer>
#include <QString>
#include <QStringList>
#include <QList>
#include <QVariant>

#include "qplacebusinessfeature.h"
#include "qplaceperiod.h"
#include "qplaceweekdayhours.h"

QT_BEGIN_NAMESPACE

class QPlaceBusinessInformationPrivate;

class Q_LOCATION_EXPORT QPlaceBusinessInformation
{
public:
    QPlaceBusinessInformation();
    QPlaceBusinessInformation(const QPlaceBusinessInformation &other);

    virtual ~QPlaceBusinessInformation();

    QPlaceBusinessInformation &operator=(const QPlaceBusinessInformation &other);

    bool operator==(const QPlaceBusinessInformation &other) const;
    bool operator!=(const QPlaceBusinessInformation &other) const {
        return !(other == *this);
    }

    QVariantHash additionalData() const;
    void setAdditionalData(const QVariantHash &data);
    QList<QPlacePeriod> annualClosings() const;
    void setAnnualClosings(const QList<QPlacePeriod> &data);
    QList<QPlaceWeekdayHours> openingHours() const;
    void setOpeningHours(const QList<QPlaceWeekdayHours> &data);
    QString annualClosingNote() const;
    void setAnnualClosingNote(const QString &data);
    QList<QPlaceBusinessFeature> features() const;
    void setFeatures(const QList<QPlaceBusinessFeature> &data);
    QString openingNote() const;
    void setOpeningNote(const QString &data);
    QStringList paymentMethods() const;
    void setPaymentMethods(const QStringList &data);

private:
    QSharedDataPointer<QPlaceBusinessInformationPrivate> d;
};

QT_END_NAMESPACE


#endif // QPLACEBUSINESSINFORMATION_H
