/* mainwindow.h: Header file for stellaris_stat_viewer main window.
 *
 * Copyright 2019 Adrian "ArdiMaster" Welcker
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#ifndef STELLARIS_STAT_VIEWER_MAINWINDOW_H
#define STELLARIS_STAT_VIEWER_MAINWINDOW_H

#include <QtWidgets/QMainWindow>

class QAction;
class QLabel;
class QMenu;
class QMenuBar;
class QProgressDialog;
class QTabWidget;

enum class LoadStage;
class EconomyView;
class FleetsView;
class OverviewView;
namespace Galaxy {
	class State;
	class StateFactory;
}
namespace Parsing { class Parser; }

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = nullptr);

signals:
	void modelChanged(const Galaxy::State *newModel);

private slots:
	void aboutQtSelected();
	void aboutSsvSelected();
	void checkForUpdatesSelected();
	void openFileSelected();

	void parserProgressUpdate(Parsing::Parser *parser, qint64 current, qint64 max);
	void galaxyProgressUpdate(Galaxy::StateFactory *factory, int current, int max);

private:
	void gamestateLoadBegin();
	void gamestateLoadSwitch();
	void gamestateLoadFinishing();
	void gamestateLoadDone();

	QAction *aboutQtAction;
	QAction *aboutSsvAction;
	QAction *checkForUpdatesAction;
	QAction *openFileAction;
	QLabel *statusLabel;
	QMenuBar *theMenuBar;
	QMenu *fileMenu;
	QMenu *helpMenu;
	QProgressDialog *currentProgressDialog;
	QTabWidget *tabs;
	

	Galaxy::State *state = nullptr;
	EconomyView *economyView;
	FleetsView *militaryView;
	OverviewView *powerRatingView;
};

#endif //STELLARIS_STAT_VIEWER_MAINWINDOW_H
