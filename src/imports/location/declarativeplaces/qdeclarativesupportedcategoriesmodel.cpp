/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativesupportedcategoriesmodel_p.h"
#include "qdeclarativeplaceicon_p.h"
#include "qgeoserviceprovider.h"
#include "error_messages.h"

#include <QCoreApplication>
#include <QtQml/QQmlInfo>
#include <QtLocation/QPlaceManager>
#include <QtLocation/QPlaceIcon>

QT_USE_NAMESPACE

/*!
    \qmlclass CategoryModel QDeclarativeSupportedCategoriesModel
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-places
    \ingroup qml-QtLocation5-places-models
    \since Qt Location 5.0

    \brief The CategoryModel element provides a model of the categories supported by a \l Plugin.

    The CategoryModel element provides a model of the categories that are available from the
    current \l Plugin.  The model supports both a flat list of categories and a hierarchical tree
    representing category groupings.  This can be controlled by the \l hierarchical property.

    The model supports the following roles:

    \table
        \header
            \li Role
            \li Type
            \li Description
        \row
            \li category
            \li \l Category
            \li Category object for the current item.
    \endtable

    The following example displays a flat list of all available categories:

    \snippet snippets/declarative/places.qml QtQuick import
    \snippet snippets/declarative/places.qml QtLocation import
    \codeline
    \snippet snippets/declarative/places.qml CategoryView

    To access the hierarchical category model it is necessary to use a \l VisualDataModel to access
    the child items.
*/

/*!
    \qmlproperty Plugin CategoryModel::plugin

    This property holds the provider \l Plugin used by this model.
*/

/*!
    \qmlproperty bool CategoryModel::hierarchical

    This property holds whether the model provides a hierarchical tree of categories or a flat
    list.  The default is true.
*/

/*!
    \qmlmethod string QtLocation5::CategoryModel::data(ModelIndex index, int role)
    \internal

    This method retrieves the the model's data per \a index and \a role.
*/

/*!
    \qmlproperty string QtLocation5::CategoryModel::errorString

    This read-only property holds the textual presentation of latest category model error.
    If no error has occurred, an empty string is returned.

    An empty string may also be returned if an error occurred which has no associated
    textual representation.
*/

/*!
    \qmlproperty enumeration QtLocation5::CategoryModel::status

    This read-only property holds the current status of the model.

    \list
    \li CategoryModel.Ready - Category request(s) have finished successfully.
    \li CategoryModel.Updating - Category request has been issued but not yet finished
    \li CategoryModel.Error - An error has occurred, details are in \l errorString
    \endlist
*/

/*!
    \internal
    \enum QDeclarativeSupportedCategoriesModel::Roles
*/

QDeclarativeSupportedCategoriesModel::QDeclarativeSupportedCategoriesModel(QObject *parent)
:   QAbstractItemModel(parent), m_plugin(0), m_hierarchical(true), m_complete(false)
{
    QHash<int, QByteArray> roles = roleNames();
    roles = QAbstractItemModel::roleNames();
    roles.insert(CategoryRole, "category");
    setRoleNames(roles);
}

QDeclarativeSupportedCategoriesModel::~QDeclarativeSupportedCategoriesModel()
{
    qDeleteAll(m_categoriesTree);
}

/*!
    \internal
*/
// From QQmlParserStatus
void QDeclarativeSupportedCategoriesModel::componentComplete()
{
    m_complete = true;
}

/*!
    \internal
*/
int QDeclarativeSupportedCategoriesModel::rowCount(const QModelIndex &parent) const
{
    if (m_categoriesTree.keys().isEmpty())
        return 0;

    PlaceCategoryNode *node = static_cast<PlaceCategoryNode *>(parent.internalPointer());
    if (!node)
        node = m_categoriesTree.value(QString());
    else if (m_categoriesTree.keys(node).isEmpty())
        return 0;

    return node->childIds.count();
}

/*!
    \internal
*/
int QDeclarativeSupportedCategoriesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return 1;
}

