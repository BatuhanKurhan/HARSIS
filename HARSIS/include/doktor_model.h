#pragma once

/**
 * doktor_model.h -- Doktor Tablo Modeli
 *
 * QAbstractTableModel tabanlı doktor listesi görünümü.
 */

#include "varliklar.h"
#include "depo.h"

#include <QAbstractTableModel>
#include <vector>

class DoktorModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit DoktorModel(Depo<std::string, Doktor>* depo, QObject* parent = nullptr);

    // QAbstractTableModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Veri güncelleme
    void yenile();
    const Doktor* doktorAl(int satir) const;

private:
    Depo<std::string, Doktor>* m_depo;
    std::vector<Doktor> m_doktorlar;
};
