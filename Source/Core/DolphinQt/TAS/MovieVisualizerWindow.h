// Copyright Adrian Richter

#pragma once

#include <QDialog>
#include <QList>

#include "Common/CommonTypes.h"
#include "DolphinQt/TAS/MovieTimelineWidget.h"

class QDialogButtonBox;
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QRegExp;
class SettingsDialog;
class QSpinBox;
class QTimer;
class QWidget;

// class MovieTimelineWidget;
// struct StateInfo;


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
  // So the whole request thing is done to accurately capture the frame the state
  // save happened.
  // A state save call from MainWindow is buffered in m_stateSaveRequests and then
  // confirmed later by the "Saved State to ..." message
  void RequestStateSave(StateInfo& request);
  void ConfirmStateSave(const QString& message);

  QLabel* m_rwModeIndicator;
  QPushButton* m_showSettingsButton;
  MovieTimelineWidget* m_timeline;
  QPlainTextEdit* m_log;

  QList<StateInfo> m_stateSaveRequests;

  // TODO
  // QStringList logBlackList
  // QStringList logWhiteList
  // QRegExp* m_regexStateSlotPath;

  SettingsDialog* m_settingsDialog;
  
  QTimer* m_timer;
  // QSpinBox* m_updateIntervalChooser;
  // QSpinBox* m_scaleChooser;
};

class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  SettingsDialog(QWidget* parent);
  ~SettingsDialog();

signals:
  void ScaleChanged(int scale);
  void UpdateIntervalChanged(int updateInterval);

private:
  void OnAccepted();
  void OnRejected();

  int m_scale;
  int m_updateInterval;

  QSpinBox* m_scaleChooser;
  QSpinBox* m_updateIntervalChooser;
  QDialogButtonBox* m_buttons;

};