/*!
    \internal
*/
QModelIndex QDeclarativeSupportedCategoriesModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column != 0 || row < 0)
        return QModelIndex();

    PlaceCategoryNode *node = static_cast<PlaceCategoryNode *>(parent.internalPointer());

    if (!node)
        node = m_categoriesTree.value(QString());
    else if (m_categoriesTree.keys(node).isEmpty()) //return root index if parent is non-existent
        return QModelIndex();

    if (row > node->childIds.count())
        return QModelIndex();

    QString id = node->childIds.at(row);
    Q_ASSERT(m_categoriesTree.contains(id));

    return createIndex(row, 0, m_categoriesTree.value(id));
}

/*!
    \internal
*/
QModelIndex QDeclarativeSupportedCategoriesModel::parent(const QModelIndex &child) const
{
    PlaceCategoryNode *childNode = static_cast<PlaceCategoryNode *>(child.internalPointer());
    if (m_categoriesTree.keys(childNode).isEmpty())
        return QModelIndex();

    return index(childNode->parentId);
}

/*!
    \internal
*/
QVariant QDeclarativeSupportedCategoriesModel::data(const QModelIndex &index, int role) const
{
    PlaceCategoryNode *node = static_cast<PlaceCategoryNode *>(index.internalPointer());
    if (!node)
        node = m_categoriesTree.value(QString());
    else if (m_categoriesTree.keys(node).isEmpty())
        return QVariant();

   QDeclarativeCategory *category = node->declCategory.data();

    switch (role) {
    case Qt::DisplayRole:
        return category->name();
    case CategoryRole:
        return QVariant::fromValue(category);
    default:
        return QVariant();
    }
}

/*!
    \internal
*/
void QDeclarativeSupportedCategoriesModel::setPlugin(QDeclarativeGeoServiceProvider *plugin)
{
    if (m_plugin == plugin)
        return;

    //disconnect the manager of the old plugin if we have one
    if (m_plugin) {
        QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
        if (serviceProvider) {
            QPlaceManager *placeManager = serviceProvider->placeManager();
            if (placeManager) {
                disconnect(placeManager, SIGNAL(categoryAdded(QPlaceCategory, QString)),
                           this, SLOT(addedCategory(QPlaceCategory, QString)));
                disconnect(placeManager, SIGNAL(categoryUpdated(QPlaceCategory, QString)),
                           this, SLOT(updatedCategory(QPlaceCategory, QString)));
                disconnect(placeManager, SIGNAL(categoryRemoved(QString, QString)),
                           this, SLOT(removedCategory(QString, QString)));
            }
        }
    }

    m_plugin = plugin;

    // handle plugin name changes -> update categories
    if (m_plugin) {
        connect(m_plugin, SIGNAL(nameChanged(QString)), this, SLOT(connectNotificationSignals()));
        connect(m_plugin, SIGNAL(nameChanged(QString)), this, SLOT(update()));
    }

    connectNotificationSignals();

    if (m_complete)
        emit pluginChanged();

    update();
}

/*!
    \internal
*/
QDeclarativeGeoServiceProvider *QDeclarativeSupportedCategoriesModel::plugin() const
{
    return m_plugin;
}

/*!
    \internal
*/
void QDeclarativeSupportedCategoriesModel::setHierarchical(bool hierarchical)
{
    if (m_hierarchical == hierarchical)
        return;

    m_hierarchical = hierarchical;
    emit hierarchicalChanged();

    updateLayout();
}

/*!
    \internal
*/
bool QDeclarativeSupportedCategoriesModel::hierarchical() const
{
    return m_hierarchical;
}

/*!
    \internal
*/
void QDeclarativeSupportedCategoriesModel::replyFinished()
{
    if (!m_response)
        return;

    if (m_response->error() == QPlaceReply::NoError) {
        m_errorString.clear();

        m_response->deleteLater();
        m_response = 0;

        updateLayout();
        setStatus(QDeclarativeSupportedCategoriesModel::Ready);
    } else {
        m_errorString = m_response->errorString();

        m_response->deleteLater();
        m_response = 0;

        setStatus(QDeclarativeSupportedCategoriesModel::Error);
    }
}

