#ifndef CTESTWIDGET_H
#define CTESTWIDGET_H

#include <QWidget>

namespace Ui {
class CTestWidget;
}

class CTestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CTestWidget(QWidget *parent = 0);
    ~CTestWidget();

private:
    Ui::CTestWidget *ui;
};

#endif // CTESTWIDGET_H
