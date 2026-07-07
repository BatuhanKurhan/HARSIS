#include "../include/doktor_model.h"
#include <QString>

DoktorModel::DoktorModel(Depo<std::string, Doktor>* depo, QObject* parent)
    : QAbstractTableModel(parent), m_depo(depo) {
    yenile();
}

int DoktorModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;
    return static_cast<int>(m_doktorlar.size());
}

int DoktorModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;
    return 4; // Sicil No, İsim, Soyisim, Branş
}

QVariant DoktorModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(m_doktorlar.size()))
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    const Doktor& doktor = m_doktorlar[index.row()];

    switch (index.column()) {
        case 0: return QString::fromStdString(doktor.sicil_no);
        case 1: return QString::fromStdString(doktor.isim);
        case 2: return QString::fromStdString(doktor.soyisim);
        case 3: return QString::fromStdString(doktor.brans);
        default: return QVariant();
    }
}

QVariant DoktorModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return tr("Sicil No");
            case 1: return tr("İsim");
            case 2: return tr("Soyisim");
            case 3: return tr("Branş");
            default: return QVariant();
        }
    }
    return QString::number(section + 1);
}

void DoktorModel::yenile() {
    beginResetModel();
    m_doktorlar.clear();
    for (const auto& [sicil, doktor] : m_depo->tumunu_al()) {
        m_doktorlar.push_back(doktor);
    }
    endResetModel();
}

const Doktor* DoktorModel::doktorAl(int satir) const {
    if (satir >= 0 && satir < static_cast<int>(m_doktorlar.size()))
        return &m_doktorlar[satir];
    return nullptr;
}
