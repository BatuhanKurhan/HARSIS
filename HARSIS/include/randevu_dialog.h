#pragma once

/**
 * randevu_dialog.h -- Randevu Oluşturma Formu
 *
 * QDialog tabanlı randevu oluşturma formu.
 */

#include "varliklar.h"
#include "depo.h"

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QPushButton>

class RandevuDialog : public QDialog {
    Q_OBJECT

public:
    explicit RandevuDialog(Depo<std::string, Hasta>* hastalar,
                          Depo<std::string, Doktor>* doktorlar,
                          QWidget* parent = nullptr);
    
    Randevu randevuAl() const;
    void randevuIdAyarla(int id);

private slots:
    void dogrula();
    void bransSecildi(const QString& brans);
    void doktorSecildi(int index);

private:
    void formOlustur();
    void hastalariYukle();
    void branslariYukle();
    void doktorlariYukle(const QString& brans);
    void uygunSaatleriYukle(const QString& sicilNo);
    
    Depo<std::string, Hasta>* m_hastalar;
    Depo<std::string, Doktor>* m_doktorlar;
    int m_randevuId;
    
    QComboBox* m_hastaComboBox;
    QComboBox* m_bransComboBox;
    QComboBox* m_doktorComboBox;
    QDateEdit* m_tarihEdit;
    QComboBox* m_saatComboBox;
    
    QPushButton* m_tamam;
    QPushButton* m_iptal;
};
