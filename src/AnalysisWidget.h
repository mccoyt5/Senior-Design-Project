#ifndef ANALYSISWIDGET_H
#define ANALYSISWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

class AnalysisWidget: public QWidget
{
    Q_OBJECT

    public:
        AnalysisWidget();

    public slots:
        void updateDock(QString path, QString virtualSize, QString actualSize, std::vector<char*> dlls);
        void updateDockError(QString name);
        void save();
        std::string removeHtmlTags(QLabel *label);

    private:
        QGridLayout *display;
        QLabel *title;
        QLabel *analyzedFile;
        QLabel *textVirtualSize;
        QLabel *textSize;
        QLabel *imports;
        QPushButton *saveToFile;
};

#endif // ANALYSISWIDGET_H
