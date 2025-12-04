#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Parse command line arguments
    QCommandLineParser parser;
    parser.setApplicationDescription("ROM Robotics Tuning Application");
    parser.addHelpOption();
    
    QCommandLineOption ipOption(QStringList() << "i" << "ip",
        "Robot IP address", "ip");
    QCommandLineOption passwordOption(QStringList() << "p" << "password",
        "Robot password", "password");
    QCommandLineOption nsOption(QStringList() << "n" << "ns",
        "Robot namespace (optional)", "namespace");
    
    parser.addOption(ipOption);
    parser.addOption(passwordOption);
    parser.addOption(nsOption);
    parser.process(a);
    
    QString ip = parser.value(ipOption);
    QString password = parser.value(passwordOption);
    QString ns = parser.value(nsOption);
    
    MainWindow w;
    
    // Auto-connect if IP and password are provided
    if (!ip.isEmpty() && !password.isEmpty()) {
        w.setConnectionParams(ip, password, ns);
        w.autoConnect();
        w.switchToControlTab();
    }
    
    w.show();
    return a.exec();
}
