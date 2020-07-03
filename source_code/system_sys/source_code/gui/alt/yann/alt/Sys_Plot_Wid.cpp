#include "source_code\Sys_Headers_Precompiled.h"
//#include "Sys_Plot_Wid.h"

//Default constructor
Sys_Plot_Wid::Sys_Plot_Wid(QWidget *parent):QwtPlot(parent){
	this->number_curve = 0;
	this->curves = NULL;
	this->symbol = NULL;
	this->legend = NULL;
	this->grid = NULL;
	this->frame=NULL;


	this->legend_flag = true;
	this->setMinimumSize(400,300);
	this->setMargin(10);
	this->setCanvasBackground(QColor(Qt::white));

	

	this->grid = new QwtPlotGrid;
	this->grid->enableXMin(true);
	this->grid->enableYMin(true);
	this->grid->setMajPen(QPen(Qt::white, 0, Qt::DotLine));
	this->grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
	this->grid->attach(this);

	this->plotMagnifier = new QwtPlotMagnifier(this->canvas());
	this->plotMagnifier->setMouseButton(Qt::MidButton);

	this->plotPanner = new QwtPlotPanner(this->canvas());
	this->plotPanner->setMouseButton(Qt::LeftButton);

	this->plotZoomer = new QwtPlotZoomer(this->canvas(), true);
	//Zoom in
	this->plotZoomer->setMousePattern(0, Qt::LeftButton, Qt::ControlModifier);
	//Zoom out by 1
	this->plotZoomer->setMousePattern(1, Qt::LeftButton, Qt::ShiftModifier);

	this->picker = new Sys_Custom_Plot_Picker(this->canvas());
	this->picker->setAxis(QwtPlot::xBottom, QwtPlot::yLeft);
	this->picker->setSelectionFlags(QwtPicker::PointSelection | QwtPicker::DragSelection);
	this->picker->setRubberBand(QwtPicker::CrossRubberBand);
	this->picker->setRubberBandPen(QColor(Qt::darkCyan));
	this->picker->setTrackerMode(QwtPicker::AlwaysOn);
	this->picker->setEnabled(true); //Picker is always enabled

	this->createActions();
	this->connectActions();
	
	this->enableZoomMode(false);
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Plot_Wid),_sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Plot_Wid::~Sys_Plot_Wid(void){
	this->clear_plot();
	this->close();
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Plot_Wid),_sys_system_modules::SYS_SYS);
}
//__________
//public
//Return pointer on the frame. This method provides an interface to access Sys_Plot_Frame functions
Sys_Plot_Frame* Sys_Plot_Wid::getFramePtr(void){	 
	//return qobject_cast<Sys_Plot_Frame* >(this->parent()->parent());
	return this->frame;
}
//Set a pointer to the parent frame
void Sys_Plot_Wid::setFramePtr(Sys_Plot_Frame *parent_frame){
	this->frame = parent_frame;
}
//Set the number of curves and allocate them
void Sys_Plot_Wid::set_number_curves(const int number){
	this->clear_plot();
	this->number_curve = number;
	this->allocate_curves();
}
//Get the number of curves
int Sys_Plot_Wid::get_number_curves(void){
	return this->number_curve;
}
//Set plot title
void Sys_Plot_Wid::set_plot_title(const string title){
	this->setTitle(title.c_str());
}
//Set title of x- and y-axis
void Sys_Plot_Wid::set_axis_title(const string x_axis, const string y_axis){
	this->setAxisTitle(QwtPlot::xBottom, x_axis.c_str());
	this->setAxisTitle(QwtPlot::yLeft, y_axis.c_str());
}
//Set curve characteristics
void Sys_Plot_Wid::set_curve_characteristics(const int index, const string title, const QwtPlotCurve::CurveStyle style, const QColor color){
	if ((0 > index) || (index >= this->number_curve)) return;
	
	this->curves[index].setTitle(title.c_str());
	this->curves[index].setStyle(style);
	this->my_color = color;
	this->curves[index].setPen(this->my_color);
}
//Set curve data
void Sys_Plot_Wid::set_curve_data(const int index, const int number_point, double *xdata, double *ydata){
	if ((0 > index) || (index >= this->number_curve)) return;
	this->curves[index].setData(xdata, ydata, number_point);
}
//Set curve symbols
void Sys_Plot_Wid::set_curve_symbols(const int index, const int size, QwtSymbol::Style style){
	if ((0 > index) || (index >= this->number_curve)) return;
	this->symbol[index].setStyle(style);
	this->symbol[index].setPen(this->my_color);
	this->symbol[index].setSize(size);
	this->curves[index].setSymbol(this->symbol[index]);
}
//Set the legend
void Sys_Plot_Wid::set_legend(const bool flag){
	this->legend_flag = flag;
}
//Set the scale for the y-axis
void Sys_Plot_Wid::set_scale_y_axis(const double min, const double max){
	this->setAxisScale(0,min,max);
	this->updateAxes();
}
//Show the plot
void Sys_Plot_Wid::show_plot(void){
	//curves to combobox
	this->addItem(true);
	//Set the curve names in the combobox as default text
	this->getFramePtr()->editComboBoxTextChanged();
	//curves to plot
	for(int i = 0; i < this->number_curve; ++i){
		this->curves[i].attach(this);
	}
	
	if(this->legend_flag == true){
		this->insertLegend(this->legend,QwtPlot::RightLegend);
	}
	if(this->parent() == NULL){
		this->show();
	}
}

