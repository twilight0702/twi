#include "widget.h"
#include "ui_widget.h"
#include "tip.h"
#include "login.h"
#include "courses.h"

#include <QCoreApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QKeyEvent>

#include <QMenu>
#include <QString>

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlRecord>



Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //页面的初始化部分
    openDatabase();
    showStudentsData();

    connect(ui->tableView, &QTableView::clicked, this, &Widget::on_tableView_clicked);
    connect(ui->tableView_4, &QTableView::clicked, this, &Widget::on_tableView_4_clicked);

    //允许表头显示排序
    ui->tableView->setSortingEnabled(true);
    ui->tableView_2->setSortingEnabled(true);
    ui->tableView_3->setSortingEnabled(true);
    //connect(ui->tableView_2->horizontalHeader(), &QHeaderView::sectionClicked, this, &Widget::showSortingMenu);
    //用过查询的排序要额外设计
    connect(ui->tableView_3->horizontalHeader(), &QHeaderView::sectionClicked, this, &Widget::showSortingMenu_3);
}

Widget::~Widget()
{
    delete ui;
}

//从login中获取文件路径，打开数据库
void Widget::openDatabase()
{
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
        qDebug() << "数据库连接成功！";
    }
}

//显示全部学生数据
void Widget::showStudentsData()
{
    model = new QSqlTableModel(this);
    model->setTable("students"); // 设置要操作的表
    model->setSort(0, Qt::AscendingOrder);
    model->select(); // 加载数据


    ui->tableView->setModel(model);

    //默认排序
    ui->tableView->sortByColumn(model->fieldIndex("student_id"), Qt::AscendingOrder);

    //隐藏密码行
    ui->tableView->hideColumn(model->fieldIndex("code"));

    //汉化
    QStringList headers={"学号","姓名","班级","电话","生日","住址"};
    for(int i=0;i<headers.length();i++)
    {
        model->setHeaderData(i,Qt::Horizontal,headers[i]);
    }
}

//添加学生
void Widget::on_add_clicked()
{
    if (!model->submitAll())
    {
        qDebug() << "Failed to save new student:" << model->lastError();
    }
    // 插入新行
    int row = model->rowCount();
    model->insertRow(row);

    // 设置默认值
    //设置默认姓名
    model->setData(model->index(row, model->fieldIndex("name")), "XXX"); // 姓名
    model->submitAll();
    //设置默认密码
    QString permanentCode = model->data(model->index(row,model->fieldIndex("code"))).toString();
    model->setData(model->index(row,7),permanentCode);

    // 让表格视图聚焦到新行
     ui->tableView->scrollTo(model->index(row, 1));
     ui->tableView->setCurrentIndex(model->index(row, 1));

     // 提交更改到数据库
     if (!model->submitAll())
     {
         qDebug() << "Failed to save new student:" << model->lastError();
     }
}

//删除学生
void Widget::on_delete_2_clicked()
{
    model->submitAll();
    qInfo()<<"删除按键被按下";


    // 获取选中的行
    QModelIndexList selectedRows = ui->tableView->selectionModel()->selectedIndexes();

    if (selectedRows.isEmpty())
    {
        qDebug() << "No row selected for deletion.";
        return;
    }

    //多行删除
    for (int i = selectedRows.size() - 1; i >= 0; --i)
    {
       if (!model->removeRow(selectedRows.at(i).row())) {
                   qDebug() << "Failed to remove row:" << model->lastError();
               }
    }

    //自动保存
    if (!model->submitAll())
    {
        qDebug() << "Failed to submit changes:" << model->lastError();
    }

    // 重新选择数据以刷新视图
    model->select();
}

//保存单元格内数据（修改了）
void Widget::on_deleteData_clicked()
{
    if (!model->submitAll())
    {
        qDebug() << "Failed to submit changes:" << model->lastError();
    }
    else
    {
        tip* t=new tip();
        t->setText("保存成功！");
        t->show();
    }
}

