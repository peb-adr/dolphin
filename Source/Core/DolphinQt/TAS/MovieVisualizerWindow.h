// Copyright Adrian Richter

#pragma once

#include <QDialog>
#include <QList>

#include "Common/CommonTypes.h"

class QLabel;
class QPlainTextEdit;
class QRegExp;
class QSpinBox;
class QTimer;
class QWidget;

class MovieTimelineWidget;
struct StateInfo;

// So the whole request thing is done to accurately capture the frame the state
// save happened.
// A state save call from MainWindow is buffered in m_stateSaveRequests and then
// confirmed later by the "Saved State to ..." message
struct StateSaveRequest
{
  int frame;
  bool isSlot;
  QString name;
  int slot;
  QString confirmingMessage;
};

class MovieVisualizerWindow : public QDialog
{
public:
  MovieVisualizerWindow(QWidget* parent);

  void Update();
  // LogWidget* GetLogWidget();

  void OnUpdateTitle(const QString& title);
  void StateLoad(const QString& path);
  void StateSave(const QString& path);
  void StateLoadSlotAt(int slot);
  void StateSaveSlotAt(int slot);
  void StateLoadLastSavedAt(int slot);
  void StateLoadUndo();
  void StateSaveUndo();
  void StateSaveOldest();
  // void SetStateSlot(int slot);

private:
  void UpdateRwModeIndicator();
  void AppendLogMessage(const QString& message);
  void Connect();
  // void RequestStateSave(bool isSlot, const QString& name, int slot, const QString& path);
  void RequestStateSave(StateInfo* request);
  void ConfirmStateSave(const QString& message);

  QTimer* m_timer;
  QSpinBox* m_updateIntervalChooser;
  QSpinBox* m_scaleChooser;
  QLabel* m_rwModeIndicator;
  MovieTimelineWidget* m_timeline;
  QPlainTextEdit* m_log;

  QList<StateInfo*> m_stateSaveRequests;

  // TODO
  // QStringList logBlackList
  // QStringList logWhiteList
  QRegExp* m_regexStateSlotPath;
};