//Clear the plot
void Sys_Plot_Wid::clear_plot(void){
	for(int i = 0; i < this->number_curve; ++i){
		this->curves[i].detach();
	}
	this->delete_curves();
	this->clear();
}

//Add curve names into the model stored in the selection combobox
void Sys_Plot_Wid::addItem(bool checked = false){
	if(this->getFramePtr() == NULL){
		return;
	}
	this->getFramePtr()->ui.selectionComboBox->addItems(this->curvesList());
	int counter = this->getFramePtr()->ui.selectionComboBox->count();
	for(int i = 0; i < counter; ++i){
		this->getFramePtr()->setComboBoxData(i, checked);
	}
}
//Return pointer on attached curves
const QStringList Sys_Plot_Wid::curvesList(void){
	QStringList cList;
	for(int i = 0; i < this->number_curve; ++i){
		cList << this->curves[i].title().text();
	}
	return cList;
}
//__________
//private
//Create the action of the frame widget
void Sys_Plot_Wid::createActions(void){
	printAct = new QAction(QIcon(":print_icon"), tr("&Print"), this);
	printPDFAct = new QAction(QIcon(":pdf_icon"), tr("&Print as PDF"), this);
    saveasAct = new QAction(QIcon(":save_icon"), tr("&Save as"), this);
    zoomAct = new QAction(QIcon(":zoom_icon"), tr("&Enable Zoom"), this);
	restoresizeAct = new QAction(QIcon(":restore_icon"), tr("&Restore Size"), this);
	showGridAct = new QAction(QIcon(":grid_icon"),tr("&Show Grid"), this);
	exportAct = new QAction(QIcon(":exp_icon"),tr("&Export"), this);
	editPlotAct = new QAction(QIcon(":edit_icon"),tr("&Edit Plot"), this);
	this->zoomAct->setCheckable(true);
	this->zoomAct->setChecked(false);
	this->showGridAct->setCheckable(true);
	this->showGridAct->setChecked(true);
}
//Connect the actions of the frame widget per signal and slot
void Sys_Plot_Wid::connectActions(void){
	QObject::connect(this->printAct, SIGNAL(triggered()), this, SLOT(printPlot()));
	QObject::connect(this->printPDFAct, SIGNAL(triggered()), this, SLOT(printPDF()));
	QObject::connect(this->saveasAct, SIGNAL(triggered()), this, SLOT(saveAS()));
	QObject::connect(this->zoomAct, SIGNAL(toggled(bool)), this, SLOT(enableZoomMode(bool)));
	QObject::connect(this->zoomAct, SIGNAL(toggled(bool)), this, SLOT(checkButtons(bool)));
	QObject::connect(this->restoresizeAct, SIGNAL(triggered()), this, SLOT(restoreSize()));
	QObject::connect(this->showGridAct, SIGNAL(toggled(bool)), this, SLOT(setShowGrid(bool)));
	QObject::connect(this->exportAct, SIGNAL(triggered()), this, SLOT(export_curve_data()));
	QObject::connect(this->editPlotAct, SIGNAL(triggered()), this, SLOT(editPlot()));
}

