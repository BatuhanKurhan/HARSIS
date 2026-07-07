#include "../include/hasta_model.h"
#include <QString>

HastaModel::HastaModel(Depo<std::string, Hasta>* depo, QObject* parent)
    : QAbstractTableModel(parent), m_depo(depo) {
    yenile();
}

int HastaModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;
    return static_cast<int>(m_hastalar.size());
}

int HastaModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;
    return 5; // TC, İsim, Soyisim, Doğum Tarihi, Telefon
}

QVariant HastaModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(m_hastalar.size()))
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    const Hasta& hasta = m_hastalar[index.row()];

    switch (index.column()) {
        case 0: return QString::fromStdString(hasta.tc_no);
        case 1: return QString::fromStdString(hasta.isim);
        case 2: return QString::fromStdString(hasta.soyisim);
        case 3: {
            // Tarihi DD-MM-YYYY formatına çevir
            QString tarih = QString::fromStdString(hasta.dogum_tarihi);
            QStringList parcalar = tarih.split("-");
            if (parcalar.size() == 3) {
                return parcalar[2] + "-" + parcalar[1] + "-" + parcalar[0];
            }
            return tarih;
        }
        case 4: return QString::fromStdString(hasta.telefon);
        default: return QVariant();
    }
}

QVariant HastaModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return tr("TC No");
            case 1: return tr("İsim");
            case 2: return tr("Soyisim");
            case 3: return tr("Doğum Tarihi");
            case 4: return tr("Telefon");
            default: return QVariant();
        }
    }
    return QString::number(section + 1);
}

void HastaModel::yenile() {
    beginResetModel();
    m_hastalar.clear();
    for (const auto& [tc, hasta] : m_depo->tumunu_al()) {
        m_hastalar.push_back(hasta);
    }
    endResetModel();
}

const Hasta* HastaModel::hastaAl(int satir) const {
    if (satir >= 0 && satir < static_cast<int>(m_hastalar.size()))
        return &m_hastalar[satir];
    return nullptr;
}
