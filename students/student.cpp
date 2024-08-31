#include "student.h"
#include "ui_student.h"
#include "login.h"
#include "tip.h"
#include "courses.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QString>
#include <QAbstractItemView>


student::student(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::student)
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

    //设置选择模式
    //第一页，不可编辑，单行选择
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    //第二页，不可编辑，多行选择
    ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_2->setSelectionMode(QAbstractItemView::MultiSelection);

    coursesInstance=new courses();
    //实时刷新
    connect(coursesInstance,&courses::dataIsChange,this,&student::showCourse);
}

void student::setCoursesInstance(courses c)
{
    coursesInstance=&c;
}

student::~student()
{
    delete ui;
}

//设置欢迎语,以及显示信息
void student::setID_and_Name(QString studentID,QString studentName)
{
    id=studentID;
    name=studentName;
    ui->welcome->setText("欢迎你， "+id+name+" 同学！");

    showCourse();
}

//显示课程信息
void student::showCourse()
{
    QSqlQuery query;
    query.prepare("SELECT course_id, course_name, credit, term, classroom, grade "
                  "FROM course "
                  "WHERE student_id=:student_id");
    query.bindValue(":student_id",id);
    if (!query.exec()) {
        qDebug() << "Failed to execute query:" << query.lastError();
        return;
    }
    model1=new QSqlQueryModel;
    model1->setQuery(query);
    ui->tableView->setModel(model1);

}

//挑选课程
void student::on_pushButton_clicked()
{
//    ui->stackedWidget->setCurrentIndex(1);
//    model2=new QSqlQueryModel;

//    QSqlQuery query;
//    query.prepare("SELECT course_id,course_name,credit,term,classroom,number_of_students,maximum_course_capacity "
//                  "FROM classes");
//    query.exec();
//    model2->setQuery(query);

//    ui->tableView_2->setModel(model2);

//    //汉化部分
//    QStringList header={"课程编号","课程名称","学分","学期","班级","已选人数","最大课容量"};
//    for(int i=0;i<model2->columnCount();i++)
//    {
//        model2->setHeaderData(i,Qt::Horizontal,header[i]);
//    }

//    //显示已选人数
//    for(int i=0;i<model2->rowCount();i++)
//    {
//        //获取一行数据
//        QSqlRecord arec=model2->record(i);
//        //获取课程编号
//        int course_id=arec.value("course_id").toInt();

//        qInfo()<<"课程编号="<<course_id;

//        //查找有多少人选了这门课
//        QSqlQueryModel* tempModel=new QSqlQueryModel;

//        QSqlQuery tempQuery;
//        tempQuery.prepare("SELECT * "
//                      "FROM course "
//                      "WHERE course_id=:courseID ");
//        tempQuery.bindValue(":courseID",course_id);

//        if(!tempQuery.exec())
//        {
//            qDebug()<<tempQuery.lastError();
//        }

//        tempModel->setQuery(tempQuery);
//        int numberOfStudents=tempModel->rowCount();
//        qInfo()<<"人数："<<numberOfStudents;

////        arec.setValue("number_of_students",numberOfStudents);
//        tempQuery.clear();



//        //刷新数据
//       QSqlQuery tempQuery2;
//       tempQuery2.prepare("UPDATE classes "
//                          "SET number_of_students=:numberofstudents "
//                          "WHERE course_id=:courseID");
//       tempQuery2.bindValue(":numberofstudents",numberOfStudents);
//       tempQuery2.bindValue(":courseID",course_id);

//       if(!tempQuery2.exec())
//       {
//           qDebug()<<tempQuery2.lastError().text();
//           qDebug()<<tempQuery2.lastError().number();
//       }
//       tempQuery2.clear();

//    }

//    //更新后的数据会自动同步到视图，不用再写一遍


    courses* c=new courses();
    coursesInstance=c;
    c->set_student_id(id);
    c->show();
    connect(c,&courses::dataIsChange,this,&student::showCourse);




}

//选课的“选择”按钮
void student::on_select_clicked()
{
    //获取选择的行
    QModelIndexList selectedRows = ui->tableView_2->selectionModel()->selectedRows();
    //循环列表
    for(int i=0;i<selectedRows.length();i++)
    {
        //获取行号
        int currentRow=selectedRows[i].row();
        //获取该行记录
        QSqlRecord rec=model2->record(currentRow);
        //判断是否已经选择了这个课程
        QString course_id=rec.value("course_id").toString();
        QSqlQuery query;
        query.prepare("SELECT * "
                      "FROM course "
                      "WHERE student_id=:student_id AND course_id=:course_id");
        query.bindValue(":student_id",id);
        query.bindValue(":course_id",course_id);
        if(query.exec())
        {
            if(query.next())
            {
                tip* t=new tip();
                t->setText("课程"+course_id+rec.value("course_name").toString()+"已经选择");
                t->show();
            }
            else
            {
                query.clear();
                query.prepare("INSERT INTO course (student_id,course_id,course_name,credit,term,classroom,grade,onlyCode)"
                              "VALUES(:student_id,:course_id,:course_name,:credit,:term,:classroom,:grade,:onlyCode)");

                query.bindValue(":student_id",id);
                query.bindValue(":course_id",rec.value("course_id").toString());
                query.bindValue(":course_name",rec.value("course_name"));
                query.bindValue(":credit",rec.value("credit"));
                query.bindValue(":term",rec.value("term"));
                query.bindValue(":classroom",rec.value("classroom"));
                query.bindValue(":grade",rec.value("grade"));
                query.bindValue(":onlyCode",id+rec.value("course_id").toString());

                qInfo()<<rec.value("course_id");
                if(!query.exec())
                {
                    qDebug()<<"选择课程出错"<<query.lastError();
                }

                //跳转回第一页，并且刷新数据
                ui->stackedWidget->setCurrentIndex(0);
                showCourse();

            }
        }
        else
        {
            qDebug()<<"判断是否选择课程出错"<<query.lastError();
        }






    }


}

//删除课程
void student::on_deleteCourse_clicked()
{
    QModelIndexList selectRows =ui->tableView->selectionModel()->selectedRows();
    for(int i=selectRows.length()-1;i>=0;i--)
    {
        int currentRow=selectRows[i].row();
        QSqlRecord rec=model1->record(currentRow);
        QSqlQuery query;
        query.prepare("DELETE "
                      "FROM course "
                      "WHERE student_id=:student_id AND course_id=:course_id");
        query.bindValue(":student_id",id);
        query.bindValue(":course_id",rec.value("course_id"));

        if(!query.exec())
        {
            qDebug()<<"删除课程错误"<<query.lastError();
        }
    }
    //刷新数据
    showCourse();
}



//刷新按钮
void student::on_refresh_clicked()
{
    showCourse();
}
