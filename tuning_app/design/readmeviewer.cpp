#include "readmeviewer.h"

#include <QTextBrowser>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QLabel>
#include <QApplication>
#include <QScreen>

ReadmeDialog::ReadmeDialog(QWidget *parent, const QString &readmePath)
    : QDialog(parent), m_browser(new QTextBrowser), m_close(new QPushButton(tr("x")))
{
    // Make the dialog modal and allow translucent background so the overlay effect works.
    setModal(true);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);

    // Overlay widget (the semi-opaque white container)
    QWidget *overlay = new QWidget(this);
    overlay->setObjectName("readmeOverlay");
    overlay->setStyleSheet("#readmeOverlay { background-color: rgba(255,255,255,230); border-radius:8px; }\n");

    // Styling for markdown elements (headers, body)
    const QString css = R"(
        body { color: black; background: transparent; font-family: "Segoe UI", Arial, sans-serif; }
        h1 { color: red; }
        h2 { color: blue; }
        h3 { color: darkred; }
        a { color: #0066cc; }
    )";
    m_browser->document()->setDefaultStyleSheet(css);
    m_browser->setOpenExternalLinks(true);

    // Layout inside overlay
    QVBoxLayout *overlayLayout = new QVBoxLayout(overlay);
    overlayLayout->setContentsMargins(12,12,12,12);
    overlayLayout->setSpacing(8);

    // Header row with title and close button
    QHBoxLayout *headerLayout = new QHBoxLayout;
    QLabel *title = new QLabel(tr("ROM Dynamics Co., Ltd."));

    title->setStyleSheet("color: #0066cc;");
    m_close->setFixedSize(20, 20);
    m_close->setStyleSheet("QPushButton { color: white; background-color: red; border-radius: 10px; font-weight: bold; font-size: 14px; color: black; }\n"
                               "QPushButton:hover { color: white; background-color: red; }");
    

    QFont titleFont = title->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    title->setFont(titleFont);
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(m_close);

    overlayLayout->addLayout(headerLayout);
    overlayLayout->addWidget(m_browser, /*stretch=*/1);

    // Main layout for this dialog -- overlay centered with margins
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40,40,40,40);
    mainLayout->addWidget(overlay);

    connect(m_close, &QPushButton::clicked, this, &ReadmeDialog::accept);

    // Make dialog cover parent to produce dimming effect; if no parent, cover primary screen
    if (parent) {
        resize(parent->size());
    } else {
        QScreen *s = QApplication::primaryScreen();
        if (s) resize(s->size());
    }

    // Give overlay a reasonable max size (so it looks like a centered modal)
    overlay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Load readme content
    loadReadme(readmePath);
}

ReadmeDialog::~ReadmeDialog()
{
}

void ReadmeDialog::loadReadme(const QString &readmePath)
{
    QString md;
    if (!readmePath.isEmpty()) {
        QFile f(readmePath);
        if (f.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream in(&f);
            md = in.readAll();
            f.close();
        }
    }

    if (md.isEmpty()) {
        md = QString("# Readme not found\n\nCould not find `README.md` at the provided path.\n\nAdd a `README.md` to the project root or pass the path to the dialog.");
    }

    // QTextBrowser supports setMarkdown (Qt >= 5.14). Use it so headers render as HTML elements and CSS applies.
    m_browser->setMarkdown(md);
}
