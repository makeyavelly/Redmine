#include <Types.h>

#include <QHeaderView>

User::User(const QString &name, const uint &id, const QString &title) :
    id(id),
    name(name),
    title(title)
{
}


Project::Project(const int &id, const QString &name, const int &parent) :
    id(id),
    parent(parent),
    name(name)
{
}


Track::Track(const int &id, const QString &name) :
    id(id),
    name(name)
{
}


Level::Level(const int &level, const QString &name) :
    level(level),
    name(name)
{
}


Button::Button(const QString &name, const QIcon &icon, const int &task, const int &status) :
    QPushButton(icon, QString()),
    _task(task),
    _status(status)
{
    setToolTip(name);
    setFlat(true);
    setIconSize(QSize(20, 20));
    //setMaximumSize(24, 24);
}

int Button::task() const
{
    return _task;
}

int Button::status() const
{
    return _status;
}


Check::Check(const int &id, const QString &name, const bool &done, QWidget *parent) :
    QCheckBox(name, parent),
    _id(id)
{
    setChecked(done);
}

int Check::id() const
{
    return _id;
}

bool Check::done()
{
    return isChecked();
}


PercentScale::PercentScale(const int &id, const int &value, QWidget *parent) :
    QWidget(parent),
    _id(id)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    this->setLayout(layout);
    layout->setMargin(0);

    _slider = new QSlider(Qt::Horizontal, this);
    _slider->setFixedWidth(200);
    _slider->setMinimum(0);
    _slider->setMaximum(100 / STEP);
    _slider->setTickInterval(TICK);
    _slider->setTickPosition(QSlider::TicksBothSides);

    QLabel *labelName = new QLabel("Процент выполнения");

    _labelPercent = new QLabel(QString("%1 %").arg(value), this);

    _btOk = new QPushButton(QIcon("://ok"), QString(), this);
    _btOk->setFlat(true);

    QObject::connect(_slider, SIGNAL(valueChanged(int)), this, SLOT(slotSetValue(int)));
    QObject::connect(_btOk, SIGNAL(clicked()), this, SLOT(slotChangeValue()));

    layout->addWidget(labelName);
    layout->addWidget(_slider);
    layout->addWidget(_labelPercent);
    layout->addWidget(_btOk);
    layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));

    this->setValue(value);
    _btOk->setEnabled(false);
}

PercentScale::~PercentScale()
{
    if (_btOk->isEnabled()) {
        if (QMessageBox::question(this, "Предупреждение", "Сохранить процент выполнения?",
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            _btOk->click();
        }
    }
}

int PercentScale::id() const
{
    return _id;
}

int PercentScale::value() const
{
    return STEP * _slider->value();
}

void PercentScale::setValue(const int &newValue)
{
    _slider->setValue(newValue / STEP);
}

void PercentScale::slotSetValue(const int &newValue)
{
    int percent = STEP * newValue;
    _labelPercent->setText(QString("%1 %").arg(percent));
    _btOk->setEnabled(true);
}

void PercentScale::slotChangeValue()
{
    _btOk->setEnabled(false);
    emit s_changeValue(value());
}



QTreeWidgetItem *findTreeWidgetItem(QTreeWidgetItem *parent, const QString &value,
                                    const int &column, const int &role)
{
    for (int i = 0; i < parent->childCount(); ++i) {
        QTreeWidgetItem *item = parent->child(i);
        if (item->data(column, role).toString() == value) {
            return item;
        }
        item = findTreeWidgetItem(item, value, column, role);
        if (item) {
            return item;
        }
    }
    return 0x0;
}

QTreeWidgetItem *findTreeWidgetItem(QTreeWidget *tree, const QString &value,
                                    const int &column, const int &role)
{
    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = tree->topLevelItem(i);
        if (item->data(column, role).toString() == value) {
            return item;
        }
        item = findTreeWidgetItem(item, value, column, role);
        if (item) {
            return item;
        }
    }
    return 0x0;
}


QTreeWidgetItem *findTreeWidgetItem(QTreeWidget *tree, const int &value,
                                    const int &column, const int &role)
{
    return findTreeWidgetItem(tree, QString::number(value), column, role);
}


QList<int> loadListIntFromFile(const QString &fileName)
{
    QList<int> list;
    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        foreach (QByteArray line, file.readAll().split('\n')) {
            int value = line.toInt();
            if (value > 0) {
                list.append(value);
            }
        }
    }
    file.close();
    return list;
}

void saveListIntIntoFile(const QList<int> &list, const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QFile::WriteOnly)) {
        foreach (int project, list) {
            file.write(QString("%1\n").arg(project).toAscii());
        }
    }
    file.close();
}


void createTableHeader(QTableWidget *table, const QStringList &labels)
{
    table->setRowCount(0);
    table->clear();
    table->setColumnCount(labels.count());
    table->setHorizontalHeaderLabels(labels);
}

int insertRowIntoTable(QTableWidget *table, const int &row)
{
    int res = row;
    if (res < 0) {
        res = table->rowCount();
    }
    table->insertRow(res);
    for (int i = 0; i < table->columnCount(); ++i) {
        table->setItem(res, i, new QTableWidgetItem());
    }
    return res;
}

void resizeTable(QTableWidget *table, const int &notFixColumn, const int &minWidthFixColumn)
{
    table->resizeColumnsToContents();
    int w = table->width() - table->verticalHeader()->width() - 24;
    for (int i = 0; i < table->columnCount(); ++i) {
        if (i != notFixColumn) {
            w -= table->columnWidth(i);
        }
    }
    w = qMax(w, minWidthFixColumn);
    table->setColumnWidth(notFixColumn, w);
    table->resizeRowsToContents();
}

void setRowColor(QTableWidget *table, const int &row, const QColor &color)
{
    if (table && row >= 0 && row < table->rowCount()) {
        for (int i = 0; i < table->columnCount(); ++i) {
            if (table->item(row, i)) {
                table->item(row, i)->setBackgroundColor(color);
            }
        }
    }
}

void setRowTextColor(QTableWidget *table, const int &row, const QColor &color)
{
    if (table && row >= 0 && row < table->rowCount()) {
        for (int i = 0; i < table->columnCount(); ++i) {
            if (table->item(row, i)) {
                table->item(row, i)->setTextColor(color);
            }
        }
    }
}


int findDataInCombo(QComboBox *combo, const QString &value)
{
    for (int i = 0; i < combo->count(); ++i) {
        if (combo->itemData(i).toString() == value) {
            return i;
        }
    }
    return -1;
}


int findDataInCombo(QComboBox *combo, const int &value)
{
    return findDataInCombo(combo, QString::number(value));
}


const QColor getStatusColor(const int &status)
{
    switch (status) {
    case st_TaskNew:
        return Qt::white;
    case st_TaskWorking:
        return QColor("#cfcfff");
    case st_TaskTesting:
        return QColor("#cfffcf");
    case st_TaskReworking:
        return Qt::white;
    case st_TaskClose:
        return QColor("#dfdfdf");
    case st_TaskReject:
        return QColor("#ffce84");
    }
    return Qt::white;
}


const QColor getNewStatusColor(const int &status)
{
    switch (status) {
    case st_TaskNew:
        return Qt::black;
    case st_TaskReworking:
        return QColor("#8f0000");
    case st_TaskPause:
        return QColor("#6f6f6f");
    }
    return Qt::white;
}