//Context menu, provides some functionalities by mouse right click
void Sys_Plot_Wid::contextMenuEvent(QContextMenuEvent *event){	
	QMenu contextMenu(this);
	contextMenu.addAction(printAct);
    contextMenu.addAction(printPDFAct);
    contextMenu.addAction(saveasAct);
	contextMenu.addSeparator();
    contextMenu.addAction(zoomAct);
	contextMenu.addAction(restoresizeAct);
	contextMenu.addAction(showGridAct);
	contextMenu.addSeparator();
	contextMenu.addAction(exportAct);
	contextMenu.addAction(editPlotAct);
	contextMenu.exec(event->globalPos());
}

//Exclude the path from the file name
QString Sys_Plot_Wid::strippedName(QString fullFileName){
	return QFileInfo(fullFileName).fileName();
}
//Exclude the path from the absolute path name
QString Sys_Plot_Wid::strippedPathName(QString fullFileName){
	return QFileInfo(fullFileName).absolutePath();
}
//Allocate the curves, symbols and legend
void Sys_Plot_Wid::allocate_curves(void){
	try{
		this->curves= new QwtPlotCurve[this->number_curve];
		Sys_Memory_Count::self()->add_mem(sizeof(QwtPlotCurve)*this->number_curve,_sys_system_modules::SYS_SYS);
		this->symbol= new QwtSymbol[this->number_curve];
		Sys_Memory_Count::self()->add_mem(sizeof(QwtSymbol)*this->number_curve,_sys_system_modules::SYS_SYS);
		this->legend=new QwtLegend;
		Sys_Memory_Count::self()->add_mem(sizeof(QwtLegend),_sys_system_modules::SYS_SYS);
					
	}
	catch (bad_alloc &t){
		Error msg = set_error(0);
		ostringstream info;
		info << "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());	
		throw msg;
	}
}
//Delete the curves, symbols and legend
void Sys_Plot_Wid::delete_curves(void){
	if(this->curves != NULL){
		delete []this->curves;
		this->curves=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(QwtPlotCurve)*this->number_curve,_sys_system_modules::SYS_SYS);

		delete []this->symbol;
		this->symbol=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(QwtSymbol)*this->number_curve,_sys_system_modules::SYS_SYS);

		delete this->legend;
		this->legend=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(QwtLegend),_sys_system_modules::SYS_SYS);

		this->number_curve=0;
	}
}
//Set error(s)
Error Sys_Plot_Wid::set_error(const int err_type){
		string place = "Sys_Plot_Wid::";
		string help;
		string reason;
		int type = 0;
		bool fatal = false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_curves(void)");
			reason = "Can not allocate the memory";
			help = "Check the memory";
			type = 10;
			break;
		default:
			place.append("set_error(const int err_type)");
			reason = "Unknown flag!";
			help = "Check the flags";
			type = 6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}
