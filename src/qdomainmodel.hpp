#ifndef QQMLOBJECTLISTMODEL_H
#define QQMLOBJECTLISTMODEL_H

#include <QAbstractListModel>
#include <QByteArray>
#include <QChar>
#include <QDebug>
#include <QHash>
#include <QList>
#include <QMetaMethod>
#include <QMetaObject>
#include <QMetaProperty>
#include <QObject>
#include <QString>
#include <QStringBuilder>
#include <QVariant>
#include <QVector>

template<typename T> QList<T> qListFromVariant (const QVariantList & list) {
    QList<T> ret;
    ret.reserve (list.size ());
    for (QVariantList::const_iterator it = list.constBegin (); it != list.constEnd (); it++) {
        const QVariant & var = (QVariant) (* it);
        ret.append (var.value<T> ());
    }
    return ret;
}

template<typename T> QVariantList qListToVariant (const QList<T> & list) {
    QVariantList ret;
    ret.reserve (list.size ());
    for (typename QList<T>::const_iterator it = list.constBegin (); it != list.constEnd (); it++) {
        const T & val = (T) (* it);
        ret.append (QVariant::fromValue (val));
    }
    return ret;
}

// custom foreach for QList, which uses no copy and check pointer non-null
#define FOREACH_PTR_IN_QLIST(_type_, _var_, _list_) \
    for (typename QList<_type_ *>::const_iterator it = _list_.begin (); it != _list_.end (); it++) \
        for (_type_ * _var_ = (_type_ *) (* it); _var_ != Q_NULLPTR; _var_ = Q_NULLPTR)

template<class ItemType>
class QDomModel : public QAbstractListModel
{
public:
    explicit QDomModel (QObject *          parent      = Q_NULLPTR)
        : QAbstractListModel (parent)
        , m_count (0)
    {
        ItemType p;

        m_roles[Qt::UserRole] = "qtObject";

        int i = 1;
        for (auto& r : p.roles)
        {
            std::string role = "x_" + r;
            m_roles[Qt::UserRole + i] = role.c_str();
            ++i;
        }
    }

    void operator=(typename dom::array<ItemType>::view v)
    {
        m_items = v;
    }

    bool setData (const QModelIndex & index, const QVariant & value, int role) {
        return false;
    }

