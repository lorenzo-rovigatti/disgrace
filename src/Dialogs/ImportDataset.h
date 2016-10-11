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

	// Slots
	void select_import_file();
	void done(int r)  Q_DECL_OVERRIDE;

private:
	ImportDatasetResult _options;
	Ui::ImportDataset *_ui;
};

} /* namespace dg */

#endif // IMPORTDATASET_H
