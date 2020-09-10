

#include "DolphinQt/TAS/MovieVisualizerWindow.h"

// DEBUG
#include <iostream>
// DEBUG END

#include <QFileInfo>
#include <QLabel>
#include <QPlainTextEdit>
#include <QRegExp>
#include <QScrollBar>
#include <QSpinBox>
#include <QTimer>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

#include <fmt/format.h>

#include "Common/FileUtil.h"
#include "Core/ConfigManager.h"
#include "Core/Movie.h"
#include "Core/State.h"
#include "DolphinQt/Host.h"
#include "DolphinQt/TAS/MovieTimelineWidget.h"

static const int s_requestStateSaveBufferSize =10;

MovieVisualizerWindow::MovieVisualizerWindow(QWidget* parent) : QDialog(parent)
{
  setWindowTitle(tr("Movie Visualizer"));
  QString path = tr(File::GetUserPath(D_STATESAVES_IDX).c_str());
  m_regexStateSlotPath = new QRegExp(QRegExp::escape(path) + tr("\\w{6}\\.s\\d\\d"));

  m_updateIntervalChooser = new QSpinBox(this);
  m_updateIntervalChooser->setRange(0, 10000);
  m_updateIntervalChooser->setValue(10);
  m_updateIntervalChooser->setToolTip(tr("Timeline update interval in ms"));

  m_scaleChooser = new QSpinBox(this);
  m_scaleChooser->setRange(1, 10);
  m_scaleChooser->setValue(1);
  m_scaleChooser->setToolTip(tr("Timeline horizontal scale in px/frame"));

  m_rwModeIndicator = new QLabel(this);
  m_rwModeIndicator->setAlignment(Qt::AlignCenter);

  m_timeline = new MovieTimelineWidget(this);

  m_log = new QPlainTextEdit(this);
  m_log->setReadOnly(true);
  m_log->setUndoRedoEnabled(false);
  QPalette palette = m_log->palette();
  palette.setColor(QPalette::Base, Qt::black);
  palette.setColor(QPalette::Text, Qt::yellow);
  m_log->setPalette(palette);
  // DEBUG
  m_log->setPlainText(tr("yer mom soo fet"));

  QHBoxLayout* hLayout = new QHBoxLayout();
  hLayout->addWidget(m_rwModeIndicator);
  hLayout->addWidget(m_scaleChooser);
  hLayout->addWidget(m_updateIntervalChooser);

  QVBoxLayout* vLayout = new QVBoxLayout();
  vLayout->addLayout(hLayout);
  vLayout->addWidget(m_timeline);
  vLayout->addWidget(m_log);
  setLayout(vLayout);

  m_timer = new QTimer(this);
  m_timer->start(m_updateIntervalChooser->value());

  Connect();
}

void MovieVisualizerWindow::Update()
{
  UpdateRwModeIndicator();

  m_timeline->Update();

  // DEBUG
  // std::cout << SConfig::GetInstance().GetGameID() << " | " <<  "asd" << std::endl;
  // DEBUG END
}

void MovieVisualizerWindow::OnUpdateTitle(const QString& title)
{
  // confirm state save request
  ConfirmStateSave(title);

  // if (title.startsWith(tr("Dolphin")) ||
  //     title.startsWith(tr("DTM")) ||
  //     title.startsWith(tr("Decompressing")) ||
  //     title.startsWith(tr("Saving")))
  // {
  //     return;
  // }
  if (title.startsWith(tr("Dolphin")))
  {
    return;
  }

  AppendLogMessage(title);
}

void MovieVisualizerWindow::UpdateRwModeIndicator()
{
  if (Movie::IsReadOnly())
  {
    m_rwModeIndicator->setText(tr("ReadOnly"));
    m_rwModeIndicator->setStyleSheet(tr("background-color: #5f5"));
  }
  else
  {
    m_rwModeIndicator->setText(tr("ReadWrite"));
    m_rwModeIndicator->setStyleSheet(tr("background-color: #f55"));
  }
}

void MovieVisualizerWindow::AppendLogMessage(const QString& message)
{
  // m_log->appendPlainText(message);
  m_log->appendPlainText(QString::number(Movie::GetCurrentFrame()) + message);
  m_log->verticalScrollBar()->setValue(m_log->verticalScrollBar()->maximum());
}


