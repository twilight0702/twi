#include "courses.h"
#include "ui_courses.h"
#include "login.h"
#include "student.h"
#include "tip.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QString>
#include <QAbstractItemView>


courses::courses(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::courses)
{
    ui->setupUi(this);

    // 创建一个数据库对象
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    // 设置数据库文件路径（对于 SQLite）
    db.setDatabaseName(login::fileName);

    // 打开数据库连接
    if (!db.open())
    {
        qDebug() << "无法连接数据库：" << db.lastError();
    }
    else
    {
        qDebug() << "数据库连接成功！2";
    }

    showCourses();
    showNumberOfStudents();
    showCourses();
    //设置，不可编辑，单行选择
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

}

courses::~courses()
{
    delete ui;
}

void courses::set_student_id(const QString &id)
{
    student_id=id;
}

void courses::showCourses()
{
    qInfo()<<"刷新";
    model=new QSqlQueryModel;
    QSqlQuery query;
    query.prepare("SELECT course_id,course_name,credit,term,classroom,number_of_students,maximum_course_capacity "
                  "FROM classes");
    if(!query.exec())
    {
        qDebug()<<"显示全部课程错误"<<query.lastError();
    }
    model->setQuery(query);
    ui->tableView->setModel(model);
    //汉化部分
    QStringList header={"课程编号","课程名称","学分","学期","班级","已选人数","最大课容量"};
    for(int i=0;i<model->columnCount();i++)
    {
        model->setHeaderData(i,Qt::Horizontal,header[i]);
    }
}

void courses::showNumberOfStudents()
{
    //显示已选人数
    for(int i=0;i<model->rowCount();i++)
    {
        //获取一行数据
        QSqlRecord arec=model->record(i);
        //获取课程编号
        int course_id=arec.value("course_id").toInt();

        //qInfo()<<"课程编号="<<course_id;

        //查找有多少人选了这门课
        QSqlQueryModel* tempModel=new QSqlQueryModel;

        QSqlQuery tempQuery;
        tempQuery.prepare("SELECT * "
                      "FROM course "
                      "WHERE course_id=:courseID ");
        tempQuery.bindValue(":courseID",course_id);

        if(!tempQuery.exec())
        {
            qDebug()<<tempQuery.lastError();
        }


        tempModel->setQuery(tempQuery);
        int numberOfStudents=tempModel->rowCount();

        //qInfo()<<"人数："<<numberOfStudents;

//        arec.setValue("number_of_students",numberOfStudents);
        tempQuery.clear();



        //刷新数据
       QSqlQuery tempQuery2;
       tempQuery2.prepare("UPDATE classes "
                          "SET number_of_students=:numberofstudents "
                          "WHERE course_id=:courseID");
       tempQuery2.bindValue(":numberofstudents",numberOfStudents);
       tempQuery2.bindValue(":courseID",course_id);

       if(!tempQuery2.exec())
       {
           qDebug()<<tempQuery2.lastError().text();
           qDebug()<<tempQuery2.lastError().number();
       }
       tempQuery2.clear();
    }
}

//选择按钮
void courses::on_pushButton_clicked()
{
       //获取选择的行
       QModelIndexList selectedRows = ui->tableView->selectionModel()->selectedRows();
       //循环列表
       for(int i=0;i<selectedRows.length();i++)
       {
           //获取行号
           int currentRow=selectedRows[i].row();
           //获取该行记录
           QSqlRecord rec=model->record(currentRow);
           //判断是否已经选择了这个课程
           QString course_id=rec.value("course_id").toString();
           QSqlQuery query;
           query.prepare("SELECT * "
                         "FROM course "
                         "WHERE student_id=:student_id AND course_id=:course_id");
           query.bindValue(":student_id",student_id);
           query.bindValue(":course_id",course_id);
           if(query.exec())
           {
               if(query.next())
               {
                   qInfo()<<"选过了";
                   tip* t=new tip();
                   t->setText("课程"+course_id+rec.value("course_name").toString()+"已经选择");
                   t->show();
               }
               else
               {
                   //判断是否选满
                   int numberodstudent=rec.value("number_of_students").toInt();
                   int max=rec.value("maximum_course_capacity").toInt();

                   if(numberodstudent<max)
                   {
                       QSqlQuery query2;
                       query2.prepare("INSERT INTO course (student_id,course_id,course_name,credit,term,classroom,grade,onlyCode)"
                                     "VALUES(:student_id,:course_id,:course_name,:credit,:term,:classroom,:grade,:onlyCode)");

                       query2.bindValue(":student_id",student_id);
                       query2.bindValue(":course_id",rec.value("course_id").toString());
                       query2.bindValue(":course_name",rec.value("course_name"));
                       query2.bindValue(":credit",rec.value("credit"));
                       query2.bindValue(":term",rec.value("term"));
                       query2.bindValue(":classroom",rec.value("classroom"));
                       query2.bindValue(":grade",rec.value("grade"));
                       query2.bindValue(":onlyCode",student_id+rec.value("course_id").toString());

                       if(!query2.exec())
                       {
                           qDebug()<<"插入数据出错"<<query2.lastError();
                       }
                   }
                   else
                   {
                       qInfo()<<"选满了";
                       tip* t=new tip();
                       t->setText("课程已被选满");
                       t->show();
                   }
               }
           }
           if(!query.exec())
           {
               qDebug()<<"选择课程出错"<<query.lastError();
           }
        }
       emit dataIsChange();
       showNumberOfStudents();
       showCourses();
}

void courses::on_refresh_clicked()
{
    showNumberOfStudents();
    showCourses();
}


