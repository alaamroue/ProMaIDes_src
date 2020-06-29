#pragma once
/**\class Sys_PlotSettings_Dia
	\author Daniel Bachmann, Yann Labou et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2012
*/

#ifndef SYS_PLOTSETTINGS_DIA_H
#define SYS_PLOTSETTINGS_DIA_H

//QT libs
#include <QDialog>
#include <QFontDialog>
#include <QColorDialog>

//forms
#include "ui_Sys_PlotSettings_Dia.h"
//system sys
#include "Common_Const.h"


///Dialog-class for the editing of a plot widget \ingroup sys
/**

*/

class Sys_PlotSettings_Dia : public QDialog
{
	Q_OBJECT

public:
	///Default constructor
	Sys_PlotSettings_Dia(QWidget *parent = NULL);
	///Default destructor
	~Sys_PlotSettings_Dia(void);

	//members
	///Form class made with the Qt Designer for the layout of the dialog
	Ui::Sys_PlotSettings_Dia ui;

	//method
	///Start the dialog
	bool start_dialog(void);
	///Get the plot title
	const QwtText getPlotTitle(void);
	///Get the title of the horizontal axis
	const QwtText getXAxisTitle(void);
	///Get the title of the vertical axis
	const QwtText getYAxisTitle(void);
	///Check whether x axis gridlines are enabled or not
	bool isVerticalGridlinesEnabled(void);
	///Check whether y axis gridlines are enabled or not
	bool isHorizontalGridlinesEnabled(void);
	///Get the legend position
	QwtPlot::LegendPosition getLegendPosition(void);
	///Get the plot title font
	const QFont getPlotTitleFont(void);
	///Get the x-axis title font
	const QFont getHorizontalAxisTitleFont(void);
	///Get the y-axis title font
	const QFont getVerticalAxisTitleFont(void);
	///Get the plot title color
	const QColor getPlotTitleColor(void);
	///Get the x-axis title color
	const QColor getHorizontalAxisTitleColor(void);
	///Get the y-axis title color
	const QColor getVerticalAxisTitleColor(void);
	

private:
	//members
	QFont plotTitleFont;
	QFont horizontalAxisTitleFont;
	QFont verticalAxisTitleFont;
	QColor plotTitleColor;
	QColor horizontalAxisTitleColor;
	QColor verticalAxisTitleColor;

private slots:
	//The following slots use the standard convention void on_<object name>_<signal name>(<signal parameters>) to benefit the Auto-Connect property of Qt
	void on_plotTitleFontPushButton_clicked();
	void on_plotTitleColorPushButton_clicked();
	void on_horizontalAxisFontPushButton_clicked();
	void on_horizontalAxisColorPushButton_clicked();
	void on_verticalAxisFontPushButton_clicked();
	void on_verticalAxisColorPushButton_clicked();

};
#endif