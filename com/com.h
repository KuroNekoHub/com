#pragma once

#include <QtWidgets/QWidget>
#include "ui_com.h"

class com : public QWidget
{
    Q_OBJECT

public:
    com(QWidget *parent = Q_NULLPTR);

private:
    Ui::comClass ui;
};