//_________
//public slots
//Paint the plot on a printer
void Sys_Plot_Wid::printPlot(void){
	QPixmap pixmap(this->size());
	pixmap.fill(Qt::transparent);

	QPrinter printer(QPrinter::HighResolution);
	printer.setOrientation(QPrinter::Landscape);
	
	QPrintDialog *printDialog = new QPrintDialog(&printer, this);
	if (printDialog->exec() == QDialog::Accepted) {
		QwtPlotPrintFilter filter;
		if ( printer.colorMode() == QPrinter::GrayScale ) {
			int options = QwtPlotPrintFilter::PrintAll;
			options &= ~QwtPlotPrintFilter::PrintBackground;
			options &= ~QwtPlotPrintFilter::PrintGrid;
			filter.setOptions(options);
		}
		this->print(printer, filter);
	}
}
//Paint the plot as PDF
void Sys_Plot_Wid::printPDF(void){
	QPixmap pixmap(this->size());
	pixmap.fill(Qt::white);

	QwtPlotPrintFilter filter;
	int options = QwtPlotPrintFilter::PrintAll;
	options &= ~QwtPlotPrintFilter::PrintBackground;
	//options &= ~QwtPlotPrintFilter::PrintGrid;
	filter.setOptions(options);

	QPrinter printer(QPrinter::HighResolution);
	if(!pixmap.isNull()) {
		QString fileName = QFileDialog::getSaveFileName(this, tr("Save the file as PDF"), 
			QDir::currentPath(), tr("Portable Document Format (*.pdf)"));
		if (!fileName.isEmpty()) {
			printer.setOutputFormat(QPrinter::PdfFormat);
			printer.setOutputFileName(fileName);
			printer.setOrientation(QPrinter::Landscape);
			printer.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);
			this->print(printer, filter);
			QMessageBox::information(this, tr("PDF was saved at..."), 
				tr("%1 was saved at %2").arg(strippedName(fileName), strippedPathName(fileName)), 
					QMessageBox::Ok);
		}
	}

}
 //Save the plot as Image (Currently available formats are .jpg and .png)
void Sys_Plot_Wid::saveAS(void){
	QPixmap pixmap(this->size());
	pixmap.fill(Qt::white);

	QwtPlotPrintFilter filter;
	int options = QwtPlotPrintFilter::PrintAll;
	options &= ~QwtPlotPrintFilter::PrintBackground;
	//options &= ~QwtPlotPrintFilter::PrintGrid;
	filter.setOptions(options);

	if(!pixmap.isNull()){
		QString fileName = QFileDialog::getSaveFileName(this, tr("Choose a filename to save under"), QDir::currentPath(), 
			tr("JPEG (*.jpg);;Portable Network Graphics (*.png);;Scalable Vector Graphics (*.svg)"));
		if (!fileName.isEmpty()){
			if(fileName.endsWith(".png")){
				this->print(pixmap, filter);
				pixmap.save(fileName, "PNG");
			}
			else if(fileName.endsWith(".jpg")){
				this->print(pixmap, filter);
				pixmap.save(fileName, "JPG");
			}
			else if(fileName.endsWith(".svg")){
				QSvgGenerator generator;
				generator.setFileName(fileName);
				generator.setSize(this->size());
				this->print(generator, filter);
			}
			else{
				return;
			}
			QMessageBox::information(this, tr("Image was saved at..."), 
				tr("%1 was saved at %2").arg(strippedName(fileName), strippedPathName(fileName)), 
					QMessageBox::Ok);
		}
	}
}

