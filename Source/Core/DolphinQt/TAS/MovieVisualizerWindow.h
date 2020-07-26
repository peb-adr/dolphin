// Copyright Adrian Richter

#pragma once

#include <QDialog>

#include "Common/CommonTypes.h"

class QDialog;
class QPlainTextEdit;
class QWidget;
class QPainter;

enum StateOp
{
  LOAD,
  SAVE,
};

class MovieVisualizerWindow : public QDialog
{
  Q_OBJECT
public:
  explicit MovieVisualizerWindow(QWidget* parent);

  void paint();
  void FrameUpdate();
  void StateLoad(QString path);
  void StateSave(QString path);
  void StateLoadSlot(int slot);
  void StateSaveSlot(int slot);

private:
  void AppendStateLogMessage(StateOp op, QString subject);

  QPlainTextEdit* m_log;
  QWidget* m_timeline;
  QPainter* m_timeline_painter;
};
