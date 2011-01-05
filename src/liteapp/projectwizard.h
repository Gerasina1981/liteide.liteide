#ifndef PROJECTWIZARD_H
#define PROJECTWIZARD_H

#include <QWizard>
#include <QWizardPage>

class QLineEdit;
class QStandardItemModel;
class QTreeView;
class ProjectInfoPage;
class ProjectWizard : public QWizard
{
    Q_OBJECT
public:
    bool processFile(const QString &infile, const QString &outfile, const QMap<QString,QString> &map);
    virtual void accept();
    explicit ProjectWizard(QWidget *parent = 0);
public:
    QString projectFileName;
protected:
    ProjectInfoPage *infoPage;
};

class ProjectInfoPage : public QWizardPage
{
    Q_OBJECT
public:
    ProjectInfoPage(QWidget *parent = 0);
public:
    QLineEdit   *nameLineEdit;
    QLineEdit   *locationLineEdit;
    QStandardItemModel    *model;
    QTreeView   *tree;
private slots:
    void browseLocation();
};

#endif // PROJECTWIZARD_H