//Provides zooming functionality
void Sys_Plot_Wid::enableZoomMode(bool on){
	//Qt::WA_NoMousePropagation prohibits mouse events from being propagated to the widget's parent.
	//Only available when zoom is enabled to avoid scrolling while zooming with mouse wheel.
	this->setAttribute(Qt::WA_NoMousePropagation, on);
	this->plotMagnifier->setEnabled(on);
	this->plotPanner->setEnabled(on);
	this->plotZoomer->setEnabled(on);
	this->plotZoomer->setZoomBase();
	this->replot();
}
//Show or hide the plot's grid
void Sys_Plot_Wid::setShowGrid(bool on){
	this->grid->setVisible(on);
	this->replot();
}
//Restore plot size to original dimensions
void Sys_Plot_Wid::restoreSize(){
	this->setAxisAutoScale(QwtPlot::xBottom);
	this->setAxisAutoScale(QwtPlot::yLeft);
	this->plotZoomer->setZoomBase();
	this->replot();
}
//Ensure that Zoom Button is checked as soon as Zoom Action is being checked
void Sys_Plot_Wid::checkButtons(bool on){
	this->getFramePtr()->ui.zoomPushButton->setChecked(on);
}
//Open the export dialog
void Sys_Plot_Wid::export_curve_data(void){
	Sys_Export_Dia exportDialog(this);
	//Checkbox not needed right now
	exportDialog.ui.selectAll_checkBox->setCheckable(false);
	exportDialog.get_ptr_selectall_groupbox()->hide();
	exportDialog.set_header_txt("Export");
	
	if(exportDialog.start_dialog()==true) {
		this->export_type = exportDialog.get_export_type();
		this->filename = exportDialog.get_output_file();

		if(this->export_type == _sys_export_type::exp_excel_comma){
			this->setOutputTypeAsciiComma();
		}
		else if(this->export_type == _sys_export_type::exp_excel_tab){
			this->setOutputTypeAsciiTab();
		} 
		else if(this->export_type == _sys_export_type::exp_tecplot){
			this->setOutputTypeTecplot();
		}
	}

	
}
//Provides various posibilities to edit the plot curves
void Sys_Plot_Wid::editPlot(void){
	Sys_PlotSettings_Dia settingsDia(this);

	settingsDia.set_title_x_y_axis(this->title().text().toStdString(), this->axisTitle(QwtPlot::xBottom).text().toStdString(), this->axisTitle(QwtPlot::yLeft).text().toStdString());
	settingsDia.set_font_title_x_y_axis(this->title().font(),this->axisTitle(QwtPlot::xBottom).font(),this->axisTitle(QwtPlot::yLeft).font());
	settingsDia.set_color_title_x_y_axis(this->title().color(),this->axisTitle(QwtPlot::xBottom).color(),this->axisTitle(QwtPlot::yLeft).color());

	for(int i=0; i< this->number_curve; i++){
		settingsDia.set_curve_setting2tabwidget(this->curves[i].title().text().toStdString());
		
	}

	if(settingsDia.start_dialog()==true){
		//title
		QwtText title;
		title.setText(settingsDia.get_title().c_str());
		title.setFont(settingsDia.get_font_title());
		title.setColor(settingsDia.get_color_title());
		this->setTitle(title);
		//x_axis
		QwtText x_axis;
		x_axis.setText(settingsDia.get_x_axis_name().c_str());
		x_axis.setFont(settingsDia.get_font_x_axis_name());
		x_axis.setColor(settingsDia.get_color_x_axis_name());
		this->setAxisTitle(QwtPlot::xBottom, x_axis);
		//y_axis
		QwtText y_axis;
		y_axis.setText(settingsDia.get_y_axis_name().c_str());
		y_axis.setFont(settingsDia.get_font_y_axis_name());
		y_axis.setColor(settingsDia.get_color_y_axis_name());
		this->setAxisTitle(QwtPlot::yLeft, y_axis);

		this->replot();
	}


}
//Export selected data to file in ascii-format separated with tab
void Sys_Plot_Wid::setOutputTypeAsciiTab(void){
	ofstream asciiFileTab;
	asciiFileTab.open (this->filename.c_str());

	if(!(asciiFileTab.is_open())){
		Error msg = this->set_error(0);
		ostringstream info;
		info <<"File name : " << this->filename << endl;
		msg.make_second_info(info.str());
	}
	ostringstream buffer;
	
	//Header
	buffer << "Title:" << this->title().text().toStdString() << endl;
	buffer << endl;
	for(int j = 0; j < this->number_curve; ++j){
		buffer << this->curves[j].title().text().toStdString() << "\t\t";
	}
	buffer << endl;
	for(int j = 0; j < this->number_curve; ++j){
		buffer << QString("X%1").arg(j).toStdString() << "\t" << QString("Y%1").arg(j).toStdString() << "\t";
	}
	buffer << endl;
	asciiFileTab << buffer.str() << endl;
	buffer.str("");

	//Data
	for (int i = 0; i < this->curves->dataSize(); ++i){
		for(int j = 0; j < this->number_curve; ++j){
			buffer << this->curves[j].x(i) << "\t" << this->curves[j].y(i) << "\t";
		}
		buffer << endl;
	}

	asciiFileTab << buffer.str();
	buffer.str("");
	asciiFileTab.close();
}

