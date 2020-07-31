#include "com.h"
#include <qdir.h>
#include <QFileDialog>
#include <QMap>
#include <QList>
#include "zlib.h"
#include <QDateTime>
#include <QDesktopServices>
#include "quicklz.h"
#include <QProcess>
using namespace std;

typedef unsigned long ulong;
typedef struct DATA
{
	uchar *p;
	ulong len;
}DATA;
com::com(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
	this->setWindowFlags(Qt::WindowCloseButtonHint);
	connect(ui.radioButton_zlib, SIGNAL(clicked()), this, SLOT(setZlib()));
	connect(ui.radioButton_quicklz, SIGNAL(clicked()), this, SLOT(setQuicklz())); 
	connect(ui.pushButton_compress, SIGNAL(clicked()), this, SLOT(onButtonClickedCompress()));
	connect(ui.pushButton_decompress, SIGNAL(clicked()), this, SLOT(onButtonClickedDecompress()));
	connect(ui.listWidget_image, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onListDoubleClicked()));
	ui.textBrowser->textCursor().insertText("welcome\n");
}

void com::setZlib()
{
	m_method = ZLIB;
}

void com::setQuicklz()
{
	m_method = QUICKLZ;
}

void com::onButtonClickedCompress()
{
	QString curPath = QDir::currentPath();
	QString dlgTitle = "select files";
	QString filter = "(*.HCT);;(*.HLX)";
	QStringList fileList = QFileDialog::getOpenFileNames(this, dlgTitle, curPath, filter);
	if (fileList.isEmpty()) {
		outputInfo("canceled...\n");
		return;
	}
	outputInfo("********************\n");
	outputInfo("reading data...\n");

	FILE *ifile;
	QList<DATA> qlData;
	ulong lenBeforeCompress = 0;

	for (int k = 0; k < fileList.count(); ++k) {
		QString str = fileList.at(k);
		QByteArray ba = str.toLocal8Bit();  // toLocal8Bit 支持中文
		const char *fileName = ba.data();
		DATA res;

		if ((ifile = fopen(fileName, "rb")) == NULL) {
			outputInfo("read error...\n");
			return;
		}

		fseek(ifile, 0, SEEK_END);

		res.len = ftell(ifile);
		lenBeforeCompress += res.len;
		res.p = new uchar[res.len]();
		fseek(ifile, 0, SEEK_SET);
		fread(res.p, sizeof(uchar), res.len, ifile);
		fclose(ifile);
		qlData << res;
	}
	uchar *src = new uchar[lenBeforeCompress];
	uchar *tmp = src;
	for (int k = 0; k < fileList.count(); ++k) {
		memcpy(tmp, qlData[k].p, sizeof(uchar)*qlData[k].len);
		tmp += qlData[k].len;
	}

	uchar *dst = NULL;//dst buffer
	ulong lenAfterCompress = lenBeforeCompress;

	int tmMsec = 0;//run time
	outputInfo("compressing...\n");

	QTime timeCounter;//计时器
	timeCounter.start();
	switch (m_method) {
	case ZLIB: {
		lenAfterCompress = lenBeforeCompress;
		dst = new uchar[lenAfterCompress]();

		compress(static_cast<Byte*>(dst), &lenAfterCompress, static_cast<Byte*>(src), lenBeforeCompress);
		
		break;
	}
	case QUICKLZ: {
		//pre malloc destination buffer
		lenAfterCompress = lenBeforeCompress + 400;
		dst = new uchar[lenAfterCompress]();

		qlz_state_compress *state_compress = (qlz_state_compress *)malloc(sizeof(qlz_state_compress));
		lenAfterCompress = qlz_compress(src, static_cast<uchar *>(dst), lenBeforeCompress, state_compress);

		break;
	}
	}

	tmMsec = timeCounter.elapsed();//毫秒数

	filter = m_method == ZLIB ? "(*.zlib)" : "(*.qlz)";
	QString save = QFileDialog::getSaveFileName(this, QString("select dir to save"), "", filter);
	m_sPath = save.left(save.lastIndexOf("/"));

	if (save.isEmpty()) {
		delete[]src; delete[]dst;
		for (int k = 0; k < fileList.count(); ++k) {
			delete[](qlData[k].p);
		}
		outputInfo("canceled...\n");
		return;
	}
	QByteArray ba = save.toLocal8Bit();  // toLocal8Bit 支持中文
	const char *fileName = ba.data();
	FILE *ofile;
	ofile = fopen(fileName, "wb");

	outputInfo("saving files...\n");

	ui.listWidget_image->clear();// 清空 list
	QListWidgetItem *item = new QListWidgetItem;
	item->setText(save);
	if (save != NULL) {
		ui.listWidget_image->addItem(item);
	}

	fwrite(dst, sizeof(uchar), lenAfterCompress, ofile);
	ulong bytesPerFile = lenBeforeCompress / fileList.count();
	fwrite(&bytesPerFile, sizeof(ulong), 1, ofile);
	ulong frames = fileList.count();
	fwrite(&frames, sizeof(ulong), 1, ofile);
	fclose(ofile);

	delete[]src; delete[]dst;
	for (int k = 0; k < fileList.count(); ++k) {
		delete[](qlData[k].p);
	}

	outputInfo("done...\n");
	char runTime[1000];
	sprintf_s(runTime, "run time is %dms...\n", tmMsec);
	outputInfo(QString::fromLocal8Bit(runTime));
	sprintf_s(runTime, "ratio is %.2f%%...\n", 100.0*lenAfterCompress / lenBeforeCompress);
	outputInfo(QString::fromLocal8Bit(runTime));
	outputInfo("********************\n");
}

