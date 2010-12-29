#ifndef PROJECTWIZARD_H
#define PROJECTWIZARD_H

#include <QWizard>
#include <QWizardPage>

class QLineEdit;
class ProjectWizard : public QWizard
{
    Q_OBJECT
public:
    virtual void accept();
    explicit ProjectWizard(QWidget *parent = 0);
public:
    QString projectName;
    QString projectLocation;

};

class ProjectInfoPage : public QWizardPage
{
    Q_OBJECT
public:
    ProjectInfoPage(QWidget *parent = 0);
protected:
    QLineEdit   *nameLineEdit;
    QLineEdit   *locationLineEdit;
private slots:
    void browseLocation();
};

#endif // PROJECTWIZARD_H