//Export selected data to file in ascii-format separated with comma
void Sys_Plot_Wid::setOutputTypeAsciiComma(void){
	ofstream asciiFileComma;
	asciiFileComma.open (this->filename.c_str());

	if(!(asciiFileComma.is_open())){
		Error msg = this->set_error(0);
		ostringstream info;
		info <<"File name : " << this->filename << endl;
		msg.make_second_info(info.str());
	}
	ostringstream buffer;
	
	//Header
	buffer << "Title:" << this->title().text().toStdString() << endl;
	buffer << endl;
	for(int j = 0; j < this->number_curve; ++j){
		buffer << this->curves[j].title().text().toStdString() << ",,";
	}
	buffer << endl;
	for(int j = 0; j < this->number_curve; ++j){
		buffer << QString("X%1").arg(j).toStdString() << "," << QString("Y%1").arg(j).toStdString() << ",";
	}
	buffer << endl;
	asciiFileComma << buffer.str() << endl;
	buffer.str("");

	//Data
	for (int i = 0; i < this->curves->dataSize(); ++i){
		for(int j = 0; j < this->number_curve; ++j){
			buffer << this->curves[j].x(i) << "," << this->curves[j].y(i) << ",";
		}
		buffer << endl;
	}

	asciiFileComma << buffer.str();
	buffer.str("");
	asciiFileComma.close();
}

//Export selected data to file in tecplot-format
void Sys_Plot_Wid::setOutputTypeTecplot(void){
	ofstream asciiFileTecplot;
	asciiFileTecplot.open (this->filename.c_str());

	if(!(asciiFileTecplot.is_open())){
		Error msg = this->set_error(0);
		ostringstream info;
		info <<"File name : " << this->filename << endl;
		msg.make_second_info(info.str());
	}
	ostringstream buffer;
	
	//Header
	buffer << "TITLE = \"" << this->title().text().toStdString() << "\"" << endl;
	buffer << "VARIABLES = \"" << "X\"" << "\t" << "\"" << "Y\"" << endl;
	buffer << endl;

	//Data
	for (int j = 0; j < this->number_curve; ++j){
		buffer << "ZONE T = \" " << this->curves[j].title().text().toStdString() << " \"" << endl;
		buffer << "I = " << this->curves->dataSize() << endl;
		for(int i = 0; i < this->curves->dataSize(); ++i){
			buffer << this->curves[j].x(i) << "\t" << this->curves[j].y(i) << "\t" << endl;
		}
		buffer << endl;
	}
	asciiFileTecplot << buffer.str();
	buffer.str("");
	asciiFileTecplot.close();
}

//Show or hide selected curves by checking or not the checkboxes in the selection combobox
void Sys_Plot_Wid::showCurves(const QModelIndex& index){
	int i = index.row();
	int counter = this->getFramePtr()->ui.selectionComboBox->count();
	for(i = 0; i < counter; ++i){
		if(this->getFramePtr()->isChecked(i)){
			this->curves[i].setVisible(true);
			//Show legend if curve is visible
			this->curves[i].setItemAttribute(QwtPlotItem::Legend, true);
		}else if(!(this->getFramePtr()->isChecked(i))){
			this->curves[i].setVisible(false);
			//Hide legend if curve is not visible anymore
			this->curves[i].setItemAttribute(QwtPlotItem::Legend, false);
		}else{
			return;
		}
		this->replot();
	}
}