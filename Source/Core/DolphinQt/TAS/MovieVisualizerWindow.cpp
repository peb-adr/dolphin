

#include "DolphinQt/TAS/MovieVisualizerWindow.h"

// DEBUG
#include <iostream>
// DEBUG END

#include <QDialogButtonBox>
#include <QFileInfo>
#include <QFormLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegExp>
#include <QScrollBar>
#include <QSpinBox>
#include <QSplitter>
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
// #include "DolphinQt/TAS/MovieTimelineWidget.h"

static const int s_requestStateSaveBufferSize = 10;
static QString s_messageStateSavedPrefix(QStringLiteral("Saved State to "));
static QString s_messageStateLoadedPrefix(QStringLiteral("Loaded state from "));
static QRegExp s_regexStateSlotSaved;
static QRegExp s_regexStateSlotLoaded;

MovieVisualizerWindow::MovieVisualizerWindow(QWidget* parent) : QDialog(parent)
{
  QString s = tr(File::GetUserPath(D_STATESAVES_IDX).c_str());
  s = QRegExp::escape(s) + tr("\\w{6}\\.s\\d\\d");
  s_regexStateSlotSaved = QRegExp(s_messageStateSavedPrefix + s);
  s_regexStateSlotLoaded = QRegExp(s_messageStateLoadedPrefix + s);

  setWindowTitle(tr("Movie Visualizer"));

  m_rwModeIndicator = new QLabel(this);
  m_rwModeIndicator->setAlignment(Qt::AlignCenter);

  m_showSettingsButton = new QPushButton(this);
  m_showSettingsButton->setText(tr("Settings"));

  m_timeline = new MovieTimelineWidget(this);

  m_log = new QPlainTextEdit(this);
  m_log->setReadOnly(true);
  m_log->setUndoRedoEnabled(false);
  QPalette palette = m_log->palette();
  palette.setColor(QPalette::Base, Qt::black);
  palette.setColor(QPalette::Text, Qt::yellow);
  m_log->setPalette(palette);

  m_settingsDialog = new SettingsDialog(this);

  // layout components
  QHBoxLayout* hLayout = new QHBoxLayout();
  hLayout->addWidget(m_rwModeIndicator);
  hLayout->addWidget(m_showSettingsButton);

  QSplitter* splitter = new QSplitter(Qt::Vertical);
  splitter->addWidget(m_timeline);
  splitter->addWidget(m_log);

  QVBoxLayout* vLayout = new QVBoxLayout();
  vLayout->addLayout(hLayout);
  vLayout->addWidget(splitter);
  setLayout(vLayout);

  m_timer = new QTimer(this);
  m_timer->start(10);

  InitializeBlackWhiteLists();
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

void MovieVisualizerWindow::OnHostUpdateTitle(const QString& title)
{
  // confirm state save request
  ConfirmStateSave(title);

  // state save and load messages get nicer formatting
  if (s_regexStateSlotSaved.exactMatch(title))
  {
    AppendLogMessage(tr("Save State [ %1 ]").arg(title.right(2)));
    return;
  }
  else if (title.startsWith(s_messageStateSavedPrefix))
  {
    AppendLogMessage(tr("Save State %1").arg(
        title.mid(s_messageStateSavedPrefix.length())));
    return;
  }
  if (s_regexStateSlotLoaded.exactMatch(title))
  {
    AppendLogMessage(tr("Load State [ %1 ]").arg(title.right(2)));
    return;
  }
  else if (title.startsWith(s_messageStateLoadedPrefix))
  {
    AppendLogMessage(tr("Load State %1").arg(
        title.mid(s_messageStateLoadedPrefix.length())));
    return;
  }

  // filter by black and white list
  bool accept = true;

  QList<QRegExp>::iterator i;
  // QStringList::iterator i;
  for (i = m_logBlackList.begin(); accept && i != m_logBlackList.end(); ++i)
  {
    accept = !(i->exactMatch(title));
    // accept = !title.contains(*i);
  }
  for (i = m_logWhiteList.begin(); !accept && i != m_logWhiteList.end(); ++i)
  {
    accept = i->exactMatch(title);
    // accept = title.contains(*i);
  }
  if (!accept)
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
  m_log->appendPlainText(message);
  m_log->verticalScrollBar()->setValue(m_log->verticalScrollBar()->maximum());
}

void MovieVisualizerWindow::InitializeBlackWhiteLists()
{
  // black list
  m_logBlackList.append(QRegExp(tr("Dolphin.*")));
  m_logBlackList.append(QRegExp(tr("DTM.*")));
  m_logBlackList.append(QRegExp(tr("Decompressing.*")));
  m_logBlackList.append(QRegExp(tr("Saving. *")));
  // m_logBlackList.append(tr("Dolphin "));
  // m_logBlackList.append(tr("DTM"));
  // m_logBlackList.append(tr("Decompressing"));
  // m_logBlackList.append(tr("Saving"));
  // white list
}

void MovieVisualizerWindow::Connect()
{  
  connect(Host::GetInstance(), &Host::RequestTitle,
      this, &MovieVisualizerWindow::OnHostUpdateTitle);
  connect(m_timer, &QTimer::timeout, this, &MovieVisualizerWindow::Update);
  connect(m_showSettingsButton, &QAbstractButton::clicked, m_settingsDialog, &QWidget::show);

  connect(m_settingsDialog, QOverload<int>::of(&SettingsDialog::ScaleChanged),
      m_timeline, QOverload<int>::of(&MovieTimelineWidget::SetScale));
  connect(m_settingsDialog, QOverload<int>::of(&SettingsDialog::UpdateIntervalChanged),
      m_timer, QOverload<int>::of(&QTimer::start));
}

void MovieVisualizerWindow::RequestStateSave(StateInfo& request)
{
  if (m_stateSaveRequests.size() == s_requestStateSaveBufferSize)
  {
    m_stateSaveRequests.removeFirst();
  }
  m_stateSaveRequests.append(request);
}

void MovieVisualizerWindow::ConfirmStateSave(const QString& message)
{
  QString confirmingMessage;
  QList<StateInfo>::iterator i;
  for (i = m_stateSaveRequests.begin(); i != m_stateSaveRequests.end(); ++i)
  {
    confirmingMessage = s_messageStateSavedPrefix + i->path;
    if (message.compare(confirmingMessage) == 0)
    {
      m_timeline->AddState(*i);

      // remove confirmed request as well as removing and deleting precedings
      // assuming that all previously requested state saves failed
      m_stateSaveRequests.erase(m_stateSaveRequests.begin(), i + 1);
      break;
    }
  }
}

void MovieVisualizerWindow::StateSave(const QString& path)
{
  StateInfo request;
  request.slot = -1;
  request.path = path;
  request.label = QFileInfo(path).baseName();
  request.frame = Movie::GetCurrentFrame();
  request.timestamp = Movie::GetRecordingStartTime();

  RequestStateSave(request);
}

void MovieVisualizerWindow::StateSaveSlotAt(int slot)
{
  // unfortunately MakeStateFilename() from Core/State.cpp is not available here
  QString path = tr(fmt::format("{}{}.s{:02d}", File::GetUserPath(D_STATESAVES_IDX),
      SConfig::GetInstance().GetGameID(), slot).c_str());

  StateInfo request;
  request.slot = slot;
  request.path = path;
  request.label = QString::number(slot);
  request.frame = Movie::GetCurrentFrame();
  request.timestamp = Movie::GetRecordingStartTime();

  RequestStateSave(request);
}


////////////////////////////////
////////////////////////////////
///// class SettingsDialog /////
////////////////////////////////
////////////////////////////////

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent)
{
  m_scale = 1;
  m_updateInterval = 10;

  setWindowTitle(tr("Movie Visualizer Settings"));

  m_scaleChooser = new QSpinBox(this);
  m_scaleChooser->setRange(1, 10);
  m_scaleChooser->setValue(1);
  m_scaleChooser->setToolTip(tr("Timeline horizontal scale in px/frame"));

  m_updateIntervalChooser = new QSpinBox(this);
  m_updateIntervalChooser->setRange(0, 10000);
  m_updateIntervalChooser->setValue(10);
  m_updateIntervalChooser->setToolTip(tr("Timeline update interval in ms"));

  m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

  QFormLayout* fLayout = new QFormLayout();
  fLayout->addRow(tr("update interval (ms)"), m_updateIntervalChooser);
  fLayout->addRow(tr("scale (px/frame)"), m_scaleChooser);

  QVBoxLayout* vLayout = new QVBoxLayout();
  vLayout->addLayout(fLayout);
  vLayout->addWidget(m_buttons);
  setLayout(vLayout);

  connect(m_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(this, &QDialog::accepted, this, &SettingsDialog::OnAccepted);
  connect(this, &QDialog::rejected, this, &SettingsDialog::OnRejected);
}

SettingsDialog::~SettingsDialog()
{

}

void SettingsDialog::OnAccepted()
{
  int newScale = m_scaleChooser->value();
  if (newScale != m_scale)
  {
    m_scale = newScale;
    emit ScaleChanged(newScale);
  }

  int newUpdateInterval = m_updateIntervalChooser->value();
  if (newUpdateInterval != m_updateInterval)
  {
    m_updateInterval = newUpdateInterval;
    emit UpdateIntervalChanged(newUpdateInterval);
  }
}

void SettingsDialog::OnRejected()
{
  m_scaleChooser->setValue(m_scale);
  m_updateIntervalChooser->setValue(m_updateInterval);
}
