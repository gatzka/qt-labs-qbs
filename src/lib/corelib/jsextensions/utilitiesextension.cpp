/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing
**
** This file is part of the Qt Build Suite.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms and
** conditions see http://www.qt.io/terms-conditions. For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, The Qt Company gives you certain additional
** rights.  These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "utilitiesextension.h"

#include <language/scriptengine.h>
#include <logging/translator.h>
#include <tools/architectures.h>
#include <tools/fileinfo.h>

#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QScriptable>
#include <QScriptEngine>

namespace qbs {
namespace Internal {

class UtilitiesExtension : public QObject, QScriptable
{
    Q_OBJECT
public:
    static QScriptValue js_ctor(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue js_canonicalArchitecture(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue js_getHash(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue js_getNativeSetting(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue js_rfc1034identifier(QScriptContext *context, QScriptEngine *engine);
};

void initializeJsExtensionUtilities(QScriptValue extensionObject)
{
    QScriptEngine *engine = extensionObject.engine();
    QScriptValue environmentObj = engine->newQMetaObject(&UtilitiesExtension::staticMetaObject,
                                             engine->newFunction(&UtilitiesExtension::js_ctor));
    environmentObj.setProperty(QStringLiteral("canonicalArchitecture"),
                               engine->newFunction(UtilitiesExtension::js_canonicalArchitecture, 1));
    environmentObj.setProperty(QStringLiteral("getHash"),
                               engine->newFunction(UtilitiesExtension::js_getHash, 1));
    environmentObj.setProperty(QStringLiteral("getNativeSetting"),
                               engine->newFunction(UtilitiesExtension::js_getNativeSetting, 3));
    environmentObj.setProperty(QStringLiteral("rfc1034Identifier"),
                               engine->newFunction(UtilitiesExtension::js_rfc1034identifier, 1));
    extensionObject.setProperty(QStringLiteral("Utilities"), environmentObj);
}

QScriptValue UtilitiesExtension::js_ctor(QScriptContext *context, QScriptEngine *engine)
{
    // Add instance variables here etc., if needed.
    Q_UNUSED(context);
    Q_UNUSED(engine);
    return true;
}

QScriptValue UtilitiesExtension::js_canonicalArchitecture(QScriptContext *context,
                                                          QScriptEngine *engine)
{
    if (Q_UNLIKELY(context->argumentCount() != 1))
        return context->throwError(QScriptContext::SyntaxError,
                                   QLatin1String("canonicalArchitecture expects 1 argument"));
    const QString architecture = context->argument(0).toString();
    return engine->toScriptValue(canonicalArchitecture(architecture));
}

QScriptValue UtilitiesExtension::js_getHash(QScriptContext *context, QScriptEngine *engine)
{
    if (Q_UNLIKELY(context->argumentCount() < 1)) {
        return context->throwError(QScriptContext::SyntaxError,
                                   QLatin1String("getHash expects 1 argument"));
    }
    const QByteArray input = context->argument(0).toString().toLatin1();
    const QByteArray hash
            = QCryptographicHash::hash(input, QCryptographicHash::Sha1).toHex().left(16);
    return engine->toScriptValue(QString::fromLatin1(hash));
}

QScriptValue UtilitiesExtension::js_getNativeSetting(QScriptContext *context, QScriptEngine *engine)
{
    if (Q_UNLIKELY(context->argumentCount() < 1 || context->argumentCount() > 3)) {
        return context->throwError(QScriptContext::SyntaxError,
                                   QLatin1String("getNativeSetting expects between 1 and 3 arguments"));
    }

    QString key = context->argumentCount() > 1 ? context->argument(1).toString() : QString();

    // We'll let empty string represent the default registry value
    if (HostOsInfo::isWindowsHost() && key.isEmpty())
        key = QLatin1String(".");

    QVariant defaultValue = context->argumentCount() > 2 ? context->argument(2).toVariant() : QVariant();

    QSettings settings(context->argument(0).toString(), QSettings::NativeFormat);
    QVariant value = settings.value(key, defaultValue);
    return value.isNull() ? engine->undefinedValue() : engine->toScriptValue(value);
}

QScriptValue UtilitiesExtension::js_rfc1034identifier(QScriptContext *context,
                                                      QScriptEngine *engine)
{
    if (Q_UNLIKELY(context->argumentCount() != 1))
        return context->throwError(QScriptContext::SyntaxError,
                                   QLatin1String("rfc1034Identifier expects 1 argument"));
    const QString identifier = context->argument(0).toString();
    return engine->toScriptValue(HostOsInfo::rfc1034Identifier(identifier));
}

} // namespace Internal
} // namespace qbs

Q_DECLARE_METATYPE(qbs::Internal::UtilitiesExtension *)

#include "utilitiesextension.moc"
