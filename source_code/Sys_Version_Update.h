#pragma once
#ifndef SYS_VERSION_UPDATE_H
#define SYS_VERSION_UPDATE_H
/**\class Sys_Version_Update
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2011
*/

///Class for check and update old ProMaIDes-versions \ingroup sys
/**

*/
class Sys_Version_Update
{
public:
	///Default constructor
	Sys_Version_Update(void);
	///Default destructor
	~Sys_Version_Update(void);

	//method
	///Check and update the hydraulic table for observation points (20.12.2011)
	void check_update_hyd_table_obs_point(QSqlDatabase *ptr_database);
	///Check and update the text of the hydraulic table of the polysegment (20.12.2011)
	void check_update_hyd_table_polysegment_name(const string project_file);
	///Check and update the hydraulic table of the hydraulic system members (21.02.2012)
	void check_update_hyd_table_system_member_sync(QSqlDatabase *ptr_database, const string project_file);

	///Check and update the result members table of the fpl section type dike (21.05.2012)
	void check_update_fpl_table_result_sec_type_dike(QSqlDatabase *ptr_database, const string project_file);
	///Check for update fpl table for results of section type dune (5.5.2012)
	void check_update_fpl_table_dune_results(QSqlDatabase *ptr_database);

	///Check and update the text of the hydraulic table of the hydraulic element result members; smax is introduced (6.07.2012)
	void check_update_hyd_table_elem_result_smax(QSqlDatabase *ptr_database, const string project_file);

private:
	///Set error(s)
	Error set_error(const int err_type);
};
#endif