void com::onButtonClickedDecompress()
{
	QString curPath = QDir::currentPath();
	QString dlgTitle = "select files";
	QString filter = m_method == ZLIB ? "(*.zlib)" : "(*.qlz)";
	QString aFileName = QFileDialog::getOpenFileName(this, dlgTitle, curPath, filter);
	if (aFileName.isEmpty()) {
		outputInfo("canceled...\n");
		return;
	}
	outputInfo("********************\n");
	outputInfo("reading files...\n");
	QByteArray ba = aFileName.toLocal8Bit();  // toLocal8Bit 支持中文
	const char *input = ba.data();
	FILE *ifile;
	if ((ifile = fopen(input, "rb")) == NULL) {
		outputInfo("read error...\n");
		return;
	}

	fseek(ifile, -2 * sizeof(ulong), SEEK_END);
	ulong byteAfterCompress = ftell(ifile); //压缩后的字节数
	ulong frames, bytesPerFile;// 幅数 每幅字节数
	fseek(ifile, -2 * sizeof(ulong), SEEK_END);
	fread(&bytesPerFile, sizeof(ulong), 1, ifile);
	fread(&frames, sizeof(ulong), 1, ifile);

	uchar *src = new uchar[byteAfterCompress]();// 压缩后的数据指针
	fseek(ifile, 0, SEEK_SET);
	fread(src, sizeof(uchar), byteAfterCompress, ifile);
	fclose(ifile);

	ulong byteBeforeCompress = frames*bytesPerFile;
	uchar *dst = new uchar[byteBeforeCompress]();// 解压缩的输出指针

	int tmMsec = 0;
	outputInfo("decompressing...\n");
	QTime timeCounter;//计时器
	timeCounter.start();

	switch (m_method) {
	case ZLIB: {
		uncompress(static_cast<Byte*>(dst), &byteBeforeCompress, static_cast<Byte*>(src), byteAfterCompress);
		
		break;
	}
	case QUICKLZ: {
		qlz_state_decompress *state_decompress = (qlz_state_decompress *)malloc(sizeof(qlz_state_decompress));
		byteBeforeCompress = qlz_size_decompressed((char*)(src));

		// decompress and write result
		byteBeforeCompress = qlz_decompress((char*)(src), dst, state_decompress);
		
		break;
	}
	}
	
	tmMsec = timeCounter.elapsed();//毫秒数
	delete[]src;

	outputInfo("saving files...\n");
	// 选择输出目录
	m_sPath = QFileDialog::getExistingDirectory(this, tr("select dir to release"), "", QFileDialog::ShowDirsOnly);
	if (m_sPath.isEmpty()) {
		delete[]dst;
		outputInfo("canceled...\n");
		return;
	}
	ba = m_sPath.toLocal8Bit();  // toLocal8Bit 支持中文
	const char *dir = ba.data();

	FILE *ofile;
	char fileName[1000];
	ui.listWidget_image->clear();
	for (int k = 0; k < frames; ++k) {
		sprintf_s(fileName, "%s/%d.HCT", dir, k);
		if ((ofile = fopen(fileName, "wb")) == NULL) {
			delete[]dst;
			outputInfo("write error...\n");
			return;
		}

		fwrite(dst + k*bytesPerFile, sizeof(uchar), bytesPerFile, ofile);
		fclose(ofile);

		// list 显示
		QString text = QString::fromLocal8Bit(fileName);
		QListWidgetItem *item = new QListWidgetItem;
		item->setText(text);
		if (text != NULL) {
			ui.listWidget_image->addItem(item);
		}
	}

	delete[]dst;
	outputInfo("done...\n");
	char runTime[100];
	sprintf_s(runTime, "run time is %dms...\n", tmMsec);
	outputInfo(QString::fromLocal8Bit(runTime));
	sprintf_s(runTime, "ratio is %.2f%%...\n", 100.0*byteAfterCompress / byteBeforeCompress);
	outputInfo(QString::fromLocal8Bit(runTime));
	outputInfo("********************\n");
}

void com::onListDoubleClicked()
{
	if (m_sPath != NULL) {
		QListWidgetItem *currentItem = ui.listWidget_image->currentItem();
		QString path = currentItem->text();
		path = path.replace("/", "\\");
		QString select = "explorer.exe  /select,\"" + path + "\"";

		QProcess process;
		process.startDetached(select);
	}
}

QString com::getTime()
{
	QDateTime curDateTime = QDateTime::currentDateTime();
	return curDateTime.toString("hh:mm:ss:");
}

void com::outputInfo(QString str)
{
	ui.textBrowser->moveCursor(QTextCursor::End);
	ui.textBrowser->textCursor().insertText(getTime() + str);
	qApp->processEvents();
}
