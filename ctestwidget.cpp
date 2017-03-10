#include "ctestwidget.h"
#include "ui_ctestwidget.h"

CTestWidget::CTestWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CTestWidget)
{
    ui->setupUi(this);
}

CTestWidget::~CTestWidget()
{
    delete ui;
}
