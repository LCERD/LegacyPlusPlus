//
// Created by DexrnZacAttack on 9/2/25 using zPc-i2.
//
#ifndef LEGACYPLUSPLUS_EDITSCREEN_H
#define LEGACYPLUSPLUS_EDITSCREEN_H

#include <qboxlayout.h>
#include <QPushButton>
#include <qtabwidget.h>
#include <QWidget>

#include "LCE/filesystem/File.h"
#include "GUI/Widgets/FileListWidget.h"

class QListWidget;
class QListWidgetItem;
class QGridLayout;

namespace LPP::GUI::Screens::Edit {
    class EditScreen : public QWidget {
        Q_OBJECT
    public:
        explicit EditScreen(IO::FileType type, QWidget *parent = nullptr);

        void initMenuBar();

        void hideEvent(QHideEvent *event) override;

        virtual bool canSaveInPlace();
    protected:
        QTabWidget *mTabs;

        QVBoxLayout *mMainLayout;

        QVBoxLayout *mLeftLayout;
        QGridLayout *mRightLayout;

        // File List Toolbar
        QPushButton *mChooseFileButton;
        QPushButton *mAddFileButton;

    public slots:
        virtual bool onChooseFileButton(IO::FileType type) = 0;
        // TODO: check if modified and update accordingly
        virtual void onSaveFileButton() = 0;
        // void onFileRightClicked(const QPoint &p);
    };
}

#endif //LEGACYPLUSPLUS_EDITSCREEN_H