/*!
    \internal
*/
void QDeclarativeSupportedCategoriesModel::addedCategory(const QPlaceCategory &category,
                                                         const QString &parentId)
{
    if (!m_categoriesTree.contains(parentId))
        return;

    if (category.categoryId().isEmpty())
        return;

    PlaceCategoryNode *parentNode = m_categoriesTree.value(parentId);
    if (!parentNode)
        return;

    int rowToBeAdded = rowToAddChild(parentNode, category);
    QModelIndex parentIndex = index(parentId);
    beginInsertRows(parentIndex, rowToBeAdded, rowToBeAdded);
    PlaceCategoryNode *categoryNode = new PlaceCategoryNode;
    categoryNode->parentId = parentId;
    categoryNode->declCategory = QSharedPointer<QDeclarativeCategory>(new QDeclarativeCategory(category, m_plugin, this));

    m_categoriesTree.insert(category.categoryId(), categoryNode);
    parentNode->childIds.insert(rowToBeAdded,category.categoryId());
    endInsertRows();

    //this is a workaround to deal with the fact that the hasModelChildren field of VisualDataodel
    //does not get updated when a child is added to a model
    beginResetModel();
    endResetModel();
}

/*!
    \internal
*/
void QDeclarativeSupportedCategoriesModel::updatedCategory(const QPlaceCategory &category,
                                                           const QString &parentId)
{
    QString categoryId = category.categoryId();

    if (!m_categoriesTree.contains(parentId))
        return;

    if (category.categoryId().isEmpty() || !m_categoriesTree.contains(categoryId))
        return;

    PlaceCategoryNode *newParentNode = m_categoriesTree.value(parentId);
    if (!newParentNode)
        return;

    PlaceCategoryNode *categoryNode = m_categoriesTree.value(categoryId);
    if (!categoryNode)
        return;

    categoryNode->declCategory->setCategory(category);

    if (categoryNode->parentId == parentId) { //reparenting to same parent
        QModelIndex parentIndex = index(parentId);
        int rowToBeAdded = rowToAddChild(newParentNode, category);
        int oldRow = newParentNode->childIds.indexOf(categoryId);

        //check if we are changing the position of the category
        if (qAbs(rowToBeAdded - newParentNode->childIds.indexOf(categoryId)) > 1) {
            //if the position has changed we are moving rows
            beginMoveRows(parentIndex, oldRow, oldRow,
                          parentIndex, rowToBeAdded);

            newParentNode->childIds.removeAll(categoryId);
            newParentNode->childIds.insert(rowToBeAdded, categoryId);
            endMoveRows();
        } else {// if the position has not changed we modifying an existing row
            QModelIndex categoryIndex = index(categoryId);
            emit dataChanged(categoryIndex, categoryIndex);
        }
    } else { //reparenting to different parents
        QPlaceCategory oldCategory = categoryNode->declCategory->category();
        PlaceCategoryNode *oldParentNode = m_categoriesTree.value(categoryNode->parentId);
        if (!oldParentNode)
            return;
        QModelIndex oldParentIndex = index(categoryNode->parentId);
        QModelIndex newParentIndex = index(parentId);
        QModelIndex removedIndex = index(categoryId);

        int rowToBeAdded = rowToAddChild(newParentNode, category);
        beginMoveRows(oldParentIndex, oldParentNode->childIds.indexOf(categoryId),
                      oldParentNode->childIds.indexOf(categoryId), newParentIndex, rowToBeAdded);
        oldParentNode->childIds.removeAll(oldCategory.categoryId());
        newParentNode->childIds.insert(rowToBeAdded, categoryId);
        categoryNode->parentId = parentId;
        endMoveRows();

        //this is a workaround to deal with the fact that the hasModelChildren field of VisualDataodel
        //does not get updated when an index is updated to contain children
        beginResetModel();
        endResetModel();
    }
}

