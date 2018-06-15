#include "P4ProcessWnd.hpp"

#include <QVBoxLayout>

#include "P4Application.hpp"
#include "main.hpp"

P4ProcessWnd::P4ProcessWnd(QWidget *parent) : QWidget{parent}
{
    setWindowTitle("Output window");
    resize(530, 344);
    if (gP4smallIcon != nullptr)
        setWindowIcon(*gP4smallIcon);

    auto vLayout = new QVBoxLayout{this};
    vLayout->setSpacing(3);
    vLayout->setContentsMargins(5, 5, 5, 5);
    auto vLayout2 = new QVBoxLayout{};
    vLayout2->setSpacing(3);

    processText_ = new QPlainTextEdit{this};
    processText_->setFont(gP4app->getCourierFont());
    processText_->setReadOnly(true);
    processText_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vLayout2->addWidget(processText_);

    auto hLayout = new QHBoxLayout{};
    hLayout->setSpacing(6);
    hLayout->addStretch();

    terminateProcessButton_ = new QPushButton{"Terminate", this};
    terminateProcessButton_->setEnabled(false);
    terminateProcessButton_->setFont(gP4app->getBoldFont());
    terminateProcessButton_->setToolTip(
        "Terminates the process.  First tries to send a "
        "safe signal to the process.\nIf this does not "
        "work, then after 2 seconds, the program is "
        "abruptly terminated.");
    hLayout->addWidget(terminateProcessButton_);

    clearProcessButton_ = new QPushButton{"Clear", this};
    clearProcessButton_->setFont(gP4app->getBoldFont());
    clearProcessButton_->setToolTip("Clears the output");
    hLayout->addWidget(clearProcessButton_);

    closeWindowButton_ = new QPushButton{"Close", this};
    closeWindowButton_->setFont(gP4app->getBoldFont());
    closeWindowButton_->setToolTip("Close this window");
    hLayout->addWidget(closeWindowButton_);

    vLayout2->addLayout(hLayout);
    vLayout->addLayout(vLayout2);

    QObject::connect(terminateProcessButton_, &QPushButton::clicked, this,
                     [this]() { emit terminateSignal(); });
    QObject::connect(clearProcessButton_, &QPushButton::clicked, this,
                     [this]() {
                         if (processText_ != nullptr)
                             processText_->clear();
                     });
    QObject::connect(closeWindowButton_, &QPushButton::clicked, this,
                     [this]() { hide(); });

    show();
}
