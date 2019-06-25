#ifndef QDOM_HPP_
#define QDOM_HPP_

#include <type_traits>
#include "dom.hpp"
#include "qdomainmodel.hpp"

#define DOM_PROPERTY(type, name) \
    Q_PROPERTY(type name READ get_##name WRITE set_##name NOTIFY name##Changed) \
    \
    void set_##name (type v) { \
    } \
    type get_##name() {\
        return *name; \
    } \
    dom::property<type> name {std::bind(&ThisClass:: name##Changed, this)}; \
    Q_SIGNALS: \
         void name##Changed(); \
    public: \

#define DOM_MODEL(type, name) \
    Q_PROPERTY(QDomModel<type>* name READ get_##name WRITE set_##name NOTIFY name##Changed) \
    \
    void set_##name (QDomModel<type>* v) { \
    } \
    QDomModel<type>* get_##name() {\
        return &name; \
    } \
    QDomModel<type> name; \
    Q_SIGNALS: \
         void name##Changed(); \
    public: \


#define QML_WRITABLE_PROPERTY(type, name) \
    protected: \
        Q_PROPERTY (type name READ get_##name WRITE set_##name NOTIFY name##Changed) \
    private: \
        type m_##name; \
    public: \
        type get_##name () const { \
            return m_##name ; \
        } \
    public Q_SLOTS: \
        bool set_##name (type name) { \
            bool ret = false; \
            if ((ret = m_##name != name)) { \
                m_##name = name; \
                emit name##Changed (m_##name); \
            } \
            return ret; \
        } \
    Q_SIGNALS: \
        void name##Changed (type name); \
    private:

#endif // QDOM_HPP_
