#include "projectwizard.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QFileDialog>
#include <QVariant>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTreeView>
#include <QApplication>
#include <QFileInfo>
#include <QDebug>
#include <QSettings>
#include <QTextCodec>
#include <QCheckBox>

ProjectWizard::ProjectWizard(QWidget *parent) :
    QWizard(parent)
{
    infoPage = new ProjectInfoPage;
    this->addPage(infoPage);
    setWindowTitle(tr("Project Wizard"));
}


ProjectInfoPage::ProjectInfoPage(QWidget *parent) :
        QWizardPage(parent)
{
    setTitle(tr("Project Information"));
    setSubTitle(tr("This wizard generates application project."));

    model = new QStandardItemModel(this);
    tree = new QTreeView(this);
    tree->setModel(model);
    tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tree->setHeaderHidden(true);
    //
    QDir dir = QFileInfo(qApp->applicationDirPath(),"template/project").absoluteFilePath();
    QFileInfoList infos = dir.entryInfoList(QStringList() << "*.inf",QDir::Files);
    foreach (QFileInfo info, infos) {
        QSettings  set(info.absoluteFilePath(),QSettings::IniFormat);
        QString name = set.value("PROJECT/NAME").toString();
        if (!name.isEmpty()) {
            QStandardItem *item = new QStandardItem(name);
            item->setData(info.absoluteFilePath());
            model->appendRow(item);
        }
    }
    QModelIndex index = model->index(0,0);
    if (index.isValid()) {
        tree->setCurrentIndex(index);
    }

    nameLineEdit = new QLineEdit;
    locationLineEdit = new QLineEdit;
    QPushButton *browseButton = new QPushButton(tr("Browse..."));
    connect(browseButton,SIGNAL(clicked()),this,SLOT(browseLocation()));

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(new QLabel(tr("Name:")),nameLineEdit);

    QHBoxLayout *locationLayout = new QHBoxLayout;
    locationLayout->addWidget(locationLineEdit);
    locationLayout->addWidget(browseButton);

    formLayout->addRow(new QLabel("Location:"),locationLayout);

    QCheckBox *checkBox = new QCheckBox("Create project sub dir",this);
    checkBox->setChecked(true);
    formLayout->addRow(new QLabel(""),checkBox);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tree);
    mainLayout->addStretch();
    mainLayout->addLayout(formLayout);


    registerField("PROJECT_NAME*",nameLineEdit);
    registerField("PROJECT_LOCATION*",locationLineEdit);
    registerField("PROJECT_SUBDIR",checkBox);

    setLayout(mainLayout);
}

void ProjectInfoPage::browseLocation()
{
    QString location = QFileDialog::getExistingDirectory(this);
    if (!location.isEmpty()) {
        locationLineEdit->setText(QDir::toNativeSeparators(location));
    }
}

void ProjectWizard::accept()
{
    QString projectName = field("PROJECT_NAME").toByteArray();
    QString projectLocation = field("PROJECT_LOCATION").toByteArray();
    bool bSubDir = field("PROJECT_SUBDIR").toBool();

    if (!QFileInfo(projectLocation).isWritable()) {
        QMessageBox::information(this,tr("Error"),
                                 tr("Project Location {%1} cannot write!").arg(projectLocation));
        return;
    }
    if (bSubDir && QFileInfo(QDir(projectLocation),projectName).isReadable()) {
        QMessageBox::information(this,tr("Error"),
                                 tr("Project exists {%1}!").arg(projectLocation));
        return;
    }

    QModelIndex index = infoPage->tree->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QStandardItem *item = infoPage->model->itemFromIndex(index);
    QString infPath = item->data().toString();

    QSettings  set(infPath,QSettings::IniFormat);
    if (set.status() != QSettings::NoError)
        return;

    QDir dir(projectLocation);
    if (bSubDir && !dir.mkdir(projectName)) {
        return;
    }

    QString path = set.value("PROJECT/PATH").toString();
    QString name = set.value("PROJECT/FILES").toString();
    QString inDir = QFileInfo(QFileInfo(infPath).absoluteDir(),path).absoluteFilePath();
    QStringList files = name.split(" ",QString::SkipEmptyParts);
    QString pro = set.value("PROJECT/PRO").toString();
    files.append(pro);

    QString outDir = projectLocation;
    if (bSubDir) {
        outDir = QFileInfo(projectLocation,projectName).absoluteFilePath();
    }

    QMap<QString,QString> map;
    map.insert("$ROOT$",projectName);


    foreach (QString file, files) {
        QString infile = QFileInfo(inDir,file).absoluteFilePath();
        file.replace("root",projectName);
        QString outfile = QFileInfo(outDir,file).absoluteFilePath();
        processFile(infile,outfile,map);
    }

    pro.replace("root",projectName);
    projectFileName = QFileInfo(outDir,pro).absoluteFilePath();

    QWizard::accept();
}

bool ProjectWizard::processFile(const QString &infile, const QString &outfile, const QMap<QString,QString> &map)
{
    QFile in(infile);
    if (!in.open(QIODevice::ReadOnly|QIODevice::Text)) {
        return false;
    }
    QFile out(outfile);
    if (!out.open(QIODevice::WriteOnly|QIODevice::Text)) {
        return false;
    }
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString all = codec->toUnicode(in.readAll());

    QMapIterator<QString,QString> i(map);
    while (i.hasNext()) {
        i.next();
        all.replace(i.key(),i.value());
    }

    out.write(codec->fromUnicode(all));

    in.close();
    out.close();
    return true;
}