/*!
    \internal
*/
void QDeclarativeSupportedCategoriesModel::removedCategory(const QString &categoryId, const QString &parentId)
{
    if (!m_categoriesTree.contains(categoryId) || !m_categoriesTree.contains(parentId))
        return;

    QModelIndex parentIndex = index(parentId);
    QModelIndex categoryIndex = index(categoryId);

    beginRemoveRows(parentIndex, categoryIndex.row(), categoryIndex.row());
    PlaceCategoryNode *parentNode = m_categoriesTree.value(parentId);
    parentNode->childIds.removeAll(categoryId);
    delete m_categoriesTree.take(categoryId);
    endRemoveRows();
}

/*!
    \internal
*/
void QDeclarativeSupportedCategoriesModel::connectNotificationSignals()
{
    if (!m_plugin)
        return;

    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    if (!serviceProvider || serviceProvider->error() != QGeoServiceProvider::NoError)
        return;

    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager) {
        qmlInfo(this) << QCoreApplication::translate(CONTEXT_NAME, PLUGIN_DOESNOT_SUPPORT_PLACES2).arg(m_plugin->name());
        return;
    }

    // listen for any category notifications so that we can reupdate the categories
    // model.
    connect(placeManager, SIGNAL(categoryAdded(QPlaceCategory, QString)),
            this, SLOT(addedCategory(QPlaceCategory, QString)));
    connect(placeManager, SIGNAL(categoryUpdated(QPlaceCategory, QString)),
            this, SLOT(updatedCategory(QPlaceCategory, QString)));
    connect(placeManager, SIGNAL(categoryRemoved(QString, QString)),
            this, SLOT(removedCategory(QString, QString)));
}

/*!
    \internal
*/
void QDeclarativeSupportedCategoriesModel::update()
{
    beginResetModel();
    qDeleteAll(m_categoriesTree);
    m_categoriesTree.clear();
    endResetModel();

    if (!m_plugin)
        return;

    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    if (!serviceProvider || serviceProvider->error() != QGeoServiceProvider::NoError)
        return;

    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager) {
        qmlInfo(this) << QCoreApplication::translate(CONTEXT_NAME, PLUGIN_DOESNOT_SUPPORT_PLACES2).arg(m_plugin->name());
        return;
    }

    if (placeManager) {
        m_response = placeManager->initializeCategories();
        if (m_response) {
            connect(m_response, SIGNAL(finished()), this, SLOT(replyFinished()));
            setStatus(QDeclarativeSupportedCategoriesModel::Updating);
        } else {
            setStatus(QDeclarativeSupportedCategoriesModel::Error);
            m_errorString = QCoreApplication::translate(CONTEXT_NAME, CATEGORIES_NOT_INITIALIZED);
        }
    }
}

/*!
    \internal
*/
void QDeclarativeSupportedCategoriesModel::updateLayout()
{
    if (!m_plugin)
        return;

    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    if (!serviceProvider || serviceProvider->error() != QGeoServiceProvider::NoError)
        return;

    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager) {
        qmlInfo(this) << QCoreApplication::translate(CONTEXT_NAME, PLUGIN_DOESNOT_SUPPORT_PLACES2).arg(m_plugin->name());
        return;
    }

    beginResetModel();
    qDeleteAll(m_categoriesTree);
    m_categoriesTree.clear();
    PlaceCategoryNode *node = new PlaceCategoryNode;
    node->childIds = populateCategories(placeManager, QPlaceCategory());
    m_categoriesTree.insert(QString(), node);
    node->declCategory = QSharedPointer<QDeclarativeCategory>
                            (new QDeclarativeCategory(QPlaceCategory(), m_plugin, this));
    endResetModel();
}

QString QDeclarativeSupportedCategoriesModel::errorString() const
{
    return m_errorString;
}

