#include "processlauncher.h"
#include <QProcess>
#include <QStandardPaths>

#define DEBUG_MODULE ProcessLauncher
#include "debuglog.h"

ProcessLauncher::ProcessLauncher(QObject *parent) : QObject(parent)
{
}

bool ProcessLauncher::launchProgram(const QString &program, const QStringList &arguments)
{
    const QString executablePath(QStandardPaths::findExecutable(program));
    if (executablePath.isEmpty()) {
        LOG("Program" << program << "not found");
        return false;
    }

    QProcess *process = new QProcess(this);
    connect(process, SIGNAL(finished(int)), process, SLOT(deleteLater()));
    return process->startDetached(program, arguments);
}
