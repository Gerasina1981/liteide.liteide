#ifndef GOLANGOPTION_H
#define GOLANGOPTION_H

#include <QWidget>
#include <QProcessEnvironment>
#include "../../api/iapp.h"
#include "../../api/ioption.h"

namespace Ui {
    class GolangOption;
}

inline QString defGOROOT() {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString def;
#ifdef Q_OS_WIN32
    def =  "c:/go";
#else
    def = env.value("HOME")+"/go";
#endif
    return env.value("GOROOT",def);
}

class GolangOption : public QWidget, public IOption
{
    Q_OBJECT

public:
    void setupList();
    explicit GolangOption(IApplication *app, QWidget *parent = 0);
    ~GolangOption();
    virtual QWidget *widget();
    virtual QIcon icon();
    virtual QString text();
    virtual void load();
    virtual void save();
private:
    Ui::GolangOption *ui;
    IApplication *liteApp;

private slots:
    void on_browseGdb_clicked();
    void on_browseGoroot_clicked();
};

#endif // GOLANGOPTION_H
