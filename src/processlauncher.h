#ifndef PROCESSLAUNCHER_H
#define PROCESSLAUNCHER_H

#include <QObject>

class ProcessLauncher : public QObject
{
    Q_OBJECT
public:
    explicit ProcessLauncher(QObject *parent = nullptr);

    Q_INVOKABLE bool launchProgram(const QString &program, const QStringList &arguments);

signals:

public slots:
};

#endif // PROCESSLAUNCHER_H