//按键盘delete批量删除单元格内数据
void Widget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete)
    {
        QModelIndexList selectedCells = ui->tableView->selectionModel()->selectedIndexes();

        qInfo()<<selectedCells.length();


        for (const QModelIndex &index : selectedCells)
        {
            //学号不能批量删除
            if((index.column()!=0)||(index.column()==0&&selectedCells.length()==1))
            {
                model->setData(index, "");// 设置为空字符串，清空单元格内容
                model->submitAll();
            }
        }
    }
}

//跳转显示详细信息（判断点击位置）
void Widget::on_tableView_clicked(const QModelIndex &index)
{
    int column = index.column(); // 获取点击的列号

    if (column == model->fieldIndex("选课信息"))
    { // 检查是否点击的是最后一列
        int row = index.row(); // 获取点击的行号
        studentID = model->data(model->index(row, model->fieldIndex("student_id"))).toString(); // 获取该行第一列的学号
        studentName = model->data(model->index(row,model->fieldIndex("name"))).toString();

        showCoursesForStudent(); // 根据学号检索并显示相关课程信息
    }
}

//跳转显示详细信息（显示）
void Widget::showCourseDataOfAStudent()
{
    model_2 = new QSqlTableModel(this);
    model_2->setTable("course");

    // 设置过滤条件，只检索与该学生ID相关的记录
    model_2->setFilter("student_id = '" + studentID + "'");


    // 加载数据
    if (!model_2->select()) {
        qDebug() << "Failed to select courses for student:" << model->lastError();
        return;
    }
    //翻页
    ui->stackedWidget->setCurrentIndex(1);


    // 显示在TableView中
    ui->tableView_2->setModel(model_2);

    //视图中隐藏两列
    ui->tableView_2->hideColumn(model_2->fieldIndex("student_id"));
    ui->tableView_2->hideColumn(model_2->fieldIndex("onlyCode"));

    model_2->setSort(model_2->fieldIndex("student_id"), Qt::AscendingOrder);

    ui->student_text->setText("学生 "+studentID+" "+studentName+" 的选课信息");

    //汉化
    QStringList headers={"学号","课程编号","课程名称","学分","学期","班级","成绩","唯一码"};
    for(int i=0;i<headers.length();i++)
    {
        model_2->setHeaderData(i,Qt::Horizontal,headers[i]);
    }
}


void Widget::showCoursesForStudent()
{
    showCourseDataOfAStudent();

}

//按钮显示全部学生信息
void Widget::on_showAllStudents_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    showStudentsData();
}

//添加课程
void Widget::on_add_3_clicked()
{
//    model_2->submitAll();
//    qInfo()<<"增加课程被按下";
//    qInfo()<<studentID;
//    if(studentID==nullptr)
//    {
//        qInfo()<<"没有";
//    }
//    // 插入新行
//    int row = model_2->rowCount();
//    model_2->insertRow(row);

//    // 设置默认值（可选）
//    model_2->setData(model_2->index(row, model_2->fieldIndex("student_id")), studentID.toInt()); // 姓名
//    //model_2->setData(model->index(row, 1), "123");
//    // 其他字段设置为默认值或空


//    // 让表格视图聚焦到新行
//     ui->tableView->scrollTo(model_2->index(row, 1));
//     ui->tableView->setCurrentIndex(model_2->index(row, 1));

//     // 提交更改到数据库
//     if (!model_2->submitAll())
//     {
//         qDebug() << "Failed to save new student:" << model_2->lastError();
//     }

    courses* c=new courses();
    c->set_student_id(studentID);
    c->show();
    connect(c,&courses::dataIsChange,this,&Widget::showCourseDataOfAStudent);


}

void Widget::on_showAllStudents_2_clicked()
{
    showCourseDataOfAStudent();
}


