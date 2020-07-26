

#include "DolphinQt/TAS/MovieVisualizerWindow.h"

#include <QPainter>
#include <QPlainTextEdit>
#include <QWidget>
#include <QVBoxLayout>

MovieVisualizerWindow::MovieVisualizerWindow(QWidget* parent) : QDialog(parent)
{
  setWindowTitle(tr("Movie Visualizer"));

  m_log = new QPlainTextEdit();
  m_log->setPlainText(tr("yer mom soo fet"));
  m_log->setReadOnly(true);
  m_log->setUndoRedoEnabled(false);

  QPalette palette = m_log->palette();
  palette.setColor(QPalette::Base, Qt::black);
  palette.setColor(QPalette::Text, Qt::yellow);
  m_log->setPalette(palette);

  m_timeline = new QWidget();
  m_timeline->resize(256, 128);
  // m_timeline_painter = new QPainter(m_timeline);

  auto* layout = new QVBoxLayout();
  layout->addWidget(m_log);
  layout->addWidget(m_timeline);

  setLayout(layout);
}

void MovieVisualizerWindow::FrameUpdate()
{
  AppendStateLogMessage(LOAD, tr("Henloo"));
}

// void MovieVisualizerWindow::paint()
// {
//   m_timeline_painter->setPen(Qt::blue);
//   m_timeline_painter->setFont(QFont(tr("Arial"), 30));
//   m_timeline_painter->drawText(rect(), Qt::AlignCenter, tr("Qt"));
// }

void MovieVisualizerWindow::StateLoad(QString path)
{
  AppendStateLogMessage(LOAD, path);
}

void MovieVisualizerWindow::StateSave(QString path)
{
  AppendStateLogMessage(SAVE, path);
}

void MovieVisualizerWindow::StateLoadSlot(int slot)
{
  AppendStateLogMessage(LOAD, tr("Slot %1").arg(QString::number(slot)));
}

void MovieVisualizerWindow::StateSaveSlot(int slot)
{
  AppendStateLogMessage(SAVE, tr("Slot %1").arg(QString::number(slot)));
}

void MovieVisualizerWindow::AppendStateLogMessage(StateOp op, QString subject)
{
  QString s = tr("Unknown");
  if (op == LOAD)
  	s = tr("Load");
  if (op == SAVE)
  	s = tr("Save");

  m_log->appendPlainText(tr("State %1 : %2").arg(s, subject));
}