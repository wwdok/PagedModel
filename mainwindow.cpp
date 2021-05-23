#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    filter.setPageData(true);//如果设为false，则1000条数据都展示在要给页面，没有分页效果，如果设为true，每页10行，共100页。
    filter.setPageSize(10);
    filter.setCurrentPage(0);
    connect(&filter,&Filter::started,this,&MainWindow::started);
    connect(&filter,&Filter::finished,this,&MainWindow::finished);
    filter.setSourceModel(&model);

    ui->tableView->setModel(&filter);

    makeTestData();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnSearch_clicked()
{
    filter.filterPaged(ui->lineEdit->text());
}


void MainWindow::on_btnBack_clicked()
{
    filter.back();
}

void MainWindow::on_btnNext_clicked()
{
    filter.next();
}

void MainWindow::on_actionCreate_Test_File_triggered()
{
    QFileDialog dlg;
    QString path = dlg.getSaveFileName(this,"File to create");
    QFileInfo fi(path);
    if(path.isEmpty()) return;
    if(fi.exists()) {
       QMessageBox::StandardButton btn = QMessageBox::question(this,"File Exists","File exists!\r\nOverwrite the file?",QMessageBox::Yes | QMessageBox::No);

       if(btn == QMessageBox::Yes) on_actionCreate_Test_File_triggered();
       return;
    }

    int value = QInputDialog::getInt(this,"Max Items","How many items to create?");
    Data data;
    if(!data.writeTest(path,value))
    {
        QMessageBox::critical(this,"Error","Could not create test data!");
        return;
    }
    loadFile(path);

}

void MainWindow::started()
{
    qDebug() << "STARTED SEARCHING";
}

void MainWindow::finished()
{
    qDebug() << "FINISHED SEARCHING";
    QString current = QString::number(filter.currentPage());
    QString total = QString::number(filter.pageCount());
   ui->lblCount->setText(current + " of " + total);
}

void MainWindow::makeTestData()
{
    Data data;
    QString path = QGuiApplication::applicationDirPath();
    path.append("/test.txt");

    if(!data.writeTest(path,1000))//writeTest()函数会生成0到999的数字并保存在path路径里的文件中
    {
        QMessageBox::critical(this,"Error","Could not create test data!");
        return;
    }
    loadFile(path);//紧接着加载生成的数字到qtableview上
}

void MainWindow::loadFile(QString path)
{
    Data data;
    ui->statusbar->showMessage("No file");
    if(!data.loadTable(path,&model))//loadTable()会把数据从刚刚生成的path文件内提取并显示在qtableview上
    {
        QMessageBox::critical(this,"Error","Could not load file!");
        return;
    }
    ui->statusbar->showMessage(path);
    ui->tableView->verticalHeader()->setVisible(false);
}
