#pragma once

/**
 * hasta_model.h -- Hasta Tablo Modeli
 *
 * QAbstractTableModel tabanlı hasta listesi görünümü.
 * Qt6 Model/View mimarisi kullanılır.
 */

#include "varliklar.h"
#include "depo.h"

#include <QAbstractTableModel>
#include <vector>

class HastaModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit HastaModel(Depo<std::string, Hasta>* depo, QObject* parent = nullptr);

    // QAbstractTableModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Veri güncelleme
    void yenile();
    const Hasta* hastaAl(int satir) const;

private:
    Depo<std::string, Hasta>* m_depo;
    std::vector<Hasta> m_hastalar;
};
