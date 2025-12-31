//
// Created by DexrnZacAttack on 9/2/25 using zPc-i2.
//
#include "EditScreen.h"

#include <fstream>
#include <QPushButton>
#include <QVBoxLayout>
#include <QListWidget>
#include <QLabel>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QSplitter>
#include <QToolBar>

#include "LPP.h"
#include "GUI/Toolbars/EditScreenMenuBar.h"
#include "GUI/Widgets/FileListWidget.h"
#include "LCE/save/SaveFile.h"

namespace LPP::GUI::Screens::Edit {
    EditScreen::EditScreen(IO::FileType type, QWidget *parent) {
        // main layout
        QWidget* mainLayout = new QWidget();
        this->mMainLayout = new QVBoxLayout(mainLayout);
        setLayout(this->mMainLayout);

        mMainLayout->setContentsMargins(0, 0, 0, 0);
        mMainLayout->setSpacing(0);
    }

    void EditScreen::initMenuBar() {
        MenuBars::EditScreenMenuBar *toolbar = new MenuBars::EditScreenMenuBar(this);
        LegacyPlusPlus::sInstance.mainWindow->setMenuBar(toolbar);
    }

    void EditScreen::hideEvent(QHideEvent *event) {
        QWidget::hideEvent(event);

        if (QMenuBar *bar = LegacyPlusPlus::sInstance.mainWindow->menuBar())
            bar->hide();
    }

    bool EditScreen::canSaveInPlace() {
        return false;
    }
}