void MovieVisualizerWindow::Connect()
{  
  connect(Host::GetInstance(), &Host::RequestTitle,
      this, &MovieVisualizerWindow::OnUpdateTitle);
  connect(m_updateIntervalChooser, QOverload<int>::of(&QSpinBox::valueChanged),
      m_timer, QOverload<int>::of(&QTimer::start));
  connect(m_scaleChooser, QOverload<int>::of(&QSpinBox::valueChanged),
      m_timeline, QOverload<int>::of(&MovieTimelineWidget::SetScale));
  connect(m_timer, &QTimer::timeout, this, &MovieVisualizerWindow::Update);
}

// void MovieVisualizerWindow::RequestStateSave(bool isSlot, const QString& name, 
//     int slot, const QString& path)
void MovieVisualizerWindow::RequestStateSave(StateInfo* request)
{
  if (m_stateSaveRequests.size() == s_requestStateSaveBufferSize)
  {
    delete m_stateSaveRequests.takeFirst();
  }

  // StateSaveRequest request;
  // request.frame = Movie::GetCurrentFrame();
  // request.isSlot = isSlot;
  // request.name = path;
  // request.slot = slot;
  // request.confirmingMessage = tr("Saved State to %1").arg(path);


  m_stateSaveRequests.append(request);
}

void MovieVisualizerWindow::ConfirmStateSave(const QString& message)
{
  QString confirmingMessage;
  QList<StateInfo*>::iterator i;
  for (i = m_stateSaveRequests.begin(); i != m_stateSaveRequests.end(); ++i)
  {
    confirmingMessage = tr("Saved State to %1").arg((*i)->path);
    if (message.compare(confirmingMessage) == 0)
    {
      m_timeline->AddState(*i);

      // remove confirmed request as well as removing and deleting precedings
      // assuming that all previously requested state saves failed
      QList<StateInfo*>::iterator j;
      for (j = m_stateSaveRequests.begin(); j != i; j = m_stateSaveRequests.erase(j))
      {
        delete (*j);
      }
      m_stateSaveRequests.erase(i);
      break;
    }
  }
}

void MovieVisualizerWindow::StateLoad(const QString& path)
{
  AppendLogMessage(tr("StateDebug"));
}

void MovieVisualizerWindow::StateSave(const QString& path)
{
  AppendLogMessage(tr("StateDebug"));
  // RequestStateSave(false, path, 42, path);

  StateInfo* request = new StateInfo();
  request->slot = -1;
  request->path = path;
  request->label = QFileInfo(path).baseName();
  request->frame = Movie::GetCurrentFrame();
  request->timestamp = Movie::GetRecordingStartTime();

  RequestStateSave(request);
}

void MovieVisualizerWindow::StateLoadSlotAt(int slot)
{
  AppendLogMessage(tr("StateDebug"));
}

void MovieVisualizerWindow::StateSaveSlotAt(int slot)
{
  AppendLogMessage(tr("StateDebug"));
  // unfortunately MakeStateFilename() from Core/State.cpp is not available here
  QString path = tr(fmt::format("{}{}.s{:02d}", File::GetUserPath(D_STATESAVES_IDX),
      SConfig::GetInstance().GetGameID(), slot).c_str());
  // RequestStateSave(true, tr("never give up"), slot, path);

  StateInfo* request = new StateInfo();
  request->slot = slot;
  request->path = path;
  request->label = QString::number(slot);
  request->frame = Movie::GetCurrentFrame();
  request->timestamp = Movie::GetRecordingStartTime();

  RequestStateSave(request);
}

void MovieVisualizerWindow::StateLoadLastSavedAt(int slot)
{
  AppendLogMessage(tr("StateDebug"));
}

void MovieVisualizerWindow::StateLoadUndo()
{
  AppendLogMessage(tr("StateDebug"));
}

void MovieVisualizerWindow::StateSaveUndo()
{
  AppendLogMessage(tr("StateDebug"));
}

void MovieVisualizerWindow::StateSaveOldest()
{
  AppendLogMessage(tr("StateDebug"));
}