/*!
    \qmlproperty enumeration CategoryModel::status

    This property holds the status of the model.  It can be one of:

    \table
        \row
            \li CategoryModel.Ready
            \li No error occurred during the last operation, further operations may be performed on
               the model.
        \row
            \li CategoryModel.Updating
            \li The model is being updated, no other operations may be performed until complete.
        \row
            \li CategoryModel.Error
            \li An error occurred during the last operation, further operations can still be
               performed on the model.
    \endtable
*/
void QDeclarativeSupportedCategoriesModel::setStatus(Status status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged();
    }
}

QDeclarativeSupportedCategoriesModel::Status QDeclarativeSupportedCategoriesModel::status() const
{
    return m_status;
}

/*!
    \internal
*/
QStringList QDeclarativeSupportedCategoriesModel::populateCategories(QPlaceManager *manager, const QPlaceCategory &parent)
{
    Q_ASSERT(manager);

    QStringList childIds;
    PlaceCategoryNode *node;

    QMap<QString, QPlaceCategory> sortedCategories;
    foreach ( const QPlaceCategory &category, manager->childCategories(parent.categoryId()))
        sortedCategories.insert(category.name(), category);

    QMapIterator<QString, QPlaceCategory> iter(sortedCategories);
    while (iter.hasNext()) {
        iter.next();
        node = new PlaceCategoryNode;
        node->parentId = parent.categoryId();
        node->declCategory = QSharedPointer<QDeclarativeCategory>(new QDeclarativeCategory(iter.value(), m_plugin ,this));

        if (m_hierarchical)
            node->childIds = populateCategories(manager, iter.value());

        m_categoriesTree.insert(node->declCategory->categoryId(), node);
        childIds.append(iter.value().categoryId());

        if (!m_hierarchical) {
            childIds.append(populateCategories(manager,node->declCategory->category()));
        }
    }
    return childIds;
}

/*!
    \internal
*/
QModelIndex QDeclarativeSupportedCategoriesModel::index(const QString &categoryId) const
{
    if (categoryId.isEmpty())
        return QModelIndex();

    if (!m_categoriesTree.contains(categoryId))
        return QModelIndex();

    PlaceCategoryNode *categoryNode = m_categoriesTree.value(categoryId);
    if (!categoryNode)
        return QModelIndex();

    QString parentCategoryId = categoryNode->parentId;

    PlaceCategoryNode *parentNode = m_categoriesTree.value(parentCategoryId);

    return createIndex(parentNode->childIds.indexOf(categoryId), 0, categoryNode);
}

/*!
    \internal
*/
int QDeclarativeSupportedCategoriesModel::rowToAddChild(PlaceCategoryNode *node, const QPlaceCategory &category)
{
    Q_ASSERT(node);
    for (int i = 0; i < node->childIds.count(); ++i) {
        if (category.name() < m_categoriesTree.value(node->childIds.at(i))->declCategory->name())
            return i;
    }
    return node->childIds.count();
}

/*!
    \internal
    Helper function to return the manager, this manager is intended to be used
    to perform the next operation.  If the checkState parameter is true,
    the model is checked to see if an operation is underway and if so
    a null pointer is returned.
*/
QPlaceManager *QDeclarativeSupportedCategoriesModel::manager(bool checkState)
{
    if (checkState) {
        if (m_status != QDeclarativeSupportedCategoriesModel::Ready && m_status != QDeclarativeSupportedCategoriesModel::Error)
            return 0;
    }

    if (m_response) {
        m_response->abort();
        m_response->deleteLater();
        m_response = 0;
    }

    if (!m_plugin) {
           qmlInfo(this) << QCoreApplication::translate(CONTEXT_NAME, PLUGIN_NOT_ASSIGNED_TO_PLACE);
           return 0;
    }

    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    if (!serviceProvider)
        return 0;

    QPlaceManager *placeManager = serviceProvider->placeManager();

    if (!placeManager) {
        qmlInfo(this) << QCoreApplication::translate(CONTEXT_NAME, PLUGIN_DOESNOT_SUPPORT_PLACES2).arg(m_plugin->name());
        return 0;
    }

    return placeManager;
}
