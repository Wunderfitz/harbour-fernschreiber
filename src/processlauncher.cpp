#include "processlauncher.h"

#define LOG(x) qDebug() << "[ProcessLauncher]" << x

ProcessLauncher::ProcessLauncher(QObject *parent) : QObject(parent)
{

}
bool ProcessLauncher::launchProgram(const QString &program, const QStringList &arguments)
{
    QString executablePath = QStandardPaths::findExecutable(program);
    if(executablePath == "") {
        LOG("[ProcessLauncher] Program " + program + "not found");
        return false;
    }
    QProcess *externalProcess = new QProcess(this);
    return externalProcess->startDetached(program, arguments);
}
