#include "widget.h"
#include "login.h"
#include "student.h"
#include "courses.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    login l;
    l.show();

    return a.exec();
}
