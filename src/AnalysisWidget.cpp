#include "AnalysisWidget.h"

AnalysisWidget::AnalysisWidget()
{
    display = new QGridLayout(this);

    title = new QLabel(this);

    analyzedFile = new QLabel(this);

    textVirtualSize = new QLabel(this);

    textSize = new QLabel(this);

    imports = new QLabel(this);

    saveToFile = new QPushButton(this);
    saveToFile->setText("Save to File");

    display->addWidget(title);
    display->addWidget(analyzedFile);
    display->addWidget(textVirtualSize);
    display->addWidget(textSize);
    display->addWidget(imports);

    display->addWidget(saveToFile);
    saveToFile->setDisabled(true);

    connect(saveToFile, SIGNAL(clicked()), this, SLOT(save()));
}

// need to fix positioning

void AnalysisWidget::updateDock(QString path, QString virtualSize, QString actualSize)
{
    saveToFile->setDisabled(false);
    analyzedFile->setText("<h3>Full file path:</h3> " + path);
    std::string substring = analyzedFile->text().toStdString();
    size_t index = substring.rfind('\\');
    substring = substring.substr(index+1);
    title->setText("<h1>Analyzation of " + QString::fromStdString(substring) + "</h1>");
    title->setAlignment(Qt::AlignCenter);
    textVirtualSize->setText("<h3>Text section virtual size:</h3> " + virtualSize);
    textSize->setText("<h3>Text section actual size:</h3> " + actualSize);
    imports->setText("<h3>Imports:</h3> TBD");
}

void AnalysisWidget::save()
{
    QString saveFile = QFileDialog::getSaveFileName(this, "Save File", "", "Text files (*.txt)");
    QFile analysisFile(saveFile);
    if(!analysisFile.open(QIODevice::WriteOnly))
    {
        return;
    }
    QTextStream output(&analysisFile);
    std::string substring = analyzedFile->text().toStdString();
    size_t index = substring.rfind('\\');
    substring = substring.substr(index+1);
    output << "Analyzation of " + QString::fromStdString(substring) + "\n\n";

    substring = removeHtmlTags(analyzedFile);
    output << "Full file path:" + QString::fromStdString(substring) + "\n\n";

    substring = removeHtmlTags(textVirtualSize);
    output << "Text section virtual size:" + QString::fromStdString(substring) + "\n\n";

    substring = removeHtmlTags(textSize);
    output << "Text section actual size:" + QString::fromStdString(substring) + "\n\n";

    substring = removeHtmlTags(imports);
    output << "Imports:" + QString::fromStdString(substring) + "\n\n";
}

std::string AnalysisWidget::removeHtmlTags(QLabel *label)
{
    std::string substring = label->text().toStdString();
    size_t index = substring.rfind("</h3>");
    substring = substring.substr(index+5);
    return substring;
}
