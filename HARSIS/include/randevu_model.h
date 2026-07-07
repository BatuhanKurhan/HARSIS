#pragma once

/**
 * randevu_model.h -- Randevu Tablo Modeli
 *
 * QAbstractTableModel tabanlı randevu listesi görünümü.
 * Durum bilgisine göre renklendirme yapılır.
 */

#include "varliklar.h"
#include "depo.h"

#include <QAbstractTableModel>
#include <vector>

class RandevuModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit RandevuModel(Depo<int, Randevu>* depo, QObject* parent = nullptr);

    // QAbstractTableModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Veri güncelleme
    void yenile();
    const Randevu* randevuAl(int satir) const;

private:
    Depo<int, Randevu>* m_depo;
    std::vector<Randevu> m_randevular;
    
    QString durumMetni(RandevuDurum durum) const;
};
