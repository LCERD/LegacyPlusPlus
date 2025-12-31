//
// Created by DexrnZacAttack on 9/12/25 using zPc-i2.
//
#include "FSEditScreen.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QtConcurrent>
#include <QFuture>
#include <QCloseEvent>

#include "LPP.h"
#include "GUI/Dialogs/SimpleDialog.h"
#include "LCE/save/SaveFile.h"
#include "LCE/io/Serializable.h"

namespace LPP::GUI::Screens::Edit {
    // I feel like this should be gutted out and have it's main init code moved to EditScreen
    FSEditScreen::FSEditScreen(IO::FileType type, QWidget *parent) : EditScreen(type, parent) {
        QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

        // side layout
        QWidget* rightLayout = new QWidget();
        mRightLayout = new QGridLayout(rightLayout);

        // setup tabs
        this->mTabs = new QTabWidget(this);

        // make filelist
        this->mFileList = new Widgets::FileListWidget(type, this);

        // make toolbar
        QWidget *toolbar = new QWidget();
        QHBoxLayout *toolbarLayout = new QHBoxLayout(toolbar);
        toolbarLayout->setContentsMargins(0, 0, 0, 0);

        // make toolbar shit
        // QLabel* fileName = new QLabel("No file has been chosen", toolbar);
        mChooseFileButton = new QPushButton(QIcon::fromTheme("document-open"), "Choose a file", toolbar);
        connect(mChooseFileButton, &QPushButton::clicked, this, [type, this]() {
            onChooseFileButton(type);
        });

        mAddFileButton = new QPushButton(QIcon::fromTheme("document-new"), "Add file", toolbar);
        connect(mAddFileButton, &QPushButton::clicked, this->mFileList, &Widgets::FileListWidget::onAddFile);

        // add the shit to toolbar layout
        toolbarLayout->addWidget(mChooseFileButton);
        toolbarLayout->addWidget(mAddFileButton);

        // create left layout
        QWidget* leftLayout = new QWidget();
        mLeftLayout = new QVBoxLayout(leftLayout);

        // add tabs
        mRightLayout->addWidget(mTabs, 0, Qt::AlignTop);

        // add widgets
        mLeftLayout->addWidget(toolbar);
        mLeftLayout->addWidget(this->mFileList);

        splitter->addWidget(leftLayout);
        splitter->addWidget(rightLayout);

        this->mMainLayout->addWidget(splitter, 0);
    }

    bool FSEditScreen::canSaveInPlace() {
        if (!mFileList)
            return false;

        if (!mFileList->getPath())
            return false;

        return mFileList->getFileType().isSavable();
    }

    bool FSEditScreen::onChooseFileButton(IO::FileType type) {
        // grab file
        QString *fileName = new QString(QFileDialog::getOpenFileName(mFileList, "Choose a file")); // hope this isn't a bad idea

        if (!fileName->isEmpty()) {
            // clear so that we don't have duplicates
            mFileList->clear();
            mTabs->clear();

            // read
            std::ifstream stream(fileName->toStdString(), std::ifstream::binary);

            std::vector<uint8_t> bytes(std::filesystem::file_size(fileName->toStdWString()));
            stream.read(reinterpret_cast<char *>(bytes.data()), bytes.size());

            try {
                // read file
                lce::fs::Filesystem *file;
                switch (type) {
                    default:
                    case IO::eFileType::BASIC:
                        file = new lce::fs::Filesystem();
                        break;
                    case IO::eFileType::SAVE_FILE_OLD:
                    case IO::eFileType::SAVE_FILE:
                        file = lce::save::SaveFileCommons::deserializeAuto(bytes);
                        break;
                    case IO::eFileType::ARCHIVE:
                        file = new lce::arc::Archive(bytes);
                        break;
                    case IO::eFileType::SOUNDBANK:
                        if (!lce::msscmp::Soundbank::isSoundbank(bytes.data())) {
                            QMessageBox::critical(this, "Invalid File", "This file is not a valid MSSCMP file");
                            return false;
                        }

                        file = new lce::msscmp::Soundbank(bytes);
                        break;
                }

                // add tab
                QWidget *saveTab = new QWidget();
                QVBoxLayout *saveTabLayout = new QVBoxLayout(saveTab);
                saveTabLayout->setSpacing(10);

                // TODO: make panel based on chosen file type
                // // create info strings
                // std::string v = "Version: " + std::to_string(file->getVersion());
                // std::string mv = "Original Version: " + std::to_string(file->getOriginalVersion());
                //
                // // add widgets
                // saveTabLayout->addWidget(new QLabel(v.c_str()));
                // saveTabLayout->addWidget(new QLabel(mv.c_str()));
                //
                // saveTabLayout->addStretch();

                // add the tab
                mTabs->addTab(saveTab, "Save File");

                // create list items in file list
                // TODO: this should read files and directories (recursively) instead of seeing all top level items as a file
                this->mFileList->createFromPhysical(fileName, file);
            } catch (const std::exception& e) {
                // Too Bad!
                std::string err = "Exception thrown whilst trying to read '" + fileName->toStdString() + "': " + e.what();
                QMessageBox::critical(mFileList, "Error", err.c_str());
            }
            return true;
        }
        return false;
    }

    void FSEditScreen::onSaveFileButton() {
        if (!canSaveInPlace())
            return;

        const std::filesystem::path p = std::filesystem::path(mFileList->getPath()->toStdWString());

        Dialog::SimpleDialog* saving = new Dialog::SimpleDialog(this);
        saving->setWindowTitle("Saving");
        saving->setBody(QString::fromStdString("Saving " + p.filename().string()));
        saving->setModal(true);
        saving->setWindowFlags((saving->windowFlags() | Qt::CustomizeWindowHint) & ~ (Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint));
        saving->setAttribute(Qt::WA_DeleteOnClose);
        saving->open();

        QFuture<bool> f = QtConcurrent::run([this, p]() {
            switch (mFileList->getFileType()) {
                default:
                case IO::eFileType::SOUNDBANK:
                case IO::eFileType::BASIC:
                    return false;
                case IO::eFileType::SAVE_FILE_OLD:
                case IO::eFileType::SAVE_FILE:
                case IO::eFileType::ARCHIVE: {
                    const lce::io::Serializable *f = dynamic_cast<lce::io::Serializable*>(this->mFileList->getFilesystem());

                    if (!f) return false;

                    f->writeFile(p);

                    return true;
                }
            }
        });

        // the all-seeing eye
        QFutureWatcher<bool>* w = new QFutureWatcher<bool>(this);
        w->setFuture(f);
        connect(w, &QFutureWatcher<bool>::finished, saving, &QDialog::close);
    }
}
