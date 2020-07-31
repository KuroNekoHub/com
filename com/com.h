#pragma once

#include <QtWidgets/QWidget>
#include "ui_com.h"

enum method
{
	QUICKLZ = 0,
	ZLIB = 1
};
class com : public QWidget
{
    Q_OBJECT

public:
    com(QWidget *parent = Q_NULLPTR);
signals:

	private slots :
		void setZlib();
		void setQuicklz();
		void onButtonClickedCompress();
		void onButtonClickedDecompress();
		void onListDoubleClicked();
private:
    Ui::comClass ui;
	method m_method = ZLIB;
	QString getTime();
	void outputInfo(QString str);
	QString m_sPath;
	
};