//删除课程
void Widget::on_delete_4_clicked()
{
    model_2->submitAll();
    qInfo()<<"删除按键被按下";

    // 获取选中的行
    QModelIndexList selectedRows = ui->tableView_2->selectionModel()->selectedIndexes();

    if (selectedRows.isEmpty())
    {
        qDebug() << "No row selected for deletion.";
        return;
    }
    qInfo()<<selectedRows.length();

    //多行删除
    for (int i = selectedRows.size() - 1; i >= 0; --i)
    {
       if (!model_2->removeRow(selectedRows.at(i).row()))
       {
           qDebug() << "Failed to remove row:" << model_2->lastError();
       }
    }

    //自动保存
    if (!model_2->submitAll())
    {
        qDebug() << "Failed to submit changes:" << model_2->lastError();
    }

    // 重新选择数据以刷新视图
    model_2->select();
}

//查询学生信息
void Widget::on_check_clicked()
{
    searchStudent();
}

void Widget::searchStudent()
{
    QString queryField = ui->comboBox->currentText(); // 获取查询范围
    QString queryValue = ui->lineEdit->text(); // 获取输入内容

       model->setFilter(queryField + " LIKE '%" + queryValue + "%'"); // 设置过滤条件

       if (!model->select())
       {
           qDebug() << "Failed to perform query:" << model->lastError();
           return;
       }

       ui->tableView->setModel(model);
}

//查询课程信息
void Widget::on_check_2_clicked()
{
    searchCourse();
}

void Widget::searchCourse()
{
    QString queryField = ui->comboBox_3->currentText(); // 获取查询范围
    QString queryValue = ui->lineEdit_2->text(); // 获取输入内容

       // 设置模型
       model_2->setFilter((queryField + " LIKE '%" + queryValue + "%' AND student_id='"+studentID+"'")); // 设置过滤条件
       //model_2->setFilter(queryField + " =" + queryValue ); // 设置过滤条件

       if (!model_2->select())
       {
           qDebug() << "Failed to perform query:" << model_2->lastError();
           return;
       }

       ui->tableView_2->setModel(model_2);
}
//跳转显示课程详细信息（跳转）
void Widget::on_tableView_4_clicked(const QModelIndex &index)
{
    int column = index.column(); // 获取点击的列号

    if (column == model_4->fieldIndex("detail"))
    {
        int row = index.row(); // 获取点击的行号
        courseID = model_4->data(model_4->index(row, model_4->fieldIndex("course_id"))).toString();
        courseName=model_4->data(model_4->index(row, model_4->fieldIndex("course_name"))).toString();// 获取该行第2列的课程号

        showCourses(courseID); // 检索并显示相关课程信息
    }
}

//跳转显示课程详细信息（显示）
void Widget::showCourses(const QString &courseID)
{
    QSqlQuery query;
       query.prepare("SELECT students.student_id,students.name, course.grade "
                     "FROM course "
                     "INNER JOIN students ON course.student_id = students.student_id "
                     "WHERE course.course_id = :course_id");
       query.bindValue(":course_id", courseID);

       if (!query.exec()) {
           qDebug() << "Failed to execute query:" << query.lastError();
           return;
       }

    model_3 = new QSqlQueryModel(this);
    model_3->setQuery(query);

    ui->stackedWidget->setCurrentIndex(2);

    // 显示在TableView中
    ui->tableView_3->setModel(model_3);

    ui->course_text->setText("选择课程 "+courseID+" "+courseName+" 的全部学生");

}


//第二页返回上一级
void Widget::on_showAllStudents_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    showStudentsData();
}

//第三页返回第二页
void Widget::on_return_2_clicked()
{
    showCourseDataOfAStudent();
}

//第三页返回第一页
void Widget::on_return_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    showStudentsData();
}

void Widget::showSortingMenu(int logicalIndex)
{
    QMenu sortingMenu;

    QAction* sortAsc = sortingMenu.addAction("升序");
    QAction* sortDesc = sortingMenu.addAction("降序");

    // Execute menu at the cursor position
    QAction* selectedAction = sortingMenu.exec(QCursor::pos());

    if (selectedAction == sortAsc) {
        ui->tableView_2->sortByColumn(logicalIndex, Qt::AscendingOrder);
    } else if (selectedAction == sortDesc) {
        ui->tableView_2->sortByColumn(logicalIndex, Qt::DescendingOrder);
    }
}

