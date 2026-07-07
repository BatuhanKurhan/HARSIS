#pragma once

/**
 * hasta_dialog.h -- Hasta Kayıt Formu
 *
 * QDialog tabanlı hasta ekleme/düzenleme formu.
 */

#include "varliklar.h"

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>

class HastaDialog : public QDialog {
    Q_OBJECT

public:
    explicit HastaDialog(QWidget* parent = nullptr);
    
    Hasta hastaAl() const;
    void hastaAyarla(const Hasta& hasta);

private slots:
    void dogrula();

private:
    void formOlustur();
    
    QLineEdit* m_tcNoEdit;
    QLineEdit* m_isimEdit;
    QLineEdit* m_soyisimEdit;
    QDateEdit* m_dogumTarihiEdit;
    QLineEdit* m_telefonEdit;
    
    QPushButton* m_tamam;
    QPushButton* m_iptal;
};
