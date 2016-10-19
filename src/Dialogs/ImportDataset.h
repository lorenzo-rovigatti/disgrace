#ifndef IMPORTDATASET_H
#define IMPORTDATASET_H

#include <QDialog>

namespace Ui {
class ImportDataset;
}

namespace dg {

struct ImportDatasetResult {
	QString filename;
	QString load_as;
	QString autoscale;
};

class ImportDataset: public QDialog {
Q_OBJECT

public:
	explicit ImportDataset(QWidget *parent = 0);
	~ImportDataset();

	ImportDatasetResult get_options() { return _options; }

public slots:
	void select_import_file();
	void begin_import();

signals:
	void import_ready(ImportDatasetResult &);

private:
	ImportDatasetResult _options;
	Ui::ImportDataset *_ui;
	bool _is_error;
};

} /* namespace dg */

#endif // IMPORTDATASET_H
