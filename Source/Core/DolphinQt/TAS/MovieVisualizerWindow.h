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
class QStringList;
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

  void OnHostUpdateTitle(const QString& title);
  void StateSave(const QString& path);
  void StateSaveSlotAt(int slot);

private:
  void UpdateRwModeIndicator();
  void AppendLogMessage(const QString& message);
  void InitializeBlackWhiteLists();
  void Connect();
  // So the whole request thing is done to accurately capture the frame the state
  // save happened.
  // A state save call from MainWindow is buffered in m_stateSaveRequests and then
  // confirmed later by the "Saved State to ..." message
  void RequestStateSave(StateInfo& request);
  void ConfirmStateSave(const QString& message);

  QList<StateInfo> m_stateSaveRequests;
  QList<QRegExp> m_logBlackList;
  QList<QRegExp> m_logWhiteList;
  // QStringList m_logBlackList;
  // QStringList m_logWhiteList;

  QLabel* m_rwModeIndicator;
  QPushButton* m_showSettingsButton;
  MovieTimelineWidget* m_timeline;
  QPlainTextEdit* m_log;
  SettingsDialog* m_settingsDialog;
  QTimer* m_timer;
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
