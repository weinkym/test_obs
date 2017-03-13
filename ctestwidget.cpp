#include "ctestwidget.h"
#include "ui_ctestwidget.h"
#include "obs-app.hpp"
#include "window-basic-main.hpp"

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

void CTestWidget::on_btnStart_clicked()
{
    OBSBasic *main = reinterpret_cast<OBSBasic*>(App()->GetMainWindow());
    main->setOutputCrop(this->geometry());
}