//第三页的排序
void Widget::showSortingMenu_3(int column)
{
    qInfo()<<courseID;

    Qt::SortOrder order = ui->tableView_3->horizontalHeader()->sortIndicatorOrder();

        QString columnName = model_3->headerData(column, Qt::Horizontal).toString();
        QString queryStr = QString("SELECT students.student_id, students.name, course.grade "
                                   "FROM course "
                                   "INNER JOIN students ON course.student_id = students.student_id "
                                   "WHERE course.course_id = :course_id "
                                   "ORDER BY students.%1 %2").arg(columnName, order == Qt::AscendingOrder ? "DESC" : "ASC");

        QSqlQuery query;
        query.prepare(queryStr);
        query.bindValue(":course_id", courseID);
        qDebug() << "Generated Query:" << queryStr;

        if (!query.exec()) {
            qDebug() << "Failed to execute query:" << query.lastError();
            return;
        }

        model_3->setQuery(query);
        ui->tableView_3->setModel(model_3);
}

//第二页的保存按钮
void Widget::on_save_2_clicked()
{
    if (!model_2->submitAll())
    {
        qDebug() << "Failed to submit changes:" << model_2->lastError();
    }
    else
    {
        tip* t=new tip();
        t->setText("保存成功！");
        t->show();
    }

}

//第四页
//全部课程信息
//显示全部课程信息
void Widget::on_showAllStudents_4_clicked()
{
    //页码跳转
    ui->stackedWidget->setCurrentIndex(3);
    model_4=new QSqlTableModel;
    model_4->setTable("classes");
    model_4->select();
    ui->tableView_4->setModel(model_4);
}

//增加课程
void Widget::on_pushButton_clicked()
{
    if (!model_4->submitAll())
    {
        qDebug() << "Failed to save new student:" << model_4->lastError();
    }
    // 插入新行
    int row = model_4->rowCount();
    model_4->insertRow(row);

    // 让表格视图聚焦到新行
     ui->tableView->scrollTo(model_4->index(row, 1));
     ui->tableView->setCurrentIndex(model_4->index(row, 1));

     // 提交更改到数据库
     if (!model_4->submitAll())
     {
         qDebug() << "Failed to save new student:" << model_4->lastError();
     }
}
//删除课程（一并删除所有有选这个课程的学生）
void Widget::on_pushButton_2_clicked()
{
    model_4->submitAll();
    qInfo()<<"删除按键被按下";


    // 获取选中的行
    QModelIndexList selectedRows = ui->tableView_4->selectionModel()->selectedIndexes();

    if (selectedRows.isEmpty())
    {
        qDebug() << "No row selected for deletion.";
        return;
    }

    //多行删除
    for (int i = selectedRows.size() - 1; i >= 0; --i)
    {
        //获取该行记录
        QSqlRecord rec=model_4->record(selectedRows[i].row());
        //获取课程编号
        QString course_id=rec.value("course_id").toString();
        //把course中该课程编号的所有数据删除
        QSqlQuery query;
        query.prepare("DELETE "
                      "FROM course "
                      "WHERE course_id=:course_id");
        query.bindValue(":course_id",course_id);
        if(!query.exec())
        {
            qDebug()<<"删除课程出错"<<query.lastError();
        }

       if (!model_4->removeRow(selectedRows.at(i).row())) {
                   qDebug() << "Failed to remove row:" << model_4->lastError();
               }
    }

    //自动保存
    if (!model_4->submitAll())
    {
        qDebug() << "Failed to submit changes:" << model_4->lastError();
    }

    // 重新选择数据以刷新视图
    model_4->select();
}
//保存按钮
void Widget::on_pushButton_3_clicked()
{
    if(model_4->submitAll())
    {
        tip* t=new tip();
        t->setText("保存成功！");
        t->show();
    }
}
