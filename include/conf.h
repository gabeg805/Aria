/* -----------------------------------------------------------------------------
 * 
 * Name:    conf.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Parse config files.
 * 
 * Notes: None.
 * 
 * -----------------------------------------------------------------------------
 */

/* Includes */
#include <glib.h>
#include <string>
#include <vector>

std::string conf_read(const char* group, const char* key);
std::string conf_read_str(const char* group, const char* key);
int conf_read_int(const char* group, const char* key);
bool conf_read_bool(const char* group, const char* key);
int conf_key_file(GKeyFile** keyfile, const char* configfile);
std::vector<std::string> conf_get_groups(void);
std::vector<std::string> conf_get_keys(const char* group);
int is_key_err(GError **err);
