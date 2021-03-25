#include "source_code\Dam_Headers_Precompiled.h"
#include "Dam_CI_System.h"

//Default constructor
Dam_CI_System::Dam_CI_System(void){

	this->no_ci_point = 0;
	this->no_ci_polygon = 0;
	this->file_ci_point = label::not_set;
	this->file_ci_polygon = label::not_set;
	this->file_ci_connection = label::not_set;
	this->dam_ci_polygon = NULL; 
	this->dam_ci_point = NULL;

	this->del_data_flag = false;
	this->del_res_flag = false;
	this->reset_result_values();

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_CI_System)+ 4*sizeof(QList<QList<int>>)+3*sizeof(QList<QList<QVariant>>), _sys_system_modules::DAM_SYS);
	
}
//Default destructor
Dam_CI_System::~Dam_CI_System(void){
	this->delete_CI_point();
	this->delete_CI_polygon();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_CI_System) + 4 * sizeof(QList<QList<int>>) + 3 * sizeof(QList <QList<QVariant>>), _sys_system_modules::DAM_SYS);
}
//__________
//public
	///Transfer CI data from file to database: points, polygons, connections
void Dam_CI_System::ci_data_file2database(QSqlDatabase *ptr_database, const _sys_system_id id) {
	try {
		this->set_start_warnings_number();
		//first delete all function data
		Dam_CI_Point::delete_data_in_point_table(ptr_database);
		Dam_CI_Point::delete_data_in_erg_table(ptr_database);
		Dam_CI_Point::delete_data_in_instat_erg_table(ptr_database);
		Dam_CI_Polygon::delete_data_in_polygon_table(ptr_database);
		Dam_CI_Polygon::delete_data_in_erg_table(ptr_database);
		Dam_CI_Polygon::delete_data_in_instat_erg_table(ptr_database);
		Dam_CI_Element_List::delete_data_in_connection_table(ptr_database);
		
		//Todo delte others

		ostringstream cout;
		cout << "Import the data of the CI system to the database ..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "IMPO> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		//CI Point
		if (this->file_ci_point != label::not_set) {
			//read the data in
			cout << "Read CI-points from file " << this->file_ci_point<<"..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			this->read_points_per_file(this->file_ci_point);
			Dam_Damage_System::check_stop_thread_flag();
			
			Dam_Damage_System::check_stop_thread_flag();
			cout << "Transfer the data of " << this->no_ci_point << " CI-points to the database ..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//transfer it to database
			this->transfer_point_members2database(ptr_database);
		}
		else {
			cout << "No file is set for the CI-point import" << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}

		//CI Polygon
		if (this->file_ci_polygon != label::not_set) {
			//read the data in
			cout << "Read CI-polygons from file " << this->file_ci_polygon << "..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			this->read_polygon_per_file(this->file_ci_polygon);
			Dam_Damage_System::check_stop_thread_flag();

			Dam_Damage_System::check_stop_thread_flag();
			cout << "Transfer the data of " << this->no_ci_polygon << " CI-polygon to the database ..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//transfer it to database
			this->transfer_polygon_members2database(ptr_database);
		}
		else {
			cout << "No file is set for the CI-polygon import" << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}


		//CI connection
		if (this->file_ci_connection != label::not_set) {
			//read the data in
			cout << "Read CI-connection from file " << this->file_ci_connection << "..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			this->read_connection_per_file(this->file_ci_connection);
			Dam_Damage_System::check_stop_thread_flag();
			Dam_Damage_System::check_stop_thread_flag();
			cout << "Transfer the data of " << this->dam_ci_connection.get_number_connection() << " CI-connection to the database ..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//transfer it to database
			this->transfer_connection_members2database(ptr_database);
		}
		else {
			cout << "No file is set for the CI-connection import" << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}

	


		
		this->output_members();

	
		this->init_system();

		this->set_warning_number();
		cout << "Data-Import of the CI system is finished" << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		
		//rewind the prefix
		Sys_Common_Output::output_dam->rewind_userprefix();

	}
	catch (Error msg) {
		//rewind the prefix
		Sys_Common_Output::output_dam->rewind_userprefix();
		if (msg.get_user_aborted_exception() == false) {
			this->number_error++;
		}
		msg.output_msg(4);
	}

	this->output_error_statistic();


}
//Delete all CI data in the database: points, polygons, connections, results etc.
void Dam_CI_System::del_ci_data_database(QSqlDatabase *ptr_database) {

	ostringstream prefix;
	prefix << "CI> ";

	ostringstream cout;
	cout << "Delete all CI-system information..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	//ecn
	try {
		if (this->del_data_flag == true) {
			cout << "CI-points..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_CI_Point::delete_data_in_point_table(ptr_database);
			cout << "CI-polygon..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_CI_Polygon::delete_data_in_polygon_table(ptr_database);
			cout << "CI-connection..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_CI_Element_List::delete_data_in_connection_table(ptr_database);
		}
		if (this->del_res_flag == true) {
			cout << "CI-system results..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			cout << "CI-points..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_CI_Point::delete_data_in_erg_table(ptr_database);
			Dam_CI_Point::delete_data_in_instat_erg_table(ptr_database);
			cout << "CI-polygons..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_CI_Polygon::delete_data_in_erg_table(ptr_database);
			Dam_CI_Polygon::delete_data_in_instat_erg_table(ptr_database);
			cout << "CI-connections..." << endl;
			Dam_CI_Element_List::delete_data_in_erg_table(ptr_database);
			Dam_CI_Element_List::delete_data_in_instat_erg_table(ptr_database);
		}
	}
	catch (Error msg) {
		msg.output_msg(4);
	}
	cout << "Deleting of CI-system information is finished!" << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->rewind_userprefix();

}
//Ask for the file of the CI data per dialog
bool Dam_CI_System::ask_file_CI_data(QWidget *parent) {
	/** If the return-value is true, the dialog is accepted. "False" as return
	value, means it is canceled. The result is set to the member file_category_func */
	Sys_Multi_Filechooser_Dia my_dia(parent);

	//set up dialog
	QIcon icon;
	icon.addFile(":ci_icon");
	my_dia.set_number_file_browser(3, icon);
	stringstream buffer;
	buffer << "Choose the files, where the CI data are stored." << endl;
	buffer << "This files contain the definition of the CI-points, CI-polygons and their connections." << endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose CI-data files");
	my_dia.get_ptr_file_browser(0)->set_text_label("CI-point file");
	my_dia.get_ptr_file_browser(1)->set_text_label("CI-polygon file");
	my_dia.get_ptr_file_browser(2)->set_text_label("CI-connection file");

	//start dialog
	if (my_dia.start_dialog() == false) {
		return false;
	}
	else {
		//points
		this->file_ci_point = my_dia.get_ptr_file_browser(0)->get_file_name();

		if (this->file_ci_point == label::not_set) {
			Sys_Diverse_Text_Dia dialog2(true);
			QIcon icon;
			icon.addFile(":dam_icon");
			dialog2.set_window_icon(icon);
			ostringstream txt;
			txt << "No file is set for the CI-points!" << endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}

		//polygons
		this->file_ci_polygon = my_dia.get_ptr_file_browser(1)->get_file_name();

		if (this->file_ci_polygon == label::not_set) {
			Sys_Diverse_Text_Dia dialog2(true);
			QIcon icon;
			icon.addFile(":dam_icon");
			dialog2.set_window_icon(icon);
			ostringstream txt;
			txt << "No file is set for the CI-polygons!" << endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}

		//connections
		this->file_ci_connection = my_dia.get_ptr_file_browser(2)->get_file_name();

		if (file_ci_connection == label::not_set) {
			Sys_Diverse_Text_Dia dialog2(true);
			QIcon icon;
			icon.addFile(":dam_icon");
			dialog2.set_window_icon(icon);
			ostringstream txt;
			txt << "No file is set for the CI-connections!" << endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}

		return true;
	}
}
//Ask for the flag, what should be deleted (points, subcategories)
bool Dam_CI_System::ask_deleting_flag(QWidget *parent) {
	Sys_Multi_CheckBox_Dia my_dia(parent);
	//set up dialog
	QIcon icon;
	icon.addFile(":ci_icon");
	my_dia.set_number_check_boxes(2, icon);
	stringstream buffer;
	buffer << "Choose what should be deleted in the CI-system in the database..." << endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose deleting CI-system");
	my_dia.get_ptr_check_box(0)->setText("Delete CI-data (points, polygons and connections)");
	my_dia.get_ptr_check_box(1)->setText("Delete CI-results");

	//start dialog
	if (my_dia.start_dialog() == false) {
		return false;
	}
	else {
		this->del_data_flag = my_dia.get_bool_check_box(0);
		this->del_res_flag = my_dia.get_bool_check_box(1);
		return true;
	}
}
//Get a text for deleting CI information in database
string Dam_CI_System::get_deleting_text(void) {
	ostringstream buffer;
	if (this->del_data_flag == true) {
		buffer << " The CI data will be deleted in database, also the result data!" << endl;
	}
	if (this->del_res_flag == true) {
		buffer << " All CI result data will be deleted in database!" << endl;
	}


	return buffer.str();
}
//Output the members
void Dam_CI_System::output_members(void) {
	ostringstream prefix;
	prefix << "CI> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;

	cout << "Output the points of the CI-system..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->output_point_member();

	cout << "Output the polygons of the CI-system..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->output_polygon_member();

	cout << "Output the connections of the CI-system..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->output_connection_member();

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output statistic of CI system
void Dam_CI_System::output_statistic(void) {
	ostringstream prefix;
	prefix << "CI> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "CI POINTS" << endl;
	cout << " Number             " << this->no_ci_point<< endl;
	cout << "CI POLYGONS" << endl;
	cout << " Number             " << this->no_ci_polygon << endl;
	cout << "CI CONNECTIONS" << endl;
	cout << " Number             " << this->dam_ci_connection.get_number_connection() << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output the sum over the raster of the damage results to display/console
void Dam_CI_System::output_result_damage(void) {
	ostringstream prefix;

	prefix << "CI> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "TOTAL RESULTS CI-SYSTEM" << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	cout << " RESULT FOR CI-POINTS" << endl;
	cout << "  Number direct failure total: " << this->tot_direct_failure << endl;
	if (this->tot_direct_failure > 0) {
		cout << "  Number direct failure per sector" << endl;
		for (int i = 0; i < this->list_direct_failure_sec.count(); i++) {
			cout << "   " << _Dam_CI_Element::convert_sector_id2txt(_Dam_CI_Element::convert_id2enum(this->list_direct_failure_sec[i].at(0))) << ": " << W(10) << this->list_direct_failure_sec[i].at(1) << endl;
		}
	}

	Sys_Common_Output::output_dam->output_txt(&cout);
	cout << "  Number sectoral failure total: " << this->tot_sectoral_failure << endl;
	if (this->tot_sectoral_failure > 0) {
		cout << "  Number sectoral failure per sector" << endl;
		for (int i = 0; i < this->list_sectoral_failure_sec.count(); i++) {
			cout << "   " << _Dam_CI_Element::convert_sector_id2txt(_Dam_CI_Element::convert_id2enum(this->list_sectoral_failure_sec[i].at(0))) << ": " << W(10) << this->list_sectoral_failure_sec[i].at(1) << endl;
		}
	}

	Sys_Common_Output::output_dam->output_txt(&cout);
	cout << "  Number transsectoral failure total: " << this->tot_transsectoral_failure << endl;
	if (this->tot_transsectoral_failure > 0) {
		cout << "  Number transsectoral failure per sector" << endl;
		for (int i = 0; i < this->list_transsectoral_failure_sec.count(); i++) {
			cout << "   " << _Dam_CI_Element::convert_sector_id2txt(_Dam_CI_Element::convert_id2enum(this->list_transsectoral_failure_sec[i].at(0))) << ": " << W(10) << this->list_transsectoral_failure_sec[i].at(1) << endl;
		}
	}


	Sys_Common_Output::output_dam->output_txt(&cout);
	cout << "  Number emergency CI-structure activated total: " << this->tot_emergency_active << endl;
	cout << "  Number emergency CI-structure activated per sector" << endl;
	for (int i = 0; i < this->list_emergency_active_sec.count(); i++) {
		cout << "   " << _Dam_CI_Element::convert_sector_id2txt(_Dam_CI_Element::convert_id2enum(this->list_emergency_active_sec[i].at(0))) << ": " << W(10) << this->list_emergency_active_sec[i].at(1) << endl;
	}
	

	Sys_Common_Output::output_dam->output_txt(&cout);
	cout << "  Number failure per sector" << endl;
	for (int i = 0; i < this->list_total_failure_sec.count(); i++) {
		cout << "   " << _Dam_CI_Element::convert_sector_id2txt(_Dam_CI_Element::convert_id2enum(this->list_total_failure_sec[i].at(0))) << ": " << W(10) << this->list_total_failure_sec[i].at(1) << endl;
	}

	Sys_Common_Output::output_dam->output_txt(&cout);
	cout << "  Number failure x level per sector" << endl;
	for (int i = 0; i < this->list_total_failure_level_sec.count(); i++) {
		cout << "   " << _Dam_CI_Element::convert_sector_id2txt(_Dam_CI_Element::convert_id2enum(this->list_total_failure_level_sec[i].at(0).toInt())) << ": " << W(10) <<P(2) << FORMAT_FIXED_REAL << this->list_total_failure_level_sec[i].at(1).toDouble() << endl;
	}

	cout << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	cout << " RESULTS FOR CI-POLYGONS" << endl;
	cout << "  Enduser affected per sector" << endl;
	for (int i = 0; i < this->list_enduser_affected_sec.count(); i++) {
		cout << "   " << _Dam_CI_Element::convert_sector_id2txt(_Dam_CI_Element::convert_id2enum(this->list_enduser_affected_sec[i].at(0).toInt())) << ": " << W(10) << P(2) << FORMAT_FIXED_REAL << this->list_enduser_affected_sec[i].at(1).toDouble() << endl;
	}

	
	Sys_Common_Output::output_dam->output_txt(&cout);
	cout << "  Enduser affected x duration per sector" << endl;
	for (int i = 0; i < this->list_enduser_affected_duration_sec.count(); i++) {
		cout << "   " << _Dam_CI_Element::convert_sector_id2txt(_Dam_CI_Element::convert_id2enum(this->list_enduser_affected_duration_sec[i].at(0).toInt())) << ": " << W(10) << P(2) << FORMAT_FIXED_REAL << this->list_enduser_affected_duration_sec[i].at(1).toDouble() << endl;
	}
	
	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->rewind_userprefix();






}
//Check the damage system 
void Dam_CI_System::check_system(void) {
	if (this->no_ci_point != 0) {
		int counter = this->check_points_connected2hyd();
		if (counter < this->no_ci_point) {
			Warning msg = this->set_warning(6);
			ostringstream info;
			info << "Points connected           : " << counter << endl;
			info << "Total number points        : " << this->no_ci_point << endl;
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}

}
//Intercept CI-points with the hydraulic elements
void Dam_CI_System::intercept_hydraulic2damage(Hyd_Model_Floodplain *fp_models, const int number_fp, QSqlDatabase *ptr_database, const _sys_system_id id) {
	ostringstream prefix;
	prefix << "CI> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);

	ostringstream cout;
	cout << "Check the interception of CI-points to " << number_fp << " hydraulic floodplain(s)..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	int *rang = NULL;
	cout << "Sort hydraulic floodplain models..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->sort_fp_models_elem_size(&rang, fp_models, number_fp);
	try {
		this->input_perdatabase_point_data(ptr_database, id);
		if (this->no_ci_point > 0) {

			for (int j = 0; j < number_fp; j++) {
				cout << "Check the interception of CI-points to hydraulic floodplain " << fp_models[rang[j]].Param_FP.get_floodplain_number() << "..." << endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
				prefix << "FP_" << fp_models[rang[j]].Param_FP.get_floodplain_number() << "> ";
				Sys_Common_Output::output_dam->set_userprefix(&prefix);

				bool delete_again = false;
				if (fp_models[rang[j]].floodplain_elems == NULL || fp_models[rang[j]].raster.get_ptr_raster_points() == NULL || fp_models[rang[j]].raster.get_ptr_raster_segments() == NULL) {
					delete_again = true;
					QSqlQueryModel bound_result;
					int number_bound = 0;
					cout << "Generate elements, raster points and segments for the interception of CI-points to hydraulic floodplain " << fp_models[rang[j]].Param_FP.get_floodplain_number() << "..." << endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
					fp_models[rang[j]].input_elems_database(ptr_database, &bound_result, number_bound, false, true);
				}
				this->set_intercepted_point2floodplain_id(&fp_models[rang[j]]);

				if (delete_again == true) {
					cout << "Delete elements, raster points and segments of the hydraulic floodplain " << fp_models[rang[j]].Param_FP.get_floodplain_number() << "..." << endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
					fp_models[rang[j]].delete_elems_raster_geo();
				}
				Dam_Damage_System::check_stop_thread_flag();
				Sys_Common_Output::output_dam->rewind_userprefix();
			}

			this->transfer_intercepted_data2database(ptr_database);
		}
	}
	catch (Error msg) {
		if (rang != NULL) {
			delete[]rang;
		}
		throw msg;
	}

	if (rang != NULL) {
		delete[]rang;
	}
	Sys_Common_Output::output_dam->rewind_userprefix();

}
///Check the connection to the hydraulic of the points (static)
bool Dam_CI_System::check_connection2hydraulic(QSqlDatabase *ptr_database, const _sys_system_id id) {
	QSqlQueryModel buffer;
	int number = Dam_CI_Point::count_relevant_points_database(&buffer, ptr_database, id, false);
	if (number == 0) {
		return true;
	}
	number = Dam_CI_Point::count_not_connected_points2hyd(ptr_database, id);

	if (number == 0) {
		return true;
	}
	else {
		return false;
	}

}
///Read in CI-system per database
void Dam_CI_System::read_system_per_database(QSqlDatabase *ptr_database, const _sys_system_id id) {
	this->input_perdatabase_point_data(ptr_database, id);
	this->input_perdatabase_polygon_data(ptr_database, id);
	this->input_perdatabase_connection_data(ptr_database, id);
}
//Init the CI-system 
void Dam_CI_System::init_system(void) {
	ostringstream cout;
	cout << "Initialize the CI-system..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	_Dam_CI_Element *buffer_elem_out = NULL;
	_Dam_CI_Element *buffer_elem_in = NULL;
	//add connection to CI-element
	for (int i = 0; i < this->dam_ci_connection.get_number_connection(); i++) {
		//incomming for connection (first)
		buffer_elem_in = this->find_ptr_CI_element(this->dam_ci_connection.elem_list.at(i).at(0), this->dam_ci_connection.elem_list.at(i).at(1));
		//outgoing for connection (second)
		buffer_elem_out = this->find_ptr_CI_element(this->dam_ci_connection.elem_list.at(i).at(2), this->dam_ci_connection.elem_list.at(i).at(3));
		if (buffer_elem_out != NULL && buffer_elem_in != NULL) {
			buffer_elem_in->add_outgoing(buffer_elem_out);
			buffer_elem_out->add_incomings(buffer_elem_in);

		}
	}
	//init lists
	for (int i = 0; i < this->no_ci_point; i++) {
		this->dam_ci_point[i].init_sec_list();

	}
	for (int i = 0; i < this->no_ci_polygon; i++) {
		this->dam_ci_polygon[i].init_sec_list();

	}
	//check after init
	this->check_CI_system();

}
//Calculate the damages
void Dam_CI_System::calculate_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number) {
	ostringstream cout;
	cout << "Calculate damages for the CI-system..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	//reset system
	for (int i = 0; i < this->no_ci_point; i++) {
		this->dam_ci_point[i].reset_result_values();
	}
	//reset system
	for (int i = 0; i < this->no_ci_polygon; i++) {
		this->dam_ci_polygon[i].reset_result_values();
	}
	
	this->calculate_direct_damage(impact_fp, number);
	this->calculate_indirect_damage();



}
//Calculate the instationary damages
void Dam_CI_System::calculate_instat_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number, const double time, const int count) {
	//ostringstream cout;
	//cout << "Calculate instationary damages for the CI-system; calculation time is "<< time<< label::sec << endl;
	//Sys_Common_Output::output_dam->output_txt(&cout);
	if (count == 0) {
		//reset system
		for (int i = 0; i < this->no_ci_point; i++) {
			this->dam_ci_point[i].reset_result_values();
		}
		//reset system
		for (int i = 0; i < this->no_ci_polygon; i++) {
			this->dam_ci_polygon[i].reset_result_values();
		}

	}

	this->calculate_instat_direct_damage(impact_fp, number, time);
	this->calculate_instat_indirect_damage(time);


}
//Delete the result members for a given system-id and a scenario (boundary-, break-)
void Dam_CI_System::delete_result_members_in_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz) {
	//delete the data in the table
	try {
		Dam_CI_Point::delete_data_in_erg_table(ptr_database, id, bound_sz, break_sz);
		Dam_CI_Polygon::delete_data_in_erg_table(ptr_database, id, bound_sz, break_sz);
		Dam_CI_Element_List::delete_data_in_erg_table(ptr_database, id, bound_sz, break_sz);
	}
	catch (Error msg) {
		throw msg;
	}

}
//Delete the instationary result members for a given system-id and a scenario (boundary-, break-)
void Dam_CI_System::delete_instat_result_members_in_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz) {
	//delete the data in the table
	try {

		Dam_CI_Point::delete_data_in_instat_erg_table(ptr_database, id, bound_sz, break_sz);

		Dam_CI_Polygon::delete_data_in_instat_erg_table(ptr_database, id, bound_sz, break_sz);
		Dam_CI_Element_List::delete_data_in_instat_erg_table(ptr_database, id, bound_sz, break_sz);
	}
	catch (Error msg) {
		throw msg;
	}


}
//Output the result members to a database table
void Dam_CI_System::output_result_member2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output) {
	ostringstream cout;
	bool must_output = false;
	if (*was_output == false) {
		must_output = true;
	}
	cout << "Output CI-system damages to database..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->output_point_results2database(ptr_database, bound_sz, break_sz, was_output);
	this->output_polygon_results2database(ptr_database, bound_sz, break_sz, was_output);
	this->output_connection_results2database(ptr_database, bound_sz, break_sz, was_output);


}
//Output the instat_result members to a database table
void Dam_CI_System::output_instat_result_member2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const string date_time, bool *was_output) {
	//ostringstream cout;
	bool must_output = false;
	if (*was_output == false) {
		must_output = true;
	}
	//cout << "Output CI-system instationary damages to database..." << endl;
	//Sys_Common_Output::output_dam->output_txt(&cout);
	this->output_point_instat_results2database(ptr_database, bound_sz, break_sz, date_time, was_output);
	this->output_polygon_instat_results2database(ptr_database, bound_sz, break_sz, date_time, was_output);
	this->output_connection_instat_results2database(ptr_database, bound_sz, break_sz, date_time, was_output);

}
//Sum up the total damage results for a given system-id and scenario (boundary-, break-) from the database
void Dam_CI_System::sum_total_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz) {
	this->reset_result_values();
	this->sum_total_point_results(ptr_database, id, bound_sz, break_sz);
	this->sum_total_polygon_results(ptr_database, id, bound_sz, break_sz);

}
//Check if all points are active again after hydraulic
bool Dam_CI_System::check_points_active_again(void) {

	for (int i = 0; i < this->no_ci_point; i++) {
		if (this->dam_ci_point[i].get_element_active() == false && this->dam_ci_point[i].get_regular_flag() == true) {
			return false;
		}
		else if (this->dam_ci_point[i].get_element_active() == true && this->dam_ci_point[i].get_regular_flag() == false) {
			return false;

		}
	}

	return true;
}
//____________
//private
//Read in the CI point data from file
void Dam_CI_System::read_points_per_file(string fname) {
	ifstream ifile;
	//open file
	ifile.open(fname.c_str(), ios_base::in);
	if (ifile.is_open() == false) {
		Error msg = this->set_error(2);
		ostringstream info;
		info << "Filename: " << fname << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	int line_counter = 0;
	string myline;
	int pos = -1;
	stringstream my_stream;
	int found_points = 0;

	try {
		//search for the begin
		do {
			getline(ifile, myline, '\n');
			line_counter++;
			_Hyd_Parse_IO::erase_comment(&myline);
			_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
			_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
			pos = myline.rfind("!BEGIN");
			if (pos >= 0) {
				myline.erase(pos + 6);
			}
		} while (myline != "!BEGIN" && ifile.eof() != true);

		if (ifile.eof() == true) {
			ostringstream info;
			info << "Filename              : " << fname << endl;
			info << "Error occurs near line: " << line_counter << endl;
			Error msg = this->set_error(7);
			msg.make_second_info(info.str());
			throw msg;
		}

		do {
			//next line for the curve number and number of points
			getline(ifile, myline, '\n');
			line_counter++;
			_Hyd_Parse_IO::erase_comment(&myline);
			_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
			_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
		} while (myline.empty() == true && ifile.eof() != true);

		if (ifile.eof() == true) {
			ostringstream info;
			info << "Filename              : " << fname << endl;
			info << "Error occurs near line: " << line_counter << endl;
			Error msg = this->set_error(7);
			msg.make_second_info(info.str());
			throw msg;
		}

		//read it in
		my_stream << myline;
		my_stream >> found_points;

		if (my_stream.fail() == true) {
			ostringstream info;
			info << "Wrong input sequenze  : " << my_stream.str() << endl;
			info << "Filename              : " << fname << endl;
			info << "Input must be         :  number_points" << endl;
			info << "Error occurs near line: " << line_counter << endl;
			Error msg = this->set_error(3);
			msg.make_second_info(info.str());
			throw msg;
		}

		this->no_ci_point = found_points;
		if (this->no_ci_point > constant::max_elems) {
			Error msg = this->set_error(8);
			throw msg;
		}

		my_stream.clear();
		my_stream.str("");

		int counter = 0;
		int col = 0;
		double x_buffer = 0.0;
		double y_buffer = 0.0;
		int buff_sector_id = 0;
		int buff_sector_level = 0;
		double buff_boundary_value = 0.0;
		double recovery_time = 0.0;
		bool buff_endflag = false;
		string name_buff = label::not_set;
		int buff_number=0;
		string reg_flag_buff_str = label::not_set;
		bool reg_flag_buff = true;
		double active_time_buff = 0.0;

		this->allocate_CI_point();

		//read in the points
		do {
			
			//delete the comments
			getline(ifile, myline, '\n');
			line_counter++;
			_Hyd_Parse_IO::erase_comment(&myline);
			_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
			_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
			if (myline.empty() != true) {
				pos = myline.rfind("!END");
				if (pos >= 0) {
					myline.erase(pos + 4);
				}
				//stop when end is reach
				if (myline == "!END" || ifile.eof() == true) {
					break;
				}
				//read it in
				col = functions::count_number_columns(myline);
				my_stream << myline;



				try {
					if (col == 10) {
						my_stream >> buff_number >> x_buffer >> y_buffer >> name_buff >> buff_sector_id >> buff_sector_level >> buff_boundary_value >> recovery_time >> reg_flag_buff_str >> active_time_buff;

						buff_endflag = _Dam_CI_Element::sector_id2endflag(buff_sector_id);
						reg_flag_buff = functions::convert_string2boolean(reg_flag_buff_str);
							

					}
					else {
						//Error
						Error msg = this->set_error(4);
						throw msg;
					}
				}
				catch (Error msg) {
					ostringstream info;
					info << "Filename              : " << fname << endl;
					info << "Error occurs near line: " << line_counter << endl;
					info << "Point id              : " << counter + 1 << endl;
					msg.make_second_info(info.str());
					throw msg;
				}

				if (my_stream.fail() == true) {
					ostringstream info;
					info << "Wrong input sequenze  : " << my_stream.str() << endl;
					info << "Filename              : " << fname << endl;
					info << "Error occurs near line: " << line_counter << endl;
					info << "Point id              : " << counter + 1 << endl;
					Error msg = this->set_error(4);
					msg.make_second_info(info.str());
					throw msg;
				}
				my_stream.clear();
				my_stream.str("");
				try {
					//set the point buffer
					this->dam_ci_point[counter].set_point_coordinate(x_buffer, y_buffer);
					this->dam_ci_point[counter].set_point_name(name_buff);
					this->dam_ci_point[counter].set_number(buff_number);
					this->dam_ci_point[counter].set_members(buff_sector_id, buff_sector_level, buff_boundary_value, recovery_time, buff_endflag, reg_flag_buff, active_time_buff);
				}
				catch (Error msg) {
					ostringstream info;
					info << "Filename              : " << fname << endl;
					info << "Error occurs near line: " << line_counter << endl;
					info << "Point id              : " << counter + 1 << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
				counter++;
			}
		} while (counter < this->no_ci_point && ifile.eof() != true);

		//check if all is read
		if (counter != this->no_ci_point && ifile.eof() != true) {
			ostringstream info;
			info << "Filename              : " << fname << endl;
			info << "Error occurs near line: " << line_counter << endl;
			Error msg = this->set_error(6);
			msg.make_second_info(info.str());
			throw msg;
		}

		//check for th !END flag
		do {
			getline(ifile, myline, '\n');
			line_counter++;
			_Hyd_Parse_IO::erase_comment(&myline);
			_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
			_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
		} while (myline.empty() == true && ifile.eof() != true);

		pos = myline.rfind("!END");
		if (pos >= 0) {
			myline.erase(pos + 4);
		}
		//no !END flag is found
		if (myline != "!END") {
			ostringstream info;
			info << "Filename " << fname << endl;
			info << "Warning occurs near line: " << line_counter << endl;
			Warning msg = this->set_warning(0);
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}
	catch (Error msg) {
		//close file
		ifile.close();
		throw msg;
	}

	//close file
	ifile.close();
}
//Transfer the CI point data to a database
void Dam_CI_System::transfer_point_members2database(QSqlDatabase *ptr_database) {
	//Set the query
	QSqlQuery query_buff(*ptr_database);

	ostringstream cout;


	//get the global index
	int glob_id = Dam_CI_Point::get_max_glob_id_point_table(ptr_database) + 1;
	//get the header for the query
	string query_header;
	query_header = Dam_CI_Point::get_insert_header_point_table(ptr_database);
	ostringstream query_data;
	ostringstream query_total;
	int counter = 0;
	string buffer_data;

	for (int i = 0; i < this->no_ci_point; i++) {
		Dam_Damage_System::check_stop_thread_flag();
		buffer_data = this->dam_ci_point[i].get_datastring_members2database(glob_id);

		if (buffer_data != label::not_set) {
			query_data << buffer_data << " ,";
			//count the global index
			glob_id++;
			counter++;
		}

		if (i % 10000 == 0 && i > 0) {
			cout << "Transfer CI-points " << i << " to " << i + 10000 << " to database..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
		//send packages of 100
		if (counter == 100) {
			query_total << query_header << query_data.str();
			//delete last komma
			string buff = query_total.str();
			buff.erase(buff.length() - 1);
			Data_Base::database_request(&query_buff, buff, ptr_database);
			//delete them
			query_total.str("");
			query_data.str("");
			counter = 0;
			if (query_buff.lastError().isValid()) {
				Warning msg = this->set_warning(1);
				ostringstream info;
				info << "Table Name                : " << Dam_CI_Point::point_table->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);
			}
		}
	}
	//send the rest
	if (counter != 0) {
		query_total << query_header << query_data.str();
		//delete last komma
		string buff = query_total.str();
		buff.erase(buff.length() - 1);
		Data_Base::database_request(&query_buff, buff, ptr_database);

		//delete them
		query_total.str("");
		query_data.str("");
		counter = 0;
		if (query_buff.lastError().isValid()) {
			Warning msg = this->set_warning(1);
			ostringstream info;
			info << "Table Name                : " << Dam_CI_Point::point_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}
}
//Input the CI point data per database: point information
void Dam_CI_System::input_perdatabase_point_data(QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output) {
	ostringstream prefix;
	ostringstream cout;
	if (with_output == true) {
		prefix << "CI> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		ostringstream cout;
		cout << "Read in the CI-point information per database..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	try {

		QSqlQueryModel elem_results;

		int number_p = 0;
		number_p = Dam_CI_Point::count_relevant_points_database(&elem_results, ptr_database, id);
		if (number_p <= 0) {
			cout << "No CI-points are found in database" << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			return;
		}

		this->no_ci_point = number_p;
		cout << this->no_ci_point << " CI-point(s) are found in database" << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		this->allocate_CI_point();

		for (int i = 0; i < this->no_ci_point; i++) {
			this->dam_ci_point[i].set_systemid(id);
		}

		int counter = 0;
		int counter2 = 0;
		//read in points
		for (int i = 0; i < this->no_ci_point; i++) {
			if (i % 10000 == 0 && i > 0) {
				Dam_Damage_System::check_stop_thread_flag();
				cout << "Input CI-points " << i << " to " << i + 10000 << "..." << endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
			}
			if (i == counter * constant::max_rows) {
				elem_results.clear();
				Dam_CI_Point::select_relevant_points_database(&elem_results, ptr_database, id, i, constant::max_rows);
				counter++;
				counter2 = 0;
			}
			this->dam_ci_point[i].input_point_perdatabase(&elem_results, counter2);
			counter2++;
		}


		
	}
	catch (Error msg) {
		if (with_output == true) {
			Sys_Common_Output::output_dam->rewind_userprefix();
		}
		throw msg;
	}


	if (with_output == true) {
		Sys_Common_Output::output_dam->rewind_userprefix();
	}
}
//Set intercepted point to the indizes of the hydraulic floodplain
void Dam_CI_System::set_intercepted_point2floodplain_id(Hyd_Model_Floodplain *fp_model) {
	ostringstream cout;
	int counter = 0;
	int id_fp_elem = 0;
	bool found_flag = false;

	int relevant_hyd_elem[9];
	//Hyd_Floodplainraster_Polygon *raster_elems=NULL;
	int end_counter = 1;

	try {
		//intercept the elements with floodplain boundary
		for (int i = 0; i < this->no_ci_point; i++) {
			found_flag = false;
			if (i % 10000 == 0 && i > 0) {
				cout << i << " (" << this->no_ci_point << ") CI-points are checked for interception with the hydraulic..." << endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
				Dam_Damage_System::check_stop_thread_flag();
			}
			counter = 0;
			//check if the mid_point is inside the floodplain boundary
			if (fp_model->raster.geometrical_bound.check_point_inside_atboundary(&this->dam_ci_point[i]) == true) {
				if (fp_model->Param_FP.get_geometrical_info().angle == 0.0) {
					int col = (int)((this->dam_ci_point[i].get_xcoordinate() - fp_model->Param_FP.get_geometrical_info().origin_global_x) / fp_model->Param_FP.get_geometrical_info().width_x);
					int row = (int)((this->dam_ci_point[i].get_ycoordinate() - fp_model->Param_FP.get_geometrical_info().origin_global_y) / fp_model->Param_FP.get_geometrical_info().width_y);
					id_fp_elem = row * fp_model->Param_FP.get_geometrical_info().number_x + col;
					if (id_fp_elem<0 || id_fp_elem>fp_model->get_number_elements() - 1) {
						continue;
					}

					if (fp_model->floodplain_elems[id_fp_elem].get_elem_type() == _hyd_elem_type::STANDARD_ELEM ||
						fp_model->floodplain_elems[id_fp_elem].get_elem_type() == _hyd_elem_type::DIKELINE_ELEM ||
						fp_model->floodplain_elems[id_fp_elem].get_elem_type() == _hyd_elem_type::RIVER_ELEM) {
						this->dam_ci_point[i].set_index_floodplain(fp_model->Param_FP.get_floodplain_number());

						this->dam_ci_point[i].set_index_floodplain_element(id_fp_elem);

					}
				}
				else {

					do {
						Dam_Damage_System::check_stop_thread_flag();

						fp_model->set_relevant_elem_indices(id_fp_elem, relevant_hyd_elem);
						for (int j = 0; j < end_counter; j++) {
							if (relevant_hyd_elem[j] < 0) {
								continue;
							}
							//search for the floodplain element
							fp_model->raster.set_geometrical_raster_polygon(relevant_hyd_elem[j]);
							if (fp_model->raster.raster_elem.check_point_outside(&this->dam_ci_point[i]) == false) {
								id_fp_elem = relevant_hyd_elem[j];
								if (fp_model->floodplain_elems[id_fp_elem].get_elem_type() == _hyd_elem_type::STANDARD_ELEM ||
									fp_model->floodplain_elems[id_fp_elem].get_elem_type() == _hyd_elem_type::DIKELINE_ELEM ||
									fp_model->floodplain_elems[id_fp_elem].get_elem_type() == _hyd_elem_type::RIVER_ELEM) {
									this->dam_ci_point[i].set_index_floodplain(fp_model->Param_FP.get_floodplain_number());

									this->dam_ci_point[i].set_index_floodplain_element(id_fp_elem);

								}
								found_flag = true;
								break;
							}
						}

						if (found_flag == true) {
							//nine neighbouring elements
							end_counter = 9;
							break;
						}

						//count the element id
						if (id_fp_elem >= fp_model->get_number_elements() - 1) {
							id_fp_elem = 0;
						}
						else {
							id_fp_elem++;
						}
						end_counter = 1;
						counter++;
					} while (counter < fp_model->get_number_elements());
				}
			}
		}
	}
	catch (Error msg) {
		throw msg;
	}




}
//Transfer data evaluated by an interception to database: identifier of the floodplain, -floodplain element
void Dam_CI_System::transfer_intercepted_data2database(QSqlDatabase *ptr_database) {
	//QSqlQuery elem_results(0,*ptr_database);
	try {
		Dam_CI_Point::set_point_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	ostringstream cout;
	cout << "Transfer new data of interception of the CI-points to database..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	ostringstream query_total;
	int counter = 0;
	//Set the query
	QSqlQuery query_buff(*ptr_database);

	for (int i = 0; i < this->no_ci_point; i++) {
		if (i % 10000 == 0 && i > 0) {
			cout << i << " (" << this->no_ci_point << ") CI-points are transfered to database..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Damage_System::check_stop_thread_flag();
		}

		if (this->dam_ci_point[i].get_string_interception_point_data2database(&query_total) == true) {
			counter++;
		};

		//send packages of 100
		if (counter == 100) {
			//delete last semikolon
			string buff = query_total.str();
			buff.erase(buff.length() - 1);
			Data_Base::database_request(&query_buff, buff, ptr_database);
			//delete them
			query_total.str("");
			counter = 0;
			if (query_buff.lastError().isValid()) {
				Warning msg = this->set_warning(2);
				ostringstream info;
				info << "Table Name                : " << Dam_CI_Point::point_table->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);
			}
		}
	}

	//send the rest
	if (counter != 0) {
		Sys_Common_Output::output_dam->output_txt(&cout);
		//delete last semicolon
		string buff = query_total.str();
		buff.erase(buff.length() - 1);
		Data_Base::database_request(&query_buff, buff, ptr_database);
		//delete them
		query_total.str("");
		counter = 0;
		if (query_buff.lastError().isValid()) {
			Warning msg = this->set_warning(2);
			ostringstream info;
			info << "Table Name                : " << Dam_CI_Point::point_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}
	query_buff.clear();
	cout << "Transfer new data of interception of the CI-points to database is finished" << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

}
//Output point member
void Dam_CI_System::output_point_member(void) {
	//output of the points
	ostringstream cout;
	cout << "CI-POINTS" << endl;
	cout << " Number points     : " << W(13) << this->no_ci_point << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	if (this->no_ci_point > 0) {
		cout << W(3) << "No. " << W(8) << "x" << label::m << W(8) << "y" << label::m << W(8) << "Sec_Id" << label::no_unit << W(8) << "Sec_Name" << label::no_unit << W(8) << "Boundary" << label::m << W(8) << "Name" << label::no_unit << W(8) << "Sec_Level" << label::no_unit << W(8) << "Recovery_time" << label::day << W(8) << "End_Flag" << label::no_unit << W(8) << "Regular_flag" << label::no_unit << W(8) << "Activation_time" << label::day << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for (int i = 0; i < this->no_ci_point; i++) {
			cout << W(3) << this->dam_ci_point[i].get_number() << W(12) << P(2) << FORMAT_FIXED_REAL << this->dam_ci_point[i].get_xcoordinate();
			cout << W(13) << P(2) << FORMAT_FIXED_REAL << this->dam_ci_point[i].get_ycoordinate();
			cout << W(13) << P(0) << FORMAT_FIXED_REAL << this->dam_ci_point[i].get_sector_id();
			cout << W(23) << P(0) << FORMAT_FIXED_REAL << this->dam_ci_point[i].get_sector_name();
			cout << W(13) << P(2) << FORMAT_FIXED_REAL << this->dam_ci_point[i].get_boundary_value();
			cout << W(33) << P(0) << FORMAT_FIXED_REAL << this->dam_ci_point[i].get_point_name();
			cout << W(13) << P(0) << FORMAT_FIXED_REAL << this->dam_ci_point[i].get_sec_level();
			cout << W(13) << P(2) << FORMAT_FIXED_REAL << this->dam_ci_point[i].get_recovery_time();
			cout << W(13) << P(0) << FORMAT_FIXED_REAL << functions::convert_boolean2string(this->dam_ci_point[i].get_end_level_flag());	
			cout << W(13) << P(0) << FORMAT_FIXED_REAL << functions::convert_boolean2string(this->dam_ci_point[i].get_regular_flag());
			cout << W(13) << P(2) << FORMAT_FIXED_REAL << this->dam_ci_point[i].get_activation_time() << endl;
			

			Sys_Common_Output::output_dam->output_txt(&cout);
		}
	}

}
//Sum up the total point damage results for a given system-id and scenario (boundary-, break-) from the database
void Dam_CI_System::sum_total_point_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz) {


	QSqlQueryModel results;
	int sec_id_buff = 0;
	int sec_level_buff = 0;
	bool reg_flag_buff = true;
	double failure_dur_buff = 0.0;
	_dam_ci_failure_type fail_type_buff = _dam_ci_failure_type::no_failure;

	
	Dam_CI_Point::select_data_in_erg_table(&results, ptr_database, id, bound_sz, break_sz);
	for (int i = 0; i < results.rowCount(); i++) {
		sec_id_buff = results.record(i).value((Dam_CI_Point::point_erg_table->get_column_name(dam_label::sector_id)).c_str()).toInt();
		sec_level_buff = results.record(i).value((Dam_CI_Point::point_erg_table->get_column_name(dam_label::sector_level)).c_str()).toInt();
		failure_dur_buff = results.record(i).value((Dam_CI_Point::point_erg_table->get_column_name(dam_label::failure_duration)).c_str()).toDouble();
		reg_flag_buff= results.record(i).value((Dam_CI_Point::point_erg_table->get_column_name(dam_label::regular_flag)).c_str()).toBool();
		fail_type_buff = _Dam_CI_Element::convert_txt2failuretype(results.record(i).value((Dam_CI_Point::point_erg_table->get_column_name(dam_label::failure_type)).c_str()).toString().toStdString());
	
		if (reg_flag_buff == true) {
			//per failure type
			if (fail_type_buff == _dam_ci_failure_type::direct) {
				if (this->list_direct_failure_sec.count() == 0) {
					//start
					QList<int> buff;
					buff.append(sec_id_buff);
					buff.append(1);
					this->list_direct_failure_sec.append(buff);
				}
				else {
					bool found = false;
					for (int j = 0; j < this->list_direct_failure_sec.count(); j++) {
						if (sec_id_buff == this->list_direct_failure_sec[j].at(0)) {
							//add to existing
							int no = 0;
							no = this->list_direct_failure_sec[j].at(1);
							this->list_direct_failure_sec[j].replace(1,no+1);
							found = true;
							break;
						}
					}
					if (found == false) {
						//new list
						QList<int> buff;
						buff.append(sec_id_buff);
						buff.append(1);
						this->list_direct_failure_sec.append(buff);
					}
				}
			}
			else if (fail_type_buff == _dam_ci_failure_type::sectoral) {
				if (this->list_sectoral_failure_sec.count() == 0) {
					//start
					QList<int> buff;
					buff.append(sec_id_buff);
					buff.append(1);
					this->list_sectoral_failure_sec.append(buff);
				}
				else {
					bool found = false;
					for (int j = 0; j < this->list_sectoral_failure_sec.count(); j++) {
						if (sec_id_buff == this->list_sectoral_failure_sec[j].at(0)) {
							//add to existing
							int no = 0;
							no = this->list_sectoral_failure_sec[j].at(1);
							this->list_sectoral_failure_sec[j].replace(1, no + 1);
							found = true;
							break;
						}
					}
					if (found == false) {
						//new list
						QList<int> buff;
						buff.append(sec_id_buff);
						buff.append(1);
						this->list_sectoral_failure_sec.append(buff);
					}
				}
			}
			else if (fail_type_buff == _dam_ci_failure_type::transsectoral) {
				if (this->list_transsectoral_failure_sec.count() == 0) {
					//start
					QList<int> buff;
					buff.append(sec_id_buff);
					buff.append(1);
					this->list_transsectoral_failure_sec.append(buff);
				}
				else {
					bool found = false;
					for (int j = 0; j < this->list_transsectoral_failure_sec.count(); j++) {
						if (sec_id_buff == this->list_transsectoral_failure_sec[j].at(0)) {
							//add to existing
							int no = 0;
							no = this->list_transsectoral_failure_sec[j].at(1);
							this->list_transsectoral_failure_sec[j].replace(1, no + 1);
							found = true;
							break;
						}
					}
					if (found == false) {
						//new list
						QList<int> buff;
						buff.append(sec_id_buff);
						buff.append(1);
						this->list_transsectoral_failure_sec.append(buff);
					}
				}

			}


			//total failure per sector
			if (this->list_total_failure_sec.count() == 0) {
				//start
				QList<int> buff;
				buff.append(sec_id_buff);
				buff.append(1);
				this->list_total_failure_sec.append(buff);
			}
			else {
				bool found = false;
				for (int j = 0; j < this->list_total_failure_sec.count(); j++) {
					if (sec_id_buff == this->list_total_failure_sec[j].at(0)) {
						//add to existing
						int no = 0;
						no = this->list_total_failure_sec[j].at(1);
						this->list_total_failure_sec[j].replace(1, no + 1);
						found = true;
						break;
					}
				}
				if (found == false) {
					//new list
					QList<int> buff;
					buff.append(sec_id_buff);
					buff.append(1);
					this->list_total_failure_sec.append(buff);
				}
			}

			//total failure x level per sector
			if (this->list_total_failure_level_sec.count() == 0) {
				//start
				QList<QVariant> buff;
				buff.append(sec_id_buff);
				buff.append(sec_level_buff*failure_dur_buff);
				this->list_total_failure_level_sec.append(buff);
			}
			else {
				bool found = false;
				for (int j = 0; j < this->list_total_failure_level_sec.count(); j++) {
					if (sec_id_buff == this->list_total_failure_level_sec[j].at(0)) {
						//add to existing
						double value = 0.0;
						value = this->list_total_failure_level_sec[j].at(1).toDouble();
						this->list_total_failure_level_sec[j].replace(1, value + sec_level_buff * failure_dur_buff);
						found = true;
						break;
					}
				}
				if (found == false) {
					//new list
					QList<QVariant> buff;
					buff.append(sec_id_buff);
					buff.append(sec_level_buff * failure_dur_buff);
					this->list_total_failure_level_sec.append(buff);
				}
			}
		}
		else{


			//emergency activated
			if (fail_type_buff == _dam_ci_failure_type::direct) {
				//Add to normal list
				//per failure type
				if (fail_type_buff == _dam_ci_failure_type::direct || fail_type_buff == _dam_ci_failure_type::direct_activ) {
					if (this->list_direct_failure_sec.count() == 0) {
						//start
						QList<int> buff;
						buff.append(sec_id_buff);
						buff.append(1);
						this->list_direct_failure_sec.append(buff);
					}
					else {
						bool found = false;
						for (int j = 0; j < this->list_direct_failure_sec.count(); j++) {
							if (sec_id_buff == this->list_direct_failure_sec[j].at(0)) {
								//add to existing
								int no = 0;
								no = this->list_direct_failure_sec[j].at(1);
								this->list_direct_failure_sec[j].replace(1, no + 1);
								found = true;
								break;
							}
						}
						if (found == false) {
							//new list
							QList<int> buff;
							buff.append(sec_id_buff);
							buff.append(1);
							this->list_direct_failure_sec.append(buff);
						}
					}
				}


			}
			if(fail_type_buff == _dam_ci_failure_type::direct_activ || fail_type_buff == _dam_ci_failure_type::no_failure){
				if (this->list_emergency_active_sec.count() == 0) {
					//start
					QList<int> buff;
					buff.append(sec_id_buff);
					buff.append(1);
					this->list_emergency_active_sec.append(buff);
				}
				else {
					bool found = false;
					for (int j = 0; j < this->list_emergency_active_sec.count(); j++) {
						if (sec_id_buff == this->list_emergency_active_sec[j].at(0)) {
							//add to existing
							int no = 0;
							no = this->list_emergency_active_sec[j].at(1);
							this->list_emergency_active_sec[j].replace(1, no + 1);
							found = true;
							break;
						}
					}
					if (found == false) {
						//new list
						QList<int> buff;
						buff.append(sec_id_buff);
						buff.append(1);
						this->list_emergency_active_sec.append(buff);
					}
				}
			}
			

		}
	
	
	}


	//sum up
	for (int i = 0; i < this->list_direct_failure_sec.count(); i++) {
		this->tot_direct_failure = this->tot_direct_failure + this->list_direct_failure_sec[i].at(1);
	}
	for (int i = 0; i < this->list_sectoral_failure_sec.count(); i++) {
		this->tot_sectoral_failure = this->tot_sectoral_failure + this->list_sectoral_failure_sec[i].at(1);
	}
	for (int i = 0; i < this->list_transsectoral_failure_sec.count(); i++) {
		this->tot_transsectoral_failure = this->tot_transsectoral_failure + this->list_transsectoral_failure_sec[i].at(1);
	}
	for (int i = 0; i < this->list_emergency_active_sec.count(); i++) {
		this->tot_emergency_active = this->tot_emergency_active + this->list_emergency_active_sec[i].at(1);
	}


}
//Read in the CI polygon data from file
void Dam_CI_System::read_polygon_per_file(string fname) {
	ifstream ifile;
	//open file
	ifile.open(fname.c_str(), ios_base::in);
	if (ifile.is_open() == false) {
		Error msg = this->set_error(16);
		ostringstream info;
		info << "Filename: " << fname << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	int line_counter = 0;
	string myline;
	int pos = -1;
	stringstream my_stream;
	int found_polygons = 0;

	try {
		//search for the begin
		do {
			getline(ifile, myline, '\n');
			line_counter++;
			_Hyd_Parse_IO::erase_comment(&myline);
			_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
			_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
			if (myline.empty()==false) {
				//read it in
				my_stream << myline;
				my_stream >> found_polygons;
				if (my_stream.fail() == true) {
					ostringstream info;
					info << "Wrong input sequenze  : " << my_stream.str() << endl;
					info << "Filename              : " << fname << endl;
					info << "Input must be         :  number_polygons" << endl;
					info << "Error occurs near line: " << line_counter << endl;
					Error msg = this->set_error(9);
					msg.make_second_info(info.str());
					throw msg;
				}
				break;
				
			}
		} while (ifile.eof() != true);

		if (ifile.eof() == true) {
			ostringstream info;
			info << "Filename              : " << fname << endl;
			info << "Error occurs near line: " << line_counter << endl;
			Error msg = this->set_error(10);
			msg.make_second_info(info.str());
			throw msg;
		}

		this->no_ci_polygon = found_polygons;
		found_polygons = 0;
		this->allocate_CI_polygon();


		for (int i = 0; i < this->no_ci_polygon; i++) {
			my_stream.clear();
			my_stream.str("");
			//search for the begin of polygon
			do {
				getline(ifile, myline, '\n');
				line_counter++;
				_Hyd_Parse_IO::erase_comment(&myline);
				_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
				_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
				pos = myline.rfind("!BEGIN");
				if (pos >= 0) {
					myline.erase(pos + 6);
				}
			} while (myline != "!BEGIN" && ifile.eof() != true);

			if (ifile.eof() == true) {
				ostringstream info;
				info << "Filename              : " << fname << endl;
				info << "Error occurs near line: " << line_counter << endl;
				Error msg = this->set_error(10);
				msg.make_second_info(info.str());
				throw msg;
			}

			do {
				//next line for the curve number and number of points
				getline(ifile, myline, '\n');
				line_counter++;
				_Hyd_Parse_IO::erase_comment(&myline);
				_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
				_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
			} while (myline.empty() == true && ifile.eof() != true);

			if (ifile.eof() == true) {
				ostringstream info;
				info << "Filename              : " << fname << endl;
				info << "Error occurs near line: " << line_counter << endl;
				Error msg = this->set_error(10);
				msg.make_second_info(info.str());
				throw msg;
			}

			double x_buffer = 0.0;
			double y_buffer = 0.0;
			double enduser = 0.0;
			int buff_sector_id = 0;
			int buff_no_point = 0;
			string name_buff = label::not_set;
			int col = 0;
			int buff_number = 0;
			//read it in
			col = functions::count_number_columns(myline);
			my_stream << myline;

			if (col == 5) {
				my_stream >> buff_number >> buff_no_point >> name_buff >> buff_sector_id >> enduser;
			}
			else {
				//Error
				Error msg = this->set_error(11);
				ostringstream info;
				info << "Filename              : " << fname << endl;
				info << "Error occurs near line: " << line_counter << endl;
				info << "Polygon id              : " << i + 1 << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			if (my_stream.fail() == true) {
				ostringstream info;
				info << "Wrong input sequenze  : " << my_stream.str() << endl;
				info << "Filename              : " << fname << endl;
				info << "Error occurs near line: " << line_counter << endl;
				info << "Polygon id           : " << i + 1 << endl;
				Error msg = this->set_error(15);
				msg.make_second_info(info.str());
				throw msg;
			}
			my_stream.clear();
			my_stream.str("");

			try {
				//set the polygon buffer
				this->dam_ci_polygon[i].set_number_points(buff_no_point);
				this->dam_ci_polygon[i].set_members(buff_sector_id, enduser, name_buff, buff_number);
			}
			catch (Error msg) {
				ostringstream info;
				info << "Filename              : " << fname << endl;
				info << "Error occurs near line: " << line_counter << endl;
				info << "Polygon id            : " << i + 1 << endl;
				msg.make_second_info(info.str());
				throw msg;
			}

			my_stream.clear();
			my_stream.str("");

			int found_points = 0;
			Geo_Point_List buff_list;
			//read in the points
			do {
				
				//delete the comments
				getline(ifile, myline, '\n');
				line_counter++;
				_Hyd_Parse_IO::erase_comment(&myline);
				_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
				_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
				if (myline.empty() != true) {
					pos = myline.rfind("!END");
					if (pos >= 0) {
						myline.erase(pos + 4);
					}
					//stop when end is reach
					if (myline == "!END" || ifile.eof() == true) {
						break;
					}

					col = functions::count_number_columns(myline);
					my_stream << myline;

					if (col == 2) {
						my_stream >> x_buffer >> y_buffer;
					}
					else {
						//Error
						Error msg = this->set_error(14);
						ostringstream info;
						info << "Filename              : " << fname << endl;
						info << "Error occurs near line: " << line_counter << endl;
						info << "Polygon id              : " << i + 1 << endl;
						msg.make_second_info(info.str());
						throw msg;
					}
					if (my_stream.fail() == true) {
						ostringstream info;
						info << "Wrong input sequenze  : " << my_stream.str() << endl;
						info << "Filename              : " << fname << endl;
						info << "Error occurs near line: " << line_counter << endl;
						info << "Polygon id           : " << i + 1 << endl;
						Error msg = this->set_error(13);
						msg.make_second_info(info.str());
						throw msg;
					}
					my_stream.clear();
					my_stream.str("");

					try {
						
						Geo_Point buff_point;
						buff_point.set_point_coordinate(x_buffer, y_buffer);
						//set the polygon buffer
						buff_list.set_new_point(&buff_point);

					}
					catch (Error msg) {
						ostringstream info;
						info << "Filename              : " << fname << endl;
						info << "Error occurs near line: " << line_counter << endl;
						info << "Polygon id            : " << i + 1 << endl;
						msg.make_second_info(info.str());
						throw msg;
					}


					
					found_points++;
				}
			} while (found_points< this->dam_ci_polygon[i].get_number_points() && ifile.eof() != true);

			//check if all is read
			if (found_points != this->dam_ci_polygon[i].get_number_points() && ifile.eof() != true) {
				ostringstream info;
				info << "Filename              : " << fname << endl;
				info << "Error occurs near line: " << line_counter << endl;
				Error msg = this->set_error(12);
				msg.make_second_info(info.str());
				throw msg;
			}
			try {
				this->dam_ci_polygon[i].set_points(&buff_list);
			}
			catch (Error msg) {
				ostringstream info;
				info << "Filename              : " << fname << endl;
				info << "Error occurs near line: " << line_counter << endl;
				info << "Polygon id            : " << i + 1 << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			this->dam_ci_polygon[i].set_polygon_mid2mid_point();

			//check for th !END flag
			do {
				getline(ifile, myline, '\n');
				line_counter++;
				_Hyd_Parse_IO::erase_comment(&myline);
				_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
				_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
			} while (myline.empty() == true && ifile.eof() != true);

			pos = myline.rfind("!END");
			if (pos >= 0) {
				myline.erase(pos + 4);
			}
			//no !END flag is found
			if (myline != "!END") {
				ostringstream info;
				info << "Filename " << fname << endl;
				info << "Warning occurs near line: " << line_counter << endl;
				Warning msg = this->set_warning(2);
				msg.make_second_info(info.str());
				msg.output_msg(4);
			}
		}
	}
	catch (Error msg) {
		//close file
		ifile.close();
		throw msg;
	}

	//close file
	ifile.close();
}
//Transfer the CI polygon data to a database
void Dam_CI_System::transfer_polygon_members2database(QSqlDatabase *ptr_database) {
	//Set the query
	QSqlQuery query_buff(*ptr_database);

	ostringstream cout;


	//get the global index
	int glob_id = Dam_CI_Polygon::get_max_glob_id_polygon_table(ptr_database) + 1;
	int glob_id_point = glob_id;
	//get the header for the query
	string query_header;
	query_header = Dam_CI_Polygon::get_insert_header_polygon_table(ptr_database);
	ostringstream query_data;
	ostringstream query_total;
	int counter = 0;
	string buffer_data;

	for (int i = 0; i < this->no_ci_polygon; i++) {
		Dam_Damage_System::check_stop_thread_flag();
		buffer_data = this->dam_ci_polygon[i].get_datastring_members2database(glob_id);

		if (buffer_data != label::not_set) {
			query_data << buffer_data << " ,";
			//count the global index
			glob_id++;
			counter++;
		}

		if (i % 10000 == 0 && i > 0) {
			cout << "Transfer CI-polygon " << i << " to " << i + 10000 << " to database..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
		//send packages of 100
		if (counter == 100) {
			query_total << query_header << query_data.str();
			//delete last komma
			string buff = query_total.str();
			buff.erase(buff.length() - 1);
			Data_Base::database_request(&query_buff, buff, ptr_database);
			//delete them
			query_total.str("");
			query_data.str("");
			counter = 0;
			if (query_buff.lastError().isValid()) {
				Warning msg = this->set_warning(1);
				ostringstream info;
				info << "Table Name                : " << Dam_CI_Polygon::polygon_table->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);
			}
		}
	}
	//send the rest
	if (counter != 0) {
		query_total << query_header << query_data.str();
		//delete last komma
		string buff = query_total.str();
		buff.erase(buff.length() - 1);
		Data_Base::database_request(&query_buff, buff, ptr_database);

		//delete them
		query_total.str("");
		query_data.str("");
		counter = 0;
		if (query_buff.lastError().isValid()) {
			Warning msg = this->set_warning(1);
			ostringstream info;
			info << "Table Name                : " << Dam_CI_Polygon::polygon_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}

	//Transfer the points
	for (int i = 0; i < this->no_ci_polygon; i++) {
		this->dam_ci_polygon[i].transfer_polygon_point2database_table(ptr_database, glob_id_point);
		glob_id_point++;
	}
}
//Input the CI point data per database: polygon information
void Dam_CI_System::input_perdatabase_polygon_data(QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output) {
	ostringstream prefix;
	ostringstream cout;
	if (with_output == true) {
		prefix << "CI> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		ostringstream cout;
		cout << "Read in the CI-polygon information per database..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	try {

		QSqlQueryModel elem_results;

		int number_p = 0;
		number_p = Dam_CI_Polygon::count_relevant_polygon_database(&elem_results, ptr_database, id);
		if (number_p <= 0) {
			cout << "No CI-polygon are found in database" << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			return;
		}

		this->no_ci_polygon = number_p;
		cout << this->no_ci_polygon << " CI-polygon(s) are found in database" << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		this->allocate_CI_polygon();

		for (int i = 0; i < this->no_ci_polygon; i++) {
			this->dam_ci_polygon[i].set_systemid(id);
		}

		int counter = 0;
		int counter2 = 0;
		//read in points
		for (int i = 0; i < this->no_ci_polygon; i++) {
			if (i % 10000 == 0 && i > 0) {
				Dam_Damage_System::check_stop_thread_flag();
				cout << "Input CI-polygon " << i << " to " << i + 10000 << "..." << endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
			}
			if (i == counter * constant::max_rows) {
				elem_results.clear();
				Dam_CI_Polygon::select_relevant_polygon_database(&elem_results, ptr_database, id, i, constant::max_rows);
				counter++;
				counter2 = 0;
			}
			this->dam_ci_polygon[i].input_polygon_perdatabase(&elem_results, counter2);
			counter2++;
		}



	}
	catch (Error msg) {
		if (with_output == true) {
			Sys_Common_Output::output_dam->rewind_userprefix();
		}
		throw msg;
	}


	if (with_output == true) {
		Sys_Common_Output::output_dam->rewind_userprefix();
	}

}
//Output polygon member
void Dam_CI_System::output_polygon_member(void) {
	//output of the points
	ostringstream cout;
	cout << "CI-POLYGON" << endl;
	cout << " Number polygons    : " << W(13) << this->no_ci_polygon << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	cout << W(3) << "No. " << W(8) << "Sec_Id" << label::no_unit << W(8) << "Sec_Name" << label::no_unit << W(8) << "Name" << label::no_unit << W(8) << "Enduser" << label::unit_variable << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	for (int i = 0; i < this->no_ci_polygon; i++) {
		cout << W(3) << this->dam_ci_polygon[i].mid_point.get_number();
		cout << W(13) << P(0) << FORMAT_FIXED_REAL << this->dam_ci_polygon[i].get_sector_id();
		cout << W(23) << P(0) << FORMAT_FIXED_REAL << this->dam_ci_polygon[i].get_sector_name();
		cout << W(33) << P(0) << FORMAT_FIXED_REAL << this->dam_ci_polygon[i].mid_point.get_point_name();
		cout << W(13) << P(2) << FORMAT_FIXED_REAL << this->dam_ci_polygon[i].get_enduser() <<endl;
		this->dam_ci_polygon[i].output_point(&cout);
		Sys_Common_Output::output_dam->output_txt(&cout);
	}
}
//Sum up the total polygon damage results for a given system-id and scenario (boundary-, break-) from the database
void Dam_CI_System::sum_total_polygon_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz) {

	QSqlQueryModel results;
	double buff_enduser = 0.0;
	int sec_id_buff = 0;
	double buff_enduser_duration = 0.0;

	Dam_CI_Polygon::select_data_in_erg_table(&results, ptr_database, id, bound_sz, break_sz);
	for (int i = 0; i < results.rowCount(); i++) {

		sec_id_buff = results.record(i).value((Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::sector_id)).c_str()).toInt();
		buff_enduser_duration = results.record(i).value((Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::enduser_duration)).c_str()).toDouble();
		buff_enduser = results.record(i).value((Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::end_user)).c_str()).toDouble();


		//total enduser per sector
		if (this->list_enduser_affected_sec.count() == 0) {
			//start
			QList<QVariant> buff;
			buff.append(sec_id_buff);
			buff.append(buff_enduser);
			this->list_enduser_affected_sec.append(buff);
		}
		else {
			bool found = false;
			for (int j = 0; j < this->list_enduser_affected_sec.count(); j++) {
				if (sec_id_buff == this->list_enduser_affected_sec[j].at(0)) {
					//add to existing
					double value = 0.0;
					value = this->list_enduser_affected_sec[j].at(1).toDouble();
					this->list_enduser_affected_sec[j].replace(1, value + buff_enduser);
					found = true;
					break;
				}
			}
			if (found == false) {
				//new list
				QList<QVariant> buff;
				buff.append(sec_id_buff);
				buff.append(buff_enduser);
				this->list_enduser_affected_sec.append(buff);
			}
		}
		//total enduser x duration per sector
		if (this->list_enduser_affected_duration_sec.count() == 0) {
			//start
			QList<QVariant> buff;
			buff.append(sec_id_buff);
			buff.append(buff_enduser_duration);
			this->list_enduser_affected_duration_sec.append(buff);
		}
		else {
			bool found = false;
			for (int j = 0; j < this->list_enduser_affected_duration_sec.count(); j++) {
				if (sec_id_buff == this->list_enduser_affected_duration_sec[j].at(0)) {
					//add to existing
					double value = 0.0;
					value = this->list_enduser_affected_duration_sec[j].at(1).toDouble();
					this->list_enduser_affected_duration_sec[j].replace(1, value + buff_enduser_duration);
					found = true;
					break;
				}
			}
			if (found == false) {
				//new list
				QList<QVariant> buff;
				buff.append(sec_id_buff);
				buff.append(buff_enduser_duration);
				this->list_enduser_affected_duration_sec.append(buff);
			}
		}


	}
	




}
//Read in the CI connection data from file
void Dam_CI_System::read_connection_per_file(string fname) {
	ifstream ifile;
	//open file
	ifile.open(fname.c_str(), ios_base::in);
	if (ifile.is_open() == false) {
		Error msg = this->set_error(17);
		ostringstream info;
		info << "Filename: " << fname << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	int line_counter = 0;
	string myline;
	int pos = -1;
	stringstream my_stream;
	int found_connection = 0;

	try {
		//search for the begin
		do {
			getline(ifile, myline, '\n');
			line_counter++;
			_Hyd_Parse_IO::erase_comment(&myline);
			_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
			_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
			pos = myline.rfind("!BEGIN");
			if (pos >= 0) {
				myline.erase(pos + 6);
			}
		} while (myline != "!BEGIN" && ifile.eof() != true);

		if (ifile.eof() == true) {
			ostringstream info;
			info << "Filename              : " << fname << endl;
			info << "Error occurs near line: " << line_counter << endl;
			Error msg = this->set_error(18);
			msg.make_second_info(info.str());
			throw msg;
		}

		do {
			//next line for the curve number and number of points
			getline(ifile, myline, '\n');
			line_counter++;
			_Hyd_Parse_IO::erase_comment(&myline);
			_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
			_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
		} while (myline.empty() == true && ifile.eof() != true);

		if (ifile.eof() == true) {
			ostringstream info;
			info << "Filename              : " << fname << endl;
			info << "Error occurs near line: " << line_counter << endl;
			Error msg = this->set_error(18);
			msg.make_second_info(info.str());
			throw msg;
		}

		//read it in
		my_stream << myline;
		my_stream >> found_connection;

		if (my_stream.fail() == true) {
			ostringstream info;
			info << "Wrong input sequenze  : " << my_stream.str() << endl;
			info << "Filename              : " << fname << endl;
			info << "Input must be         :  number_connections" << endl;
			info << "Error occurs near line: " << line_counter << endl;
			Error msg = this->set_error(19);
			msg.make_second_info(info.str());
			throw msg;
		}

		my_stream.clear();
		my_stream.str("");

		int counter = 0;
		int col = 0;
		int id_in = 0;
		string point_in = label::not_defined;
		int id_out = 0;
		string point_out = label::not_defined;
		int buff_number = 0;

		

		//read in the connections
		do {

			//delete the comments
			getline(ifile, myline, '\n');
			line_counter++;
			_Hyd_Parse_IO::erase_comment(&myline);
			_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
			_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
			if (myline.empty() != true) {
				pos = myline.rfind("!END");
				if (pos >= 0) {
					myline.erase(pos + 4);
				}
				//stop when end is reach
				if (myline == "!END" || ifile.eof() == true) {
					break;
				}
				//read it in
				col = functions::count_number_columns(myline);
				my_stream << myline;



				try {
					if (col == 5) {
						my_stream >> buff_number >> id_in >> point_in >> id_out >> point_out;
					}
					else {
						//Error
						Error msg = this->set_error(20);
						throw msg;
					}
				}
				catch (Error msg) {
					ostringstream info;
					info << "Filename              : " << fname << endl;
					info << "Error occurs near line: " << line_counter << endl;
					info << "Point id              : " << counter + 1 << endl;
					msg.make_second_info(info.str());
					throw msg;
				}

				if (my_stream.fail() == true) {
					ostringstream info;
					info << "Wrong input sequenze  : " << my_stream.str() << endl;
					info << "Filename              : " << fname << endl;
					info << "Error occurs near line: " << line_counter << endl;
					info << "Point id              : " << counter + 1 << endl;
					Error msg = this->set_error(20);
					msg.make_second_info(info.str());
					throw msg;
				}
				my_stream.clear();
				my_stream.str("");
				try {
					//set the point buffer
					QList<int> list_buff;
					list_buff.append(id_in);
					point_in=functions::convert_string2lower_case(point_in);
					if (point_in=="point") {
						list_buff.append(0);
					}
					else if(point_in == "polygon") {
						list_buff.append(1);
					}
					else {
						ostringstream info;
						info << "String found          : " << point_in << endl;
						Error msg = this->set_error(22);
						msg.make_second_info(info.str());
						throw msg;
					}

					list_buff.append(id_out);
					point_out = functions::convert_string2lower_case(point_out);
					if (point_out== "point") {
						list_buff.append(0);
					}
					else if (point_out == "polygon") {
						list_buff.append(1);
					}
					else {
						ostringstream info;
						info << "String found          : " << point_out << endl;
						Error msg = this->set_error(22);
						msg.make_second_info(info.str());
						throw msg;

					}
					this->dam_ci_connection.elem_list.append(list_buff);
					this->dam_ci_connection.check_members();
				}
				catch (Error msg) {
					ostringstream info;
					info << "Filename              : " << fname << endl;
					info << "Error occurs near line: " << line_counter << endl;
					info << "Connection id         : " << counter + 1 << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
				counter++;
			}
		} while (counter < found_connection && ifile.eof() != true);

		//check if all is read
		if (counter != found_connection && ifile.eof() != true) {
			ostringstream info;
			info << "Filename              : " << fname << endl;
			info << "Error occurs near line: " << line_counter << endl;
			Error msg = this->set_error(21);
			msg.make_second_info(info.str());
			throw msg;
		}

		//check for th !END flag
		do {
			getline(ifile, myline, '\n');
			line_counter++;
			_Hyd_Parse_IO::erase_comment(&myline);
			_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
			_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
		} while (myline.empty() == true && ifile.eof() != true);

		pos = myline.rfind("!END");
		if (pos >= 0) {
			myline.erase(pos + 4);
		}
		//no !END flag is found
		if (myline != "!END") {
			ostringstream info;
			info << "Filename " << fname << endl;
			info << "Warning occurs near line: " << line_counter << endl;
			Warning msg = this->set_warning(4);
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}
	catch (Error msg) {
		//close file
		ifile.close();
		throw msg;
	}

	//close file
	ifile.close();
}
//Transfer the CI connection data to a database
void Dam_CI_System::transfer_connection_members2database(QSqlDatabase *ptr_database) {
	//Set the query
	QSqlQuery query_buff(*ptr_database);

	ostringstream cout;


	//get the global index
	int glob_id = Dam_CI_Element_List::get_max_glob_id_connection_table(ptr_database) + 1;
	//get the header for the query
	string query_header;
	query_header = Dam_CI_Element_List::get_insert_header_connection_table(ptr_database);
	ostringstream query_data;
	ostringstream query_total;
	int counter = 0;
	string buffer_data;

	for (int i = 0; i < this->dam_ci_connection.get_number_connection(); i++) {
		Dam_Damage_System::check_stop_thread_flag();

		Geo_Point buff_in;
		Geo_Point buff_out;
		_Dam_CI_Element elem_in;
		_Dam_CI_Element elem_out;
		//set it!
		buff_in = this->find_mid_point_CI_element(this->dam_ci_connection.elem_list.at(i).at(0), this->dam_ci_connection.elem_list.at(i).at(1));
		buff_out = this->find_mid_point_CI_element(this->dam_ci_connection.elem_list.at(i).at(2), this->dam_ci_connection.elem_list.at(i).at(3));

		elem_in =this->find_CI_element(this->dam_ci_connection.elem_list.at(i).at(0), this->dam_ci_connection.elem_list.at(i).at(1));
		elem_out = this->find_CI_element(this->dam_ci_connection.elem_list.at(i).at(2), this->dam_ci_connection.elem_list.at(i).at(3));
		bool transsec_flag = false;
		if (elem_in.get_sector_id() != elem_out.get_sector_id()) {
			transsec_flag = true;
		}

		buffer_data = this->dam_ci_connection.get_datastring_members2database(glob_id,i,&buff_in, &buff_out, elem_in.get_sector_id(), elem_in.get_sector_name(), transsec_flag);

		if (buffer_data != label::not_set) {
			query_data << buffer_data << " ,";
			//count the global index
			glob_id++;
			counter++;
		}

		if (i % 10000 == 0 && i > 0) {
			cout << "Transfer CI-connection " << i << " to " << i + 10000 << " to database..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
		//send packages of 100
		if (counter == 100) {
			query_total << query_header << query_data.str();
			//delete last komma
			string buff = query_total.str();
			buff.erase(buff.length() - 1);
			Data_Base::database_request(&query_buff, buff, ptr_database);
			//delete them
			query_total.str("");
			query_data.str("");
			counter = 0;
			if (query_buff.lastError().isValid()) {
				Warning msg = this->set_warning(1);
				ostringstream info;
				info << "Table Name                : " << Dam_CI_Element_List::connection_table->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);
			}
		}
	}
	//send the rest
	if (counter != 0) {
		query_total << query_header << query_data.str();
		//delete last komma
		string buff = query_total.str();
		buff.erase(buff.length() - 1);
		Data_Base::database_request(&query_buff, buff, ptr_database);

		//delete them
		query_total.str("");
		query_data.str("");
		counter = 0;
		if (query_buff.lastError().isValid()) {
			Warning msg = this->set_warning(8);
			ostringstream info;
			info << "Table Name                : " << Dam_CI_Element_List::connection_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}
}
//Input the CI connection data per database: polygon information
void Dam_CI_System::input_perdatabase_connection_data(QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output) {
	this->dam_ci_connection.elem_list.clear();
	ostringstream prefix;
	ostringstream cout;
	if (with_output == true) {
		prefix << "CI> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		ostringstream cout;
		cout << "Read in the CI-connection information per database..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	try {

		QSqlQueryModel elem_results;

		int number_p = 0;
		number_p = Dam_CI_Element_List::count_relevant_connection_database(&elem_results, ptr_database, id);
		if (number_p <= 0) {
			cout << "No CI-connections are found in database" << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			return;
		}

		
		cout << number_p << " CI-connection(s) are found in database" << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		
		this->dam_ci_connection.set_systemid(id);

		int counter = 0;
		int counter2 = 0;
		//read in points
		for (int i = 0; i < number_p; i++) {
			if (i % 10000 == 0 && i > 0) {
				Dam_Damage_System::check_stop_thread_flag();
				cout << "Input CI-connection " << i << " to " << i + 10000 << "..." << endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
			}
			if (i == counter * constant::max_rows) {
				elem_results.clear();
				Dam_CI_Element_List::select_relevant_connection_database(&elem_results, ptr_database, id, i, constant::max_rows);
				counter++;
				counter2 = 0;
			}

			//input
			this->dam_ci_connection.input_connection_perdatabase(&elem_results, counter2);
			counter2++;
		}



	}
	catch (Error msg) {
		if (with_output == true) {
			Sys_Common_Output::output_dam->rewind_userprefix();
		}
		throw msg;
	}


	if (with_output == true) {
		Sys_Common_Output::output_dam->rewind_userprefix();
	}




}
//Output polygon member
void Dam_CI_System::output_connection_member(void) {
	//output of the points
	ostringstream cout;
	cout << "CI-CONNECTION" << endl;
	cout << " Number connection    : " << W(13) << this->dam_ci_connection.get_number_connection()<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	cout << W(3) << "No. " << W(8) << "ID_in" << label::no_unit << W(8) << "Point_flag_(0:=point_1:=polygon)" << label::no_unit << W(15) << "ID_out" << label::no_unit << W(8) << "Point_flag_(0:=point_1:=polygon)" << label::no_unit << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	this->dam_ci_connection.output_connection(&cout);
	Sys_Common_Output::output_dam->output_txt(&cout);


}
//Allocate the CI points
void Dam_CI_System::allocate_CI_point(void) {
	try {
		this->dam_ci_point = new Dam_CI_Point[this->no_ci_point];
	}
	catch (bad_alloc &t) {
		Error msg = this->set_error(0);
		ostringstream info;
		info << "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the CI points
void Dam_CI_System::delete_CI_point(void) {
	if (this->dam_ci_point != NULL) {
		delete[]this->dam_ci_point;
		this->dam_ci_point = NULL;
	}
}
//Allocate the CI polygon
void Dam_CI_System::allocate_CI_polygon(void) {
	try {
		this->dam_ci_polygon = new Dam_CI_Polygon[this->no_ci_polygon];
	}
	catch (bad_alloc &t) {
		Error msg = this->set_error(1);
		ostringstream info;
		info << "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete the CI polygon
void Dam_CI_System::delete_CI_polygon(void) {
	if (this->dam_ci_polygon != NULL) {
		delete[]this->dam_ci_polygon;
		this->dam_ci_polygon = NULL;
	}
}
//Output the error_statistic of the system
void Dam_CI_System::output_error_statistic(void) {
	ostringstream prefix;
	ostringstream cout;
	prefix << "CI> STA> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	cout << "Statistics of the CI-system..." << endl;
	cout << "GENERAL" << endl;
	if (this->no_ci_point > 0) {
		cout << " Number CI-point(s)       :" << W(3) << this->no_ci_point << endl;
	}
	if (this->no_ci_polygon > 0) {
		cout << " Number CI-polygon(s)     :" << W(3) << this->no_ci_polygon << endl;
	}
	if (this->dam_ci_connection.get_number_connection() > 0) {
		cout << " Number CI-connection(s)  :" << W(3) << this->dam_ci_connection.get_number_connection() << endl;
	}
	cout << "TOTAL ERROR-/WARNING-NUMBER" << endl;
	cout << " Error(s)                 :" << W(3) << this->number_error << endl;
	cout << " Warning(s)               :" << W(3) << this->get_occured_warning() << endl;
	if (Dam_Damage_System::get_stop_thread_flag() == true) {
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_dam->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Find the midpoint of a CI-Element
Geo_Point Dam_CI_System::find_mid_point_CI_element(const int id, const int point_flag) {
	Geo_Point buffer;
	bool found_flag = false;
	//look in point data
	if (point_flag == 0) {
		for (int i = 0; i < this->no_ci_point; i++) {
			if (this->dam_ci_point[i].get_number() == id) {
				buffer = this->dam_ci_point[i];
				found_flag = true;
				break;
			}
		}
	}
	//look in polygon data
	else if (point_flag == 1) {
		for (int i = 0; i < this->no_ci_polygon; i++) {
			if (this->dam_ci_polygon[i].mid_point.get_number() == id) {
				buffer = this->dam_ci_polygon[i].mid_point;
				found_flag = true;
				break;
			}
		}

	}
	else {
		Error msg = this->set_error(23);
		ostringstream info;
		info << "Point flag: " << point_flag << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if (found_flag == false) {
		Error msg = this->set_error(25);
		ostringstream info;
		info << "Id to connect                     : " << id << endl;
		info << "Point flag (0:=point; 1:=polygon) : " << point_flag << endl;
		msg.make_second_info(info.str());
		throw msg;

	}
	return buffer;
}
//Find the CI-Element
_Dam_CI_Element Dam_CI_System::find_CI_element(const int id, const int point_flag) {
	_Dam_CI_Element buffer;
	bool found_flag = false;
	//look in point data
	if (point_flag == 0) {
		for (int i = 0; i < this->no_ci_point; i++) {
			if (this->dam_ci_point[i].get_number() == id) {
				buffer = this->dam_ci_point[i];
				found_flag = true;
				break;
			}
		}
	}
	//look in polygon data
	else if (point_flag == 1) {
		for (int i = 0; i < this->no_ci_polygon; i++) {
			if (this->dam_ci_polygon[i].mid_point.get_number() == id) {
				buffer = this->dam_ci_polygon[i];
				found_flag = true;
				break;
			}
		}

	}
	else {
		Error msg = this->set_error(24);
		ostringstream info;
		info << "Point flag: " << point_flag << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if (found_flag == false) {
		Error msg = this->set_error(26);
		ostringstream info;
		info << "Id to connect                     : " << id << endl;
		info << "Point flag (0:=point; 1:=polygon) : " << point_flag << endl;
		msg.make_second_info(info.str());
		throw msg;

	}
	return buffer;

}
//Find the pointer to CI-Element
_Dam_CI_Element* Dam_CI_System::find_ptr_CI_element(const int id, const int point_flag) {
	_Dam_CI_Element *buffer = NULL;

	//look in point data
	if (point_flag == 0) {
		for (int i = 0; i < this->no_ci_point; i++) {
			if (this->dam_ci_point[i].get_number() == id) {
				buffer = &this->dam_ci_point[i];

				break;
			}
		}
	}
	//look in polygon data
	else if (point_flag == 1) {
		for (int i = 0; i < this->no_ci_polygon; i++) {
			if (this->dam_ci_polygon[i].mid_point.get_number() == id) {
				buffer = &this->dam_ci_polygon[i];

				break;
			}
		}

	}
	else {
		Error msg = this->set_error(24);
		ostringstream info;
		info << "Point flag: " << point_flag << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return buffer;
}
//Calculate the direct damages
void Dam_CI_System::calculate_direct_damage(Dam_Impact_Value_Floodplain *impact_fp, const int number) {
	ostringstream cout;
	cout << "Calculate direct damages for the CI-system..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	int counter = 0;
	int counter_fp_id = 0;
	Dam_Impact_Values *impact;
	//search for each damage element for the corresponding hydraulic impact element
	for (int i = 0; i < this->no_ci_point; i++) {
		if (this->dam_ci_point[i].get_index_floodplain() >= 0) {
			counter = 0;
			//search for the floodplain index
			do {
				if (this->dam_ci_point[i].get_index_floodplain() == impact_fp[counter_fp_id].get_index_floodplain()) {
					break;
				}
				if (counter_fp_id == number - 1) {
					counter_fp_id = 0;
				}
				else {
					counter_fp_id++;
				}
				counter++;
			} while (counter < number);
			//search for the element
			if (this->dam_ci_point[i].get_index_floodplain_element() >= 0 && this->dam_ci_point[i].get_index_floodplain_element() < impact_fp[counter_fp_id].get_number_element()) {
				impact = &(impact_fp[counter_fp_id].impact_values[this->dam_ci_point[i].get_index_floodplain_element()]);
				//calculate the damges
				this->dam_ci_point[i].calculate_direct_damages(impact);
			}
		}
	}
}
//Calculate the idirect damages
void Dam_CI_System::calculate_indirect_damage(void) {
	ostringstream cout;
	cout << "Calculate indirect damages for the CI-system..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	//calc points

	//go through all elements
	for (int i = 0; i < this->no_ci_point; i++) {
		if (this->dam_ci_point[i].get_failure_type() == _dam_ci_failure_type::direct) {
			for (int j = 0; j < this->dam_ci_point[i].get_number_outgoing(); j++) {
				this->dam_ci_point[i].get_outgoing_elements()[j]->calculate_indirect_damages();
			}
		}
	}

	for (int i = 0; i < this->no_ci_polygon; i++) {
		this->dam_ci_polygon[i].finalize_results();
	}

	

}
//Calculate the direct damages instationary
void Dam_CI_System::calculate_instat_direct_damage(Dam_Impact_Value_Floodplain *impact_fp, const int number, const double time) {
	int counter = 0;
	int counter_fp_id = 0;
	Dam_Impact_Values *impact;
	//search for each damage element for the corresponding hydraulic impact element
	for (int i = 0; i < this->no_ci_point; i++) {
		if (this->dam_ci_point[i].get_index_floodplain() >= 0) {
			counter = 0;
			//search for the floodplain index
			do {
				if (this->dam_ci_point[i].get_index_floodplain() == impact_fp[counter_fp_id].get_index_floodplain()) {
					break;
				}
				if (counter_fp_id == number - 1) {
					counter_fp_id = 0;
				}
				else {
					counter_fp_id++;
				}
				counter++;
			} while (counter < number);
			//search for the element
			if (this->dam_ci_point[i].get_index_floodplain_element() >= 0 && this->dam_ci_point[i].get_index_floodplain_element() < impact_fp[counter_fp_id].get_number_element()) {
				impact = &(impact_fp[counter_fp_id].impact_values[this->dam_ci_point[i].get_index_floodplain_element()]);
				//calculate the damges
				this->dam_ci_point[i].calculate_direct_damages_instationary(impact,time);
			}
			else {
				Dam_Impact_Values buffer;
				this->dam_ci_point[i].calculate_direct_damages_instationary(&buffer, time);
				

			}
		}
	}

}
//Calculate the indirect damages instationary
void Dam_CI_System::calculate_instat_indirect_damage(const double time) {



	//go through all elements
	for (int i = 0; i < this->no_ci_point; i++) {
		//if (this->dam_ci_point[i].get_failure_type() == _dam_ci_failure_type::direct) {
			for (int j = 0; j < this->dam_ci_point[i].get_number_outgoing(); j++) {
				this->dam_ci_point[i].get_outgoing_elements()[j]->calculate_indirect_damages_instationary(time);
			}
		//}
	}

	for (int i = 0; i < this->no_ci_polygon; i++) {
		this->dam_ci_polygon[i].finalize_results();
	}

}
//Ouput CI-point results to database
void Dam_CI_System::output_point_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output) {
	//Set the query
	QSqlQuery query_buff(*ptr_database);

	ostringstream cout;
	cout << "Transfer the CI-point damage results to database..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	//get the global index
	int glob_id = Dam_CI_Point::get_max_glob_id_point_erg_table(ptr_database) + 1;
	//get the header for the query
	string query_header;
	query_header = Dam_CI_Point::get_insert_header_erg_table(ptr_database);

	ostringstream query_data;
	ostringstream query_total;
	int counter = 0;
	string buffer_data;
	bool must_output2 = false;
	bool must_output = true;

	for (int i = 0; i < this->no_ci_point; i++) {
		if (i % 10000 == 0 && i > 0) {
			cout << i << " (" << this->no_ci_point << ") results of CI-points are transfered to database..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Damage_System::check_stop_thread_flag();
		}

		if (i == this->no_ci_point - 1 && must_output == true && *was_output == false) {
			must_output2 = true;
		}

		buffer_data = this->dam_ci_point[i].get_datastring_results2database(glob_id, bound_sz, break_sz, must_output2);
		if (buffer_data != label::not_set) {
			query_data << buffer_data << " ,";
			//count the global index
			glob_id++;
			counter++;
			*was_output = true;
		}
		//send packages of 100
		if (counter == 100) {
			query_total << query_header << query_data.str();
			//delete last komma
			string buff = query_total.str();
			buff.erase(buff.length() - 1);
			Data_Base::database_request(&query_buff, buff, ptr_database);
			//delete them
			query_total.str("");
			query_data.str("");
			counter = 0;
			if (query_buff.lastError().isValid()) {
				Warning msg = this->set_warning(9);
				ostringstream info;
				info << "Table Name                : " << Dam_CI_Point::point_erg_table->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);
			}
		}
	}
	//send the rest
	if (counter != 0) {
		query_total << query_header << query_data.str();
		//delete last komma
		string buff = query_total.str();
		buff.erase(buff.length() - 1);
		Data_Base::database_request(&query_buff, buff, ptr_database);
		//delete them
		query_total.str("");
		query_data.str("");
		counter = 0;
		if (query_buff.lastError().isValid()) {
			Warning msg = this->set_warning(9);
			ostringstream info;
			info << "Table Name                : " << Dam_CI_Point::point_erg_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}

}
//Ouput CI-point instationary results to database
void Dam_CI_System::output_point_instat_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const string time_date, bool *was_output) {
	//Set the query
	QSqlQuery query_buff(*ptr_database);

	//get the global index
	int glob_id = Dam_CI_Point::get_max_glob_id_point_instat_erg_table(ptr_database) + 1;
	//get the header for the query
	string query_header;
	query_header = Dam_CI_Point::get_insert_header_instat_erg_table(ptr_database);

	ostringstream query_data;
	ostringstream query_total;
	int counter = 0;
	string buffer_data;
	bool must_output2 = false;
	bool must_output = true;

	for (int i = 0; i < this->no_ci_point; i++) {
		if (i % 10000 == 0 && i > 0) {
			//cout << i << " (" << this->no_ci_point << ") results of CI-points are transfered to database..." << endl;
			//Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Damage_System::check_stop_thread_flag();
		}

		if (i == this->no_ci_point - 1 && must_output == true && *was_output == false) {
			must_output2 = true;
		}

		buffer_data = this->dam_ci_point[i].get_datastring_instat_results2database(glob_id, bound_sz, break_sz, time_date, must_output2);
		if (buffer_data != label::not_set) {
			query_data << buffer_data << " ,";
			//count the global index
			glob_id++;
			counter++;
			*was_output = true;
		}
		//send packages of 100
		if (counter == 100) {
			query_total << query_header << query_data.str();
			//delete last komma
			string buff = query_total.str();
			buff.erase(buff.length() - 1);
			Data_Base::database_request(&query_buff, buff, ptr_database);
			//delete them
			query_total.str("");
			query_data.str("");
			counter = 0;
			if (query_buff.lastError().isValid()) {
				Warning msg = this->set_warning(13);
				ostringstream info;
				info << "Table Name                : " << Dam_CI_Point::point_instat_erg_table->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);
			}
		}
	}
	//send the rest
	if (counter != 0) {
		query_total << query_header << query_data.str();
		//delete last komma
		string buff = query_total.str();
		buff.erase(buff.length() - 1);
		Data_Base::database_request(&query_buff, buff, ptr_database);
		//delete them
		query_total.str("");
		query_data.str("");
		counter = 0;
		if (query_buff.lastError().isValid()) {
			Warning msg = this->set_warning(13);
			ostringstream info;
			info << "Table Name                : " << Dam_CI_Point::point_instat_erg_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}

}
//Ouput CI-polygon results to database
void Dam_CI_System::output_polygon_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output) {
	//Set the query
	QSqlQuery query_buff(*ptr_database);

	ostringstream cout;
	cout << "Transfer the CI-polygon damage results to database..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	//get the global index
	int glob_id = Dam_CI_Polygon::get_max_glob_id_polygon_erg_table(ptr_database) + 1;
	//get the header for the query
	string query_header;
	query_header = Dam_CI_Polygon::get_insert_header_erg_table(ptr_database);

	ostringstream query_data;
	ostringstream query_total;
	int counter = 0;
	string buffer_data;
	bool must_output2 = false;
	bool must_output = true;

	for (int i = 0; i < this->no_ci_polygon; i++) {
		if (i % 10000 == 0 && i > 0) {
			cout << i << " (" << this->no_ci_polygon << ") results of CI-polygon are transfered to database..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Damage_System::check_stop_thread_flag();
		}

		if (i == this->no_ci_polygon - 1 && must_output == true && *was_output == false) {
			must_output2 = true;
		}

		buffer_data = this->dam_ci_polygon[i].get_datastring_results2database(glob_id, bound_sz, break_sz, must_output2);
		if (buffer_data != label::not_set) {
			query_data << buffer_data << " ,";
			//count the global index
			glob_id++;
			counter++;
			*was_output = true;
		}
		//send packages of 100
		if (counter == 100) {
			query_total << query_header << query_data.str();
			//delete last komma
			string buff = query_total.str();
			buff.erase(buff.length() - 1);
			Data_Base::database_request(&query_buff, buff, ptr_database);
			//delete them
			query_total.str("");
			query_data.str("");
			counter = 0;
			if (query_buff.lastError().isValid()) {
				Warning msg = this->set_warning(11);
				ostringstream info;
				info << "Table Name                : " << Dam_CI_Polygon::polygon_erg_table->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);
			}
		}
	}
	//send the rest
	if (counter != 0) {
		query_total << query_header << query_data.str();
		//delete last komma
		string buff = query_total.str();
		buff.erase(buff.length() - 1);
		Data_Base::database_request(&query_buff, buff, ptr_database);
		//delete them
		query_total.str("");
		query_data.str("");
		counter = 0;
		if (query_buff.lastError().isValid()) {
			Warning msg = this->set_warning(11);
			ostringstream info;
			info << "Table Name                : " << Dam_CI_Polygon::polygon_erg_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}

}
//Ouput CI-polygon instationary results to database
void Dam_CI_System::output_polygon_instat_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const string time_date, bool *was_output) {
	//Set the query
	QSqlQuery query_buff(*ptr_database);

	//ostringstream cout;
	//cout << "Transfer the CI-polygon damage results to database..." << endl;
	//Sys_Common_Output::output_dam->output_txt(&cout);

	//get the global index
	int glob_id = Dam_CI_Polygon::get_max_glob_id_polygon_instat_erg_table(ptr_database) + 1;
	//get the header for the query
	string query_header;
	query_header = Dam_CI_Polygon::get_insert_header_instat_erg_table(ptr_database);

	ostringstream query_data;
	ostringstream query_total;
	int counter = 0;
	string buffer_data;
	bool must_output2 = false;
	bool must_output = true;

	for (int i = 0; i < this->no_ci_polygon; i++) {
		if (i % 10000 == 0 && i > 0) {
			//cout << i << " (" << this->no_ci_polygon << ") results of CI-polygon are transfered to database..." << endl;
			//Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Damage_System::check_stop_thread_flag();
		}

		if (i == this->no_ci_polygon - 1 && must_output == true && *was_output == false) {
			must_output2 = true;
		}

		buffer_data = this->dam_ci_polygon[i].get_datastring_instat_results2database(glob_id, bound_sz, break_sz, time_date,must_output2);
		if (buffer_data != label::not_set) {
			query_data << buffer_data << " ,";
			//count the global index
			glob_id++;
			counter++;
			*was_output = true;
		}
		//send packages of 100
		if (counter == 100) {
			query_total << query_header << query_data.str();
			//delete last komma
			string buff = query_total.str();
			buff.erase(buff.length() - 1);
			Data_Base::database_request(&query_buff, buff, ptr_database);
			//delete them
			query_total.str("");
			query_data.str("");
			counter = 0;
			if (query_buff.lastError().isValid()) {
				Warning msg = this->set_warning(14);
				ostringstream info;
				info << "Table Name                : " << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);
			}
		}
	}
	//send the rest
	if (counter != 0) {
		query_total << query_header << query_data.str();
		//delete last komma
		string buff = query_total.str();
		buff.erase(buff.length() - 1);
		Data_Base::database_request(&query_buff, buff, ptr_database);
		//delete them
		query_total.str("");
		query_data.str("");
		counter = 0;
		if (query_buff.lastError().isValid()) {
			Warning msg = this->set_warning(14);
			ostringstream info;
			info << "Table Name                : " << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}

}
//Ouput CI-connection results to database
void Dam_CI_System::output_connection_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output) {
	//Set the query
	QSqlQuery query_buff(*ptr_database);

	ostringstream cout;
	cout << "Transfer the CI-connection damage results to database..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	//get the global index
	int glob_id = Dam_CI_Element_List::get_max_glob_id_connection_erg_table(ptr_database) + 1;
	//get the header for the query
	string query_header;
	query_header = Dam_CI_Element_List::get_insert_header_erg_table(ptr_database);

	ostringstream query_data;
	ostringstream query_total;
	int counter = 0;
	string buffer_data;
	bool must_output2 = false;
	bool must_output = true;

	for (int i = 0; i < this->no_ci_point; i++) {
		if (i % 10000 == 0 && i > 0) {
			cout << i << " (" << this->no_ci_point << ") results of CI-point connections are transfered to database..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Damage_System::check_stop_thread_flag();
		}


		if (this->dam_ci_point[i].get_failure_type() == _dam_ci_failure_type::no_failure && this->dam_ci_point[i].get_regular_flag() == true) {
			buffer_data = label::not_set;
		}
		else if (this->dam_ci_point[i].get_end_level_flag() == true || this->dam_ci_point[i].get_number_outgoing() == 0) {
			buffer_data = label::not_set;
		}
		else if (this->dam_ci_point[i].get_element_active() ==false && this->dam_ci_point[i].get_regular_flag() == false) {
			buffer_data = label::not_set;
		}
		else {
			buffer_data = this->dam_ci_point[i].get_datastring_conect_results2database(&glob_id, bound_sz, break_sz, must_output2);

		}


		if (i == this->no_ci_point - 1 && must_output == true && *was_output == false) {
			must_output2 = true;
		}

		
		if (buffer_data != label::not_set) {
			query_data << buffer_data << " ,";
			//count the global index
			//glob_id++;
			counter++;
			*was_output = true;
		}
		//send packages of 100
		if (counter == 100) {
			query_total << query_header << query_data.str();
			//delete last komma
			string buff = query_total.str();
			buff.erase(buff.length() - 1);
			Data_Base::database_request(&query_buff, buff, ptr_database);
			//delete them
			query_total.str("");
			query_data.str("");
			counter = 0;
			if (query_buff.lastError().isValid()) {
				Warning msg = this->set_warning(15);
				ostringstream info;
				info << "Table Name                : " << Dam_CI_Element_List::connection_erg_table->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);
			}
		}
	}
	//send the rest
	if (counter != 0) {
		query_total << query_header << query_data.str();
		//delete last komma
		string buff = query_total.str();
		buff.erase(buff.length() - 1);
		Data_Base::database_request(&query_buff, buff, ptr_database);
		//delete them
		query_total.str("");
		query_data.str("");
		counter = 0;
		if (query_buff.lastError().isValid()) {
			Warning msg = this->set_warning(15);
			ostringstream info;
			info << "Table Name                : " << Dam_CI_Element_List::connection_erg_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}

}
//Ouput CI-connection instationary results to database
void Dam_CI_System::output_connection_instat_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const string time_date, bool *was_output) {
	//Set the query
	QSqlQuery query_buff(*ptr_database);

	
	//get the global index
	int glob_id = Dam_CI_Element_List::get_max_glob_id_connection_instat_erg_table(ptr_database) + 1;
	//get the header for the query
	string query_header;
	query_header = Dam_CI_Element_List::get_insert_header_instat_erg_table(ptr_database);

	ostringstream query_data;
	ostringstream query_total;
	int counter = 0;
	string buffer_data;
	bool must_output2 = false;
	bool must_output = true;

	for (int i = 0; i < this->no_ci_point; i++) {
		if (i % 10000 == 0 && i > 0) {
			Dam_Damage_System::check_stop_thread_flag();
		}


		if (this->dam_ci_point[i].get_failure_type() == _dam_ci_failure_type::no_failure && this->dam_ci_point[i].get_regular_flag() == true) {
			buffer_data = label::not_set;
		}
		else if (this->dam_ci_point[i].get_end_level_flag() == true || this->dam_ci_point[i].get_number_outgoing() == 0) {
			buffer_data = label::not_set;
		}
		else if (this->dam_ci_point[i].get_element_active() == false && this->dam_ci_point[i].get_regular_flag() == false) {
			buffer_data = label::not_set;
		}
		else {
			buffer_data = this->dam_ci_point[i].get_datastring_conect_instat_results2database(&glob_id, bound_sz, break_sz, time_date, must_output2);

		}


		if (i == this->no_ci_point - 1 && must_output == true && *was_output == false) {
			must_output2 = true;
		}


		if (buffer_data != label::not_set) {
			query_data << buffer_data << " ,";
			//count the global index
			
			counter++;
			*was_output = true;
		}
		//send packages of 100
		if (counter == 100) {
			query_total << query_header << query_data.str();
			//delete last komma
			string buff = query_total.str();
			buff.erase(buff.length() - 1);
			Data_Base::database_request(&query_buff, buff, ptr_database);
			//delete them
			query_total.str("");
			query_data.str("");
			counter = 0;
			if (query_buff.lastError().isValid()) {
				Warning msg = this->set_warning(16);
				ostringstream info;
				info << "Table Name                : " << Dam_CI_Element_List::connection_instat_erg_table->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);
			}
		}
	}
	//send the rest
	if (counter != 0) {
		query_total << query_header << query_data.str();
		//delete last komma
		string buff = query_total.str();
		buff.erase(buff.length() - 1);
		Data_Base::database_request(&query_buff, buff, ptr_database);
		//delete them
		query_total.str("");
		query_data.str("");
		counter = 0;
		if (query_buff.lastError().isValid()) {
			Warning msg = this->set_warning(16);
			ostringstream info;
			info << "Table Name                : " << Dam_CI_Element_List::connection_instat_erg_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}

}

//Check if the points are connected to the hydraulic
int Dam_CI_System::check_points_connected2hyd(void) {
	int counter = 0;

	for (int i = 0; i < this->no_ci_point; i++) {
		if (this->dam_ci_point[i].get_connected_flag() == true) {
			counter++;
		}
	}

	return counter;
}
//Check the CI-system
void Dam_CI_System::check_CI_system(void) {
	ostringstream cout;
	cout << "Check the CI-system..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	int count = 0;
	try {
		//check point
		cout << "Check the CI-points..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for (int i = 0; i < this->no_ci_point; i++) {
			count = i;
			this->dam_ci_point[i].check_connections();
		}
	}
	catch(Error msg){
		ostringstream info;
		info << "Point Name       : " << this->dam_ci_point[count].get_point_name() << endl;
		info << "Point Id         : " << this->dam_ci_point[count].get_number() << endl;
		msg.make_second_info(info.str());
		throw msg;

	}
	try {
		cout << "Check the CI-polygons..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for (int i = 0; i < this->no_ci_polygon; i++) {
			count = i;
			this->dam_ci_polygon[i].check_connections();
			this->dam_ci_polygon[i].check_polygon_incoming();
		}
	}
	catch (Error msg) {
		ostringstream info;
		info << "Polygon Name       : " << this->dam_ci_polygon[count].mid_point.get_point_name() << endl;
		info << "Polygon Id         : " << this->dam_ci_polygon[count].mid_point.get_number() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}



	cout << "Check the CI-system finished" << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

}
//Reset results values
void Dam_CI_System::reset_result_values(void) {

	this->list_direct_failure_sec.clear();
	this->list_sectoral_failure_sec.clear();
	this->list_transsectoral_failure_sec.clear();
	this->list_total_failure_sec.clear();
	this->list_total_failure_level_sec.clear();
	this->tot_direct_failure=0; 
	this->tot_sectoral_failure=0;
	this->tot_transsectoral_failure=0;

	this->list_enduser_affected_sec.clear();
	this->list_enduser_affected_duration_sec.clear();

	this->list_emergency_active_sec.clear();

	this->tot_emergency_active = 0;

}
//Set error(s)
Error Dam_CI_System::set_error(const int err_type) {
	string place = "Dam_CI_System::";
	string help;
	string reason;
	int type = 0;
	bool fatal = false;
	stringstream info;
	Error msg;

	switch (err_type) {
	case 0://bad alloc
		place.append("allocate_CI_point(void)");
		reason = "Can not allocate the memory";
		help = "Check the memory";
		type = 10;
		break;
	case 1://bad alloc
		place.append("allocate_CI_polygon(void)");
		reason = "Can not allocate the memory";
		help = "Check the memory";
		type = 10;
		break;
	case 2://can not open file
		place.append("read_points_per_file(string fname)");
		reason = "Can not open file";
		help = "Check the file";
		type = 5;
		break;
	case 3://wrong input sequenze
		place.append("read_points_per_file(string fname)");
		reason = "Wrong input by reading the number of points";
		help = "Check the file";
		type = 5;
	case 4://not all info there
		place.append("read_points_per_file(string fname)");
		reason = "Not all relevant point data are found in file";
		help = "Check the point data in file";
		type = 5;
		break;
	case 5://can not read in the x-ycoordinates properly
		place.append("read_points_per_file(string fname)");
		reason = "Can not read in the x-, y-coordinates and data of the simple counting points properly";
		help = "Check the coordinates and other data in file";
		type = 5;
		break;
	case 6://not all points are found
		place.append("read_points_per_file(string fname)");
		reason = "Not all points are found";
		help = "Check the number of points and the given point data";
		info << "Number of searched points " << this->no_ci_point << endl;
		type = 5;
		break;
	case 7://not all data are found
		place.append("read_points_per_file(string fname)");
		reason = "Not all data are found";
		help = "Check the file";
		type = 5;
		break;
	case 8://to much points
		place.append("read_points_per_file(string fname)");
		reason = "To much points are given; Maximum is 1000000 points";
		help = "Check the file";
		type = 5;
		break;
	case 9://wrong input sequenze
		place.append("read_polygon_per_file(string fname)");
		reason = "Wrong input by reading the number of polygons";
		help = "Check the file";
		type = 5;
	case 10://not all data are found
		place.append("read_polygon_per_file(string fname)");
		reason = "Not all data are found";
		help = "Check the file";
		type = 5;
		break;
	case 11://not all info there
		place.append("read_polygon_per_file(string fname)");
		reason = "Not all relevant polygon data are found in file";
		help = "Check the point data in file";
		type = 5;
		break;
	case 12://not all points are found
		place.append("read_polygon_per_file(string fname)");
		reason = "Not all points for polygon are found";
		help = "Check the number of points in polygon";
		type = 5;
		break;
	case 13://wrong input sequenze
		place.append("read_polygon_per_file(string fname)");
		reason = "Wrong input by reading points of polygons";
		help = "Check the file";
		type = 5;
	case 14://not all info there
		place.append("read_polygon_per_file(string fname)");
		reason = "Not all relevant point data for polygon are found in file";
		help = "Check the point data in file";
		type = 5;
		break;
	case 15://wrong input sequenze
		place.append("read_polygon_per_file(string fname)");
		reason = "Wrong input by reading the polygon data";
		help = "Check the file";
		type = 5;
	case 16://can not open file
		place.append("read_polygon_per_file(string fname)");
		reason = "Can not open file";
		help = "Check the file";
		type = 5;
		break;
	case 17://can not open file
		place.append("read_connection_per_file(string fname)");
		reason = "Can not open file";
		help = "Check the file";
		type = 5;
		break;
	case 18://not all data are found
		place.append("read_connection_per_file(string fname)");
		reason = "Not all data are found";
		help = "Check the file";
		type = 5;
		break;
	case 19://wrong input sequenze
		place.append("read_connection_per_file(string fname)");
		reason = "Wrong input by reading the number of points";
		help = "Check the file";
		type = 5;
	case 20://not all info there
		place.append("read_connection_per_file(string fname)");
		reason = "Not all relevant connection data are found in file";
		help = "Check the connection data in file";
		type = 5;
		break;
	case 21://not all connections are found
		place.append("read_connection_per_file(string fname)");
		reason = "Not all connections are found";
		help = "Check the number of connections and the given connection data";
		type = 5;
		break;
	case 22://not all connections are found
		place.append("read_connection_per_file(string fname)");
		reason = "Wrong point/polygon flag found in file";
		help = "Check the connection data in file";
		type = 5;
		break;
	case 23://not all connections are found
		place.append("find_mid_point_CI_element(const int i, const int point_flag)");
		reason = "Wrong int for point flag";
		help = "Check the connection data";
		type = 5;
		break;
	case 24://not all connections are found
		place.append("find_CI_element(const int i, const int point_flag)");
		reason = "Wrong int for point flag";
		help = "Check the connection data";
		type = 5;
		break;
	case 25://connection not found
		place.append("find_mid_point_CI_element(const int id, const int point_flag) ");
		reason = "The connection is not found";
		help = "Check the connection data and the relevant CI-point/-polygon data";
		type = 34;
		break;
	case 26://connection not found
		place.append("find_CI_element(const int id, const int point_flag) ");
		reason = "The connection is not found";
		help = "Check the connection data and the relevant CI-point/-polygon data";
		type = 34;
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
//Set warning(s)
Warning Dam_CI_System::set_warning(const int warn_type) {
	string place = "Dam_CI_System::";
	string help;
	string reason;
	string reaction;
	int type = 0;
	Warning msg;
	stringstream info;

	switch (warn_type) {
		case 0://no !END-flag is found in file
			place.append("read_points_per_file(string fname)");
			reason = "The point data block is not closed with !END in file";
			help = "Check the file and the number of point specification in file";
			type = 1;
			break;
		case 1://input datas can not submitted
			place.append("transfer_point_members2database(QSqlDatabase *ptr_database)");
			reason = "Can not submit the point data to the database";
			help = "Check the database";
			type = 2;
			break;
		case 2://no !END-flag is found in file
			place.append("read_polygon_per_file(string fname)");
			reason = "The polygon data block is not closed with !END in file";
			help = "Check the file and the number of polygon specification in file";
			type = 1;
			break;
		case 3://input datas can not submitted
			place.append("transfer_polygon_members2database(QSqlDatabase *ptr_database)");
			reason = "Can not submit the polygon data to the database";
			help = "Check the database";
			type = 2;
			break;
		case 4://no !END-flag is found in file
			place.append("read_connection_per_file(string fname)");
			reason = "The connection data block is not closed with !END in file";
			help = "Check the file and the number of connection specification in file";
			type = 1;
			break;
		case 6://not all points are connected
			place.append("check_system(void)");
			reason = "Not all points are connected to the hydraulic system";
			help = "Reconnect the CI-system";
			type = 1;
			break;

		case 8://input datas can not submitted
			place.append("transfer_connection_members2database(QSqlDatabase *ptr_database)");
			reason = "Can not submit the CI-connection data to the database";
			help = "Check the database";
			type = 2;
			break;
		case 9://output datas can not submitted
			place.append("output_point_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output)");
			reason = "Can not submit the CI-point result data to the database";
			help = "Check the database";
			type = 2;
			break;
		case 10://output datas can not submitted
			place.append("output_point_instat_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output)");
			reason = "Can not submit the CI-point instationary result data to the database";
			help = "Check the database";
			type = 2;
			break;
		case 11://output datas can not submitted
			place.append("output_polygon_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output)");
			reason = "Can not submit the CI-polygon result data to the database";
			help = "Check the database";
			type = 2;
			break;
		case 12://output datas can not submitted
			place.append("output_polygon_instat_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output)");
			reason = "Can not submit the CI-polygon instationary result data to the database";
			help = "Check the database";
			type = 2;
			break;
		case 13://output datas can not submitted
			place.append("output_point_instat_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output)");
			reason = "Can not submit the CI-point instationary result data to the database";
			help = "Check the database";
			type = 2;
			break;
		case 14://output datas can not submitted
			place.append("output_polygon_instat_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output)");
			reason = "Can not submit the CI-polygon instationary result data to the database";
			help = "Check the database";
			type = 2;
			break;
		case 15://output datas can not submitted
			place.append("output_connection_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output)");
			reason = "Can not submit the CI-connection result data to the database";
			help = "Check the database";
			type = 2;
			break;
		case 16://output datas can not submitted
			place.append("output_connection_instat_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output)");
			reason = "Can not submit the CI-connection result data to the database";
			help = "Check the database";
			type = 2;
			break;
		default:
			place.append("set_warning(const int warn_type)");
			reason = "Unknown flag!";
			help = "Check the flags";
			type = 5;
	}
	msg.set_msg(place, reason, help, reaction, type);
	msg.make_second_info(info.str());
	return msg;
}