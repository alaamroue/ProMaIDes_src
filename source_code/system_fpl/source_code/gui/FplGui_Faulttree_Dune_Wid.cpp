#include "source_code\Fpl_Headers_Precompiled.h"
//#include "FplGui_Faulttree_Dune_Wid.h"

//Default constructor
FplGui_Faulttree_Dune_Wid::FplGui_Faulttree_Dune_Wid(DataRole role, QWidget *parent): _Sys_Abstract_Base_Wid(parent){
	QWidget *widget = new QWidget(parent);
	ui.setupUi(widget);
	this->set_child(widget);
	this->set_child(widget);

	this->area2print=ui.scrollAreaWidgetContents;

	this->set_headLabel_text("Faulttree dune section");
	this->set_default_values();

	
	switch(role) {
		case 0: //DISPLAY_ROLE
			this->set_editable(false);
			this->head_pixmap->setPixmap(QPixmap(":/32x32/faulttree"));

			break;
		case 1: //EDIT_ROLE
			this->set_editable(true);
			this->headWidget->hide();
			QObject::disconnect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(context_menu_request(QPoint)));
	}
	



	QObject::connect(this->ui.checkBox_wind_wave, SIGNAL(clicked(bool )), SLOT(check_wind_wave(bool )));

	ADD_MEM(FplGui_Faulttree_Dune_Wid, _sys_system_modules::FPL_SYS);
}
//Default destructor
FplGui_Faulttree_Dune_Wid::~FplGui_Faulttree_Dune_Wid(void){

	MIN_MEM(FplGui_Faulttree_Dune_Wid, _sys_system_modules::FPL_SYS);
}
//________
//public
//Set the member, which are displayed in the widget
void FplGui_Faulttree_Dune_Wid::set_member(_Sys_Abstract_Base_Wid *ptr){
	//upcast to derived object
	FplGui_Faulttree_Dune_Wid *other = (FplGui_Faulttree_Dune_Wid *) ptr;
	if (other == NULL) {
		std::cerr << "reinterpret_cast error: could not cast object\n";
		return;
	}

	this->ui.checkBox_ero_landward->setChecked(other->ui.checkBox_ero_landward->isChecked());
	this->ui.checkBox_ero_seaward->setChecked(other->ui.checkBox_ero_seaward->isChecked());
	this->ui.checkBox_overflow->setChecked(other->ui.checkBox_overflow->isChecked());
	this->ui.checkBox_overtopping->setChecked(other->ui.checkBox_overtopping->isChecked());
	this->ui.checkBox_wind_wave->setChecked(other->ui.checkBox_wind_wave->isChecked());
}
//Changes the editable state for all child widgets in the formular/dialog
void FplGui_Faulttree_Dune_Wid::set_editable(const bool state){
	this->ui.checkBox_ero_landward->setEnabled(false);
	this->ui.checkBox_ero_seaward->setEnabled(state);
	this->ui.checkBox_overflow->setEnabled(false);
	this->ui.checkBox_overtopping->setEnabled(state);
	this->ui.checkBox_wind_wave->setEnabled(state);

	
	if(this->ui.checkBox_wind_wave->isChecked()==false){
		this->ui.checkBox_ero_seaward->setEnabled(false);
		this->ui.checkBox_overtopping->setEnabled(false);
		this->ui.checkBox_overtopping->setChecked(false);
		this->ui.checkBox_ero_seaward->setChecked(false);
	}
}
//Set default values
void FplGui_Faulttree_Dune_Wid::set_default_values(void){
	this->ui.checkBox_ero_landward->setChecked(true);
	this->ui.checkBox_ero_seaward->setChecked(true);
	this->ui.checkBox_overflow->setChecked(true);
	this->ui.checkBox_overtopping->setChecked(true);
	this->ui.checkBox_wind_wave->setChecked(true);
}
//Set the member of the widget
void FplGui_Faulttree_Dune_Wid::set_member(QSqlDatabase *ptr_database){
	int number_mecha=0;
	QSqlQueryModel model;

	this->ptr_database=ptr_database;

	//get data from database table
	try{
		number_mecha=_Fpl_Section_Type::select_mechanism(&model, this->ptr_database, _fpl_section_types::dune_sec);
	}
	catch(Error msg){
		msg.output_msg(0);
	}

	//read the data out and the the widgets
	//overflow
	string buffer;
	bool applied=false;
	bool in_tree=false;
	for(int i=0; i< number_mecha; i++) {

		buffer=model.record(i).value((_Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name)).c_str()).toString().toStdString();
		applied=functions::convert_string2boolean(model.record(i).value((_Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied)).c_str()).toString().toStdString());
		in_tree=functions::convert_string2boolean(model.record(i).value((_Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied_tree)).c_str()).toString().toStdString());
	
		if (buffer == fpl_label::mech_overflow)
		{
			this->ui.checkBox_overflow->setChecked(applied);
		}
		else if (buffer == fpl_label::mech_wave_runup)
		{
			this->ui.checkBox_overtopping->setChecked(applied);
		}
		else if (buffer == fpl_label::mech_erosion_landside)
		{
			this->ui.checkBox_ero_landward->setChecked(applied);
		}
		else if (buffer == fpl_label::mech_erosion_waterside_gent)
		{
			this->ui.checkBox_ero_seaward->setChecked(applied);
		}
		else if (buffer == fpl_label::mech_wind_wave)
		{
			this->ui.checkBox_wind_wave->setChecked(applied);
			if(applied==false){
				this->ui.checkBox_ero_seaward->setEnabled(false);
				this->ui.checkBox_overtopping->setEnabled(false);
				this->ui.checkBox_overtopping->setChecked(false);
				this->ui.checkBox_ero_seaward->setChecked(false);
			}
		}
	}
}
//________
//public slots
//Open the dialog to edit values in the widget
void FplGui_Faulttree_Dune_Wid::show_as_dialog(void){

	Sys_Base_Edit_Dia dialog(NULL, this);
	FplGui_Faulttree_Dune_Wid inDiaWid(DataRole::EditRole, &dialog);
	inDiaWid.set_member(this);
	dialog.add_child(&inDiaWid);
	dialog.setWindowTitle(this->head_label->text());
	QIcon icon;
	icon.addPixmap(*this->head_pixmap->pixmap());
	dialog.setWindowIcon(icon);
	inDiaWid.set_editable(true);
	//accepted
	if(dialog.start_dialog()==true){
		//transfer members to database
		this->transfer_members2database(&inDiaWid);
		
	}
	//rejected
	else{
		return;	
	}
}
//Check checkbox wind wave mechanism
void FplGui_Faulttree_Dune_Wid::check_wind_wave(bool clicked){

	this->ui.checkBox_ero_seaward->setEnabled(clicked);
	this->ui.checkBox_overtopping->setEnabled(clicked);
	this->ui.checkBox_overtopping->setChecked(clicked);
	this->ui.checkBox_ero_seaward->setChecked(clicked);
	

}
//________
//private
//Transfer members to database
void FplGui_Faulttree_Dune_Wid::transfer_members2database(FplGui_Faulttree_Dune_Wid *dialog){
		//the table is set (the name and the column names) and allocated
	try 
	{
		_Fpl_Section_Type::set_table_mechanism(this->ptr_database);
	}
	catch(Error msg) 
	{
		throw msg;
	}

	QSqlQueryModel query;
	ostringstream query_string;
	//set the new data to database
	//overflow mechanism 
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->ui.checkBox_overflow->isChecked())<< "  ";
	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dune_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_overflow<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid())
	{
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//overtopping mechanism 
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->ui.checkBox_overtopping->isChecked())<< "  ";
	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dune_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_wave_runup<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid())
	{
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//ero land mechanism 
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->ui.checkBox_ero_landward->isChecked())<< "  ";
	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dune_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_erosion_landside<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid())
	{
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//ero water mechanism 
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->ui.checkBox_ero_seaward->isChecked())<< "  ";
	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dune_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_erosion_waterside_gent<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid())
	{
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//wind wave mechanism 
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->ui.checkBox_wind_wave->isChecked())<< "  ";
	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dune_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_wind_wave<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid())
	{
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}




	this->set_member(dialog);
}
//set the error
Error FplGui_Faulttree_Dune_Wid::set_error(const int err_type){
		string place="FplGui_Faulttree_Dune_Wid::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://problems with database query
			place.append("transfer_members2database(FplGui_Faulttree_Dune_Wid *dialog)") ;
			reason="Can not submit the query to the database";
			help="Check the query error";
			type=2;	
			break;
		default:
			place.append("set_error(const int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}