//
// Created by DexrnZacAttack on 9/7/25 using zPc-i2.
//
#include "FileListWidget.h"

#include "LCE/filesystem/Directory.h"
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <ranges>

namespace LPP::GUI::Widgets {
    FileListWidget::FileListWidget(IO::FileType type, QWidget *parent) : QTreeWidget(parent), mType(type) {
        this->setSelectionMode(ExtendedSelection);
        this->setHeaderLabels({"Filename", "Size"});

        this->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &QWidget::customContextMenuRequested, this, &FileListWidget::onContextMenuShow);

        connect(this, &QTreeWidget::itemDoubleClicked, this, &FileListWidget::onFileDoubleClicked);
    }

    void FileListWidget::create(lce::fs::Filesystem *fs) {
        this->clear();

        this->mFs = fs;
        this->refreshItems();
    }

    void FileListWidget::createFromPhysical(QString *path, lce::fs::Filesystem *fs) {
        this->mCurrentPath = path;
        this->create(fs);
    }

    void FileListWidget::exportObject(const lce::fs::FSObject *o) {
        if (!o)
            return;

        if (o->isFile()) {
            const lce::fs::File *f = dynamic_cast<const lce::fs::File*>(o);

            const QString n = QFileDialog::getSaveFileName(this, "Choose where you want to export the file", QString::fromStdWString(o->getName()));

            if (!n.isEmpty())
                f->writeOutFullPath(n.toStdWString());
        } else {
            const lce::fs::Directory *dir = dynamic_cast<const lce::fs::Directory*>(o);

            const QString n = QFileDialog::getExistingDirectory(this, "Choose where you want to export the file", QString::fromStdWString(o->getName()));

            if (!n.isEmpty())
                dir->writeOut(n.toStdWString());
        }
    }

    void FileListWidget::addObject(const std::wstring &n, lce::fs::FSObject &o) {
        QTreeWidgetItem *p = mPaths.value(QString::fromStdWString(o.getParent()->getPath()), nullptr);
        if (!p) // parent doesn't exist
            return;

        QTreeWidgetItem *i = new QTreeWidgetItem(p);
        i->setText(0, QString::fromStdWString(n));
        i->setData(0, Qt::UserRole, QVariant::fromValue<lce::fs::FSObject*>(&o));

        if (!o.isFile()) {
            const lce::fs::Directory *d = dynamic_cast<lce::fs::Directory*>(&o);

            i->setIcon(0, QIcon::fromTheme("folder"));
            i->setText(1, QString::fromStdString(std::to_string(d->getSize())));
            mPaths[QString::fromStdWString(o.getPath())] = i;
        } else {
            const lce::fs::File *f = dynamic_cast<lce::fs::File*>(&o);

            i->setIcon(0, QIcon::fromTheme("text-x-generic"));
            i->setText(1, QString::fromStdString(std::to_string(f->getSize())));
        }
    }

    void FileListWidget::refreshItems() {
        this->clear();
        mPaths.clear();

        this->invisibleRootItem()->setData(0, Qt::UserRole, QVariant::fromValue<lce::fs::FSObject*>(this->mFs->getRoot()));
        mPaths[QString::fromStdWString(this->mFs->getRoot()->getPath())] = this->invisibleRootItem();

        this->mFs->getRoot()->forEachRecursive([&](const std::wstring &n, lce::fs::FSObject &o) {
            addObject(n, o);
        });
    }

    IO::FileType FileListWidget::getFileType() const {
        return mType;
    }

    lce::fs::Filesystem * FileListWidget::getFilesystem() const {
        return this->mFs;
    }

    QString * FileListWidget::getPath() const {
        return this->mCurrentPath;
    }

    void FileListWidget::onFileDoubleClicked(const QTreeWidgetItem *item) {
        const auto d = item->data(0, Qt::UserRole);
        const lce::fs::File *p = d.value<lce::fs::File*>();

        if (!p)
            return;

        std::wcout << p << std::endl;
        // todo: do something here
        // this->mSideLayout->addItem()
        // __builtin_trap();
    }

    void FileListWidget::onContextMenuShow(const QPoint &pos) {
        QList<QTreeWidgetItem *> s = this->selectedItems();

        if (s.isEmpty())
            return;

        QMenu m(this);

        const QAction *ex = m.addAction(QIcon::fromTheme("document-save"), "Export");
        ex->connect(ex, &QAction::triggered, this, [this, s]() {
            if (s.size() == 1)
                onExportObject(s.first());
            else
                onExportObjects(s);
        });

        const QAction *del = m.addAction(QIcon::fromTheme("document-delete"), "Delete");
        ex->connect(del, &QAction::triggered, this, [this, s]() {
            onDeleteObjects(s);
        });

        m.exec(this->viewport()->mapToGlobal(pos));
    }

    void FileListWidget::onExportObject(const QTreeWidgetItem *item) {
        const auto d = item->data(0, Qt::UserRole);

        exportObject(d.value<lce::fs::FSObject*>());
    }

    void FileListWidget::onExportObjects(const QList<QTreeWidgetItem *> &items) {
        const QString n = QFileDialog::getExistingDirectory(this, "Choose where you want to export the files");

        if (!n.isEmpty()) {
            for (const auto item : items) {
                const auto d = item->data(0, Qt::UserRole);
                lce::fs::FSObject *p = d.value<lce::fs::FSObject*>();

                if (!p) continue;

                if (p->isFile()) {
                    const lce::fs::File *f = dynamic_cast<lce::fs::File*>(p);

                    f->writeOut(n.toStdWString());
                } else {
                    const lce::fs::Directory *dir = dynamic_cast<lce::fs::Directory*>(p);

                    dir->writeOut(n.toStdWString());
                }
            }
        }
    }

    void FileListWidget::onDeleteObjects(const QList<QTreeWidgetItem *> &items) {
        if (items.size() < 1) {
            QMessageBox dialog(QMessageBox::Icon::Critical, "Couldn't delete", "There are no items to delete.");
            return;
        }

        QMessageBox dialog(QMessageBox::Icon::Question, "Delete " + QString::fromStdString(std::to_string(items.size())) + " item(s)?", "Are you sure you want to delete " + QString::fromStdString(std::to_string(items.size())) + " items?");
        dialog.setStandardButtons(QMessageBox::YesToAll | QMessageBox::No);
        dialog.setDefaultButton(QMessageBox::YesToAll);
        dialog.setEscapeButton(QMessageBox::No);

        if (dialog.exec() == QMessageBox::YesToAll) {
            for (const auto item : items) {
                // TODO: deletion should be reversible
                const auto d = item->data(0, Qt::UserRole);
                if (!d.isValid()) continue;

                lce::fs::FSObject *p = d.value<lce::fs::FSObject*>();

                this->mPaths.remove(QString::fromStdWString(p->getPath()));

                QTreeWidgetItem *r = item;

                p->remove();
                QTreeWidgetItem *ip = item->parent();

                if (!ip)
                    ip = this->invisibleRootItem();

                // update size
                while (r && r->parent()) {
                    r = r->parent();

                    QVariant data = r->data(0, Qt::UserRole);
                    if (!data.isValid()) break;

                    const lce::fs::FSObject* rf = data.value<lce::fs::FSObject*>();
                    if (!rf) break;

                    r->setText(1, QString::fromStdString(std::to_string(rf->getSize())));
                }

                ip->takeChild(ip->indexOfChild(item));
                // delete p;
                delete item;
            }
        }
    }

    void FileListWidget::onSaveButtonClicked() {

    }

    void FileListWidget::onAddFile() {
        // get selected shit

        // slava C++20
        auto s = this->selectedItems() | std::views::filter([](const QTreeWidgetItem *i) {
            const auto d = i->data(0, Qt::UserRole);
            if (!d.isValid()) return false;

            lce::fs::FSObject *p = d.value<lce::fs::FSObject*>();

            if (!p->isFile()) return true;

            return false;
        });

        QList dirs(
            s.begin(), s.end()
        );

        if (dirs.size() > 1) {
            QMessageBox dialog(QMessageBox::Icon::Critical, "Can't add file", "More than one directory is selected.");
            dialog.open();
            return;
        }

        // get files
        QStringList files = QFileDialog::getOpenFileNames(this, "Select file(s)");

        if (!files.isEmpty()) {
            const QTreeWidgetItem *selected = this->invisibleRootItem();
            if (!dirs.empty()) {
                selected = dirs.first();
            }

            const auto d = selected->data(0, Qt::UserRole);
            if (!d.isValid()) return;

            lce::fs::FSObject *p = d.value<lce::fs::FSObject*>();

            if (p->isFile()) return;

            lce::fs::Directory *dir = dynamic_cast<lce::fs::Directory*>(p);

            for (QString file : files) {
                std::filesystem::path fp(file.toStdWString());
                // if dir already contains
                if (dir->contains(fp.filename().wstring())) continue;
                // if fp doesnt exist on real fs
                if (!exists(fp)) continue;

                if (is_regular_file(fp)) {
                    dir->addChild(std::make_unique<lce::fs::File>(fp));
                } else if (is_directory(fp)) {
                    dir->addChild(std::make_unique<lce::fs::Directory>(fp));
                }
            }

            refreshItems();
        }
    }
}
