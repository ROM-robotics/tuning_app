#ifndef READMEVIEWER_H
#define READMEVIEWER_H

#include <QDialog>

class QTextBrowser;
class QPushButton;

class ReadmeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ReadmeDialog(QWidget *parent = nullptr, const QString &readmePath = QString());
    ~ReadmeDialog() override;

    // Attempts to load the given path; if empty or missing, a placeholder message is shown.
    void loadReadme(const QString &readmePath);

private:
    QTextBrowser *m_browser;
    QPushButton *m_close;
};

#endif // READMEVIEWER_H
