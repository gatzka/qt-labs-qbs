/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
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
#include "commandlineparser.h"

#include <logging/translator.h>
#include <tools/error.h>

#include <QFileInfo>

using qbs::Internal::Tr;

static QString helpOptionShort() { return QLatin1String("-h"); }
static QString helpOptionLong() { return QLatin1String("--help"); }
static QString detectOption() { return QLatin1String("--detect"); }
static QString settingsDirOption() { return QLatin1String("--settings-dir"); }

void CommandLineParser::parse(const QStringList &commandLine)
{
    m_commandLine = commandLine;
    Q_ASSERT(!m_commandLine.isEmpty());
    m_command = QFileInfo(m_commandLine.takeFirst()).fileName();
    m_helpRequested = false;
    m_autoDetectionMode = false;
    m_qmakePath.clear();
    m_profileName.clear();
    m_settingsDir.clear();

    if (m_commandLine.isEmpty())
        throwError(Tr::tr("No command-line arguments provided."));

    while (!m_commandLine.isEmpty()) {
        const QString arg = m_commandLine.first();
        if (!arg.startsWith(QLatin1String("--")))
            break;
        m_commandLine.removeFirst();
        if (arg == helpOptionShort() || arg == helpOptionLong())
            m_helpRequested = true;
        else if (arg == detectOption())
            m_autoDetectionMode = true;
        else if (arg == settingsDirOption())
            assignOptionArgument(settingsDirOption(), m_settingsDir);
    }

    if (m_helpRequested || m_autoDetectionMode) {
        if (!m_commandLine.isEmpty())
            complainAboutExtraArguments();
        return;
    }

    switch (m_commandLine.count()) {
    case 0:
    case 1:
        throwError(Tr::tr("Not enough command-line arguments provided."));
    case 2:
        m_qmakePath = m_commandLine.at(0);
        m_profileName = m_commandLine.at(1);
        break;
    default:
        complainAboutExtraArguments();
    }
}

void CommandLineParser::throwError(const QString &message)
{
    qbs::ErrorInfo error(Tr::tr("Syntax error: %1").arg(message));
    error.append(usageString());
    throw error;
}

QString CommandLineParser::usageString() const
{
    QString s = Tr::tr("This tool creates qbs profiles from Qt versions.\n");
    s += Tr::tr("Usage:\n");
    s += Tr::tr("    %1 [%2 <settings directory>] %3\n")
            .arg(m_command, settingsDirOption(), detectOption());
    s += Tr::tr("    %1 [%2 <settings directory>] <path to qmake> <profile name>\n")
            .arg(m_command, settingsDirOption());
    s += Tr::tr("    %1 %2|%3\n").arg(m_command, helpOptionShort(), helpOptionLong());
    s += Tr::tr("The first form tries to auto-detect all known Qt versions, looking them up "
                "via the PATH environment variable.\n");
    s += Tr::tr("The second form creates one profile for one Qt version.");
    return s;
}

void CommandLineParser::assignOptionArgument(const QString &option, QString &argument)
{
    if (m_commandLine.isEmpty())
        throwError(Tr::tr("Option '%1' needs an argument.").arg(option));
    argument = m_commandLine.takeFirst();
    if (argument.isEmpty())
        throwError(Tr::tr("Argument for option '%1' must not be empty.").arg(option));
}

void CommandLineParser::complainAboutExtraArguments()
{
    throwError(Tr::tr("Extraneous command-line arguments '%1'.")
               .arg(m_commandLine.join(QLatin1String(" "))));
}
