// SPDX-FileCopyrightText: 2020 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "unlockpartitiondialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QAbstractButton>
#include <DPasswordEdit>
#include <dcommandlinkbutton.h>

using namespace dfmplugin_diskenc;
DWIDGET_USE_NAMESPACE

UnlockPartitionDialog::UnlockPartitionDialog(UnlockType type, QWidget *parent)
    : DDialog(parent),
      currType(type),
      initType(type)
{
    setModal(true);
    initUI();
    initConnect();
}

UnlockPartitionDialog::~UnlockPartitionDialog()
{
}

void UnlockPartitionDialog::initUI()
{
    setIcon(QIcon::fromTheme("dialog-warning"));

    QFrame *content = new QFrame;
    passwordLineEdit = new DPasswordEdit;
    chgUnlockType = new DCommandLinkButton("");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(10);
    mainLayout->addWidget(passwordLineEdit);
    mainLayout->addWidget(chgUnlockType, 0, Qt::AlignRight);
    mainLayout->addSpacing(10);
    content->setLayout(mainLayout);
    addContent(content);

    addButton(tr("Cancel"));
    addButton(tr("Unlock"), true, DDialog::ButtonRecommend);
    auto unlockBtn = getButton(1);
    if (unlockBtn) unlockBtn->setEnabled(false);

    updateUserHint();
}

void UnlockPartitionDialog::initConnect()
{
    connect(this, &DDialog::buttonClicked, this, &UnlockPartitionDialog::handleButtonClicked);
    connect(chgUnlockType, &DCommandLinkButton::clicked, this, &UnlockPartitionDialog::switchUnlockType);
    connect(passwordLineEdit, &DPasswordEdit::textChanged, this, [this](const QString &txt) {
        auto unlockBtn = getButton(1);
        if (unlockBtn) unlockBtn->setEnabled(txt.length() != 0);
    });
}

void UnlockPartitionDialog::updateUserHint()
{
    setTitle(tr("Unlock encryption partition"));
    chgUnlockType->setText(tr("Unlock by recovery key"));
    switch (currType) {
    case kRec: {
        setTitle(tr("Unlock by recovery key"));
        QString text = (initType == kPwd)
                ? tr("Unlock by passphrase")
                : tr("Unlock by PIN");
        chgUnlockType->setText(text);
        passwordLineEdit->setPlaceholderText(tr("Please input recovery key to unlock device"));
        break;
    }
    case kPwd:
        passwordLineEdit->setPlaceholderText(tr("Please input passphrase to unlock device"));
        break;
    case kPin:
        passwordLineEdit->setPlaceholderText(tr("Please input PIN to unlock device"));
        break;
    }
}

void UnlockPartitionDialog::handleButtonClicked(int index, QString text)
{
    Q_UNUSED(text)
    if (index == 1)
        key = passwordLineEdit->text();

    accept();
}

void UnlockPartitionDialog::switchUnlockType()
{
    if (currType == kRec)
        currType = initType;
    else if (currType == kPin || currType == kPwd)
        currType = kRec;

    updateUserHint();
}

void UnlockPartitionDialog::showEvent(QShowEvent *event)
{
    passwordLineEdit->setFocus();
    DDialog::showEvent(event);
}

QPair<UnlockPartitionDialog::UnlockType, QString> UnlockPartitionDialog::getUnlockKey() const
{
    return { currType, key };
}