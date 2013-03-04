/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Build Suite.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#ifndef QBS_EVALUATOR_H
#define QBS_EVALUATOR_H

#include "forward_decls.h"
#include "itemobserver.h"
#include <language/scriptengine.h>

#include <QHash>
#include <QScriptValue>

namespace qbs {
namespace Internal {

class EvaluatorScriptClass;

class Evaluator : private ItemObserver
{
    friend class SVConverter;

public:
    Evaluator(ScriptEngine *scriptEngine, const Logger &logger);
    virtual ~Evaluator();

    ScriptEngine *engine() const;
    QScriptValue property(const ItemConstPtr &item, const QString &name);
    QScriptValue property(const ItemConstPtr &item, const QStringList &nameParts);
    QScriptValue scriptValue(const ItemConstPtr &item);
    QScriptValue fileScope(const FileContextConstPtr &file);

private:
    void onItemPropertyChanged(Item *item);
    void onItemDestroyed(Item *item);

    ScriptEngine *m_scriptEngine;
    EvaluatorScriptClass *m_scriptClass;
    mutable QHash<const Item *, QScriptValue> m_scriptValueMap;
    mutable QHash<FileContextConstPtr, QScriptValue> m_fileScopeMap;
};

inline ScriptEngine *Evaluator::engine() const
{
    return m_scriptEngine;
}

} // namespace Internal
} // namespace qbs

#endif // QBS_EVALUATOR_H