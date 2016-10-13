#ifndef SETAPPEARANCE_H
#define SETAPPEARANCE_H

#include <QDialog>

namespace Ui {
class SetAppearance;
}

class SetAppearance: public QDialog {
Q_OBJECT

public:
	explicit SetAppearance(QWidget *parent = 0);
	~SetAppearance();

private:
	Ui::SetAppearance *ui;
};

#endif // SETAPPEARANCE_H
