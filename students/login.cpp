#include "login.h"
#include "ui_login.h"
#include "tip.h"
#include "widget.h"
#include "student.h"
#include "courses.h"

#include <qsqldatabase.h>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>

QString login::fileName;
QSqlDatabase login::db;

login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    //翻到第二页
    ui->stackedWidget->setCurrentIndex(1);
    //将密码输入改为“密码”模式
    ui->code->setEchoMode(QLineEdit::Password);
    //隐藏错误信息
    ui->error->hide();

}

login::~login()
{
    delete ui;
}

//获取文件路径的方法（弹窗，用户选择）
void login::setDatabase()
{
    // 创建一个数据库对象
    db = QSqlDatabase::addDatabase("QSQLITE");

    fileName = QFileDialog::getOpenFileName(nullptr, "Open Database", "", "SQLite Database Files (*.db)");
        if (fileName.isEmpty()) {
            // 用户取消了选择或关闭了对话框
            QMessageBox::information(nullptr, "Error", "No database file selected.");
        }

    // 设置数据库文件路径（对于 SQLite）
    db.setDatabaseName(fileName);

    // 打开数据库连接
    if (!db.open())
    {
        qDebug() << "无法连接数据库：" << db.lastError();
    }
    else
    {
        qDebug() << "数据库连接成功！";
    }
}

//显示和隐藏密码信息的按钮
void login::on_showCode_clicked()
{
    if(ui->code->echoMode()==QLineEdit::Password)
    {
        ui->code->setEchoMode(QLineEdit::Normal);
        ui->showCode->setText("隐藏");
    }
    else
    {
        ui->code->setEchoMode(QLineEdit::Password);
        ui->showCode->setText("显示");
    }
}

//在数据库收纳柜查找是否有这个学生
bool login::checkEdit()
{
    editName=ui->name->text();
    editCode=ui->code->text();

    QSqlQuery query;
        query.prepare("SELECT 1 FROM students WHERE student_id = :studentId AND code = :code LIMIT 1");
        query.bindValue(":studentId", editName);
        query.bindValue(":code", editCode);

        //如果正确执行
        if (query.exec())
        {
            //有查询结果
            if (query.next())
            {
                return true;
            }
            else
            {
                return false; // 如果没有结果，说明不存在这个学生
            }
        }
        else
        {
            qDebug() << "Query failed:" << query.lastError();
        }
        return false;
}

//登录按钮，以及后续跳转和报错
void login::on_loginButton_clicked()
{
    if(checkEdit()==true)
    {
        qInfo()<<"有数据";

        //查找姓名
        QString name;
        // 假设已经有一个名为db的QSqlDatabase对象并且已经成功打开
            QSqlQuery query;

            // 准备SQL查询
            // 注意替换"your_table_name"为你的实际表名
            query.prepare("SELECT name FROM students WHERE student_id = :id");

            // 绑定参数
            query.bindValue(":id", editName);

            // 执行查询
            if (!query.exec())
            {
                qDebug() << "查询失败:" << query.lastError().text();
                return;
            }

            // 处理查询结果
            if (query.next()) {
                // 假设查询成功并且返回了至少一行结果
                name = query.value(0).toString(); // 获取name
                qDebug() << "找到姓名:" << name;
            } else {
                qDebug() << "没有找到对应的ID";
            }

        //新建一个窗口，显示详细信息
          student* s=new student();
          s->setID_and_Name(editName,name);
          s->show();

          //这个窗口自动关闭
        this->close();

    }
    else
    {
        //管理员模式
        if(editName=="0000"&&editCode=="0000")
        {
            Widget* w=new Widget();
            w->show();
            this->close();
        }
        else
        {
            qInfo()<<"没数据";
            ui->error->show();
            ui->error->setText("账号或密码错误");
        }

    }
}

//当输入框中内容发生变化的时候，把错误提示文字隐藏
void login::on_name_textChanged(const QString &arg1)
{
    //不想看见警告
    Q_UNUSED(arg1);
    ui->error->hide();
}
void login::on_code_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    ui->error->hide();
}

//按钮选择数据库，如果为空，自己会报错，可以再选择，如果获取到了，就转到正常登录界面
void login::on_selectDatabase_clicked()
{
    setDatabase();
    if(!fileName.isEmpty())
    {
        ui->stackedWidget->setCurrentIndex(0);



    }
}
