#include "../include/randevu_model.h"
#include <QString>
#include <QColor>
#include <QBrush>

RandevuModel::RandevuModel(Depo<int, Randevu>* depo, QObject* parent)
    : QAbstractTableModel(parent), m_depo(depo) {
    yenile();
}

int RandevuModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;
    return static_cast<int>(m_randevular.size());
}

int RandevuModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;
    return 6; // ID, TC No, Sicil No, Tarih, Saat, Durum
}

QVariant RandevuModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(m_randevular.size()))
        return QVariant();

    const Randevu& randevu = m_randevular[index.row()];

    // Durum sütunu için renklendirme
    if (role == Qt::BackgroundRole && index.column() == 5) {
        switch (randevu.durum) {
            case RandevuDurum::Beklemede:
                return QBrush(QColor(255, 255, 200)); // Açık sarı
            case RandevuDurum::Onaylandi:
                return QBrush(QColor(200, 255, 200)); // Açık yeşil
            case RandevuDurum::Iptal:
                return QBrush(QColor(255, 200, 200)); // Açık kırmızı
            case RandevuDurum::Tamamlandi:
                return QBrush(QColor(200, 200, 255)); // Açık mavi
        }
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    switch (index.column()) {
        case 0: return randevu.randevu_id;
        case 1: return QString::fromStdString(randevu.tc_no);
        case 2: return QString::fromStdString(randevu.sicil_no);
        case 3: {
            // Tarihi DD-MM-YYYY formatına çevir
            QString tarih = QString::fromStdString(randevu.tarih);
            QStringList parcalar = tarih.split("-");
            if (parcalar.size() == 3) {
                return parcalar[2] + "-" + parcalar[1] + "-" + parcalar[0];
            }
            return tarih;
        }
        case 4: return QString::fromStdString(randevu.saat);
        case 5: return durumMetni(randevu.durum);
        default: return QVariant();
    }
}

QVariant RandevuModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return tr("ID");
            case 1: return tr("Hasta TC");
            case 2: return tr("Doktor Sicil");
            case 3: return tr("Tarih");
            case 4: return tr("Saat");
            case 5: return tr("Durum");
            default: return QVariant();
        }
    }
    return QString::number(section + 1);
}

void RandevuModel::yenile() {
    beginResetModel();
    m_randevular.clear();
    for (const auto& [id, randevu] : m_depo->tumunu_al()) {
        m_randevular.push_back(randevu);
    }
    endResetModel();
}

const Randevu* RandevuModel::randevuAl(int satir) const {
    if (satir >= 0 && satir < static_cast<int>(m_randevular.size()))
        return &m_randevular[satir];
    return nullptr;
}

QString RandevuModel::durumMetni(RandevuDurum durum) const {
    switch (durum) {
        case RandevuDurum::Beklemede: return tr("Beklemede");
        case RandevuDurum::Onaylandi: return tr("Onaylandı");
        case RandevuDurum::Iptal: return tr("İptal");
        case RandevuDurum::Tamamlandi: return tr("Tamamlandı");
    }
    return tr("Bilinmiyor");
}
