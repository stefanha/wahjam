#include <QLabel>
#include <QFormLayout>
#include <QSettings>

#include "ConnectDialog.h"

ConnectDialog::ConnectDialog(QWidget *parent)
  : QDialog(parent)
{
  QSettings settings;

  hostEdit = new QLineEdit(settings.value("connect/host").toString());
  userEdit = new QLineEdit(settings.value("connect/user").toString());
  passEdit = new QLineEdit;
  passEdit->setEchoMode(QLineEdit::Password);

  connectButton = new QPushButton(tr("&Connect"));

  connect(connectButton, SIGNAL(clicked()), this, SLOT(connectToHost()));

  QVBoxLayout *layout = new QVBoxLayout;
  QWidget *form = new QWidget;
  QFormLayout *formLayout = new QFormLayout;
  formLayout->addRow(tr("&Server:"), hostEdit);
  formLayout->addRow(tr("&Username:"), userEdit);

  publicCheckbox = new QCheckBox(tr("&Public server"));
  if (settings.value("connect/public", true).toBool()) {
    publicCheckbox->setChecked(true);
    passEdit->setEnabled(false);
  }
  connect(publicCheckbox, SIGNAL(stateChanged(int)), this, SLOT(publicServerStateChanged(int)));
  formLayout->addWidget(publicCheckbox);

  formLayout->addRow(tr("&Password:"), passEdit);
  form->setLayout(formLayout);
  layout->addWidget(form);
  layout->addWidget(connectButton);
  setLayout(layout);
  setWindowTitle(tr("Connect to server..."));
}

void ConnectDialog::connectToHost()
{
  host = hostEdit->text();
  user = userEdit->text();
  pass = passEdit->text();
  isPublicServer = publicCheckbox->isChecked();

  hostEdit->clear();
  userEdit->clear();
  passEdit->clear();
  passEdit->setEnabled(false);
  publicCheckbox->setChecked(true);

  QSettings settings;
  settings.setValue("connect/host", host);
  settings.setValue("connect/user", user);
  settings.setValue("connect/public", isPublicServer);

  accept();
}

void ConnectDialog::publicServerStateChanged(int state)
{
  passEdit->setEnabled(state == Qt::Unchecked);
}

QString ConnectDialog::GetHost()
{
  return host;
}

QString ConnectDialog::GetUser()
{
  return user;
}

QString ConnectDialog::GetPass()
{
  return pass;
}

bool ConnectDialog::IsPublicServer()
{
  return isPublicServer;
}
