#include "Action.h"

void Action::saveTable()
{
    emit getTable();
}

void Action::processTable(QTableWidget *processes)
{
    QString saveFile = QFileDialog::getSaveFileName(nullptr, "Save File", "", "Text files (*.csv)");
    QFile tableFile(saveFile);
    if(!tableFile.open(QIODevice::WriteOnly))
    {
        return;
    }
    QTextStream output(&tableFile);

    output << "PID, Process Name, Connections, Notes\n";

    for(int i = 0; i < processes->rowCount(); i++)
    {
        for(int j = 0; j < processes->columnCount(); j++)
        {
            QTableWidgetItem *item;
            item = processes->item(i, j);
            if(item != nullptr)
            {
                output << item->text();
                output << ",";
            }
            else
            {
                output << " ";
                output << ",";
            }

        }
        output << "\n";
    }
}

void Action::help()
{
    HelpWindow *helpWindow = new HelpWindow();
    helpWindow->setMinimumWidth(1000);
    helpWindow->setMinimumHeight(700);
    helpWindow->exec();
}

void Action::quit()
{
    exit(0);
}
