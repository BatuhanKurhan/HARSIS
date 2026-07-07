#pragma once

/**
 * doktor_dialog.h -- Doktor Kayıt Formu
 *
 * QDialog tabanlı doktor ekleme/düzenleme formu.
 */

#include "varliklar.h"

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QListWidget>
#include <QPushButton>

class DoktorDialog : public QDialog {
    Q_OBJECT

public:
    explicit DoktorDialog(QWidget* parent = nullptr);
    
    Doktor doktorAl() const;
    void doktorAyarla(const Doktor& doktor);

private slots:
    void dogrula();
    void saatEkle();
    void saatSil();

private:
    void formOlustur();
    
    QLineEdit* m_sicilNoEdit;
    QLineEdit* m_isimEdit;
    QLineEdit* m_soyisimEdit;
    QComboBox* m_bransComboBox;
    QListWidget* m_uygunSaatlerList;
    QComboBox* m_saatComboBox;
    
    QPushButton* m_tamam;
    QPushButton* m_iptal;
};