    QVariant data (const QModelIndex & index, int role) const {
        std::string roleName = m_roles[role].toStdString();
        return m_items.at(index.row())->data(roleName);
    }
    QHash<int, QByteArray> roleNames (void) const {
        return m_roles;
    }
    typedef typename QList<ItemType *>::iterator iterator;
    iterator begin (void) const {
        return m_items.begin ();
    }
    iterator end (void) const {
        return m_items.end ();
    }
    typedef typename QList<ItemType *>::const_iterator const_iterator;
    const_iterator constBegin (void) const {
        return m_items.constBegin ();
    }
    const_iterator constEnd (void) const {
        return m_items.constEnd ();
    }

public: // C++ API
    ItemType * at (int idx) const {
        ItemType * ret = Q_NULLPTR;
        if (idx >= 0 && idx < m_items.size ()) {
            ret = m_items.value (idx);
        }
        return ret;
    }
    ItemType * getByUid (const QString & uid) const {
        return m_indexByUid.value (uid, Q_NULLPTR);
    }
    int roleForName (const QByteArray & name) const {
        return m_roles.key (name, -1);
    }
    int count (void) const {
        return m_count;
    }
    int size (void) const {
        return m_count;
    }
    bool isEmpty (void) const {
        return m_items.isEmpty ();
    }
    bool contains (ItemType * item) const {
        return m_items.contains (item);
    }
    int indexOf (ItemType * item) const {
        return m_items.indexOf (item);
    }
    void clear (void) {
        if (!m_items.isEmpty ()) {
            beginRemoveRows (noParent (), 0, m_items.count () -1);
            FOREACH_PTR_IN_QLIST (ItemType, item, m_items) {
                dereferenceItem (item);
            }
            m_items.clear ();
            endRemoveRows ();
            updateCounter ();
        }
    }
    void append (ItemType * item) {
        if (item != Q_NULLPTR) {
            const int pos = m_items.count ();
            beginInsertRows (noParent (), pos, pos);
            m_items.append (item);
            referenceItem (item);
            endInsertRows ();
            updateCounter ();
        }
    }
    void prepend (ItemType * item) {
        if (item != Q_NULLPTR) {
            beginInsertRows (noParent (), 0, 0);
            m_items.prepend (item);
            referenceItem (item);
            endInsertRows ();
            updateCounter ();
        }
    }
    void insert (int idx, ItemType * item) {
        if (item != Q_NULLPTR) {
            beginInsertRows (noParent (), idx, idx);
            m_items.insert (idx, item);
            referenceItem (item);
            endInsertRows ();
            updateCounter ();
        }
    }
    void append (const QList<ItemType *> & itemList) {
        if (!itemList.isEmpty ()) {
            const int pos = m_items.count ();
            beginInsertRows (noParent (), pos, pos + itemList.count () -1);
            m_items.reserve (m_items.count () + itemList.count ());
            m_items.append (itemList);
            FOREACH_PTR_IN_QLIST (ItemType, item, itemList) {
                referenceItem (item);
            }
            endInsertRows ();
            updateCounter ();
        }
    }
    void prepend (const QList<ItemType *> & itemList) {
        if (!itemList.isEmpty ()) {
            beginInsertRows (noParent (), 0, itemList.count () -1);
            m_items.reserve (m_items.count () + itemList.count ());
            int offset = 0;
            FOREACH_PTR_IN_QLIST (ItemType, item, itemList) {
                m_items.insert (offset, item);
                referenceItem (item);
                offset++;
            }
            endInsertRows ();
            updateCounter ();
        }
    }
    void insert (int idx, const QList<ItemType *> & itemList) {
        if (!itemList.isEmpty ()) {
            beginInsertRows (noParent (), idx, idx + itemList.count () -1);
            m_items.reserve (m_items.count () + itemList.count ());
            int offset = 0;
            FOREACH_PTR_IN_QLIST (ItemType, item, itemList) {
                m_items.insert (idx + offset, item);
                referenceItem (item);
                offset++;
            }
            endInsertRows ();
            updateCounter ();
        }
    }
    void move (int idx, int pos) {
        if (idx != pos) {
            const int lowest  = qMin (idx, pos);
            const int highest = qMax (idx, pos);
            beginMoveRows (noParent (), highest, highest, noParent (), lowest);
            m_items.move (highest, lowest);
            endMoveRows ();
        }
    }
    void remove (ItemType * item) {
        if (item != Q_NULLPTR) {
            const int idx = m_items.indexOf (item);
            remove (idx);
        }
    }
    void remove (int idx) {
        if (idx >= 0 && idx < m_items.size ()) {
            beginRemoveRows (noParent (), idx, idx);
            ItemType * item = m_items.takeAt (idx);
            dereferenceItem (item);
            endRemoveRows ();
            updateCounter ();
        }
    }
    ItemType * first (void) const {
        return m_items.first ();
    }
    ItemType * last (void) const {
        return m_items.last ();
    }
    const QList<ItemType *> & toList (void) const {
        return m_items;
    }

public: // QML slots implementation
    void append (QObject * item) {
        append (qobject_cast<ItemType *> (item));
    }
    void prepend (QObject * item) {
        prepend (qobject_cast<ItemType *> (item));
    }
    void insert (int idx, QObject * item) {
        insert (idx, qobject_cast<ItemType *> (item));
    }
    void remove (QObject * item) {
        remove (qobject_cast<ItemType *> (item));
    }
    bool contains (QObject * item) const {
        return contains (qobject_cast<ItemType *> (item));
    }
    int indexOf (QObject * item) const {
        return indexOf (qobject_cast<ItemType *> (item));
    }
    int indexOf (const QString & uid) const {
        return indexOf (get (uid));
    }
    QObject * get (int idx) const {
        return static_cast<QObject *> (at (idx));
    }
    QObject * get (const QString & uid) const {
        return static_cast<QObject *> (getByUid (uid));
    }
    QObject * getFirst (void) const {
        return static_cast<QObject *> (first ());
    }
    QObject * getLast (void) const {
        return static_cast<QObject *> (last ());
    }
    QVariantList toVarArray (void) const {
        return qListToVariant<ItemType *> (m_items);
    }

protected: // internal stuff
    static const QString & emptyStr (void) {
        static const QString ret = QStringLiteral ("");
        return ret;
    }
    static const QByteArray & emptyBA (void) {
        static const QByteArray ret = QByteArrayLiteral ("");
        return ret;
    }
    static const QModelIndex & noParent (void) {
        static const QModelIndex ret = QModelIndex ();
        return ret;
    }
    static const int & baseRole (void) {
        static const int ret = Qt::UserRole;
        return ret;
    }
    int rowCount (const QModelIndex & parent = QModelIndex ()) const {
        Q_UNUSED (parent);
        return m_items.size();
    }
    void referenceItem (ItemType * item) {
        if (item != Q_NULLPTR) {
            if (item->parent () == Q_NULLPTR) {
                item->setParent (this);
            }
            const QList<int> signalsIdxList = m_signalIdxToRole.keys ();
            for (QList<int>::const_iterator it = signalsIdxList.constBegin (); it != signalsIdxList.constEnd (); it++) {
                const int signalIdx = (int) (* it);
                QMetaMethod notifier = item->metaObject ()->method (signalIdx);
                connect (item, notifier, this, m_handler, Qt::UniqueConnection);
            }
            if (!m_uidRoleName.isEmpty ()) {
                const QString key = m_indexByUid.key (item, emptyStr ());
                if (!key.isEmpty ()) {
                    m_indexByUid.remove (key);
                }
                const QString value = item->property (m_uidRoleName).toString ();
                if (!value.isEmpty ()) {
                    m_indexByUid.insert (value, item);
                }
            }
        }
    }
    void dereferenceItem (ItemType * item) {
        if (item != Q_NULLPTR) {
            disconnect (this, Q_NULLPTR, item, Q_NULLPTR);
            disconnect (item, Q_NULLPTR, this, Q_NULLPTR);
            if (!m_uidRoleName.isEmpty ()) {
                const QString key = m_indexByUid.key (item, emptyStr ());
                if (!key.isEmpty ()) {
                    m_indexByUid.remove (key);
                }
            }
            if (item->parent () == this) { // FIXME : maybe that's not the best way to test ownership ?
                item->deleteLater ();
            }
        }
    }
    void onItemPropertyChanged (void) {
        ItemType * item = qobject_cast<ItemType *> (sender ());
        const int row = m_items.indexOf (item);
        const int sig = senderSignalIndex ();
        const int role = m_signalIdxToRole.value (sig, -1);
        if (row >= 0 && role >= 0) {
            QModelIndex index = QAbstractListModel::index (row, 0, noParent ());
            QVector<int> rolesList;
            rolesList.append (role);
            if (m_roles.value (role) == m_dispRoleName) {
                rolesList.append (Qt::DisplayRole);
            }
            emit dataChanged (index, index, rolesList);
        }
        if (!m_uidRoleName.isEmpty ()) {
            const QByteArray roleName = m_roles.value (role, emptyBA ());
            if (!roleName.isEmpty () && roleName == m_uidRoleName) {
                const QString key = m_indexByUid.key (item, emptyStr ());
                if (!key.isEmpty ()) {
                    m_indexByUid.remove (key);
                }
                const QString value = item->property (m_uidRoleName).toString ();
                if (!value.isEmpty ()) {
                    m_indexByUid.insert (value, item);
                }
            }
        }
    }
    inline void updateCounter (void) {
        /*if (m_count != m_items.count ()) {
            m_count = m_items.count ();
            emit countChanged ();
        }*/
    }

private: // data members
    int                        m_count;
    QByteArray                 m_uidRoleName;
    QByteArray                 m_dispRoleName;
    QMetaObject                m_metaObj;
    QMetaMethod                m_handler;
    QHash<int, QByteArray>     m_roles;
    QHash<int, int>            m_signalIdxToRole;
    QHash<QString, ItemType *> m_indexByUid;
    dom::array<ItemType>       m_items;
};

#endif // QQMLOBJECTLISTMODEL_H
